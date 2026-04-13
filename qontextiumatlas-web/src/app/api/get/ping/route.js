import { NextResponse } from 'next/server';

export async function GET(req) {
  try {
    const url = new URL(req.url);
    const ping = url.searchParams.get('ping');
    
    return NextResponse.json({ pong: ping }, { status: 200 });
  } catch (error) {
    return NextResponse.json({ error: 'Wrong format: {"ping": "message"}' }, { status: 500 });
  }
}
