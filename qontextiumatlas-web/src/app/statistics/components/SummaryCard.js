export default function SummaryCard({ title, value, onClick }) {
  return (
    <div className="bg-zinc-900 border border-zinc-700 p-6 rounded-xl">

      <p className="text-gray-400">{title}</p>

      <p className="text-2xl font-bold text-white">{value}</p>

      <button
        onClick={onClick}
        className="mt-4 text-sm bg-white text-black px-3 py-1 rounded"
      >
        Voir détails
      </button>

    </div>
  );
}