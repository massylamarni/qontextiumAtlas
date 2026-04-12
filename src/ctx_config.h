#ifndef CTX_CONFIG
#define CTX_CONFIG

#define FILE_NAME_SIZE 64

#include "ctx_config_formats.h"
#include "lib/cJSON.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
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
} ctx_conf_info;

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

typedef struct {
  unsigned int id;
  ctx_format format;

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) bool name;
#define _X_STR(name) char name[128];
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
} inclusion_filters;

/* Private */
int scan_dimension(const char *dir_name, size_t *row_count_p,
                   size_t *col_count_p, size_t *n_qubits_p);
int load_json_file(const char *dir_name, cJSON **json);
int save_json_file(const char *dir_name, cJSON *json);
search_filters init_search_filters();

/* Public */
void search_ctx_configs(const char *dir_name, size_t *out_count,
                        search_filters sf, ctx_conf_info configs_info[128],
                        pauli_matrix pms[128]);
void print_ctx_conf_info(ctx_conf_info conf_info);
void search_ctx_configs_info(const char *dir_name, size_t *out_count,
                             search_filters sf,
                             ctx_conf_info configs_info[128]);

// new
void add_ctx_config(const char *src_dir_name, const char *dest_dir_name,
                    const pauli_matrix *pm, const char *author_name);
ctx_conf_info get_ctx_config_info(const char *dir_name);
void list_ctx_config_attributes(search_filters sfs, inclusion_filters ifs);
int is_file_valid(char *file_name);
int is_ctx_conf_valid(ctx_conf_info conf_info);

/* TODO */
void is_ctx_config_present();
#endif