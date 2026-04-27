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
  <main className="min-h-screen bg-gradient-to-b from-zinc-950 via-zinc-950 to-black text-white px-6 py-10">

    <div className="max-w-7xl mx-auto space-y-10">

      {/* HEADER */}
      <div className="relative">

        <h1 className="text-4xl font-extrabold tracking-tight">
          <span className="text-white">Statistics</span>{" "}
          <span className="text-green-400">Dashboard</span>
        </h1>

        <p className="text-zinc-500 mt-2 text-sm">
          Quantum configuration analytics & context visualization
        </p>

        {/* glow line */}
        <div className="absolute -bottom-3 left-0 w-40 h-[2px] bg-green-500/40 blur-sm"></div>

      </div>

      {/* SUMMARY CARD */}
      <div className="
        bg-zinc-900/60
        backdrop-blur-md
        border border-zinc-800
        rounded-2xl
        p-6
        shadow-[0_0_30px_rgba(0,0,0,0.4)]
      ">
        <SummaryGrid stats={stats} onSelect={setSelectedType} />
      </div>

      {/* TABLE CARD */}
      <div className="
        bg-zinc-900/60
        backdrop-blur-md
        border border-zinc-800
        rounded-2xl
        p-6
        shadow-[0_0_40px_rgba(34,197,94,0.05)]
        relative overflow-hidden
      ">

        {/* subtle glow background */}
        <div className="absolute inset-0 bg-green-500/5 blur-3xl pointer-events-none"></div>

        <div className="relative">
          <DetailsTable
            type={selectedType}
            data={detailedData}
            onRowClick={setSelectedRow}
          />
        </div>

      </div>

    </div>

    {/* MODAL (TON CODE INCHANGÉ MAIS COHÉRENT) */}
    {selectedRow && (
      <div
        className="fixed inset-0 bg-black/70 backdrop-blur-sm flex items-center justify-center z-50"
        onClick={(e) => {
          if (e.target === e.currentTarget) setSelectedRow(null);
        }}
      >
        <div className="
          relative w-[460px]
          bg-zinc-950
          border border-green-500/20
          rounded-2xl
          shadow-[0_0_50px_rgba(34,197,94,0.15)]
          p-6
        ">

          {/* glow */}
          <div className="absolute inset-0 bg-green-500/5 blur-2xl rounded-2xl pointer-events-none"></div>

          {/* HEADER */}
          <div className="flex justify-between items-center mb-4 relative">
            <h2 className="text-lg font-bold text-green-400">
              Configuration details
            </h2>

            <button
              onClick={() => setSelectedRow(null)}
              className="text-zinc-400 hover:text-green-400 transition"
            >
              ✕
            </button>
          </div>

          {/* BODY */}
          <div className="space-y-3 text-sm relative">

            <div className="p-3 bg-zinc-900 border border-zinc-800 rounded-lg">
              <p className="text-xs text-green-500 mb-1">CTX CONF</p>
              <pre className="font-mono text-green-300 whitespace-pre-wrap break-words">
                {selectedRow.ctx_conf ?? "-"}
              </pre>
            </div>

            <div className="grid grid-cols-2 gap-3">

              <div className="p-3 bg-zinc-900 border border-zinc-800 rounded-lg">
                <p className="text-zinc-500 text-xs">Degree</p>
                <p className="text-white font-semibold">
                  {selectedRow.ctx_degree ?? "-"}
                </p>
              </div>

              <div className="p-3 bg-zinc-900 border border-zinc-800 rounded-lg">
                <p className="text-zinc-500 text-xs">Qubits</p>
                <p className="text-white font-semibold">
                  {selectedRow.qubits_count ?? "-"}
                </p>
              </div>

              <div className="p-3 bg-zinc-900 border border-zinc-800 rounded-lg">
                <p className="text-zinc-500 text-xs">Ctx count</p>
                <p className="text-white font-semibold">
                  {selectedRow.ctx_count ?? "-"}
                </p>
              </div>

            </div>

          </div>

          {/* FOOTER */}
          <div className="mt-5 flex justify-end relative">
            <button
              onClick={() => setSelectedRow(null)}
              className="
                px-4 py-2
                bg-green-500/10
                text-green-400
                border border-green-500/30
                rounded-lg
                hover:bg-green-500/20
                transition
              "
            >
              Close
            </button>
          </div>

        </div>
      </div>
    )}

  </main>
);
}