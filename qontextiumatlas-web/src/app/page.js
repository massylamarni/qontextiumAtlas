'use client';

import { useState } from 'react';
import Nav from './components/Nav';
import FilterPanel from './components/FilterPanel';
import ResultsTable from './components/ResultsTable';
import DetailView from './components/DetailView';
import styles from './page.module.css';

export default function Home() {
  const [results, setResults] = useState(null);
  const [count, setCount] = useState(0);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [selected, setSelected] = useState(null);
  const [selectedId, setSelectedId] = useState(null);

  const handleSearch = async (body) => {
    setLoading(true);
    setError(null);
    setSelected(null);
    setSelectedId(null);

    try {
      const res = await fetch('/api/post/results', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });
      const data = await res.json();

      if (!res.ok) {
        setError(data.error || 'Unknown error from backend.');
        setResults(null);
      } else {
        setResults(data.results ?? []);
        setCount(data.count ?? 0);
      }
    } catch (e) {
      setError('Could not reach the backend at localhost:8080. Is the server running?');
      setResults(null);
    } finally {
      setLoading(false);
    }
  };

  const handleSelect = (row, id) => {
    if (selectedId === id) {
      setSelected(null);
      setSelectedId(null);
    } else {
      setSelected(row);
      setSelectedId(id);
    }
  };

  return (
    <div className={styles.layout}>
      <Nav />
      <main className={styles.main}>
        <FilterPanel onSearch={handleSearch} loading={loading} />

        <div className={styles.content}>
          {error && (
            <div className={styles.errorBanner}>
              <span className={styles.errorIcon}>⚠</span>
              {error}
            </div>
          )}

          {!error && results === null && !loading && (
            <div className={styles.splash}>
              <div className={styles.splashIcon}>⟨ψ|</div>
              <div className={styles.splashText}>
                Set filters and press <strong>Search</strong> to explore the atlas.
              </div>
            </div>
          )}

          {loading && (
            <div className={styles.splash}>
              <div className={styles.loadingDots}>
                <span /><span /><span />
              </div>
              <div className={styles.splashText}>Querying backend…</div>
            </div>
          )}

          {!loading && results !== null && (
            <ResultsTable
              results={results}
              count={count}
              onSelect={handleSelect}
              selectedId={selectedId}
            />
          )}
        </div>

        <DetailView result={selected} onClose={() => { setSelected(null); setSelectedId(null); }} />
      </main>
    </div>
  );
}
