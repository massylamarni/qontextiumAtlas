#ifndef CTX_CONFIG
#define CTX_CONFIG

#define FILE_NAME_SIZE 64

#include "ctx_config_formats.h"
#include "lib/cJSON.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int id;
  ctx_format format;
  char file_name[FILE_NAME_SIZE];

#define X(kind, type, name) _X_##kind(type, name)
#define _X_INT(type, name) type name;
#define _X_STR(type, name) char name[128];
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
} ctx_conf;

typedef struct {
  int min;
  int max;
} s_interval;

typedef struct {
  unsigned int id;
  ctx_format format;

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) s_interval name;
#define _X_STR(name) char name[128];
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
} search_filters;

void print_ctx_conf(ctx_conf ctx_conf_i);
int is_file_valid(char *file_name);
int is_ctx_conf_valid(ctx_conf ctx_conf_i);
int scan_dimension(const char *dir_name, size_t *row_count_p,
                   size_t *col_count_p, size_t *n_qubits_p);
pauli_matrix load_ctx_config(const char *dir_name);
void free_pauli_matrix(pauli_matrix *m);
void save_ctx_config(const char *dir_name, const pauli_matrix *pm);
int load_json_file(const char *dir_name, cJSON **json);
int save_json_file(const char *dir_name, cJSON *json);
ctx_conf load_ctx_config_info(const char *dir_name);
void save_ctx_config_info(const char *dir_name, const ctx_conf *conf);
pauli_matrix *load_ctx_configs(const char *dir_name, size_t *out_count);
ctx_conf *load_ctx_configs_info(const char *dir_name, size_t *out_count);
void search_ctx_configs(const char *dir_name, size_t *out_count,
                        search_filters sf, ctx_conf configs_info[128]);
search_filters init_search_filters();
#endif