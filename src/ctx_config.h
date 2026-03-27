#ifndef CTX_CONFIG
#define CTX_CONFIG

#define FILE_NAME_SIZE 64

#include "ctx_config_formats.h"

typedef struct {
  int id;
  char file_name[FILE_NAME_SIZE];
  ctx_format format;
  int qubits_count;
  int ctx_degree;
  int ctx_count;
  int neg_ctx_count;
  int best_hamming_distance;
  int dimension;
  int observable_count;
} ctx_conf;

typedef struct {
  int id;
  char file_name[FILE_NAME_SIZE];
  ctx_format format;
  int qubits_count;
  int ctx_degree;
  int ctx_count;
  int neg_ctx_count;
  int best_hamming_distance;
  int dimension;
  int observable_count;
} search_filters;

void print_ctx_conf(ctx_conf ctx_conf_i);
int is_file_valid(char *file_name);
int is_ctx_conf_valid(ctx_conf ctx_conf_i);

/* TODO */

void search_ctx_configs(search_filters sf, pauli_matrix pm[128]);

#endif