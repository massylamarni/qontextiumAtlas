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
    maxDegree:    0,
    maxQubits:    0,
    maxNegCtx:    0,
    maxCtxCount:  0, 
  });
  const [detailedData, setDetailedData] = useState([]);
  const [loadingDetails, setLoadingDetails] = useState(false);
  const [selectedRow, setSelectedRow] = useState(null);

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
          maxCtxCount:  data.maxCtxCount  || 0,
        });
      } catch {
        //
      }
    };
    fetchStats();
  }, []);

  useEffect(() => {
    if (!selectedType) { setDetailedData([]); return; }

    const metricMap = {
      degree:   'ctx_degree',
      negative: 'neg_ctx_count',
      total:    'ctx_count',
      qubits:   'qubits_count',
      ctx:      'ctx_count',    
    };

    const metric = metricMap[selectedType];

    const fetchDetails = async () => {
      setLoadingDetails(true);
      try {
        const res = await fetch(`/api/get/stats-detail?metric=${metric}`);
        if (!res.ok) throw new Error(`HTTP ${res.status}`);
        const data = await res.json();
        setDetailedData(Array.isArray(data) ? data : []);
      } catch {
       
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

        <div className={styles.hero}>
          <div className={styles.eyebrow}>Statistics</div>
          <h1 className={styles.title}>QATLAS Statistics</h1>
          <p className={styles.sub}>
            Quantum configuration analytics &amp; context visualization
          </p>
        </div>

        <div className={styles.divider} />

        <SummaryGrid
          stats={stats}
          onSelect={setSelectedType}
          selectedType={selectedType}
        />

        <div className={styles.contentRow}>
          <div className={styles.tableCol}>
            <DetailsTable
              type={selectedType}
              data={detailedData}
              loading={loadingDetails}
              onRowClick={setSelectedRow}
            />
          </div>
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