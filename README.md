<div align="center">

# 📚 Athenaeum — 3D Library Management System

### *A full-stack library system with a **C++ REST engine** and a **living, 3D React catalogue.***

<br/>

![C++](https://img.shields.io/badge/Backend-C%2B%2B17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![React](https://img.shields.io/badge/Frontend-React%2018-61DAFB?style=for-the-badge&logo=react&logoColor=black)
![Vite](https://img.shields.io/badge/Build-Vite-646CFF?style=for-the-badge&logo=vite&logoColor=white)
![REST API](https://img.shields.io/badge/API-REST-3fbf8f?style=for-the-badge)
![License](https://img.shields.io/badge/Project-Internship-d4a857?style=for-the-badge)

<br/>

<img src="https://readme-typing-svg.demolab.com?font=Fraunces&weight=600&size=24&duration=3000&pause=800&color=D4A857&center=true&vCenter=true&width=600&lines=Real+3D+book+shelf+%F0%9F%93%96;Glassmorphic+dashboard+%E2%9C%A8;Automatic+fine+tracking+%E2%8F%B1;Secure+role-based+access+%F0%9F%94%90" alt="features" />

</div>

---

## ✨ Highlights

- **🎨 Realistic 3D UI** — books render as true 3D objects (cover, spine, page edges) that tilt and lift on hover, over an atmospheric library backdrop with glassmorphic panels.
- **⚙️ C++ REST backend** — a single-file C++17 server (`cpp-httplib` + `nlohmann/json`) with O(1) lookups and JSON file storage — *no database to install.*
- **🔐 Role-based auth** — token-based login for **Admin** (full control) and **Member** (browse + own loans), enforced on the server.
- **🔄 Full circulation** — issue / return with a 14-day loan period and **automatic overdue fine** calculation.
- **🔎 Smart catalogue** — search-as-you-type plus category & availability filters.
- **📊 Live dashboard & 📁 CSV export** — real-time stats and one-click circulation reports.

---

## 🗂 Project Structure

```
library-system/
├── backend/              # C++17 REST API
│   ├── server.cpp        # all endpoints, auth, fines, CSV  ← the engine
│   ├── httplib.h         # HTTP library (vendored)
│   └── json.hpp          # JSON library (vendored)
├── frontend/             # React + Vite (3D UI)
│   └── src/
│       ├── App.jsx       # all views & components
│       └── styles.css    # 3D / glassmorphism theme
├── docs/                 # 📄 project documentation (report)
├── presentation/         # 📊 slides (PPT)
└── README.md
```
> 📌 Full design, testing and analysis are in the **documentation** and **presentation** files in this repo.

---

## 🚀 Quick Start

> Needs **GCC 9+** (MinGW-w64 UCRT on Windows) and **Node.js 18+**. Run the two parts in separate terminals.

**1️⃣ Backend** — `http://localhost:8080`
```bash
cd backend
g++ -std=c++17 -O2 server.cpp -o server.exe -lws2_32   # Windows
# Linux/macOS:  g++ -std=c++17 -O2 -pthread server.cpp -o server
./server.exe
```

**2️⃣ Frontend** — `http://localhost:5173`
```bash
cd frontend
npm install
npm run dev
```

---

## 🔑 Demo Logins

| Role   | Email               | Password   |
|--------|---------------------|------------|
| 👑 Admin  | `admin@library.com` | `admin123` |
| 👤 Member | `bhanu@library.com` | `pass123`  |

---

## 🔌 API at a Glance

| Method | Endpoint | Access | Purpose |
|:------:|----------|:------:|---------|
| `POST` | `/api/login` · `/api/register` | Public | Auth |
| `GET`  | `/api/books` | Public | List / search / filter |
| `POST` `PUT` `DELETE` | `/api/books` | Admin | Manage catalogue |
| `POST` | `/api/loans` · `/api/loans/:id/return` | Admin | Issue / return |
| `GET`  | `/api/loans` | Auth | Loans (own for members) |
| `GET`  | `/api/stats` · `/api/report/loans.csv` | Admin | Dashboard & report |

---

## 🛠 Tech Stack

**Backend:** C++17 · cpp-httplib · nlohmann/json · JSON file persistence
**Frontend:** React 18 · Vite · pure-CSS 3D (perspective transforms, glassmorphism)

<div align="center">
<br/>
<sub>Built as an internship project • <b>Athenaeum</b> 📖</sub>
</div>
