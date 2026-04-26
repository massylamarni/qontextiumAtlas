
// export default function DetailsTable({ type, data }) {
//   if (!type) return null;

//   if (!data.length) {
//     return (
//       <div className="text-center py-8 text-zinc-500">
//         No data available
//       </div>
//     );
//   }

//   return (
//     <div className="w-full overflow-x-auto">

//       <h2 className="text-xl font-semibold mb-4 text-white">
//         Configurations ({type})
//       </h2>

//       <table className="w-full text-sm border border-zinc-700 rounded-lg overflow-hidden">

//         <thead className="bg-zinc-800 text-zinc-400">
//           <tr>
//             <th className="p-3">#</th>
//             <th className="p-3">Degree</th>
//             <th className="p-3">Qubits</th>
//             <th className="p-3">Ctx Count</th>
//             <th className="p-3">Neg Ctx</th>
//             <th className="p-3">Hamming</th>
//             <th className="p-3">Dimension</th>
//           </tr>
//         </thead>

//         <tbody>
//           {data.map((item, index) => (
//             <tr
//               key={index}
//               className="border-t border-zinc-700 hover:bg-zinc-800"
//             >
//               <td className="p-3">{index + 1}</td>
//               <td className="p-3">{item.ctx_degree}</td>
//               <td className="p-3">{item.qubits_count}</td>
//               <td className="p-3">{item.ctx_count}</td>
//               <td className="p-3">{item.neg_ctx_count}</td>
//               <td className="p-3">{item.best_hamming_distance}</td>
//               <td className="p-3">{item.dimension}</td>
//             </tr>
//           ))}
//         </tbody>

//       </table>
//     </div>
//   );
// }
export default function DetailsTable({ type, data }) {
  if (!type) return null;

  if (!data.length) {
    return (
      <div className="text-center py-8 text-zinc-500">
        No data available
      </div>
    );
  }

  const thClass = "p-3 text-left font-semibold whitespace-nowrap";
  const tdClass = "p-3 text-left whitespace-nowrap";

  return (
    <div className="w-full overflow-x-auto">

      <h2 className="text-xl font-semibold mb-4 text-white">
        Configurations ({type})
      </h2>

      <table className="w-full text-sm border border-zinc-700 rounded-lg overflow-hidden table-fixed">

        <thead className="bg-zinc-800 text-zinc-400">
          <tr>
            <th className={`${thClass} w-12`}>#</th>
            <th className={`${thClass} w-28`}>Degree</th>
            <th className={`${thClass} w-24`}>Qubits</th>
            <th className={`${thClass} w-28`}>Ctx Count</th>
            <th className={`${thClass} w-28`}>Neg Ctx</th>
            <th className={`${thClass} w-28`}>Hamming</th>
            <th className={`${thClass} w-28`}>Dimension</th>
          </tr>
        </thead>

        <tbody>
          {data.map((item, index) => (
            <tr
              key={index}
              className="border-t border-zinc-700 hover:bg-zinc-800 transition-colors"
            >
              <td className={tdClass}>{index + 1}</td>
              <td className={tdClass}>{item.ctx_degree}</td>
              <td className={tdClass}>{item.qubits_count}</td>
              <td className={tdClass}>{item.ctx_count}</td>
              <td className={tdClass}>{item.neg_ctx_count}</td>
              <td className={tdClass}>{item.best_hamming_distance}</td>
              <td className={tdClass}>{item.dimension}</td>
            </tr>
          ))}
        </tbody>

      </table>
    </div>
  );
}