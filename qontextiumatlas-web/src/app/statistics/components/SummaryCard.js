import styles from './../statistics.module.css';

export default function SummaryCard({ title, value, onClick }) {
  return (
    <div onClick={onClick} className={styles.card}>
      <div className={styles.cardTitle}>{title}</div>
      <div className={styles.cardDesc}>{value}</div>
    </div>
  );
}