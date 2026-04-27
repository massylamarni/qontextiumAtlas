import { NextResponse } from 'next/server';

const BACKEND_URL = 'http://localhost:8080';

export async function POST(req) {
  try {
    const body = await req.json();

    // Normalise author_name to lowercase — the backend uses strcmp,
    // so we lowercase here to give the UI a case-insensitive feel.
    if (Array.isArray(body.filters)) {
      body.filters = body.filters.map(f =>
        f.startsWith('author_name=')
          ? `author_name=${f.slice('author_name='.length).toLowerCase()}`
          : f
      );
    }

    const response = await fetch(`${BACKEND_URL}/get`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body),
    });

    if (!response.ok) {
      const text = await response.text();
      return NextResponse.json({ error: text }, { status: response.status });
    }

    const data = await response.json();
    return NextResponse.json(data, { status: 200 });
  } catch (error) {
    return NextResponse.json({ error: 'Backend unreachable: ' + error.message }, { status: 503 });
  }
}
