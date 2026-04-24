# `o` — CLI & HTTP Reference

## CLI Usage

### `--run` — Process a file

```
./build/o --run <format> <path> [--saveas <author_name>] [--pick <attr>]
```

**Example:**
```bash
./build/o --run assignment localdb/ctx/ctx1.txt
```

---

### `--get` — Query stored results

```
./build/o --get [<filter>=<min>:<max>]* [--conf]
```

Ranges use integer values. Use `-` for `INT_MIN` and `+` for `INT_MAX`.

**Examples:**
```bash
# Exact value for ctx_degree, range for ctx_count
./build/o --get ctx_degree=2 ctx_count=1:6

# Range for ctx_degree
./build/o --get ctx_degree=1:3 ctx_count=1:6

# Unbounded range (INT_MIN to INT_MAX)
./build/o --get ctx_degree=-:+ ctx_count=1:6
```

---

### `--serve` — Start HTTP server

```
./build/o --serve <port>
```

**Example:**
```bash
./build/o --serve 8080
```

---

## HTTP API

### `POST /run`

Process a file, with optional save and field selection.

**Request fields:**

| Field | Type | Description |
|-------|------|-------------|
| `format` | string | File format (e.g. `"assignment"`) |
| `path` | string | Path to input file |
| `saveas` | string *(optional)* | Author name to associate with the result |
| `pick` | string *(optional)* | Return only this field from the result |

**Basic run:**
```bash
curl -X POST http://localhost:8080/run \
  -H "Content-Type: application/json" \
  -d '{"format": "assignment", "path": "localdb/ctx/ctx1.txt"}'
```
```json
{
  "id": "00000000-0000-0000-0000-000000000000",
  "format": "assignment",
  "qubits_count": -1,
  "ctx_degree": 1,
  "ctx_count": 6,
  "neg_ctx_count": 3,
  "best_hamming_distance": 1,
  "dimension": 0,
  "observable_count": 0,
  "author_name": ""
}
```

**With `saveas`:**
```bash
curl -X POST http://localhost:8080/run \
  -H "Content-Type: application/json" \
  -d '{"format": "assignment", "path": "localdb/ctx/ctx1.txt", "saveas": "jhonny"}'
```
```json
{
  "id": "07fb57bc-5dc3-4cbc-9c5c-740792b831e9",
  "format": "assignment",
  "qubits_count": -1,
  "ctx_degree": 1,
  "ctx_count": 6,
  "neg_ctx_count": 3,
  "best_hamming_distance": 1,
  "dimension": 0,
  "observable_count": 0,
  "author_name": "jhonny"
}
```

**With `pick`:**
```bash
curl -X POST http://localhost:8080/run \
  -H "Content-Type: application/json" \
  -d '{"format": "assignment", "path": "localdb/ctx/ctx1.txt", "pick": "ctx_degree"}'
```

---

### `POST /get`

Query stored results by filter.

**Request fields:**

| Field | Type | Description |
|-------|------|-------------|
| `filters` | string[] | Filter expressions (e.g. `"ctx_degree=1:3"`) |
| `show_conf` | bool *(optional)* | Include `ctx_conf` in each result |

Filter syntax: `<attr>=<min>:<max>` for a range, or `<attr>=<value>` for an exact match. Use `-:+` for an unbounded range.

**Unbounded range:**
```bash
curl -X POST http://localhost:8080/get \
  -H "Content-Type: application/json" \
  -d '{"filters": ["ctx_degree=-:+", "ctx_count=1:6"]}'
```
```json
{
  "count": 5,
  "results": [
    { "id": "00000000-...", "ctx_degree": 1, "ctx_count": 6, "author_name": "", ... },
    { "id": "00000000-...", "ctx_degree": 2, "ctx_count": 3, "author_name": "", ... },
    { "id": "07fb57bc-...", "ctx_degree": 1, "ctx_count": 6, "author_name": "jhonny", ... },
    { "id": "c007b900-...", "ctx_degree": 1, "ctx_count": 6, "author_name": "jhonny", ... },
    { "id": "00000000-...", "ctx_degree": 3, "ctx_count": 1, "author_name": "", ... }
  ]
}
```

**Exact value match:**
```bash
curl -X POST http://localhost:8080/get \
  -H "Content-Type: application/json" \
  -d '{"filters": ["ctx_degree=2", "ctx_count=1:6"]}'
```
```json
{
  "count": 1,
  "results": [
    { "id": "00000000-...", "ctx_degree": 2, "ctx_count": 3, "author_name": "", ... }
  ]
}
```

**With `show_conf`:**
```bash
curl -X POST http://localhost:8080/get \
  -H "Content-Type: application/json" \
  -d '{"filters": ["ctx_degree=1:3"], "show_conf": true}'
```

Each result will include a `ctx_conf` field, e.g.:
```
YZ,ZX,XY
ZY,XZ,YX
XX,YY,ZZ
...
```