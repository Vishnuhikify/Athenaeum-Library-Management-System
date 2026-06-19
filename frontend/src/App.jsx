import { useState, useEffect, useCallback } from 'react'

const API = 'http://localhost:8080/api'

/* ---------------- API helper ---------------- */
function useApi(token) {
  return useCallback(async (path, opts = {}) => {
    const res = await fetch(API + path, {
      ...opts,
      headers: {
        'Content-Type': 'application/json',
        ...(token ? { Authorization: 'Bearer ' + token } : {}),
        ...(opts.headers || {}),
      },
    })
    const text = await res.text()
    const data = text ? JSON.parse(text) : null
    if (!res.ok) throw new Error(data?.error || 'Request failed')
    return data
  }, [token])
}

/* ---------------- Brand ---------------- */
function Brand() {
  return (
    <div className="brand">
      <div className="brand-mark">A</div>
      <div className="brand-name">Athenaeum</div>
    </div>
  )
}

/* ---------------- Auth ---------------- */
function Auth({ onLogin }) {
  const [mode, setMode] = useState('login')
  const [form, setForm] = useState({ name: '', email: '', password: '' })
  const [err, setErr] = useState('')
  const [busy, setBusy] = useState(false)
  const api = useApi(null)
  const set = (k) => (e) => setForm({ ...form, [k]: e.target.value })

  const submit = async () => {
    setErr(''); setBusy(true)
    try {
      if (mode === 'register') {
        await api('/register', { method: 'POST', body: JSON.stringify(form) })
      }
      const { token, user } = await api('/login', {
        method: 'POST',
        body: JSON.stringify({ email: form.email, password: form.password }),
      })
      onLogin(token, user)
    } catch (e) { setErr(e.message) }
    setBusy(false)
  }

  return (
    <div className="auth-wrap">
      <div className="auth-card">
        <Brand />
        <p className="auth-sub">
          {mode === 'login' ? 'Sign in to the library catalogue.' : 'Create a member account.'}
        </p>
        {err && <div className="err-msg">{err}</div>}
        {mode === 'register' && (
          <div className="field">
            <label>Full Name</label>
            <input value={form.name} onChange={set('name')} placeholder="Jane Doe" />
          </div>
        )}
        <div className="field">
          <label>Email</label>
          <input value={form.email} onChange={set('email')} placeholder="you@library.com"
                 onKeyDown={(e) => e.key === 'Enter' && submit()} />
        </div>
        <div className="field">
          <label>Password</label>
          <input type="password" value={form.password} onChange={set('password')} placeholder="••••••••"
                 onKeyDown={(e) => e.key === 'Enter' && submit()} />
        </div>
        <button className="btn btn-primary btn-block" onClick={submit} disabled={busy}>
          {busy ? 'Please wait…' : mode === 'login' ? 'Sign In' : 'Create Account'}
        </button>
        <div className="auth-toggle">
          {mode === 'login' ? "No account? " : 'Already a member? '}
          <button onClick={() => { setMode(mode === 'login' ? 'register' : 'login'); setErr('') }}>
            {mode === 'login' ? 'Register' : 'Sign in'}
          </button>
        </div>
        {mode === 'login' && (
          <div className="hint">
            <b>Demo logins</b><br />
            Admin — admin@library.com / admin123<br />
            Member — bhanu@library.com / pass123
          </div>
        )}
      </div>
    </div>
  )
}

/* ---------------- Reusable bits ---------------- */
function Stat({ label, value, foot, accent, warn }) {
  return (
    <div className={`stat-card ${accent ? 'accent' : ''} ${warn ? 'warn' : ''}`}>
      <div className="label">{label}</div>
      <div className="value">{value}</div>
      {foot && <div className="foot">{foot}</div>}
    </div>
  )
}

function Modal({ title, sub, children, onClose }) {
  return (
    <div className="overlay" onClick={onClose}>
      <div className="modal" onClick={(e) => e.stopPropagation()}>
        <h2>{title}</h2>
        {sub && <p className="sub">{sub}</p>}
        {children}
      </div>
    </div>
  )
}

/* ---------------- Dashboard ---------------- */
function Dashboard({ token, toast }) {
  const api = useApi(token)
  const [s, setS] = useState(null)
  useEffect(() => { api('/stats').then(setS).catch((e) => toast(e.message, true)) }, [api])
  if (!s) return <div className="empty">Loading…</div>
  return (
    <>
      <div className="page-head"><h1>Dashboard</h1><p>Library at a glance.</p></div>
      <div className="stat-grid stagger">
        <Stat label="Titles" value={s.titles} foot={`${s.totalCopies} total copies`} />
        <Stat label="Available" value={s.available} foot="ready to borrow" />
        <Stat label="On Loan" value={s.onLoan} accent />
        <Stat label="Members" value={s.members} />
        <Stat label="Active Loans" value={s.activeLoans} foot={`${s.overdue} overdue`} />
        <Stat label="Outstanding Fines" value={'₹' + s.totalFines.toFixed(0)} warn={s.totalFines > 0} />
      </div>
    </>
  )
}

/* ---------------- Books ---------------- */
function Books({ token, user, toast }) {
  const api = useApi(token)
  const isAdmin = user.role === 'admin'
  const [books, setBooks] = useState([])
  const [cats, setCats] = useState([])
  const [q, setQ] = useState('')
  const [cat, setCat] = useState('')
  const [onlyAvail, setOnlyAvail] = useState(false)
  const [modal, setModal] = useState(null) // null | {} (new) | book (edit)

  const load = useCallback(async () => {
    const params = new URLSearchParams()
    if (q) params.set('q', q)
    if (cat) params.set('category', cat)
    if (onlyAvail) params.set('available', 'true')
    setBooks(await api('/books?' + params))
  }, [api, q, cat, onlyAvail])

  useEffect(() => { api('/categories').then(setCats) }, [api])
  useEffect(() => { const t = setTimeout(load, 180); return () => clearTimeout(t) }, [load])

  const save = async (b) => {
    try {
      if (b.id) await api('/books/' + b.id, { method: 'PUT', body: JSON.stringify(b) })
      else await api('/books', { method: 'POST', body: JSON.stringify(b) })
      setModal(null); toast(b.id ? 'Book updated' : 'Book added'); load()
      api('/categories').then(setCats)
    } catch (e) { toast(e.message, true) }
  }
  const del = async (id) => {
    if (!confirm('Delete this book?')) return
    try { await api('/books/' + id, { method: 'DELETE' }); toast('Book deleted'); load() }
    catch (e) { toast(e.message, true) }
  }

  return (
    <>
      <div className="page-head head-row">
        <div><h1>Catalogue</h1><p>{books.length} titles shown</p></div>
        {isAdmin && <button className="btn btn-primary" onClick={() => setModal({})}>＋ Add Book</button>}
      </div>
      <div className="toolbar">
        <div className="search-box">
          <span>🔍</span>
          <input placeholder="Search title or author…" value={q} onChange={(e) => setQ(e.target.value)} />
        </div>
        <select className="select" value={cat} onChange={(e) => setCat(e.target.value)}>
          <option value="">All categories</option>
          {cats.map((c) => <option key={c} value={c}>{c}</option>)}
        </select>
        <select className="select" value={onlyAvail ? '1' : ''} onChange={(e) => setOnlyAvail(!!e.target.value)}>
          <option value="">Any status</option>
          <option value="1">Available only</option>
        </select>
      </div>
      <div className="panel">
        {books.length === 0 ? <div className="empty"><div className="big">📚</div>No books match.</div> : (
          <table>
            <thead><tr>
              <th>Title</th><th>Category</th><th>ISBN</th><th>Availability</th>{isAdmin && <th></th>}
            </tr></thead>
            <tbody>
              {books.map((b) => (
                <tr key={b.id}>
                  <td><div className="t-title spine">{b.title}</div><div className="t-sub">{b.author}</div></td>
                  <td>{b.category}</td>
                  <td className="mono t-sub">{b.isbn || '—'}</td>
                  <td>
                    <span className={`badge ${b.available > 0 ? 'green' : 'red'}`}>
                      <span className="dot" />{b.available}/{b.total}
                    </span>
                  </td>
                  {isAdmin && <td style={{ textAlign: 'right', whiteSpace: 'nowrap' }}>
                    <button className="btn btn-ghost btn-sm" onClick={() => setModal(b)}>Edit</button>{' '}
                    <button className="btn btn-danger btn-sm" onClick={() => del(b.id)}>Delete</button>
                  </td>}
                </tr>
              ))}
            </tbody>
          </table>
        )}
      </div>
      {modal && <BookModal book={modal} onClose={() => setModal(null)} onSave={save} />}
    </>
  )
}

function BookModal({ book, onClose, onSave }) {
  const [f, setF] = useState({
    title: book.title || '', author: book.author || '', isbn: book.isbn || '',
    category: book.category || '', total: book.total || 1, id: book.id,
  })
  const set = (k) => (e) => setF({ ...f, [k]: e.target.value })
  return (
    <Modal title={book.id ? 'Edit Book' : 'Add Book'} sub="Catalogue entry details." onClose={onClose}>
      <div className="field"><label>Title</label><input value={f.title} onChange={set('title')} /></div>
      <div className="field"><label>Author</label><input value={f.author} onChange={set('author')} /></div>
      <div className="grid-2">
        <div className="field"><label>ISBN</label><input value={f.isbn} onChange={set('isbn')} /></div>
        <div className="field"><label>Category</label><input value={f.category} onChange={set('category')} /></div>
      </div>
      <div className="field"><label>Total Copies</label>
        <input type="number" min="1" value={f.total} onChange={(e) => setF({ ...f, total: parseInt(e.target.value) || 1 })} />
      </div>
      <div className="modal-actions">
        <button className="btn btn-ghost" onClick={onClose}>Cancel</button>
        <button className="btn btn-primary" onClick={() => onSave(f)}>Save</button>
      </div>
    </Modal>
  )
}

/* ---------------- Members (admin) ---------------- */
function Members({ token, toast }) {
  const api = useApi(token)
  const [members, setMembers] = useState([])
  useEffect(() => { api('/members').then(setMembers).catch((e) => toast(e.message, true)) }, [api])
  return (
    <>
      <div className="page-head"><h1>Members</h1><p>{members.length} registered</p></div>
      <div className="panel">
        <table>
          <thead><tr><th>ID</th><th>Name</th><th>Email</th><th>Role</th></tr></thead>
          <tbody>
            {members.map((m) => (
              <tr key={m.id}>
                <td className="mono">#{m.id}</td>
                <td className="t-title">{m.name}</td>
                <td className="t-sub">{m.email}</td>
                <td><span className={`role-chip ${m.role === 'member' ? 'member' : ''}`}>{m.role}</span></td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </>
  )
}

/* ---------------- Loans ---------------- */
function Loans({ token, user, toast }) {
  const api = useApi(token)
  const isAdmin = user.role === 'admin'
  const [loans, setLoans] = useState([])
  const [filter, setFilter] = useState('active')
  const [issuing, setIssuing] = useState(false)
  const [books, setBooks] = useState([])
  const [members, setMembers] = useState([])

  const load = useCallback(async () => {
    const p = filter === 'all' ? '' : '?status=' + filter
    setLoans(await api('/loans' + p))
  }, [api, filter])
  useEffect(() => { load() }, [load])

  const openIssue = async () => {
    try {
      const [b, m] = await Promise.all([api('/books?available=true'), api('/members')])
      setBooks(b); setMembers(m.filter((x) => x.role === 'member')); setIssuing(true)
    } catch (e) { toast(e.message, true) }
  }
  const issue = async (bookId, memberId) => {
    try {
      await api('/loans', { method: 'POST', body: JSON.stringify({ bookId: +bookId, memberId: +memberId }) })
      setIssuing(false); toast('Book issued'); load()
    } catch (e) { toast(e.message, true) }
  }
  const ret = async (id) => {
    try { const r = await api(`/loans/${id}/return`, { method: 'POST' })
      toast(r.fine > 0 ? `Returned · fine ₹${r.fine.toFixed(0)}` : 'Returned on time'); load()
    } catch (e) { toast(e.message, true) }
  }

  const downloadCsv = async () => {
    const res = await fetch(API + '/report/loans.csv', { headers: { Authorization: 'Bearer ' + token } })
    const blob = await res.blob()
    const url = URL.createObjectURL(blob)
    const a = document.createElement('a'); a.href = url; a.download = 'loans_report.csv'; a.click()
    URL.revokeObjectURL(url); toast('Report downloaded')
  }

  return (
    <>
      <div className="page-head head-row">
        <div><h1>{isAdmin ? 'Circulation' : 'My Loans'}</h1><p>{loans.length} records</p></div>
        <div style={{ display: 'flex', gap: 10 }}>
          {isAdmin && <button className="btn btn-ghost" onClick={downloadCsv}>⬇ Export CSV</button>}
          {isAdmin && <button className="btn btn-primary" onClick={openIssue}>＋ Issue Book</button>}
        </div>
      </div>
      <div className="toolbar">
        <select className="select" value={filter} onChange={(e) => setFilter(e.target.value)}>
          <option value="active">Active</option>
          <option value="returned">Returned</option>
          <option value="all">All</option>
        </select>
      </div>
      <div className="panel">
        {loans.length === 0 ? <div className="empty"><div className="big">📖</div>No loans here.</div> : (
          <table>
            <thead><tr>
              <th>Book</th>{isAdmin && <th>Member</th>}<th>Issued</th><th>Due</th><th>Status</th><th>Fine</th>{isAdmin && <th></th>}
            </tr></thead>
            <tbody>
              {loans.map((l) => (
                <tr key={l.id}>
                  <td className="t-title">{l.bookTitle || '#' + l.bookId}</td>
                  {isAdmin && <td>{l.memberName || '#' + l.memberId}</td>}
                  <td className="mono t-sub">{l.issuedDate}</td>
                  <td className="mono t-sub">{l.dueDate}</td>
                  <td>
                    {l.returnedAt
                      ? <span className="badge green"><span className="dot" />Returned</span>
                      : l.overdue
                        ? <span className="badge red"><span className="dot" />Overdue</span>
                        : <span className="badge amber"><span className="dot" />On loan</span>}
                  </td>
                  <td className="mono">{(l.liveFine || l.fine) > 0 ? '₹' + (l.liveFine || l.fine).toFixed(0) : '—'}</td>
                  {isAdmin && <td style={{ textAlign: 'right' }}>
                    {!l.returnedAt && <button className="btn btn-ghost btn-sm" onClick={() => ret(l.id)}>Return</button>}
                  </td>}
                </tr>
              ))}
            </tbody>
          </table>
        )}
      </div>
      {issuing && <IssueModal books={books} members={members} onClose={() => setIssuing(false)} onIssue={issue} />}
    </>
  )
}

function IssueModal({ books, members, onClose, onIssue }) {
  const [bookId, setBookId] = useState(books[0]?.id || '')
  const [memberId, setMemberId] = useState(members[0]?.id || '')
  return (
    <Modal title="Issue Book" sub="Loan period is 14 days; fines accrue daily after." onClose={onClose}>
      <div className="field"><label>Book</label>
        <select className="select" style={{ width: '100%' }} value={bookId} onChange={(e) => setBookId(e.target.value)}>
          {books.length === 0 && <option value="">No available books</option>}
          {books.map((b) => <option key={b.id} value={b.id}>{b.title} ({b.available} left)</option>)}
        </select>
      </div>
      <div className="field"><label>Member</label>
        <select className="select" style={{ width: '100%' }} value={memberId} onChange={(e) => setMemberId(e.target.value)}>
          {members.map((m) => <option key={m.id} value={m.id}>{m.name} — {m.email}</option>)}
        </select>
      </div>
      <div className="modal-actions">
        <button className="btn btn-ghost" onClick={onClose}>Cancel</button>
        <button className="btn btn-primary" disabled={!bookId || !memberId} onClick={() => onIssue(bookId, memberId)}>Issue</button>
      </div>
    </Modal>
  )
}

/* ---------------- App shell ---------------- */
export default function App() {
  const [auth, setAuth] = useState(null) // {token, user}
  const [view, setView] = useState('dashboard')
  const [toast, setToast] = useState(null)

  const showToast = (msg, err) => { setToast({ msg, err }); setTimeout(() => setToast(null), 2800) }

  if (!auth) return <Auth onLogin={(token, user) => { setAuth({ token, user }); setView(user.role === 'admin' ? 'dashboard' : 'books') }} />

  const { token, user } = auth
  const isAdmin = user.role === 'admin'
  const nav = isAdmin
    ? [['dashboard', '◷', 'Dashboard'], ['books', '📚', 'Catalogue'], ['members', '👤', 'Members'], ['loans', '🔄', 'Circulation']]
    : [['books', '📚', 'Catalogue'], ['loans', '🔄', 'My Loans']]

  return (
    <div className="shell">
      <aside className="sidebar">
        <Brand />
        {nav.map(([id, ico, label]) => (
          <button key={id} className={`nav-item ${view === id ? 'active' : ''}`} onClick={() => setView(id)}>
            <span className="nav-ico">{ico}</span><span>{label}</span>
          </button>
        ))}
        <div className="nav-spacer" />
        <div className="nav-user">
          <div className="who">
            <b>{user.name}</b><span>{user.email}</span><br />
            <span className={`role-chip ${isAdmin ? '' : 'member'}`}>{user.role}</span>
          </div>
          <button className="nav-item" onClick={() => setAuth(null)}>
            <span className="nav-ico">⎋</span><span>Sign out</span>
          </button>
        </div>
      </aside>
      <main className="main">
        {view === 'dashboard' && isAdmin && <Dashboard token={token} toast={showToast} />}
        {view === 'books' && <Books token={token} user={user} toast={showToast} />}
        {view === 'members' && isAdmin && <Members token={token} toast={showToast} />}
        {view === 'loans' && <Loans token={token} user={user} toast={showToast} />}
      </main>
      {toast && <div className={`toast ${toast.err ? 'err' : ''}`}>{toast.err ? '✕' : '✓'} {toast.msg}</div>}
    </div>
  )
}
