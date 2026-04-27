import { NextResponse } from "next/server";

export async function POST(req) {
  try {
    const body = await req.json();

  const res = await fetch("http://bore.pub:17722/get", {
    method: "POST",
    headers: {
      "Content-Type": "application/json"
    },
    body: JSON.stringify(body)
  });

  const data = await res.json();

  return NextResponse.json(data);
  } catch (error) {
  console.error("API ERROR:", error);

  return NextResponse.json(
    { error: error.message },
    { status: 500 }
  );
}
}