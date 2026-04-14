#include "ctx_config.h"
#include "localdb.h"
#include "qtxium_interface.h"
#include <stdio.h>

/* Private */
int scan_dimension(const char *filename, size_t *row_count_p,
                   size_t *col_count_p, size_t *n_qubits_p) {
  FILE *f = fopen(filename, "r");
  if (!f) {
    perror(filename);
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
        fprintf(stderr, "Column mismatch in %s\n", filename);
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

int load_json_file(const char *filename, cJSON **json) {
  FILE *file = fopen(filename, "r");
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

int save_json_file(const char *filename, cJSON *json) {
  if (!json)
    return 0;

  char *string = cJSON_Print(json);

  FILE *file = fopen(filename, "w");
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

void search_ctx_configs_info(const char *dir_name, size_t *out_count,
                             search_filters sf,
                             ctx_conf_info configs_info[128]) {
  ctx_conf_info *loaded_config_infos = load_ctx_configs_info(dir_name, out_count);

#define IN_RANGE(val, interval)                                                \
  ((interval).min == -1 || (val) >= (interval).min) &&                         \
      ((interval).max == -1 || (val) <= (interval).max)

  int found = 0;
  for (size_t i = 0; i < *out_count && found < 128; i++) {
    ctx_conf_info *c = &loaded_config_infos[i];
    if (1
#define X(kind, type, name) &&_X_##kind(name)
#define _X_INT(name) IN_RANGE(c->name, sf.name)
#define _X_STR(name) (sf.name[0] == '\0' || strcmp(c->name, sf.name) == 0)
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
    ) {
      configs_info[found++] = *c;
    }
  }

#undef IN_RANGE

  *out_count = found;
  free(loaded_config_infos);
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
  pauli_matrix *loaded_pms =
      load_ctx_configs(CTX_CONF_DIR, &loaded_count, (const char **)dir_names,
                       *out_count);
  for (size_t i = 0; i < loaded_count; i++) {
    pms[i] = loaded_pms[i];
  }
  free(loaded_pms);
  *out_count = loaded_count;
  if (loaded_count == 0)
    printf("Error loading all configurations !\n");
}

void print_ctx_conf_info(ctx_conf_info conf_info) {
  printf("format: %s\n", ctx_format_to_qtxium[conf_info.format]);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) printf(#name ": %d\n", conf_info.name);
#define _X_STR(name) printf(#name ": %s\n", conf_info.name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
}

// new
void add_ctx_config(ctx_conf_info conf_info, const pauli_matrix *pm,
                    const char *author_name) {
  char *save_dir;
  get_new_id(conf_info.id);
  strcpy(conf_info.author_name, author_name);
  snprintf(save_dir, 32, "%s/%s", CTX_CONF_DIR, conf_info.id);
  save_ctx_config(save_dir, pm);
  snprintf(save_dir, 32, "%s/%s", CTX_CONF_INFO_DIR, conf_info.id);
  save_ctx_config_info(save_dir, &conf_info);
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