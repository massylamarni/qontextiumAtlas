import { NextResponse } from 'next/server';
import { connectToDatabase } from '../../../lib/mongodb';

export async function POST(req) {
  try {
    const data = await req.json();
    
    const db = await connectToDatabase();
    const collection = db.collection('quantic_config');

    const newQC = await collection.insertOne({
      data: data.data,
      createdAt: new Date(),
    });

    return NextResponse.json({ qcId: newQC.insertedId }, { status: 201 });
  } catch (error) {
    console.error('Error saving quantic configuration:', error);
    return NextResponse.json({ error: 'Failed to save quantic configuration' }, { status: 500 });
  }
}
