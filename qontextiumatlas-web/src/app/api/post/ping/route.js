import { NextResponse } from 'next/server';

export async function POST(req) {
  try {
    const data = await req.json(); // Parse incoming JSON request

    return NextResponse.json({ pong: data.ping }, { status: 200 });
  } catch (error) {
    return NextResponse.json({ error: 'Wrong format: {"ping": "message"}' }, { status: 500 });
  }
}
