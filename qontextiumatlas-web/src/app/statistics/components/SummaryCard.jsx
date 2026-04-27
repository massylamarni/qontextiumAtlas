import styles from './../statistics.module.css';

export default function SummaryCard({ title, value, onClick, active }) {
  return (
    <div
      onClick={onClick}
      className={`${styles.summaryCard} ${active ? styles.summaryCardActive : ''}`}
    >
      <div className={styles.summaryCardLabel}>{title}</div>
      <div className={`${styles.summaryCardValue} ${active ? styles.summaryCardActiveValue : ''}`}>
        {value ?? '—'}
      </div>
    </div>
  );
}
