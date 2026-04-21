// export default function SummaryCard({ title, value, onClick }) {
//   return (
//     <div className="bg-zinc-900 border border-zinc-700 p-6 rounded-xl">

//       <p className="text-gray-400">{title}</p>

//       <p className="text-2xl font-bold text-white">{value}</p>

//       <button
//         onClick={onClick}
//         className="mt-4 text-sm bg-white text-black px-3 py-1 rounded"
//       >
//         Voir détails
//       </button>

//     </div>
//   );
// }

// export default function SummaryCard({ title, value, onClick }) {
//   return (
//     <div className="bg-zinc-800 hover:bg-zinc-700 transition p-6 rounded-xl border border-zinc-700 shadow-md">

//       <p className="text-zinc-400 text-sm mb-2">{title}</p>

//       <p className="text-3xl font-bold text-white">{value}</p>

//       <button
//         onClick={onClick}
//         className="mt-4 text-sm bg-blue-500 hover:bg-blue-600 text-white px-4 py-2 rounded-lg transition"
//       >
//         Voir détails
//       </button>

//     </div>
//   );
// }

export default function SummaryCard({ title, value, color, icon, onClick }) {
  return (
    <div
      onClick={onClick}
      className="flex items-center justify-between bg-white text-black p-5 rounded-xl shadow hover:shadow-lg transition cursor-pointer"
    >
      <div>
        <p className="text-sm text-gray-500">{title}</p>
        <p className="text-2xl font-bold">{value}</p>
      </div>

      <div className={`w-10 h-10 flex items-center justify-center rounded-full ${color}`}>
        {icon}
      </div>
    </div>
  );
}