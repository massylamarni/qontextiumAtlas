import styles from './../statistics.module.css';

const TYPE_LABELS = {
  total:    'Total Configs',
  degree:   'Max Degree',
  qubits:   'Max Qubits',
  negative: 'Max Neg. Ctx',
};

const COLUMNS = [
  { key: 'ctx_degree',            label: 'Ctx Degree' },
  { key: 'ctx_count',             label: 'Contexts' },
  { key: 'neg_ctx_count',         label: 'Neg. Ctx' },
  { key: 'qubits_count',          label: 'Qubits' },
  { key: 'best_hamming_distance', label: 'Hamming' },
  { key: 'observable_count',      label: 'Observables' },
  { key: 'dimension',             label: 'Dimension' },
  { key: 'format',                label: 'Format' },
  { key: 'author_name',           label: 'Author' },
];

function CellValue({ colKey, value }) {
  // Valeur vide ou -1
  if (value === null || value === undefined || value === -1) {
    return <span style={{ color: 'var(--greyed-text-color)' }}>—</span>;
  }

  // Ctx Degree → badge coloré
  if (colKey === 'ctx_degree') {
    return (
      <span
        className={styles.badge}
        data-degree={Math.min(value, 5)}
      >
        {value}
      </span>
    );
  }

  // Author → signature color
  if (colKey === 'author_name') {
    return (
      <span style={{ color: 'var(--signature-color)', fontFamily: "'satoshi-black', sans-serif" }}>
        {value}
      </span>
    );
  }

  // Neg ctx → rouge
  if (colKey === 'neg_ctx_count') {
    return <span style={{ color: '#f87171' }}>{value}</span>;
  }

  return value;
}

export default function DetailsTable({ type, data, loading, onRowClick }) {
  if (!type) {
    return (
      <div className={styles.emptyHint}>
        Select a metric above to explore the top configurations.
      </div>
    );
  }

  return (
    <div className={styles.tableCard}>

      <div className={styles.tableCardHeader}>
        <span className={styles.tableCardTitle}>
          {TYPE_LABELS[type] ?? type} — top results
        </span>
        <span className={styles.tableCardTitle}>
          {loading ? '…' : `${data.length} result${data.length !== 1 ? 's' : ''}`}
        </span>
      </div>

      {loading && (
        <div className={styles.emptyTable}>
          <div className={styles.emptyText}>Loading…</div>
        </div>
      )}

      {!loading && data.length === 0 && (
        <div className={styles.emptyTable}>
          <div className={styles.emptyIcon}>∅</div>
          <div className={styles.emptyText}>No data available.</div>
        </div>
      )}

      {!loading && data.length > 0 && (
        <div className={styles.tableScroll}>
          <table className={styles.table}>

            <thead>
              <tr>
                <th className={styles.th}>#</th>
                {COLUMNS.map(col => (
                  <th key={col.key} className={styles.th}>
                    {col.label}
                  </th>
                ))}
              </tr>
            </thead>

            <tbody>
              {data.map((item, index) => (
                <tr
                  key={index}
                  className={styles.tr}
                  onClick={() => onRowClick(item)}
                >
                  <td
                    className={styles.td}
                    style={{ color: 'var(--greyed-text-color)', fontSize: '11px' }}
                  >
                    {index + 1}
                  </td>

                  {COLUMNS.map(col => (
                    <td key={col.key} className={styles.td}>
                      <CellValue colKey={col.key} value={item[col.key]} />
                    </td>
                  ))}
                </tr>
              ))}
            </tbody>

          </table>
        </div>
      )}

    </div>
  );
}