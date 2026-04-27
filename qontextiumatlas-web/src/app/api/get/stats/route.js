import { NextResponse } from 'next/server';

export const runtime = "nodejs";

export async function GET() {
  try {
    const controller = new AbortController();
    const timeout = setTimeout(() => controller.abort(), 5000);

    const res = await fetch("http://localhost:8080/get", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ filters: ["ctx_degree=-:+"] }),
      signal: controller.signal
    });

    clearTimeout(timeout);

    if (!res.ok) {
      return NextResponse.json(
        { totalConfigs: 0, maxDegree: 0, maxQubits: 0, maxNegCtx: 0, maxCtxCount: 0 },
        { status: 200 }
      );
    }

    const data    = await res.json().catch(() => ({}));
    const results = data.results ?? [];

    return NextResponse.json({
      totalConfigs: data.count ?? 0,
      maxDegree:    results.length ? Math.max(...results.map(r => r.ctx_degree    ?? 0)) : 0,
      maxQubits:    results.length ? Math.max(...results.map(r => r.qubits_count  ?? 0)) : 0,
      maxNegCtx:    results.length ? Math.max(...results.map(r => r.neg_ctx_count ?? 0)) : 0,
      maxCtxCount:  results.length ? Math.max(...results.map(r => r.ctx_count     ?? 0)) : 0,  
    });

  } catch {
    return NextResponse.json({
      totalConfigs: 0,
      maxDegree:    0,
      maxQubits:    0,
      maxNegCtx:    0,
      maxCtxCount:  0, 
    }, { status: 200 });
  }
}