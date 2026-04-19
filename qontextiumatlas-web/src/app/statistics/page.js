'use client';

import { useState, useEffect } from "react";
import SummaryGrid from "./components/SummaryGrid";
import DetailsTable from "./components/DetailsTable";

export default function StatisticsPage() {
  const [selectedType, setSelectedType] = useState(null);
  const [stats, setStats] = useState({
    maxDegree: 0,
    totalCtx: 0,
    negativeCtx: 0,
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
          maxDegree: data.maxDegree || 0,
          totalCtx: data.totalConfigs || 0,
          negativeCtx: data.maxNegCtx || 0,
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
          total: 'ctx_count'
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

  return (
    <main className="min-h-screen bg-black p-8 space-y-8">

      <h1 className="text-3xl font-bold text-white">
        Statistics Dashboard
      </h1>

      {/* Cards avec vraies données */}
      <SummaryGrid stats={stats} onSelect={setSelectedType} />

      {/* Tableau avec vraies données */}
      <DetailsTable
        type={selectedType}
        data={detailedData}
        loading={loadingDetails}
      />

    </main>
  );
}