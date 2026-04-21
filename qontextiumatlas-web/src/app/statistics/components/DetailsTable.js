export default function DetailsTable({ type, data, loading = false }) {
  if (!type) return null;

  const total = data.reduce((sum, item) => sum + item.count, 0);

  return (
    <div className="w-full overflow-x-auto">

      <h2 className="text-xl font-semibold mb-4">
        {type === "degree" && "📈 Degrees Distribution"}
        {type === "negative" && "⚠️ Negative ctx Distribution"}
        {type === "total" && "📊 Context Distribution"}
        {type === "qubits" && "🧠 Qubits Distribution"}
      </h2>

      {loading ? (
        <div className="text-center py-8 text-zinc-400">
          Chargement...
        </div>
      ) : data.length === 0 ? (
        <div className="text-center py-8 text-zinc-500">
          Aucune donnée disponible
        </div>
      ) : (
        <table className="w-full text-sm">

  <thead>
    <tr className="text-left border-b text-gray-500">
      <th className="p-3">Value</th>
      <th className="p-3">%</th>
    </tr>
  </thead>

  <tbody>
    {data.map((item, index) => {
      const total = data.reduce((sum, i) => sum + i.count, 0);
      const percentage = ((item.count / total) * 100).toFixed(1);

      return (
        <tr key={index} className="border-b hover:bg-gray-50">
          <td className="p-3 font-medium text-gray-800">{item.value}</td>
          <td className="p-3 text-gray-600">{percentage}%</td>
        </tr>
      );
    })}
  </tbody>

</table>
      )}
    </div>
  );
}