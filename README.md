# Athenaeum — Library Management System

A full-stack library management system built as an internship project.

- **Backend:** C++17 REST API (`cpp-httplib` + `nlohmann/json`)
- **Frontend:** React 18 + Vite (3D realistic "reading room" UI — varnished oak shelving, embossed leather book spines, brass fittings, raked lighting and tactile depth)
- **Storage:** JSON file persistence (no external database needed)

---

## Features

**Authentication & Roles**
- Login + member self-registration, token-based sessions
- Two roles: **admin** (full control) and **member** (browse catalogue, view own loans)
- Server-side role enforcement on every protected endpoint

**Catalogue**
- Add / edit / delete books (admin)
- Live search by title or author, filter by category and availability
- Per-title copy tracking (available / total)

**Members**
- Self-registration; admin can view all members

**Circulation**
- Issue books (14-day loan period) and process returns
- **Fines & overdue tracking** — fines accrue automatically per overdue day (₹2/day, configurable)
- Filter loans by active / returned / all

**Dashboard**
- Live stats: titles, copies, on-loan, members, active loans, overdue count, total outstanding fines

**Reports**
- One-click **CSV export** of the full loan history

---

## Project Structure

```
library-system/
├── backend/
│   ├── server.cpp        # the REST API (single file)
│   ├── httplib.h         # HTTP library (vendored)
│   ├── json.hpp          # JSON library (vendored)
│   └── library_db.json   # auto-created on first run
└── frontend/
    ├── src/
    │   ├── App.jsx        # all views + components
    │   ├── styles.css     # editorial theme
    │   └── main.jsx
    ├── index.html
    └── package.json
```

---

## Running It

You need **two terminals** — one for the backend, one for the frontend.

### 1. Backend (C++)

**Requirements:** a C++17 compiler (g++ / MinGW on Windows, clang on Mac).

```bash
cd backend
g++ -std=c++17 -O2 -pthread server.cpp -o server   # Windows: add -lws2_32
./server                                             # Windows: .\server.exe
```

The API starts on `http://localhost:8080`.

> **Windows note:** the compile command needs the sockets library:
> ```
> g++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32
> ```

### 2. Frontend (React)

**Requirements:** Node.js 18+.

```bash
cd frontend
npm install
npm run dev
```

Open the URL it prints (usually `http://localhost:5173`).

---

## Demo Logins (seeded automatically)

| Role   | Email                | Password |
|--------|----------------------|----------|
| Admin  | admin@library.com    | admin123 |
| Member | bhanu@library.com    | pass123  |

---

## API Reference

| Method | Endpoint                     | Access  | Purpose                       |
|--------|------------------------------|---------|-------------------------------|
| POST   | `/api/login`                 | public  | Authenticate, returns token   |
| POST   | `/api/register`              | public  | Register a new member         |
| GET    | `/api/books`                 | public  | List/search/filter books      |
| GET    | `/api/categories`            | public  | Distinct categories           |
| POST   | `/api/books`                 | admin   | Add book                      |
| PUT    | `/api/books/:id`             | admin   | Update book                   |
| DELETE | `/api/books/:id`             | admin   | Delete book                   |
| GET    | `/api/members`               | admin   | List members                  |
| GET    | `/api/loans`                 | auth    | Loans (own for members)       |
| POST   | `/api/loans`                 | admin   | Issue book                    |
| POST   | `/api/loans/:id/return`      | admin   | Return book + settle fine     |
| GET    | `/api/stats`                 | admin   | Dashboard stats               |
| GET    | `/api/report/loans.csv`      | admin   | CSV export                    |

---

## Configuration

Edit the constants at the top of `backend/server.cpp`:

```cpp
static const int    PORT      = 8080;   // API port
static const int    LOAN_DAYS = 14;     // borrow period
static const double FINE_RATE = 2.0;    // fine per overdue day
```

---

## Notes & Possible Extensions

- Tokens are kept in memory, so restarting the backend logs everyone out (fine for a demo).
- Data persists in `library_db.json` across restarts.
- Ideas to extend: password hashing (bcrypt), reservation queue, email reminders for due dates, pagination, and a PDF report alongside the CSV.
