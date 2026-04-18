[--run <format> <path>] [--saveas <author_name>]? [--pick <attr>]?
./build/o --run assignment localdb/ctx/ctx1.txt
[--get [<filter> = <max>:<min>]*] [--conf]? (INT_MIN = -, INT_MAX = +)
./build/o --get ctx_degree=1:3 ctx_count=1:6
./build/o --get ctx_degree=2 ctx_count=1:6
./build/o --get ctx_degree=-:+ ctx_count=1:6
--serve <port>
./build/o --serve 8080

# Basic run
curl -X POST http://localhost:8080/run \
  -H "Content-Type: application/json" \
  -d '{"format": "assignment", "path": "localdb/ctx/ctx1.txt"}'
{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":""}

# With --saveas
curl -X POST http://localhost:8080/run \
  -H "Content-Type: application/json" \
  -d '{"format": "assignment", "path": "localdb/ctx/ctx1.txt", "saveas": "jhonny"}'
{"id":"07fb57bc-5dc3-4cbc-9c5c-740792b831e9","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"jhonny"}

# With --pick
curl -X POST http://localhost:8080/run \
  -H "Content-Type: application/json" \
  -d '{"format": "assignment", "path": "localdb/ctx/ctx1.txt", "pick": "ctx_degree"}'

# ctx_degree=-:+ ctx_count=1:6  (unbounded range)
curl -X POST http://localhost:8080/get \
  -H "Content-Type: application/json" \
  -d '{"filters": ["ctx_degree=-:+", "ctx_count=1:6"]}'
{"count":5,"results":[{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":""},{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":2,"ctx_count":3,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":""},{"id":"07fb57bc-5dc3-4cbc-9c5c-740792b831e9","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"jhonny"},{"id":"c007b900-4e2e-4c87-9a36-b78f724ab641","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"jhonny"},{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":3,"ctx_count":1,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":""}]}

# ctx_degree=2 ctx_count=1:6  (single value)
curl -X POST http://localhost:8080/get \
  -H "Content-Type: application/json" \
  -d '{"filters": ["ctx_degree=2", "ctx_count=1:6"]}'
{"count":1,"results":[{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":2,"ctx_count":3,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":""}]}

# With show_conf
curl -X POST http://localhost:8080/get \
  -H "Content-Type: application/json" \
  -d '{"filters": ["ctx_degree=1:3"], "show_conf": true}'
{"count":5,"results":[{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"","ctx_conf":"YZ,ZX,XY\nZY,XZ,YX\nXX,YY,ZZ\nYZ,ZY,XX\nZX,XZ,YY\nXY,YX,ZZ\n"},{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":2,"ctx_count":3,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"","ctx_conf":"YZ,ZX,XY\nZY,XZ,YX\nXX,YY,ZZ\nYZ,ZY,XX\nZX,XZ,YY\nXY,YX,ZZ\n"},{"id":"07fb57bc-5dc3-4cbc-9c5c-740792b831e9","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"jhonny","ctx_conf":"ZZ,ZX,XZ\nZZ,XZ,ZX\nXX,ZZ,ZZ\nZZ,ZZ,XX\nZX,XZ,ZZ\nXZ,ZX,ZZ\n"},{"id":"c007b900-4e2e-4c87-9a36-b78f724ab641","format":"assignment","qubits_count":-1,"ctx_degree":1,"ctx_count":6,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"jhonny","ctx_conf":"YZ,ZX,XY\nZY,XZ,YX\nXX,YY,ZZ\nYZ,ZY,XX\nZX,XZ,YY\nXY,YX,ZZ\n"},{"id":"00000000-0000-0000-0000-000000000000","format":"assignment","qubits_count":-1,"ctx_degree":3,"ctx_count":1,"neg_ctx_count":3,"best_hamming_distance":1,"dimension":0,"observable_count":0,"author_name":"","ctx_conf":"YZ,ZX,XY\nZY,XZ,YX\nXX,YY,ZZ\nYZ,ZY,XX\nZX,XZ,YY\nXY,YX,ZZ\n"}]}
