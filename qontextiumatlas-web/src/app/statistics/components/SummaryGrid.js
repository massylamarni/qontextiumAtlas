import SummaryCard from "./SummaryCard"; 
// export default function SummaryGrid({ stats, onSelect }) {
//   return (
//     <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-4 gap-6">

//       <SummaryCard
//         title="Total Configs"
//         value={stats.totalConfigs}
//         onClick={() => onSelect("total")}
//       />

//       <SummaryCard
//         title="Max Degree"
//         value={stats.maxDegree}
//         onClick={() => onSelect("degree")}
//       />

//       <SummaryCard
//         title="Max Qubits"
//         value={stats.maxQubits}
//         onClick={() => onSelect("qubits")}
//       />

//       <SummaryCard
//         title="Max Negative ctx"
//         value={stats.maxNegCtx}
//         onClick={() => onSelect("negative")}
//       />

//     </div>
//   );
// }
import { FiUsers, FiCpu, FiActivity, FiAlertCircle } from "react-icons/fi";

export default function SummaryGrid({ stats, onSelect }) {
  return (
    <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-4 gap-6">

      <SummaryCard
        title="Total Configs"
        value={stats.totalConfigs}
        color="bg-blue-100 text-blue-600"
        icon={<FiUsers />}
        onClick={() => onSelect("total")}
      />

      <SummaryCard
        title="Max Degree"
        value={stats.maxDegree}
        color="bg-yellow-100 text-yellow-600"
        icon={<FiActivity />}
        onClick={() => onSelect("degree")}
      />

      <SummaryCard
        title="Max Qubits"
        value={stats.maxQubits}
        color="bg-purple-100 text-purple-600"
        icon={<FiCpu />}
        onClick={() => onSelect("qubits")}
      />

      <SummaryCard
        title="Max Negative"
        value={stats.maxNegCtx}
        color="bg-green-100 text-green-600"
        icon={<FiAlertCircle />}
        onClick={() => onSelect("negative")}
      />

    </div>
  );
}