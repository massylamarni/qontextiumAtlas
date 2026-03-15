#include "ctx_config.h"

void print_ctx_conf(ctx_conf ctx_conf_i) {
  printf("file_name: %s\n", ctx_conf_i.file_name);
  printf("format: %s\n", ctx_format_to_qtxium[ctx_conf_i.format]);
  printf("qubits_count: %i\n", ctx_conf_i.qubits_count);
  printf("ctx_degree: %i\n", ctx_conf_i.ctx_degree);
  printf("ctx_count: %i\n", ctx_conf_i.ctx_count);
  printf("neg_ctx_count: %i\n", ctx_conf_i.neg_ctx_count);
  printf("best_hamming_distance: %i\n", ctx_conf_i.best_hamming_distance);
}