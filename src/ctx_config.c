#include "ctx_config.h"
#include "localdb.h"
#include "qtxium_interface.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Private */
int scan_dimension(const char *dir_name, size_t *row_count_p,
                   size_t *col_count_p, size_t *n_qubits_p) {
  FILE *f = fopen(dir_name, "r");
  if (!f) {
    perror(dir_name);
    return 0;
  }

  char line[512];
  size_t row_count = 0, col_count = 0, n_qubits = 0;

  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = 0;

    size_t current_col_count = 1;
    for (char *p = line; *p; p++) {
      if (*p == ',')
        current_col_count++;
    }

    if (row_count == 0) {
      col_count = current_col_count;

      char *comma = strchr(line, ',');
      size_t len = comma ? (size_t)(comma - line) : strlen(line);
      n_qubits = len;
    } else {
      if (current_col_count != col_count) {
        fprintf(stderr, "Column mismatch in %s\n", dir_name);
        fclose(f);
        return 0;
      }
    }

    row_count++;
  }

  fclose(f);

  *row_count_p = row_count;
  *col_count_p = col_count;
  *n_qubits_p = n_qubits;
  return 1;
}

int load_json_file(const char *dir_name, cJSON **json) {
  FILE *file = fopen(dir_name, "r");
  if (!file)
    return 0;

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);

  char *data = malloc(length + 1);
  fread(data, 1, length, file);
  data[length] = '\0';
  fclose(file);

  *json = cJSON_Parse(data);
  free(data);

  if (!*json)
    return 0;
  return 1;
}

int save_json_file(const char *dir_name, cJSON *json) {
  if (!json)
    return 0;

  char *string = cJSON_Print(json);

  FILE *file = fopen(dir_name, "w");
  if (!file) {
    free(string);
    return 0;
  }

  fputs(string, file);
  fclose(file);
  free(string);

  return 1;
}

search_filters init_search_filters() {
  search_filters sf = {0};

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) sf.name = (s_interval){-1, -1};
#define _X_STR(name) sf.name[0] = '\0';
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR

  return sf;
}
static sort_ctx_t g_sort_ctx;
static int cmp_configs(const void *a, const void *b) {
  const ctx_conf_info *ca = (const ctx_conf_info *)a;
  const ctx_conf_info *cb = (const ctx_conf_info *)b;

  for (int k = 0; k < g_sort_ctx.keys; k++) {
    int va = *(int *)((char *)ca + g_sort_ctx.field_offset[k]);
    int vb = *(int *)((char *)cb + g_sort_ctx.field_offset[k]);
    if (va != vb)
      return g_sort_ctx.dir[k] * (va - vb);
  }
  return 0;
}

void search_ctx_configs_info(const char *dir_name, size_t *out_count,
                             search_filters sf,
                             ctx_conf_info configs_info[128]) {
  ctx_conf_info *loaded = load_ctx_configs_info(dir_name, out_count);

  static const unsigned char s_zero_id[16] = {0};
  if (memcmp(sf.id, s_zero_id, 16) != 0) {
    for (size_t i = 0; i < *out_count; i++) {
      if (memcmp(loaded[i].id, sf.id, 16) == 0) {
        configs_info[0] = loaded[i];
        *out_count = 1;
        free(loaded);
        return;
      }
    }
    *out_count = 0;
    free(loaded);
    return;
  }

#define IN_RANGE(val, interval)                                                \
  ((interval).min == -1 || (val) >= (interval).min) &&                         \
      ((interval).max == -1 || (val) <= (interval).max)

  // Resolve g | l
#define X(kind, type, name) _X_RESOLVE_##kind(name)
#define _X_RESOLVE_INT(name)                                                   \
  int global_least_##name = INT_MAX;                                           \
  int global_greatest_##name = INT_MIN;                                        \
  if (sf.name.use_lowest || sf.name.use_greatest) {                            \
    for (size_t _i = 0; _i < *out_count; _i++) {                               \
      if (loaded[_i].name < global_least_##name)                               \
        global_least_##name = loaded[_i].name;                                 \
      if (loaded[_i].name > global_greatest_##name)                            \
        global_greatest_##name = loaded[_i].name;                              \
    }                                                                          \
    if (sf.name.use_lowest)                                                    \
      sf.name.min = global_least_##name;                                       \
    if (sf.name.use_greatest)                                                  \
      sf.name.max = global_greatest_##name;                                    \
    if (sf.name.sort_dir == SORT_NONE) {                                       \
      if (sf.name.use_lowest)                                                  \
        sf.name.max = sf.name.min;                                             \
      if (sf.name.use_greatest)                                                \
        sf.name.min = sf.name.max;                                             \
    }                                                                          \
  }
#define _X_RESOLVE_STR(name)
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_RESOLVE_INT
#undef _X_RESOLVE_STR

  // Collecting sort fields by arg order
  typedef struct {
    int arg_index;
    int offset;
    int dir;
  } sort_field_t;
  sort_field_t sort_fields[8];
  int sort_field_count = 0;

#define X(kind, type, name) _X_SORT_##kind(name)
#define _X_SORT_INT(name)                                                      \
  if (sf.name.sort_dir != SORT_NONE) {                                         \
    sort_fields[sort_field_count++] =                                          \
        (sort_field_t){sf.name.arg_index, (int)offsetof(ctx_conf_info, name),  \
                       sf.name.sort_dir == SORT_ASC ? 1 : -1};                 \
  }
#define _X_SORT_STR(name)
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_SORT_INT
#undef _X_SORT_STR

  // Sort by arg index
  for (int a = 0; a < sort_field_count - 1; a++)
    for (int b = a + 1; b < sort_field_count; b++)
      if (sort_fields[a].arg_index > sort_fields[b].arg_index) {
        sort_field_t tmp = sort_fields[a];
        sort_fields[a] = sort_fields[b];
        sort_fields[b] = tmp;
      }

  // Apply
  int found = 0;
  bool any_invalid = false;

#define X(kind, type, name) _X_INVALID_##kind(name)
#define _X_INVALID_INT(name)                                                   \
  if (sf.name.invalid)                                                         \
    any_invalid = true;
#define _X_INVALID_STR(name)
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INVALID_INT
#undef _X_INVALID_STR

  if (!any_invalid) {
    for (size_t i = 0; i < *out_count && found < 128; i++) {
      ctx_conf_info *c = &loaded[i];
      if (1
#define X(kind, type, name) &&_X_FILTER_##kind(name)
#define _X_FILTER_INT(name) IN_RANGE(c->name, sf.name)
#define _X_FILTER_STR(name)                                                    \
  (sf.name[0] == '\0' || strcmp(c->name, sf.name) == 0)
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_FILTER_INT
#undef _X_FILTER_STR
      ) {
        configs_info[found++] = *c;
      }
    }
  }

  // Sort
  int keys = sort_field_count < 2 ? sort_field_count : 2;
  if (keys > 0 && found > 1) {
    g_sort_ctx.keys = keys;
    for (int k = 0; k < keys; k++) {
      g_sort_ctx.field_offset[k] = sort_fields[k].offset;
      g_sort_ctx.dir[k] = sort_fields[k].dir;
    }
    qsort(configs_info, found, sizeof(ctx_conf_info), cmp_configs);
  }

#undef IN_RANGE
  *out_count = found;
  free(loaded);
}

s_interval parse_interval(const char *val, int arg_index) {
  s_interval interval = {-1, -1, false, false, SORT_NONE, false, arg_index};
  char *colon = strchr(val, ':');

  if (!colon) {
    interval.min = atoi(val);
    interval.max = interval.min;
    return interval;
  }

  char min_str[32] = {0}, max_str[32] = {0};
  size_t min_len = colon - val;
  strncpy(min_str, val,
          min_len < sizeof(min_str) - 1 ? min_len : sizeof(min_str) - 1);
  strncpy(max_str, colon + 1, sizeof(max_str) - 1);

  bool min_empty = (min_str[0] == '\0');
  bool max_empty = (max_str[0] == '\0');
  bool min_is_l = (min_str[0] == 'l' && min_str[1] == '\0');
  bool min_is_g = (min_str[0] == 'g' && min_str[1] == '\0');
  bool max_is_l = (max_str[0] == 'l' && max_str[1] == '\0');
  bool max_is_g = (max_str[0] == 'g' && max_str[1] == '\0');

  if ((min_is_g && max_empty) || (max_is_l && min_empty)) {
    interval.invalid = true;
    return interval;
  }

  if (min_is_l && max_empty) {
    interval.use_lowest = true;
    interval.use_greatest = false;
    interval.sort_dir = SORT_NONE;
    return interval;
  }
  if (max_is_g && min_empty) {
    interval.use_greatest = true;
    interval.use_lowest = false;
    interval.sort_dir = SORT_NONE;
    return interval;
  }

  if (min_empty)
    interval.min = -1;
  else if (min_is_l)
    interval.use_lowest = true;
  else if (min_str[0] == '-')
    interval.min = INT_MIN;
  else if (min_str[0] == '+')
    interval.min = INT_MAX;
  else
    interval.min = atoi(min_str);

  if (max_empty)
    interval.max = -1;
  else if (max_is_g)
    interval.use_greatest = true;
  else if (max_str[0] == '+')
    interval.max = INT_MAX;
  else if (max_str[0] == '-')
    interval.max = INT_MIN;
  else
    interval.max = atoi(max_str);

  if (min_is_l) {
    interval.sort_dir = SORT_ASC;
    if (max_is_g || max_str[0] == '+')
      interval.use_greatest = false, interval.max = -1;
  } else if (max_is_g) {
    interval.sort_dir = SORT_DESC;
    if (min_is_g || min_str[0] == '-')
      interval.use_lowest = false, interval.min = -1;
  } else if (min_is_g && max_is_l) {
    interval.sort_dir = SORT_DESC;
    interval.min = -1;
    interval.max = -1;
  }

  return interval;
}

search_filters parse_search_filters(int argc, char *argv[]) {
  search_filters sf = init_search_filters();

  for (int i = 0; i < argc; i++) {
    char *eq = strchr(argv[i], '=');
    if (!eq) {
      fprintf(stderr, "Ignored unknown arg: %s\n", argv[i]);
      continue;
    }

    *eq = '\0'; /* split key / value in-place */
    const char *key = argv[i];
    const char *val = eq + 1;

    if (!strcmp(key, "id")) {
      if (uuid_parse(val, sf.id) != 0) {
        fprintf(stderr, "Invalid UUID: %s\n", val);
        exit(1);
      }
    }
#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name)                                                           \
  else if (!strcmp(key, #name)) sf.name = parse_interval(val, i);
#define _X_STR(name)                                                           \
  else if (!strcmp(key, #name)) strncpy(sf.name, val, sizeof(sf.name) - 1);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
    else {
      fprintf(stderr, "Unknown filter key: %s\n", key);
      exit(1);
    }

    *eq = '='; /* restore argv */
  }
  return sf;
}

/* Public */
void search_ctx_configs(const char *dir_name, size_t *out_count,
                        search_filters sf, ctx_conf_info configs_info[128],
                        pauli_matrix pms[128]) {
  search_ctx_configs_info(dir_name, out_count, sf, configs_info);
  char *dir_names[128];
  for (size_t i = 0; i < *out_count; i++) {
    dir_names[i] = configs_info[i].dir_name;
  }
  size_t loaded_count = 0;
  pauli_matrix *loaded_pms = load_ctx_configs(
      CTX_CONF_DIR, &loaded_count, (const char **)dir_names, *out_count);
  for (size_t i = 0; i < loaded_count; i++) {
    pms[i] = loaded_pms[i];
  }
  free(loaded_pms);
  *out_count = loaded_count;
}

void fprint_ctx_conf_info(FILE *f, ctx_conf_info conf_info) {
  char uuid_str[37];
  uuid_unparse(conf_info.id, uuid_str);
  fprintf(f, "id: %s\n", uuid_str);
  fprintf(f, "format: %s\n", ctx_format_to_qtxium[conf_info.format]);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) fprintf(f, #name ": %d\n", conf_info.name);
#define _X_STR(name) fprintf(f, #name ": %s\n", conf_info.name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
}

void add_ctx_config(ctx_conf_info *conf_info, const pauli_matrix *pm,
                    const char *author_name) {
  char save_dir[128];
  char uuid_str[37];
  get_new_id(conf_info->id);
  uuid_unparse(conf_info->id, uuid_str);
  snprintf(conf_info->author_name, sizeof(conf_info->author_name), "%s",
           author_name);
  snprintf(save_dir, sizeof(save_dir), "%s/%s", CTX_CONF_DIR, uuid_str);
  save_ctx_config(save_dir, pm);
  snprintf(save_dir, sizeof(save_dir), "%s/%s", CTX_CONF_INFO_DIR, uuid_str);
  save_ctx_config_info(save_dir, conf_info);
}

ctx_conf_info get_ctx_config_info(const char *dir_name) {
  return exec_qtxium(dir_name, "assignment");
}

void list_ctx_config_attributes(search_filters sf, inclusion_filters ifs) {
  ctx_conf_info configs[128];
  size_t out_count = 0;

  search_ctx_configs_info(CTX_CONF_INFO_DIR, &out_count, sf, configs);

  for (size_t i = 0; i < out_count; i++) {
    ctx_conf_info *c = &configs[i];

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name)                                                           \
  if (ifs.name)                                                                \
    printf(#name ": %d\n", c->name);
#define _X_STR(name)                                                           \
  if (ifs.name[0] != '\0')                                                     \
    printf(#name ": %s\n", c->name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
  }
}

int is_ctx_conf_valid(const char *dir_name) {
  return is_ctx_conf_info_valid(exec_qtxium(dir_name, "assignment"));
}

int is_ctx_conf_info_valid(ctx_conf_info conf_info) {
  return conf_info.ctx_degree != 0;
}