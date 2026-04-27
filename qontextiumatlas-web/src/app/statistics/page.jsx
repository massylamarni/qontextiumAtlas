'use client';
import { useState, useEffect } from 'react';
import styles from './statistics.module.css';
import Nav from '../components/Nav';
import SummaryGrid from './components/SummaryGrid';
import DetailsTable from './components/DetailsTable';

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

        {/* Detail table */}
        <DetailsTable
          type={selectedType}
          data={detailedData}
          loading={loadingDetails}
          onRowClick={setSelectedRow}
        />
      </main>

      {/* Modal */}
      {selectedRow && (
        <div
          className={styles.modalOverlay}
          onClick={(e) => { if (e.target === e.currentTarget) setSelectedRow(null); }}
        >
          <div className={styles.modal}>
            <div className={styles.modalHeader}>
              <span className={styles.modalTitle}>Configuration details</span>
              <button className={styles.modalClose} onClick={() => setSelectedRow(null)}>✕</button>
            </div>

            <div className={styles.modalBody}>
              {/* Stats mini-grid */}
              <div className={styles.modalStatsGrid}>
                {[
                  { label: 'Ctx Degree',  value: selectedRow.ctx_degree   ?? '—' },
                  { label: 'Qubits',      value: selectedRow.qubits_count === -1 ? '—' : (selectedRow.qubits_count ?? '—') },
                  { label: 'Neg. Ctx',    value: selectedRow.neg_ctx_count ?? '—' },
                  { label: 'Contexts',    value: selectedRow.ctx_count     ?? '—' },
                ].map(({ label, value }) => (
                  <div key={label} className={styles.modalStatCard}>
                    <div className={styles.modalStatLabel}>{label}</div>
                    <div className={styles.modalStatVal}>{value}</div>
                  </div>
                ))}
              </div>

              {/* Configuration string */}
              {selectedRow.ctx_conf && (
                <div className={styles.modalConfSection}>
                  <div className={styles.modalConfLabel}>Configuration</div>
                  <pre className={styles.modalConfPre}>{selectedRow.ctx_conf}</pre>
                </div>
              )}
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
