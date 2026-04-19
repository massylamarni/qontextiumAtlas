export default function DetailsTable({ type, data, loading = false }) {
  if (!type) return null;

  return (
    <div className="bg-zinc-900 border border-zinc-700 p-6 rounded-xl">

      <h2 className="text-white text-lg mb-4">
        {type === "degree" && "Degrees Leaderboard"}
        {type === "negative" && "Negative ctx Ranking"}
        {type === "total" && "All ctx"}
      </h2>

      {loading ? (
        <div className="text-white text-center py-8">
          Chargement des données...
        </div>
      ) : data.length === 0 ? (
        <div className="text-zinc-400 text-center py-8">
          Aucune donnée disponible
        </div>
      ) : (
        <table className="w-full text-white border border-zinc-700">

          <thead>
            <tr className="bg-zinc-800">
              <th className="p-3 border">Value</th>
              <th className="p-3 border">Count</th>
            </tr>
          </thead>

          <tbody>
            {data
              .sort((a, b) => b.value - a.value)
              .map((item, index) => (
                <tr key={index} className="text-center">
                  <td className="p-3 border">{item.value}</td>
                  <td className="p-3 border">{item.count}</td>
                </tr>
              ))}
          </tbody>

        </table>
      )}
    </div>
  );
}