// ============================================================
//  Library Management System - C++ REST API Backend
//  Stack: cpp-httplib (HTTP) + nlohmann/json (JSON)
//  Features: JWT-ish token auth (admin/member), books, members,
//            loans, fines/overdue tracking, search, dashboard,
//            CSV report export. JSON file persistence.
// ============================================================

#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <mutex>
#include <random>
#include <ctime>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

// ---------------- Config ----------------
static const int    PORT       = 8080;
static const int    LOAN_DAYS  = 14;      // borrow period
static const double FINE_RATE  = 2.0;     // currency units per overdue day
static const std::string DB_FILE = "library_db.json";

// ---------------- Helpers ----------------
static std::mutex g_mutex;          // guards all data + file IO

static long today_epoch() {
    return (long)std::time(nullptr);
}
static long days_to_secs(int d) { return (long)d * 86400L; }

static std::string epoch_to_date(long e) {
    std::time_t t = (std::time_t)e;
    std::tm* lt = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return buf;
}

static std::string gen_token() {
    static std::mt19937_64 rng(std::random_device{}());
    static const char* hex = "0123456789abcdef";
    std::string s;
    for (int i = 0; i < 32; ++i) s += hex[rng() % 16];
    return s;
}

// ---------------- Data Models ----------------
struct Book {
    int id;
    std::string title, author, isbn, category;
    int total, available;
    json to_json() const {
        return {{"id",id},{"title",title},{"author",author},{"isbn",isbn},
                {"category",category},{"total",total},{"available",available}};
    }
};

struct Member {
    int id;
    std::string name, email, password, role; // role: "admin" | "member"
    json to_json(bool with_pw=false) const {
        json j = {{"id",id},{"name",name},{"email",email},{"role",role}};
        if (with_pw) j["password"] = password;
        return j;
    }
};

struct Loan {
    int id, bookId, memberId;
    long issuedAt, dueAt, returnedAt; // epoch; returnedAt=0 if active
    double fine;
    json to_json() const {
        return {{"id",id},{"bookId",bookId},{"memberId",memberId},
                {"issuedAt",issuedAt},{"dueAt",dueAt},{"returnedAt",returnedAt},
                {"issuedDate",epoch_to_date(issuedAt)},
                {"dueDate",epoch_to_date(dueAt)},
                {"returnedDate", returnedAt? epoch_to_date(returnedAt) : ""},
                {"fine",fine}};
    }
};

// ---------------- Store ----------------
class Store {
public:
    std::unordered_map<int, Book>   books;
    std::unordered_map<int, Member> members;
    std::vector<Loan>               loans;
    std::unordered_map<std::string,int> tokens; // token -> memberId
    int nextBook=1, nextMember=1, nextLoan=1;

    Store() { load(); if (members.empty()) seed(); }

    void seed() {
        members[nextMember] = {nextMember,"Admin","admin@library.com","admin123","admin"}; nextMember++;
        members[nextMember] = {nextMember,"Bhanu","bhanu@library.com","pass123","member"}; nextMember++;
        auto add=[&](std::string t,std::string a,std::string i,std::string c,int n){
            books[nextBook]={nextBook,t,a,i,c,n,n}; nextBook++; };
        add("The Pragmatic Programmer","Hunt & Thomas","978-0135957059","Software",3);
        add("Clean Code","Robert C. Martin","978-0132350884","Software",2);
        add("Introduction to Algorithms","Cormen et al.","978-0262046305","CS",4);
        add("The C++ Programming Language","Bjarne Stroustrup","978-0321563842","Programming",2);
        add("Design Patterns","Gang of Four","978-0201633610","Software",1);
        save();
    }

    // ---- Persistence ----
    void load() {
        std::ifstream f(DB_FILE);
        if (!f) return;
        json j; try { f >> j; } catch(...) { return; }
        nextBook   = j.value("nextBook",1);
        nextMember = j.value("nextMember",1);
        nextLoan   = j.value("nextLoan",1);
        for (auto& b : j.value("books",json::array()))
            books[b["id"]] = {b["id"],b["title"],b["author"],b["isbn"],
                              b["category"],b["total"],b["available"]};
        for (auto& m : j.value("members",json::array()))
            members[m["id"]] = {m["id"],m["name"],m["email"],m["password"],m["role"]};
        for (auto& l : j.value("loans",json::array()))
            loans.push_back({l["id"],l["bookId"],l["memberId"],l["issuedAt"],
                             l["dueAt"],l["returnedAt"],l["fine"]});
    }

    void save() {
        json j;
        j["nextBook"]=nextBook; j["nextMember"]=nextMember; j["nextLoan"]=nextLoan;
        j["books"]=json::array();
        for (auto&[id,b]:books) j["books"].push_back(b.to_json());
        j["members"]=json::array();
        for (auto&[id,m]:members) j["members"].push_back(m.to_json(true));
        j["loans"]=json::array();
        for (auto&l:loans) j["loans"].push_back(l.to_json());
        std::ofstream f(DB_FILE); f << j.dump(2);
    }

    // ---- Fine computation (live, for active overdue loans) ----
    double compute_fine(const Loan& l) const {
        if (l.returnedAt) return l.fine;            // settled
        long now = today_epoch();
        if (now <= l.dueAt) return 0.0;
        int overdue = (int)((now - l.dueAt) / 86400L);
        return overdue * FINE_RATE;
    }
};

static Store store;

// ---------------- Auth ----------------
static int auth_member(const httplib::Request& req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) return -1;
    std::string h = it->second;
    if (h.rfind("Bearer ",0)==0) h = h.substr(7);
    auto t = store.tokens.find(h);
    return t==store.tokens.end() ? -1 : t->second;
}
static bool is_admin(int mid) {
    auto it = store.members.find(mid);
    return it!=store.members.end() && it->second.role=="admin";
}

// ---------------- Main ----------------
int main() {
    httplib::Server svr;

    // CORS for the React dev server
    svr.set_default_headers({
        {"Access-Control-Allow-Origin","*"},
        {"Access-Control-Allow-Methods","GET,POST,PUT,DELETE,OPTIONS"},
        {"Access-Control-Allow-Headers","Content-Type,Authorization"}
    });
    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res){
        res.status = 204;
    });

    auto json_res = [](httplib::Response& res, const json& j, int code=200){
        res.status = code;
        res.set_content(j.dump(), "application/json");
    };

    // ---------- AUTH: login ----------
    svr.Post("/api/login", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        json body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return json_res(res,{{"error","bad json"}},400);
        std::string email = body.value("email","");
        std::string pw    = body.value("password","");
        for (auto&[id,m]:store.members) {
            if (m.email==email && m.password==pw) {
                std::string tok = gen_token();
                store.tokens[tok]=id;
                return json_res(res,{{"token",tok},{"user",m.to_json()}});
            }
        }
        json_res(res,{{"error","Invalid credentials"}},401);
    });

    // ---------- AUTH: register (member) ----------
    svr.Post("/api/register", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        json body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) return json_res(res,{{"error","bad json"}},400);
        std::string email = body.value("email","");
        for (auto&[id,m]:store.members)
            if (m.email==email) return json_res(res,{{"error","Email already exists"}},409);
        Member m{store.nextMember, body.value("name",""), email,
                 body.value("password",""), "member"};
        store.members[store.nextMember++] = m;
        store.save();
        json_res(res,{{"message","Registered"},{"user",m.to_json()}},201);
    });

    // ---------- BOOKS: list + search + filter ----------
    svr.Get("/api/books", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        std::string q   = req.has_param("q")? req.get_param_value("q") : "";
        std::string cat = req.has_param("category")? req.get_param_value("category") : "";
        std::string avail = req.has_param("available")? req.get_param_value("available") : "";
        std::transform(q.begin(),q.end(),q.begin(),::tolower);
        json arr = json::array();
        std::vector<const Book*> sorted;
        for (auto&[id,b]:store.books) sorted.push_back(&b);
        std::sort(sorted.begin(),sorted.end(),[](auto a,auto b){return a->id<b->id;});
        for (auto* b : sorted) {
            std::string t=b->title,a=b->author;
            std::transform(t.begin(),t.end(),t.begin(),::tolower);
            std::transform(a.begin(),a.end(),a.begin(),::tolower);
            if (!q.empty() && t.find(q)==std::string::npos && a.find(q)==std::string::npos) continue;
            if (!cat.empty() && b->category!=cat) continue;
            if (avail=="true" && b->available<=0) continue;
            arr.push_back(b->to_json());
        }
        json_res(res, arr);
    });

    // ---------- BOOKS: categories ----------
    svr.Get("/api/categories", [&](const httplib::Request&, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        std::vector<std::string> cats;
        for (auto&[id,b]:store.books)
            if (std::find(cats.begin(),cats.end(),b.category)==cats.end())
                cats.push_back(b.category);
        std::sort(cats.begin(),cats.end());
        json_res(res, cats);
    });

    // ---------- BOOKS: create (admin) ----------
    svr.Post("/api/books", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        json b = json::parse(req.body,nullptr,false);
        if (b.is_discarded()) return json_res(res,{{"error","bad json"}},400);
        int total = b.value("total",1);
        Book nb{store.nextBook, b.value("title",""), b.value("author",""),
                b.value("isbn",""), b.value("category","General"), total, total};
        store.books[store.nextBook++]=nb;
        store.save();
        json_res(res, nb.to_json(), 201);
    });

    // ---------- BOOKS: update (admin) ----------
    svr.Put(R"(/api/books/(\d+))", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        int id = std::stoi(req.matches[1]);
        if (!store.books.count(id)) return json_res(res,{{"error","Not found"}},404);
        json b = json::parse(req.body,nullptr,false);
        Book& bk = store.books[id];
        int issued = bk.total - bk.available;
        bk.title    = b.value("title",bk.title);
        bk.author   = b.value("author",bk.author);
        bk.isbn     = b.value("isbn",bk.isbn);
        bk.category = b.value("category",bk.category);
        bk.total    = b.value("total",bk.total);
        bk.available= std::max(0, bk.total - issued);
        store.save();
        json_res(res, bk.to_json());
    });

    // ---------- BOOKS: delete (admin) ----------
    svr.Delete(R"(/api/books/(\d+))", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        int id = std::stoi(req.matches[1]);
        store.books.erase(id);
        store.save();
        json_res(res,{{"message","Deleted"}});
    });

    // ---------- MEMBERS: list (admin) ----------
    svr.Get("/api/members", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        json arr=json::array();
        std::vector<const Member*> sorted;
        for (auto&[id,m]:store.members) sorted.push_back(&m);
        std::sort(sorted.begin(),sorted.end(),[](auto a,auto b){return a->id<b->id;});
        for (auto* m:sorted) arr.push_back(m->to_json());
        json_res(res, arr);
    });

    // ---------- LOANS: issue (admin) ----------
    svr.Post("/api/loans", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        json b=json::parse(req.body,nullptr,false);
        if (b.is_discarded()) return json_res(res,{{"error","bad json"}},400);
        int bookId=b.value("bookId",0), memberId=b.value("memberId",0);
        if (!store.books.count(bookId)) return json_res(res,{{"error","Book not found"}},404);
        if (!store.members.count(memberId)) return json_res(res,{{"error","Member not found"}},404);
        Book& bk=store.books[bookId];
        if (bk.available<=0) return json_res(res,{{"error","No copies available"}},400);
        bk.available--;
        long now=today_epoch();
        Loan l{store.nextLoan++,bookId,memberId,now,now+days_to_secs(LOAN_DAYS),0,0.0};
        store.loans.push_back(l);
        store.save();
        json_res(res, l.to_json(), 201);
    });

    // ---------- LOANS: return ----------
    svr.Post(R"(/api/loans/(\d+)/return)", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        int id=std::stoi(req.matches[1]);
        for (auto& l:store.loans) {
            if (l.id==id && !l.returnedAt) {
                l.returnedAt=today_epoch();
                l.fine = store.compute_fine({l.id,l.bookId,l.memberId,l.issuedAt,l.dueAt,0,0});
                if (store.books.count(l.bookId)) store.books[l.bookId].available++;
                store.save();
                return json_res(res, l.to_json());
            }
        }
        json_res(res,{{"error","Active loan not found"}},404);
    });

    // ---------- LOANS: list (admin sees all; member sees own) ----------
    svr.Get("/api/loans", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (mid<0) return json_res(res,{{"error","Unauthorized"}},401);
        bool admin=is_admin(mid);
        std::string status = req.has_param("status")? req.get_param_value("status") : "";
        json arr=json::array();
        for (auto& l:store.loans) {
            if (!admin && l.memberId!=mid) continue;
            if (status=="active" && l.returnedAt) continue;
            if (status=="returned" && !l.returnedAt) continue;
            json lj=l.to_json();
            lj["liveFine"]=store.compute_fine(l);
            lj["overdue"]= (!l.returnedAt && today_epoch()>l.dueAt);
            if (store.books.count(l.bookId))   lj["bookTitle"]=store.books[l.bookId].title;
            if (store.members.count(l.memberId)) lj["memberName"]=store.members[l.memberId].name;
            arr.push_back(lj);
        }
        json_res(res, arr);
    });

    // ---------- DASHBOARD stats ----------
    svr.Get("/api/stats", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) return json_res(res,{{"error","Admin only"}},403);
        int totalCopies=0,avail=0,active=0,overdue=0; double fines=0;
        for (auto&[id,b]:store.books){ totalCopies+=b.total; avail+=b.available; }
        for (auto& l:store.loans){
            if (!l.returnedAt){ active++; if (today_epoch()>l.dueAt) overdue++; }
            fines += store.compute_fine(l);
        }
        json_res(res,{
            {"titles",(int)store.books.size()},
            {"totalCopies",totalCopies},
            {"available",avail},
            {"onLoan",totalCopies-avail},
            {"members",(int)store.members.size()},
            {"activeLoans",active},
            {"overdue",overdue},
            {"totalFines",fines}
        });
    });

    // ---------- REPORT: CSV export (admin) ----------
    svr.Get("/api/report/loans.csv", [&](const httplib::Request& req, httplib::Response& res){
        std::lock_guard<std::mutex> lk(g_mutex);
        int mid=auth_member(req);
        if (!is_admin(mid)) { res.status=403; res.set_content("Admin only","text/plain"); return; }
        std::ostringstream os;
        os << "LoanID,Book,Member,IssuedDate,DueDate,ReturnedDate,Status,Fine\n";
        for (auto& l:store.loans){
            std::string bt = store.books.count(l.bookId)? store.books[l.bookId].title : "?";
            std::string mn = store.members.count(l.memberId)? store.members[l.memberId].name : "?";
            std::replace(bt.begin(),bt.end(),',',';');
            std::replace(mn.begin(),mn.end(),',',';');
            std::string status = l.returnedAt? "Returned" : (today_epoch()>l.dueAt?"Overdue":"Active");
            os << l.id << ',' << bt << ',' << mn << ','
               << epoch_to_date(l.issuedAt) << ','
               << epoch_to_date(l.dueAt) << ','
               << (l.returnedAt? epoch_to_date(l.returnedAt):"") << ','
               << status << ',' << store.compute_fine(l) << "\n";
        }
        res.set_header("Content-Disposition","attachment; filename=loans_report.csv");
        res.set_content(os.str(),"text/csv");
    });

    std::cout << "Library API running on http://localhost:" << PORT << "\n";
    std::cout << "Seed logins:\n  admin@library.com / admin123 (admin)\n"
              << "  bhanu@library.com / pass123 (member)\n";
    svr.listen("0.0.0.0", PORT);
    return 0;
}
