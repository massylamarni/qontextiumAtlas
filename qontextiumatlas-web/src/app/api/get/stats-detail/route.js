import { NextResponse } from 'next/server';
import { connectToDatabase } from '@/lib/mongodb';

export async function GET(req) {
  try {
    const url = new URL(req.url);
    const metric = url.searchParams.get('metric'); // 'ctx_degree', 'neg_ctx_count', etc.

    if (!metric) {
      return NextResponse.json({ error: 'Metric parameter required' }, { status: 400 });
    }

    const db = await connectToDatabase();
    const collection = db.collection('quantic_config');

    // Agrégation pour grouper par valeur de la métrique
    const result = await collection.aggregate([
      {
        $group: {
          _id: `$${metric}`,
          count: { $sum: 1 }
        }
      },
      {
        $sort: { _id: -1 } // Tri décroissant
      }
    ]).toArray();

    // Transformer le résultat
    const data = result.map(item => ({
      value: item._id,
      count: item.count
    }));

    return NextResponse.json(data);
  } catch (err) {
    console.error('Error getting detailed stats:', err);
    return NextResponse.json({ error: err.message }, { status: 500 });
  }
}