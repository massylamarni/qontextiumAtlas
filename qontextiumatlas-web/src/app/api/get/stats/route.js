import { NextResponse } from 'next/server';
import { connectToDatabase } from '@/lib/mongodb';

export async function GET(req) {
  try {
    const db = await connectToDatabase();
    const collection = db.collection('quantic_config');

    // Agrégation MongoDB pour récupérer le max de ctx_degree
    const result = await collection.aggregate([
  {
    $group: {
      _id: null,
      totalConfigs: { $sum: 1 },
      maxDegree: { $max: '$ctx_degree' },
      maxNegCtx: { $max: '$neg_ctx_count' },
      maxQubits: { $max: '$qubits_count' } 
    }
  }
]).toArray();

    if (result.length === 0) {
      return NextResponse.json({
        totalConfigs: 0,
        maxDegree: 0,
        maxNegCtx: 0,
        maxHammingDistance: 0,
        maxCtxCount: 0
      });
    }

    return NextResponse.json(result[0]);
  } catch (err) {
    console.error('Error getting statistics:', err);
    return NextResponse.json({ error: err.message }, { status: 500 });
  }
}