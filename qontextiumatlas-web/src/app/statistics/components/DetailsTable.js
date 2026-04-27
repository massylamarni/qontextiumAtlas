export default function DetailsTable({ type, data, onRowClick }) {
  if (!type) return null;

  if (!data.length) {
    return (
      <div className="text-center py-10 text-zinc-500">
        No data available
      </div>
    );
  }

  const thClass =
    "p-3 text-left font-semibold text-zinc-300 uppercase tracking-wider text-xs";
  const tdClass = "p-3 text-left whitespace-nowrap text-zinc-200";

  return (
    <div className="w-full overflow-x-auto rounded-xl border border-zinc-800">

      {/* HEADER */}
      <div className="p-4 border-b border-zinc-800 bg-zinc-900/50">
        <h2 className="text-lg font-bold text-green-400 tracking-wide">
          Configurations ({type})
        </h2>
      </div>

      {/* TABLE */}
      <table className="w-full text-sm table-fixed">

        <thead className="bg-zinc-900 sticky top-0 z-10 border-b border-zinc-800">
          <tr>
            <th className={`${thClass} w-12`}>#</th>
            <th className={thClass}>Degree</th>
            <th className={thClass}>Qubits</th>
            <th className={thClass}>Neg Ctx</th>
            <th className={thClass}>Ctx Count</th>
            <th className={thClass}>Hamming</th>
            <th className={thClass}>Dimension</th>
            <th className={thClass}>Format</th>
            <th className={thClass}>Obs</th>
            <th className={thClass}>Author</th>
          </tr>
        </thead>

        <tbody>
          {data.map((item, index) => (
            <tr
              key={index}
              onClick={() => onRowClick(item)}
              className="
                border-t border-zinc-800
                hover:bg-green-500/5 hover:shadow-[inset_0_0_10px_rgba(34,197,94,0.15)]
                transition-all duration-150
                cursor-pointer
              "
            >
              {/* index */}
              <td className={`${tdClass} text-zinc-500`}>
                {index + 1}
              </td>

              {/* degree */}
              <td className={tdClass}>
                <span className="px-2 py-1 rounded-md bg-zinc-800 border border-zinc-700 text-green-400 font-mono">
                  {item.ctx_degree ?? "-"}
                </span>
              </td>

              <td className={tdClass}>
                {item.qubits_count ?? "-"}
              </td>

              <td className={tdClass}>
                <span className="text-rose-400 font-medium">
                  {item.neg_ctx_count ?? "-"}
                </span>
              </td>

              <td className={tdClass}>
                <span className="text-green-300">
                  {item.ctx_count ?? "-"}
                </span>
              </td>

              <td className={tdClass}>
                {item.best_hamming_distance ?? "-"}
              </td>

              <td className={tdClass}>
                {item.dimension ?? "-"}
              </td>

              <td className={tdClass}>
                <span className="text-zinc-400 italic">
                  {item.format ?? "-"}
                </span>
              </td>

              <td className={tdClass}>
                {item.observable_count ?? "-"}
              </td>

              <td className={tdClass}>
                <span className="text-zinc-300">
                  {item.author_name ?? "-"}
                </span>
              </td>

            </tr>
          ))}
        </tbody>

      </table>
    </div>
  );
}