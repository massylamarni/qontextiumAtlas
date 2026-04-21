import { NextResponse } from 'next/server';
import { connectToDatabase } from '@/lib/mongodb';

export async function POST() {
  try {
    const db = await connectToDatabase();
    const collection = db.collection("quantic_config");

    const result = await collection.insertMany([
      {
        dir_name: "ctxs/ctx2.txt",
        format:	"assignment",
	    qubits_count:	-1,
	    ctx_degree:	1,
	    ctx_count:	6,
	    neg_ctx_count:	3,
	    best_hamming_distance:	1,
	    dimension:	0,
	    observable_count:	0,
	    author_name:	"jhonny"
      },
      {
        dir_name: "ctxs/ctx1.txt",
        format:	"assignment",
	    qubits_count:	3,
	    ctx_degree:	2,
	    ctx_count:	6,
	    neg_ctx_count:	3,
	    best_hamming_distance:	1,
	    dimension:	0,
	    observable_count:	0,
	    author_name:	"jhonny"
      },
      {
        dir_name: "ctxs/ctx3.txt",
        format:	"assignment",
	    qubits_count:	1,
	    ctx_degree:	3,
	    ctx_count:	6,
	    neg_ctx_count:	3,
	    best_hamming_distance:	1,
	    dimension:	0,
	    observable_count:	0,
	    author_name:	"jhonny"
      }
    ]);

    return NextResponse.json({
      inserted: result.insertedCount
    });

  } catch (error) {
    return NextResponse.json(
      { error: "Seed failed" },
      { status: 500 }
    );
  }
}