'use client';

import { useState, useEffect } from "react";
import SummaryGrid from "./components/SummaryGrid";
import DetailsTable from "./components/DetailsTable";

export default function StatisticsPage() {
  const [selectedType, setSelectedType] = useState(null);
  const [stats, setStats] = useState({
  totalConfigs: 0,
  maxDegree: 0,
  maxQubits: 0,
  maxNegCtx: 0,
});
  const [detailedData, setDetailedData] = useState([]);
  const [loading, setLoading] = useState(true);
  const [loadingDetails, setLoadingDetails] = useState(false);

  // Récupération des vraies statistiques depuis la DB
  useEffect(() => {
    const fetchStats = async () => {
      try {
        const res = await fetch('/api/get/stats');
        const data = await res.json();
        setStats({
  totalConfigs: data.totalConfigs || 0,
  maxDegree: data.maxDegree || 0,
  maxQubits: data.maxQubits || 0,
  maxNegCtx: data.maxNegCtx || 0,
});
      } catch (error) {
        console.error('Erreur lors du chargement des stats:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchStats();
  }, []);

  // Récupération des données détaillées quand une métrique est sélectionnée
  useEffect(() => {
    if (!selectedType) {
      setDetailedData([]);
      return;
    }

    const fetchDetails = async () => {
      setLoadingDetails(true);
      try {
        const metricMap = {
  degree: 'ctx_degree',
  negative: 'neg_ctx_count',
  total: 'ctx_count',
  qubits: 'qubits_count' 
};

        const res = await fetch(`/api/get/stats-detail?metric=${metricMap[selectedType]}`);
        const data = await res.json();
        setDetailedData(data);
      } catch (error) {
        console.error('Erreur lors du chargement des détails:', error);
        setDetailedData([]);
      } finally {
        setLoadingDetails(false);
      }
    };

    fetchDetails();
  }, [selectedType]);

  if (loading) {
    return (
      <main className="min-h-screen bg-black p-8 space-y-8">
        <div className="text-white text-center">Chargement des statistiques...</div>
      </main>
    );
  }


//   return (
//   <main className="min-h-screen bg-zinc-950 text-white px-4 md:px-10 py-10">

//     <div className="max-w-6xl mx-auto space-y-10">

//       <h1 className="text-3xl md:text-4xl font-bold">
//         📊 Statistics Dashboard
//       </h1>

//       {/* Cards container */}
//       <div className="bg-zinc-900 p-6 rounded-2xl shadow-lg">
//         <SummaryGrid stats={stats} onSelect={setSelectedType} />
//       </div>

//       {/* Table container */}
//       <div className="bg-zinc-900 p-6 rounded-2xl shadow-lg">
//         <DetailsTable
//           type={selectedType}
//           data={detailedData}
//           loading={loadingDetails}
//         />
//       </div>

//     </div>
//   </main>
// );
return (
  <main className="min-h-screen bg-gray-100 px-6 py-10">

    <div className="max-w-7xl mx-auto space-y-8">

      <h1 className="text-2xl font-bold text-gray-800">
        📊 Statistics Dashboard
      </h1>

      {/* Cards */}
      <SummaryGrid stats={stats} onSelect={setSelectedType} />

      {/* Table */}
      <div className="bg-white rounded-xl shadow p-6">
        <DetailsTable
          type={selectedType}
          data={detailedData}
          loading={loadingDetails}
        />
      </div>

    </div>
  </main>
);
}