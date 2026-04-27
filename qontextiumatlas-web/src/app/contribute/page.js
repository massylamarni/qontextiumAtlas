import Nav from '../components/Nav';
import styles from './contribute.module.css';

export const metadata = { title: 'Contribute - QATLAS' };

const STEPS = [
  {
    n: 1,
    title: 'Prepare your configuration file',
    desc: 'Format your config as a Pauli assignment matrix or a Gram matrix.',
    pills: ['.ctx', '.grm'],
  },
  {
    n: 2,
    title: 'Run the solver locally',
    desc: 'Compute the contextuality degree before submitting.',
    pills: ['qontextium --import assignment yourfile.ctx'],
  },
  {
    n: 3,
    title: 'Open a pull request',
    desc: 'Fork the repository, add your config and metadata to the database directories, and open a PR with a short description of the configuration\'s origin.',
    pills: ['localdb/ctx/', 'localdb/md/'],
  },
  {
    n: 4,
    title: 'Review & merge',
    desc: 'Maintainers will verify the result with the SAT solver and merge. Your author name will appear in the atlas.',
    pills: [],
  },
];

export default function Contribute() {
  return (
    <div className={styles.layout}>
      <Nav />
      <main className={styles.main}>
        <div className={styles.hero}>
          <div className={styles.eyebrow}>Contribute</div>
          <h1 className={styles.title}>Add your configurations</h1>
          <p className={styles.sub}>
            Submit new quantum configurations to expand the atlas. Contributions are reviewed
            and merged into the shared database, accessible to all users of this instance.
          </p>
        </div>

        <div className={styles.divider} />

        <div className={styles.steps}>
          {STEPS.map(({ n, title, desc, pills }) => (
            <div key={n} className={styles.step}>
              <div className={styles.stepNum}>{n}</div>
              <div className={styles.stepContent}>
                <div className={styles.stepTitle}>{title}</div>
                <div className={styles.stepDesc}>
                  {desc}
                  {pills.length > 0 && (
                    <span className={styles.pills}>
                      {pills.map(p => <code key={p} className={styles.pill}>{p}</code>)}
                    </span>
                  )}
                </div>
              </div>
            </div>
          ))}
        </div>

        <div className={styles.divider} />

        <div className={styles.formatSection}>
          <div className={styles.formatTitle}>Accepted file formats</div>
          <div className={styles.formatGrid}>
            <div className={styles.formatCard}>
              <code className={styles.formatExt}>.ctx</code>
              <div className={styles.formatDesc}>Assignment matrix - rows are contexts, columns are observables</div>
            </div>
            <div className={styles.formatCard}>
              <code className={styles.formatExt}>.grm</code>
              <div className={styles.formatDesc}>Gram matrix - inner product matrix of the quantum states</div>
            </div>
          </div>
        </div>

        <div className={styles.actions}>
          <a className={styles.btnPrimary} href="https://github.com/massylamarni/qontextiumAtlas" target="_blank" rel="noopener noreferrer">
            Fork on GitHub
          </a>
          <a className={styles.btnGhost} href="#" target="_blank" rel="noopener noreferrer">
            Read the contribution guide
          </a>
        </div>
      </main>
    </div>
  );
}
