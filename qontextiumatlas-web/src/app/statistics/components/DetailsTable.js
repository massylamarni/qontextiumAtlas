import styles from './../statistics.module.css';

export default function DetailsTable({ type, data, onRowClick }) {
  if (!type) return null;

 if (!data.length) {
  return (
    <div className={styles.empty}>
      No data available
    </div>
  );
}

return (
  <div className={styles.tableCard}>

    <div className={styles.tableHeader}>
      Configurations ({type})
    </div>

    <div className={styles.tableWrapper}>
      <table className={styles.table}>
        <thead>
          <tr>
            <th>#</th>
            <th>Degree</th>
            <th>Qubits</th>
            <th>Neg</th>
            <th>Ctx</th>
          </tr>
        </thead>

        <tbody>
          {data.map((item, index) => (
            <tr key={index} onClick={() => onRowClick(item)}>
              <td>{index + 1}</td>
              <td>{item.ctx_degree}</td>
              <td>{item.qubits_count}</td>
              <td>{item.neg_ctx_count}</td>
              <td>{item.ctx_count}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>

  </div>
);
}