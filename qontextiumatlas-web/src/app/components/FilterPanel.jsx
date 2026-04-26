'use client';

import { useState } from 'react';
import styles from './FilterPanel.module.css';

/* INT filters - sent as "key=lo:hi", "key=lo:+" , "key=-:hi", or "key=-:+" */
const INT_FILTERS = [
  { key: 'ctx_degree',           label: 'Contextuality Degree' },
  { key: 'ctx_count',            label: 'Context Count' },
  { key: 'neg_ctx_count',        label: 'Negative Contexts' },
  { key: 'qubits_count',         label: 'Qubit Count' },
  { key: 'best_hamming_distance',label: 'Hamming Distance' },
  { key: 'dimension',            label: 'Dimension' },
  { key: 'observable_count',     label: 'Observable Count' },
];

const EMPTY = { lo: '', hi: '' };

function buildIntFilter(key, lo, hi) {
  const loVal = lo.trim() === '' ? '-' : lo.trim();
  const hiVal = hi.trim() === '' ? '+' : hi.trim();
  // Skip if fully unbounded
  if (loVal === '-' && hiVal === '+') return null;
  if (loVal === hiVal && loVal !== '-' && loVal !== '+') return `${key}=${loVal}`;
  return `${key}=${loVal}:${hiVal}`;
}

function IntFilter({ filterKey, label, values, onChange }) {
  const { lo, hi } = values;
  const active = lo.trim() !== '' || hi.trim() !== '';

  return (
    <div className={styles.intFilter}>
      <div className={styles.filterRow}>
        <span className={`${styles.filterLabel} ${active ? styles.filterLabelActive : ''}`}>
          {label}
        </span>
      </div>
      <div className={styles.inputPair}>
        <div className={styles.inputWrap}>
          <span className={styles.inputHint}>min</span>
          <input
            type="number"
            className={styles.numInput}
            placeholder="any"
            value={lo}
            min={0}
            onChange={e => onChange(filterKey, { lo: e.target.value, hi })}
          />
        </div>
        <span className={styles.inputSep}>–</span>
        <div className={styles.inputWrap}>
          <span className={styles.inputHint}>max</span>
          <input
            type="number"
            className={styles.numInput}
            placeholder="any"
            value={hi}
            min={0}
            onChange={e => onChange(filterKey, { lo, hi: e.target.value })}
          />
        </div>
      </div>
    </div>
  );
}

const DEFAULTS = Object.fromEntries(INT_FILTERS.map(f => [f.key, EMPTY]));

export default function FilterPanel({ onSearch, loading }) {
  const [intValues, setIntValues] = useState(DEFAULTS);
  const [author, setAuthor] = useState('');
  const [showConf, setShowConf] = useState(true);

  const handleInt = (key, val) => setIntValues(v => ({ ...v, [key]: val }));

  const activeCount = [
    ...INT_FILTERS.filter(f => intValues[f.key].lo || intValues[f.key].hi),
    ...(author.trim() ? ['author'] : []),
  ].length;

  const buildFilters = () => {
    const filters = [];
    for (const { key } of INT_FILTERS) {
      const { lo, hi } = intValues[key];
      const f = buildIntFilter(key, lo, hi);
      if (f) filters.push(f);
    }
    if (author.trim()) filters.push(`author_name=${author.trim()}`);
    return filters;
  };

  const handleSubmit = () => {
    onSearch({ filters: buildFilters(), show_conf: showConf });
  };

  const handleReset = () => {
    setIntValues(DEFAULTS);
    setAuthor('');
    setShowConf(true);
  };

  return (
    <aside className={styles.panel}>
      <div className={styles.panelHeader}>
        <div className={styles.panelTitleRow}>
          <span className={styles.panelTitle}>Filters</span>
          {activeCount > 0 && (
            <span className={styles.activeBadge}>{activeCount}</span>
          )}
        </div>
        <button className={styles.resetBtn} onClick={handleReset}>Reset</button>
      </div>

      <div className={styles.filters}>
        {INT_FILTERS.map(f => (
          <IntFilter
            key={f.key}
            filterKey={f.key}
            label={f.label}
            values={intValues[f.key]}
            onChange={handleInt}
          />
        ))}

        <div className={styles.divider} />

        <div className={styles.strFilter}>
          <div className={styles.filterRow}>
            <span className={`${styles.filterLabel} ${author ? styles.filterLabelActive : ''}`}>
              Author name
            </span>
          </div>
          <input
            type="text"
            placeholder="exact name…"
            value={author}
            onChange={e => setAuthor(e.target.value)}
            onKeyDown={e => e.key === 'Enter' && handleSubmit()}
            className={styles.strInput}
          />
          <span className={styles.strHint}>
            Case-insensitive
          </span>
        </div>

        <div className={styles.divider} />

        <div className={styles.toggleRow}>
          <span className={styles.filterLabel}>Show configuration</span>
          <div
            className={`${styles.toggle} ${showConf ? styles.toggleOn : ''}`}
            onClick={() => setShowConf(v => !v)}
            role="switch"
            aria-checked={showConf}
          >
            <div className={styles.toggleKnob} />
          </div>
        </div>
      </div>

      <button
        className={`${styles.searchBtn} ${loading ? styles.searchBtnLoading : ''}`}
        onClick={handleSubmit}
        disabled={loading}
      >
        {loading
          ? <><span className={styles.spinner} />Searching…</>
          : <>
              <svg width="12" height="12" viewBox="0 0 16 16" fill="none" stroke="currentColor" strokeWidth="2.2" strokeLinecap="round">
                <circle cx="6.5" cy="6.5" r="4.5"/><line x1="10" y1="10" x2="14" y2="14"/>
              </svg>
              Search
            </>
        }
      </button>
    </aside>
  );
}
