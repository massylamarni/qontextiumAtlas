import { NextResponse } from 'next/server';

export const runtime = "nodejs";

export async function GET(req) {
  try {
    const url = new URL(req.url);
    const metric = url.searchParams.get("metric") || "ctx_degree";

    const res = await fetch("http://localhost:8080/get", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        filters: [`${metric}=-:+`],
    	show_conf: true
      })
    });

    if (!res.ok) {
      throw new Error(`Backend error ${res.status}`);
    }

    const text = await res.text();
    const data = text ? JSON.parse(text) : {};

    let results = data.results || [];

    // 
    results.sort((a, b) => (b[metric] || 0) - (a[metric] || 0));

    return NextResponse.json(results);

  } catch (err) {
    console.error("Details error:", err);
    return NextResponse.json(
      { error: err.message },
      { status: 500 }
    );
  }
}