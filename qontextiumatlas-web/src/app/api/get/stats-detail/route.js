import { NextResponse } from 'next/server';

export const runtime = "nodejs";

export async function GET(req) {
  try {
    const url    = new URL(req.url);
    const metric = url.searchParams.get("metric") || "ctx_degree";

    const controller = new AbortController();
    const timeout    = setTimeout(() => controller.abort(), 5000);

    const res = await fetch("http://localhost:8080/get", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ filters: [`${metric}=-:+`], show_conf: true }),
      signal: controller.signal
    });

    clearTimeout(timeout);

    if (!res.ok) {
      return NextResponse.json([], { status: 200 });
    }

    const data    = await res.json().catch(() => ({}));
    const results = (data.results ?? [])
      .sort((a, b) => (b?.[metric] ?? 0) - (a?.[metric] ?? 0));

    return NextResponse.json(results);

  } catch {
    return NextResponse.json([], { status: 200 });
  }
}