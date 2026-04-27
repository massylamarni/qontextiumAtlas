import Nav from '../components/Nav';
import styles from './about.module.css';

export const metadata = { title: 'About - QATLAS' };

export default function About() {
  return (
    <div className={styles.layout}>
      <Nav />
      <main className={styles.main}>
        <div className={styles.hero}>
          <div className={styles.eyebrow}>About</div>
          <h1 className={styles.title}>QATLAS</h1>
          <p className={styles.sub}>
            QATLAS is a searchable database of quantum configurations classified by their
            contextuality degree - a measure of how strongly a quantum system violates
            classical hidden-variable models. Built at FEMTO-ST, Université de Marie et Louis Pasteur.
          </p>
        </div>

        <div className={styles.divider} />

        {/* <div className={styles.cardGrid}>
          <div className={styles.card}>
            <div className={styles.cardIcon}>
              <svg viewBox="0 0 16 16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                <circle cx="8" cy="8" r="5"/><line x1="8" y1="3" x2="8" y2="8"/><line x1="8" y1="8" x2="11" y2="11"/>
              </svg>
            </div>
            <div className={styles.cardTitle}>SAT-based solving</div>
            <div className={styles.cardDesc}>
              Exact contextuality degree via Boolean circuit SAT (BCpackage-0.40),
              with a heuristic fallback for large configurations.
            </div>
          </div>

          <div className={styles.card}>
            <div className={styles.cardIcon}>
              <svg viewBox="0 0 16 16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                <rect x="2" y="2" width="5" height="5" rx="1"/><rect x="9" y="2" width="5" height="5" rx="1"/>
                <rect x="2" y="9" width="5" height="5" rx="1"/><rect x="9" y="9" width="5" height="5" rx="1"/>
              </svg>
            </div>
            <div className={styles.cardTitle}>Pauli operator configs</div>
            <div className={styles.cardDesc}>
              Configurations encoded as Pauli operator contexts — XY, ZX, YZ —
              stored and visualised as colour-coded grids with negativity indicators.
            </div>
          </div>

          <div className={styles.card}>
            <div className={styles.cardIcon}>
              <svg viewBox="0 0 16 16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round">
                <polyline points="2,12 5,7 8,9 11,4 14,6"/>
              </svg>
            </div>
            <div className={styles.cardTitle}>Multi-qubit support</div>
            <div className={styles.cardDesc}>
              From single-qubit up to n-qubit systems. Filter by qubit count,
              context count, Hamming distance, and more.
            </div>
          </div>

          <div className={styles.card}>
            <div className={styles.cardIcon}>
              <svg viewBox="0 0 16 16" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                <ellipse cx="8" cy="8" rx="6" ry="3"/>
                <line x1="8" y1="5" x2="8" y2="11"/>
                <path d="M2 8 Q8 12 14 8"/>
              </svg>
            </div>
            <div className={styles.cardTitle}>REST API</div>
            <div className={styles.cardDesc}>
              Local C HTTP server exposes <code>POST /run</code> and <code>POST /get</code> —
              drop your config file in, get results back as JSON.
            </div>
          </div>
        </div> */}

        <div className={styles.actions}>
          <a className={styles.btnPrimary} href="#" target="_blank" rel="noopener noreferrer">
            Read the paper
          </a>
          <a className={styles.btnGhost} href="https://github.com/massylamarni/qontextiumAtlas" target="_blank" rel="noopener noreferrer">
            View on GitHub
          </a>
        </div>
      </main>
    </div>
  );
}
