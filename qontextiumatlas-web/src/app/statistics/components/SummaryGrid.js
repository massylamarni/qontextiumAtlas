import SummaryCard from "./SummaryCard"; 
import styles from './../statistics.module.css';
export default function SummaryGrid({ stats, onSelect }) {
  return (
    <div className={styles.cardGrid}>

      <SummaryCard
        title="Total Configs"
        value={stats.totalConfigs}
        onClick={() => onSelect("total")}
      />

      <SummaryCard
        title="Max Degree"
        value={stats.maxDegree}
        onClick={() => onSelect("degree")}
      />

      <SummaryCard
        title="Max Qubits"
        value={stats.maxQubits}
        onClick={() => onSelect("qubits")}
      />

      <SummaryCard
        title="Max Negative"
        value={stats.maxNegCtx}
        onClick={() => onSelect("negative")}
      />

    </div>
  );
}