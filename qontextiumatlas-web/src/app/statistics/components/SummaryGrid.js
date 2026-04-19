import SummaryCard from "./SummaryCard";

export default function SummaryGrid({ stats, onSelect }) {
  return (
    <div className="grid grid-cols-3 gap-6">

      <SummaryCard
        title="Max Degree"
        value={stats.maxDegree}
        onClick={() => onSelect("degree")}
      />

      <SummaryCard
        title="Total ctx"
        value={stats.totalCtx}
        onClick={() => onSelect("total")}
      />

      <SummaryCard
        title="Negative ctx"
        value={stats.negativeCtx}
        onClick={() => onSelect("negative")}
      />

    </div>
  );
}