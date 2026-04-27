'use client';

import { useMemo } from 'react';
import styles from './DetailView.module.css';

function parseConf(confStr) {
  if (!confStr) return null;
  return confStr
    .trim()
    .split('\n')
    .filter(Boolean)
    .map(row => row.split(',').map(s => s.trim()));
}

function pauliClass(label) {
  if (!label || label === 'II' || label === 'I') return styles.pII;
  const unique = [...new Set(label.split(''))].filter(c => c !== 'I');
  if (unique.length === 1) {
    return { X: styles.pX, Y: styles.pY, Z: styles.pZ }[unique[0]] || styles.pII;
  }
  return styles.pMix;
}

function PauliCell({ label }) {
  return <div className={`${styles.pauliCell} ${pauliClass(label)}`}>{label}</div>;
}

function ConfGrid({ confStr }) {
  const grid = useMemo(() => parseConf(confStr), [confStr]);
  if (!grid) return <span className={styles.noConf}>No configuration data</span>;

  return (
    <div className={styles.gridOuter}>
      <div className={styles.confLegend}>
        <span className={styles.legendItem}><span className={`${styles.legendDot} ${styles.pX}`} />X</span>
        <span className={styles.legendItem}><span className={`${styles.legendDot} ${styles.pY}`} />Y</span>
        <span className={styles.legendItem}><span className={`${styles.legendDot} ${styles.pZ}`} />Z</span>
        <span className={styles.legendItem}><span className={`${styles.legendDot} ${styles.pMix}`} />mix</span>
      </div>
      <div className={styles.pauliGrid}>
        {grid.map((row, i) => (
          <div key={i}>
            {i > 0 && i % 2 === 0 && <div className={styles.rowDivider} />}
            <div className={styles.pauliRow}>
              <span className={styles.ctxLabel}>c{i + 1}</span>
              {row.map((cell, j) => <PauliCell key={j} label={cell} />)}
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}

const STAT_FIELDS = [
  { key: 'ctx_count',             label: 'Contexts' },
  { key: 'neg_ctx_count',         label: 'Neg. Ctx' },
  { key: 'qubits_count',          label: 'Qubits',      transform: v => v === -1 ? '-' : v },
  { key: 'best_hamming_distance', label: 'Hamming' },
  { key: 'observable_count',      label: 'Observables', transform: v => v === 0 ? '-' : v },
  { key: 'dimension',             label: 'Dimension',   transform: v => v === 0 ? '-' : v },
  { key: 'format',                label: 'Format',      span: true },
  { key: 'author_name',           label: 'Author',      span: true, accent: true, transform: v => v || '-' },
];

export default function DetailView({ result, onClose }) {
  if (!result) {
    return (
      <div className={styles.placeholder}>
        <div className={styles.placeholderIcon}>◈</div>
        <div className={styles.placeholderText}>Select a result to inspect</div>
      </div>
    );
  }

  return (
    <div className={styles.panel}>
      <div className={styles.panelHeader}>
        <span className={styles.panelTitle}>Detail</span>
        <button className={styles.closeBtn} onClick={onClose} title="Close">✕</button>
      </div>

      <div className={styles.panelBody}>
        <div className={styles.statHero}>
          <div className={styles.statHeroLabel}>Contextuality Degree</div>
          <div className={styles.statHeroVal}>{result.ctx_degree ?? '-'}</div>
          <div className={styles.statHeroSub}>
            {result.ctx_degree > 0 ? 'above critical threshold' : 'non-contextual'}
          </div>
        </div>

        <div className={styles.statsGrid}>
          {STAT_FIELDS.map(({ key, label, transform, span, accent }) => {
            const raw = result[key];
            const val = transform ? transform(raw) : (raw ?? '-');
            return (
              <div
                key={key}
                className={`${styles.statCard} ${span ? styles.statCardSpan : ''}`}
              >
                <div className={styles.statLabel}>{label}</div>
                <div className={`${styles.statVal} ${accent ? styles.statValAccent : ''}`}>{val}</div>
              </div>
            );
          })}
        </div>

        <div className={styles.confSection}>
          <div className={styles.sectionLabel}>Configuration</div>
          {result.ctx_conf
            ? <ConfGrid confStr={result.ctx_conf} />
            : <div className={styles.noConfHint}>Enable <em>Show configuration</em> in filters to see the Pauli grid.</div>
          }
        </div>
      </div>
    </div>
  );
}
