import { NextResponse } from 'next/server';
import { connectToDatabase } from '@/lib/mongodb';

export async function GET(req) {
  try {
    const url = new URL(req.url);
    const id = url.searchParams.get('id');

    const db = await connectToDatabase();
    const collection = db.collection('quantic_config');
    
    const records = await collection.find({ id: id }).sort({ createdAt: 1 }).toArray();
    return NextResponse.json(records);
  } catch (err) {
    console.error('Error connecting to MongoDB:', err);
    return NextResponse.json({ error: err.message }, { status: 500 });
  }
}
