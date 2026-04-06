#ifndef CTX_CONFIG
#define CTX_CONFIG

#define FILE_NAME_SIZE 64

#include "ctx_config_formats.h"
#include "lib/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>


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
static int scan_dimension(const char *dir_name, size_t *row_count_p, size_t *col_count_p, size_t *n_qubits_p);
pauli_matrix load_ctx_config(const char *dir_name);
void free_pauli_matrix(pauli_matrix *m);
void save_ctx_config(const char *dir_name, const pauli_matrix *pm);
int load_json_file(const char *dir_name, cJSON **json);
int save_json_file(const char *dir_name, cJSON *json);
ctx_conf load_ctx_config_info(const char *dir_name);
void save_ctx_config_info(const char *dir_name, const ctx_conf *conf);
pauli_matrix* load_ctx_configs(const char *dir_name, size_t *out_count);
ctx_conf* load_ctx_configs_info(const char *dir_name, size_t *out_count);

/* TODO */

void search_ctx_configs(search_filters sf, pauli_matrix pm[128]);

#endif