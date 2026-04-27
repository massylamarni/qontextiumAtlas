#ifndef CTX_CONFIG
#define CTX_CONFIG

#define FILE_NAME_SIZE 64

#include "ctx_config_formats.h"
#include "lib/cjson/cJSON.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned char id[16];
  ctx_format format;
  char dir_name[FILE_NAME_SIZE];

#define X(kind, type, name) _X_##kind(type, name)
#define _X_INT(type, name) type name;
#define _X_STR(type, name) char name[128];
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
} ctx_conf_info;

typedef enum {
  SORT_NONE,
  SORT_ASC,
  SORT_DESC,
} sort_dir_t;

typedef struct {
  int min;
  int max;
  bool use_lowest;
  bool use_greatest;
  sort_dir_t sort_dir;
  bool invalid;
  int arg_index;
} s_interval;

typedef struct {
  int field_offset[2]; // offsetof the field within ctx_conf_info
  int dir[2];          // +1 asc, -1 desc
  int keys;            // 1 or 2
} sort_ctx_t;

typedef struct {
  unsigned char id[16];
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
  unsigned char *id;
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
void search_ctx_configs_info(const char *dir_name, size_t *out_count,
                             search_filters sf,
                             ctx_conf_info configs_info[128]);
search_filters parse_search_filters(int argc, char *argv[]);
s_interval parse_interval(const char *val, int arg_index);

/* Public */
void search_ctx_configs(const char *dir_name, size_t *out_count,
                        search_filters sf, ctx_conf_info configs_info[128],
                        pauli_matrix pms[128]);
void fprint_ctx_conf_info(FILE *f, ctx_conf_info conf_info);

void add_ctx_config(ctx_conf_info *conf_info, const pauli_matrix *pm,
                    const char *author_name);
ctx_conf_info get_ctx_config_info(const char *dir_name);
void list_ctx_config_attributes(search_filters sfs, inclusion_filters ifs);
int is_ctx_conf_valid(const char *dir_name);
int is_ctx_conf_info_valid(ctx_conf_info conf_info);

#endif