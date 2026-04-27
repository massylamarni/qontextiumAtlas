'use client';

import styles from './ResultsTable.module.css';

const COLS = [
  { key: 'ctx_degree',          label: 'Ctx Degree' },
  { key: 'ctx_count',           label: 'Contexts' },
  { key: 'neg_ctx_count',       label: 'Neg. Ctx' },
  { key: 'qubits_count',        label: 'Qubits' },
  { key: 'best_hamming_distance', label: 'Hamming' },
  { key: 'author_name',         label: 'Author' },
  { key: 'format',              label: 'Format' },
];

function shortId(idArr) {
  if (!idArr) return '-';
  const hex = Array.from(idArr).map(b => b.toString(16).padStart(2,'0')).join('');
  if (hex === '0'.repeat(32)) return <span style={{color:'var(--greyed-text-color)'}}>-</span>;
  return hex.slice(0, 8) + '…';
}

function Cell({ col, value }) {
  if (col === 'author_name') {
    return <span className={styles.author}>{value || <span className={styles.empty}>-</span>}</span>;
  }
  if (col === 'qubits_count' && value === -1) {
    return <span className={styles.empty}>-</span>;
  }
  if (col === 'ctx_degree') {
    return (
      <span className={styles.badge} data-degree={Math.min(value, 5)}>
        {value}
      </span>
    );
  }
  return <span>{value ?? '-'}</span>;
}

export default function ResultsTable({ results, count, onSelect, selectedId }) {
  if (!results) return null;

  if (results.length === 0) {
    return (
      <div className={styles.empty}>
        <div className={styles.emptyIcon}>∅</div>
        <div className={styles.emptyText}>No results match the current filters.</div>
      </div>
    );
  }

  return (
    <div className={styles.wrapper}>
      <div className={styles.tableHeader}>
        <span className={styles.countBadge}>{count} result{count !== 1 ? 's' : ''}</span>
      </div>
      <div className={styles.tableScroll}>
        <table className={styles.table}>
          <thead>
            <tr>
              {COLS.map(c => (
                <th key={c.key} className={styles.th}>{c.label}</th>
              ))}
            </tr>
          </thead>
          <tbody>
            {results.map((row, i) => {
              const rowId = row.id
                ? Array.from(row.id).map(b => b.toString(16).padStart(2,'0')).join('')
                : String(i);
              const isSelected = selectedId === rowId;
              return (
                <tr
                  key={i}
                  className={`${styles.row} ${isSelected ? styles.rowSelected : ''}`}
                  onClick={() => onSelect(row, rowId)}
                >
                  {COLS.map(c => (
                    <td key={c.key} className={styles.td}>
                      <Cell col={c.key} value={row[c.key]} />
                    </td>
                  ))}
                </tr>
              );
            })}
          </tbody>
        </table>
      </div>
    </div>
  );
}
