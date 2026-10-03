(() => {
  const themeToggle = document.querySelector('[data-theme-toggle]');
  const themeColor = document.querySelector('[data-theme-color]');
  const systemTheme = window.matchMedia('(prefers-color-scheme: dark)');

  const storedTheme = () => {
    try {
      const value = localStorage.getItem('driversentinel-theme');
      return value === 'light' || value === 'dark' ? value : null;
    } catch (_) {
      return null;
    }
  };

  const applyTheme = (theme) => {
    document.documentElement.dataset.theme = theme;
    document.documentElement.style.colorScheme = theme;
    if (themeColor) themeColor.setAttribute('content', theme === 'light' ? '#f5f8fa' : '#071018');
    if (themeToggle) {
      const next = theme === 'dark' ? 'light' : 'dark';
      const label = `Switch to ${next} theme`;
      themeToggle.setAttribute('aria-label', label);
      themeToggle.setAttribute('title', label);
    }
  };

  applyTheme(document.documentElement.dataset.theme || (systemTheme.matches ? 'dark' : 'light'));

  themeToggle?.addEventListener('click', () => {
    const next = document.documentElement.dataset.theme === 'dark' ? 'light' : 'dark';
    try { localStorage.setItem('driversentinel-theme', next); } catch (_) {}
    applyTheme(next);
  });

  const syncSystemTheme = (event) => {
    if (!storedTheme()) applyTheme(event.matches ? 'dark' : 'light');
  };

  if (systemTheme.addEventListener) systemTheme.addEventListener('change', syncSystemTheme);
  else if (systemTheme.addListener) systemTheme.addListener(syncSystemTheme);

  const toggle = document.querySelector('[data-nav-toggle]');
  const nav = document.querySelector('[data-nav]');
  const header = document.querySelector('[data-header]');

  if (toggle && nav) {
    toggle.addEventListener('click', () => {
      const open = toggle.getAttribute('aria-expanded') === 'true';
      toggle.setAttribute('aria-expanded', String(!open));
      nav.classList.toggle('is-open', !open);
      document.body.classList.toggle('nav-open', !open);
    });

    nav.querySelectorAll('a').forEach((link) => {
      link.addEventListener('click', () => {
        toggle.setAttribute('aria-expanded', 'false');
        nav.classList.remove('is-open');
        document.body.classList.remove('nav-open');
      });
    });
  }

  const syncHeader = () => header?.classList.toggle('is-scrolled', window.scrollY > 12);
  syncHeader();
  window.addEventListener('scroll', syncHeader, { passive: true });
})();
