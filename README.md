<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 360" width="1200" height="360" font-family="Georgia, 'Times New Roman', serif">
  <defs>
    <!-- Atmospheric background -->
    <radialGradient id="atmos" cx="20%" cy="15%" r="90%">
      <stop offset="0%" stop-color="#1c2540"/>
      <stop offset="55%" stop-color="#111729"/>
      <stop offset="100%" stop-color="#0a0e1a"/>
    </radialGradient>
    <radialGradient id="glowBrass" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#d4a857" stop-opacity="0.55"/>
      <stop offset="100%" stop-color="#d4a857" stop-opacity="0"/>
    </radialGradient>
    <radialGradient id="glowEmerald" cx="50%" cy="50%" r="50%">
      <stop offset="0%" stop-color="#3fbf8f" stop-opacity="0.45"/>
      <stop offset="100%" stop-color="#3fbf8f" stop-opacity="0"/>
    </radialGradient>
    <linearGradient id="brassText" x1="0" y1="0" x2="1" y2="1">
      <stop offset="0%" stop-color="#f3dca0"/>
      <stop offset="50%" stop-color="#d4a857"/>
      <stop offset="100%" stop-color="#b8893a"/>
    </linearGradient>
    <!-- Book cover gradients -->
    <linearGradient id="cv1" x1="0" y1="0" x2="1" y2="1"><stop offset="0%" stop-color="#3a2d5c"/><stop offset="100%" stop-color="#1d1530"/></linearGradient>
    <linearGradient id="cv2" x1="0" y1="0" x2="1" y2="1"><stop offset="0%" stop-color="#1f4d4a"/><stop offset="100%" stop-color="#0f2826"/></linearGradient>
    <linearGradient id="cv3" x1="0" y1="0" x2="1" y2="1"><stop offset="0%" stop-color="#5c3a2d"/><stop offset="100%" stop-color="#301b15"/></linearGradient>
    <linearGradient id="cv4" x1="0" y1="0" x2="1" y2="1"><stop offset="0%" stop-color="#5c2d44"/><stop offset="100%" stop-color="#301523"/></linearGradient>
    <linearGradient id="cvHero" x1="0" y1="0" x2="1" y2="1"><stop offset="0%" stop-color="#2a3658"/><stop offset="100%" stop-color="#161d33"/></linearGradient>
    <linearGradient id="pages" x1="0" y1="0" x2="1" y2="0">
      <stop offset="0%" stop-color="#efe9d8"/><stop offset="100%" stop-color="#cfc7b2"/>
    </linearGradient>
    <filter id="soft" x="-30%" y="-30%" width="160%" height="160%">
      <feGaussianBlur stdDeviation="6"/>
    </filter>
    <style>
      @keyframes floaty { 0%,100% { transform: translateY(0); } 50% { transform: translateY(-12px); } }
      @keyframes floaty2 { 0%,100% { transform: translateY(0); } 50% { transform: translateY(-7px); } }
      @keyframes shimmer { 0%,100% { opacity:.5; } 50% { opacity:1; } }
      @keyframes shadowPulse { 0%,100% { opacity:.55; transform: scaleX(1);} 50% { opacity:.32; transform: scaleX(.82);} }
      @keyframes fadeUp { 0% { opacity:0; transform: translateY(14px);} 100% { opacity:1; transform: none;} }
      @keyframes drift { 0% { transform: translate(0,0);} 100% { transform: translate(26px,16px);} }
      .hero  { transform-box: fill-box; transform-origin: center; animation: floaty 6s ease-in-out infinite; }
      .bk-a  { transform-box: fill-box; transform-origin: center; animation: floaty2 5.5s ease-in-out infinite .3s; }
      .bk-b  { transform-box: fill-box; transform-origin: center; animation: floaty2 6.2s ease-in-out infinite .8s; }
      .bk-c  { transform-box: fill-box; transform-origin: center; animation: floaty2 5.8s ease-in-out infinite 1.1s; }
      .bk-d  { transform-box: fill-box; transform-origin: center; animation: floaty2 6.6s ease-in-out infinite .5s; }
      .hsh   { transform-box: fill-box; transform-origin: center; animation: shadowPulse 6s ease-in-out infinite; }
      .glow1 { animation: shimmer 7s ease-in-out infinite; }
      .glow2 { animation: shimmer 9s ease-in-out infinite 1s; }
      .drift { animation: drift 18s ease-in-out infinite alternate; }
      .t1 { opacity:0; animation: fadeUp .8s ease-out .2s forwards; }
      .t2 { opacity:0; animation: fadeUp .8s ease-out .5s forwards; }
      .t3 { opacity:0; animation: fadeUp .8s ease-out .8s forwards; }
    </style>
  </defs>

  <!-- background -->
  <rect width="1200" height="360" rx="20" fill="url(#atmos)"/>
  <circle class="glow1 drift" cx="180" cy="120" r="240" fill="url(#glowBrass)"/>
  <circle class="glow2 drift" cx="1040" cy="300" r="260" fill="url(#glowEmerald)"/>

  <!-- ===== left: text ===== -->
  <g>
    <text class="t1" x="70" y="120" font-size="30" fill="#7a85a3" letter-spacing="7">THE  DIGITAL  LIBRARY</text>
    <text class="t2" x="66" y="205" font-size="86" font-weight="700" fill="url(#brassText)" letter-spacing="-2">Athenaeum</text>
    <text class="t3" x="70" y="252" font-size="23" fill="#b8c0d4">A 3D Library Management System · C++ &amp; React</text>
    <g class="t3" fill="#b8c0d4" font-family="Arial, sans-serif" font-size="15">
      <rect x="70"  y="280" width="150" height="34" rx="17" fill="#ffffff" fill-opacity="0.06" stroke="#ffffff" stroke-opacity="0.12"/>
      <text x="92"  y="302">📚 3D Catalogue</text>
      <rect x="232" y="280" width="150" height="34" rx="17" fill="#ffffff" fill-opacity="0.06" stroke="#ffffff" stroke-opacity="0.12"/>
      <text x="254" y="302">🔄 Circulation</text>
      <rect x="394" y="280" width="148" height="34" rx="17" fill="#ffffff" fill-opacity="0.06" stroke="#ffffff" stroke-opacity="0.12"/>
      <text x="416" y="302">⏱ Fine Tracking</text>
    </g>
  </g>

  <!-- ===== right: 3D book scene ===== -->
  <!-- background shelf books (smaller, tilted) -->
  <g transform="translate(700,96)">
    <!-- book A -->
    <g class="bk-a" transform="translate(0,44)">
      <polygon points="0,18 26,8 26,150 0,160" fill="url(#pages)" opacity="0.9"/>
      <polygon points="26,8 96,8 96,150 26,150" fill="url(#cv1)" stroke="#ffffff" stroke-opacity="0.1"/>
      <rect x="32" y="20" width="58" height="126" rx="3" fill="none" stroke="#d4a857" stroke-opacity="0.4"/>
    </g>
    <!-- book B -->
    <g class="bk-b" transform="translate(92,30)">
      <polygon points="0,18 26,8 26,150 0,160" fill="url(#pages)" opacity="0.9"/>
      <polygon points="26,8 96,8 96,150 26,150" fill="url(#cv2)" stroke="#ffffff" stroke-opacity="0.1"/>
      <rect x="32" y="20" width="58" height="126" rx="3" fill="none" stroke="#3fbf8f" stroke-opacity="0.4"/>
    </g>
    <!-- book C -->
    <g class="bk-c" transform="translate(286,34)">
      <polygon points="0,18 26,8 26,150 0,160" fill="url(#pages)" opacity="0.9"/>
      <polygon points="26,8 96,8 96,150 26,150" fill="url(#cv3)" stroke="#ffffff" stroke-opacity="0.1"/>
      <rect x="32" y="20" width="58" height="126" rx="3" fill="none" stroke="#d4a857" stroke-opacity="0.4"/>
    </g>
    <!-- book D -->
    <g class="bk-d" transform="translate(378,46)">
      <polygon points="0,18 26,8 26,150 0,160" fill="url(#pages)" opacity="0.9"/>
      <polygon points="26,8 96,8 96,150 26,150" fill="url(#cv4)" stroke="#ffffff" stroke-opacity="0.1"/>
      <rect x="32" y="20" width="58" height="126" rx="3" fill="none" stroke="#e0556b" stroke-opacity="0.4"/>
    </g>
  </g>

  <!-- hero book (front, larger, open-ish 3D) -->
  <g transform="translate(878,62)">
    <ellipse class="hsh" cx="70" cy="250" rx="92" ry="16" fill="#000000" opacity="0.5" filter="url(#soft)"/>
    <g class="hero">
      <!-- page block (right side, gives depth) -->
      <polygon points="118,30 150,46 150,214 118,222" fill="url(#pages)"/>
      <polygon points="118,30 150,46 150,52 118,36" fill="#000000" opacity="0.15"/>
      <!-- spine (left) -->
      <polygon points="22,40 0,54 0,222 22,232" fill="#0d1326"/>
      <!-- front cover -->
      <polygon points="22,40 118,30 118,222 22,232" fill="url(#cvHero)" stroke="#ffffff" stroke-opacity="0.12"/>
      <!-- gilt frame -->
      <polygon points="36,56 106,48 106,206 36,214" fill="none" stroke="#d4a857" stroke-opacity="0.6" stroke-width="1.5"/>
      <!-- title block -->
      <text x="50" y="96" font-size="20" font-weight="700" fill="url(#brassText)">Athen-</text>
      <text x="50" y="120" font-size="20" font-weight="700" fill="url(#brassText)">aeum</text>
      <!-- big A monogram -->
      <text x="60" y="200" font-size="64" font-weight="700" fill="#d4a857" fill-opacity="0.22">A</text>
      <!-- spine glints -->
      <line x1="11" y1="60" x2="11" y2="210" stroke="#d4a857" stroke-opacity="0.3" stroke-width="1.5"/>
    </g>
  </g>
</svg>
