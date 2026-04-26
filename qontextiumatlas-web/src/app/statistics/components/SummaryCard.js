export default function SummaryCard({ title, value, onClick }) {
  return (
    <div
      onClick={onClick}
      className="bg-zinc-800 hover:bg-zinc-700 transition p-5 rounded-xl border border-zinc-700 cursor-pointer"
    >
      <p className="text-sm text-zinc-400 mb-1">{title}</p>
      <p className="text-3xl font-bold text-white">{value}</p>
    </div>
  );
}