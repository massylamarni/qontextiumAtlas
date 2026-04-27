import SummaryCard from './SummaryCard';
import styles from './../statistics.module.css';

const CARDS = [
  { key: 'total',    label: 'Total Configs',  stat: 'totalConfigs' },
  { key: 'degree',   label: 'Max Degree',     stat: 'maxDegree'    },
  { key: 'qubits',   label: 'Max Qubits',     stat: 'maxQubits'    },
  { key: 'ctx',      label: 'Max Ctx Count',  stat: 'maxCtxCount'  }, 
  { key: 'negative', label: 'Max Neg. Ctx',   stat: 'maxNegCtx'    },
];

export default function SummaryGrid({ stats, onSelect, selectedType }) {
  return (
    <div className={styles.cardGrid}>
      {CARDS.map(({ key, label, stat }) => (
        <SummaryCard
          key={key}
          title={label}
          value={stats[stat]}
          active={selectedType === key}
          onClick={() => onSelect(selectedType === key ? null : key)}
        />
      ))}
    </div>
  );
}