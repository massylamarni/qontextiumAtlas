import styles from './../statistics.module.css';

const TYPE_LABELS = {
  total:    'Total Configs',
  degree:   'Max Degree',
  qubits:   'Max Qubits',
  negative: 'Max Neg. Ctx',
};

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
                <th className={styles.th}>Ctx Degree</th>
                <th className={styles.th}>Qubits</th>
                <th className={styles.th}>Neg. Ctx</th>
                <th className={styles.th}>Contexts</th>
                <th className={styles.th}>Author</th>
              </tr>
            </thead>
            <tbody>
              {data.map((item, index) => (
                <tr key={index} className={styles.tr} onClick={() => onRowClick(item)}>
                  <td className={styles.td} style={{ color: 'var(--greyed-text-color)', fontSize: '11px' }}>
                    {index + 1}
                  </td>
                  <td className={styles.td}>
                    <span
                      className={styles.badge}
                      data-degree={Math.min(item.ctx_degree ?? 0, 5)}
                    >
                      {item.ctx_degree ?? '—'}
                    </span>
                  </td>
                  <td className={styles.td}>
                    {item.qubits_count === -1 ? <span style={{ color: 'var(--greyed-text-color)' }}>—</span> : item.qubits_count ?? '—'}
                  </td>
                  <td className={styles.td}>{item.neg_ctx_count ?? '—'}</td>
                  <td className={styles.td}>{item.ctx_count ?? '—'}</td>
                  <td className={styles.td} style={{ color: 'var(--signature-color)', fontFamily: "'satoshi-black', sans-serif" }}>
                    {item.author_name || <span style={{ color: 'var(--greyed-text-color)' }}>—</span>}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
}
