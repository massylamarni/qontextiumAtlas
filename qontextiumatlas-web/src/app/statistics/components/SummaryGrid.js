import SummaryCard from "./SummaryCard"; 
export default function SummaryGrid({ stats, onSelect }) {
  return (
    <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-4 gap-6">

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