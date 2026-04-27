'use client';
import styles from './statistics.module.css';
import Nav from '../components/Nav';
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
  const [loadingDetails, setLoadingDetails] = useState(false);
  const [selectedRow, setSelectedRow] = useState(null);

  // FETCH STATS
  useEffect(() => {
    const fetchStats = async () => {
      try {
        const res = await fetch('/api/get/stats');

        if (!res.ok) throw new Error(`HTTP ${res.status}`);

        const text = await res.text();
        const data = text ? JSON.parse(text) : {};

        setStats({
          totalConfigs: data.totalConfigs || 0,
          maxDegree: data.maxDegree || 0,
          maxQubits: data.maxQubits || 0,
          maxNegCtx: data.maxNegCtx || 0,
        });

      } catch (error) {
        console.error('Erreur stats:', error);
      }
    };

    fetchStats();
  }, []);

  // FETCH DETAILS
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

        const metric = metricMap[selectedType];

        const res = await fetch(`/api/get/stats-detail?metric=${metric}`);

        if (!res.ok) throw new Error(`HTTP ${res.status}`);

        const text = await res.text();
        const data = text ? JSON.parse(text) : [];

        setDetailedData(data);

      } catch (error) {
        console.error('Erreur détails:', error);
        setDetailedData([]);
      } finally {
        setLoadingDetails(false);
      }
    };

    fetchDetails();
  }, [selectedType]);

return (
  <div className={styles.layout}>
    <Nav />

    <main className={styles.main}>

      {/* HERO (identique About) */}
      <div className={styles.hero}>
        <div className={styles.eyebrow}>Statistics</div>

        <h1 className={styles.title}>
          QATLAS Statistics
        </h1>

        <p className={styles.sub}>
          Quantum configuration analytics & context visualization
        </p>
      </div>

      <div className={styles.divider} />

      {/* SUMMARY */}
      <div className={styles.cardGrid}>
        <SummaryGrid stats={stats} onSelect={setSelectedType} />
      </div>

      {/* TABLE */}
      <div className={styles.card}>
        <DetailsTable
          type={selectedType}
          data={detailedData}
          onRowClick={setSelectedRow}
        />
      </div>

    </main>

    {/* MODAL (tu peux garder Tailwind ici pour l’instant) */}
    {selectedRow && (
      <div
        className="fixed inset-0 bg-black/70 flex items-center justify-center z-50"
        onClick={(e) => {
          if (e.target === e.currentTarget) setSelectedRow(null);
        }}
      >
        <div className="bg-zinc-900 border rounded-xl p-6 w-[460px]">
          <h2 className="text-lg font-bold mb-4">Configuration details</h2>

          <pre>{selectedRow.ctx_conf}</pre>

          <button onClick={() => setSelectedRow(null)}>Close</button>
        </div>
      </div>
    )}
  </div>
);
}