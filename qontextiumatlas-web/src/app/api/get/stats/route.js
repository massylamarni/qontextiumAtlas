import { NextResponse } from 'next/server';

export const runtime = "nodejs";

export async function GET() {
  try {
    const res = await fetch("http://localhost:8080/get", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        filters: ["ctx_degree=-:+"]
      })
    });

    if (!res.ok) {
      throw new Error(`Backend error ${res.status}`);
    }

    const text = await res.text();
    const data = text ? JSON.parse(text) : {};

    const results = data.results || [];

    return NextResponse.json({
      totalConfigs: data.count || 0,
      maxDegree: results.length ? Math.max(...results.map(r => r.ctx_degree || 0)) : 0,
      maxQubits: results.length ? Math.max(...results.map(r => r.qubits_count || 0)) : 0,
      maxNegCtx: results.length ? Math.max(...results.map(r => r.neg_ctx_count || 0)) : 0
    });

  } catch (err) {
    console.error("Stats error:", err);
    return NextResponse.json(
      { error: err.message },
      { status: 500 }
    );
  }
}