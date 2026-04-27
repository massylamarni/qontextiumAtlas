'use client';
import { useState, useEffect } from 'react';
import styles from './statistics.module.css';
import Nav from '../components/Nav';
import SummaryGrid from './components/SummaryGrid';
import DetailsTable from './components/DetailsTable';
import DetailView from './../components/DetailView';
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

  // Fetch summary stats
  useEffect(() => {
    const fetchStats = async () => {
      try {
        const res = await fetch('/api/get/stats');
        if (!res.ok) throw new Error(`HTTP ${res.status}`);
        const data = await res.json();
        setStats({
          totalConfigs: data.totalConfigs || 0,
          maxDegree:    data.maxDegree    || 0,
          maxQubits:    data.maxQubits    || 0,
          maxNegCtx:    data.maxNegCtx    || 0,
        });
      } catch (err) {
        console.error('Stats fetch error:', err);
      }
    };
    fetchStats();
  }, []);

  // Fetch detail rows when a card is selected
  useEffect(() => {
    if (!selectedType) { setDetailedData([]); return; }
    const metricMap = {
      degree:   'ctx_degree',
      negative: 'neg_ctx_count',
      total:    'ctx_count',
      qubits:   'qubits_count',
    };
    const metric = metricMap[selectedType];
    const fetchDetails = async () => {
      setLoadingDetails(true);
      try {
        const res = await fetch(`/api/get/stats-detail?metric=${metric}`);
        if (!res.ok) throw new Error(`HTTP ${res.status}`);
        const data = await res.json();
        setDetailedData(Array.isArray(data) ? data : []);
      } catch (err) {
        console.error('Detail fetch error:', err);
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
        {/* Hero */}
        <div className={styles.hero}>
          <div className={styles.eyebrow}>Statistics</div>
          <h1 className={styles.title}>QATLAS Statistics</h1>
          <p className={styles.sub}>
            Quantum configuration analytics &amp; context visualization
          </p>
        </div>

        <div className={styles.divider} />

        {/* Summary cards — pass selectedType for active state */}
        <SummaryGrid
          stats={stats}
          onSelect={setSelectedType}
          selectedType={selectedType}
        />
{/* Table + Detail side by side */}
        <div className={styles.contentRow}>

          {/* Table */}
          <div className={styles.tableCol}>
            <DetailsTable
              type={selectedType}
              data={detailedData}
              loading={loadingDetails}
              onRowClick={setSelectedRow}
            />
          </div>

          {/* Detail panel — même style que la page search */}
          <div className={styles.detailCol}>
            <DetailView
              result={selectedRow}
              onClose={() => setSelectedRow(null)}
            />
          </div>

        </div>
      </main>

    </div>
  );
}
