#define _GNU_SOURCE
#include "ctx_config.h"
#include "lib/cJSON.h"
#include "qtxium_interface.h"
#include <stdio.h>
#include <string.h>

void print_ctx_conf(ctx_conf c) {
  printf("format: %s\n", ctx_format_to_qtxium[c.format]);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) printf(#name ": %d\n", c.name);
#define _X_STR(name) printf(#name ": %s\n", c.name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
}

int is_file_valid(char *file_name) {
  return is_ctx_conf_valid(exec_qtxium(file_name, "assignment"));
}

int is_ctx_conf_valid(ctx_conf ctx_conf_i) {
  return ctx_conf_i.ctx_degree != 0;
}

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

pauli_matrix load_ctx_config(const char *filename) {
  size_t row_count, col_count, n_qubits;
  pauli_matrix pm = {0};

  if (!scan_dimension(filename, &row_count, &col_count, &n_qubits))
    return (pauli_matrix){0};

  FILE *f = fopen(filename, "r");
  if (!f) {
    perror(filename);
    return (pauli_matrix){0};
  }

  pm.row_count = row_count;
  pm.col_count = col_count;
  pm.pauli_rows = malloc(row_count * col_count * sizeof(pauli_row));

  if (!pm.pauli_rows) {
    fprintf(stderr, "Failed to allocate memory for pauli_rows\n");
    fclose(f);
    return (pauli_matrix){0};
  }

  char line[512];
  size_t r = 0;

  while (fgets(line, sizeof(line), f)) {
    line[strcspn(line, "\n")] = 0;

    char *saveptr;
    char *token = strtok_r(line, ",", &saveptr);

    size_t c = 0;
    while (token) {
      if (strlen(token) != n_qubits) {
        fprintf(stderr,
                "Inconsistent qubit count in %s: expected %zu, got %zu\n",
                filename, n_qubits, strlen(token));
        fclose(f);
        free_pauli_matrix(&pm);
        return (pauli_matrix){0};
      }

      pauli_row *ps = &pm.pauli_rows[r * col_count + c];

      ps->n_qubits = n_qubits;
      ps->ops = malloc(n_qubits * sizeof(pauli_operator));

      if (!ps->ops) {
        fprintf(stderr, "Failed to allocate memory for ops\n");
        fclose(f);
        free_pauli_matrix(&pm);
        return (pauli_matrix){0};
      }

      for (size_t i = 0; i < n_qubits; i++) {
        pauli_operator op = char_to_pauli(token[i]);
        if (op == -1) {
          fprintf(stderr, "Invalid char '%c' in %s at row %zu col %zu\n",
                  token[i], filename, r, c);
          fclose(f);
          free_pauli_matrix(&pm);
          return (pauli_matrix){0};
        }
        ps->ops[i] = op;
      }

      c++;
      token = strtok_r(NULL, ",", &saveptr);
    }

    r++;
  }

  fclose(f);
  return pm;
}

void free_pauli_matrix(pauli_matrix *m) {
  for (size_t i = 0; i < m->row_count * m->col_count; i++) {
    free(m->pauli_rows[i].ops);
  }
  free(m->pauli_rows);
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

void save_ctx_config(const char *dir_name, const pauli_matrix *pm) {
  FILE *f = fopen(dir_name, "w");
  if (!f)
    return;

  fprint_pauli_matrix(f, pm);

  fclose(f);
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

ctx_conf load_ctx_config_info(const char *filename) {
  cJSON *json = NULL;
  if (!load_json_file(filename, &json))
    return (ctx_conf){0};

  ctx_conf conf = {0};
  cJSON *item = NULL;

  item = cJSON_GetObjectItem(json, "format");
  if (item && item->valuestring)
    conf.format = qtxium_to_ctx_format(item->valuestring);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name)                                                           \
  item = cJSON_GetObjectItem(json, #name);                                     \
  if (item)                                                                    \
    conf.name = item->valueint;
#define _X_STR(name)                                                           \
  item = cJSON_GetObjectItem(json, #name);                                     \
  if (item && item->valuestring)                                               \
    strncpy(conf.name, item->valuestring, sizeof(conf.name) - 1);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR

  cJSON_Delete(json);
  return conf;
}

void save_ctx_config_info(const char *filename, const ctx_conf *conf) {
  cJSON *json = cJSON_CreateObject();

  cJSON_AddStringToObject(json, "format", ctx_format_to_qtxium[conf->format]);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) cJSON_AddNumberToObject(json, #name, conf->name);
#define _X_STR(name) cJSON_AddStringToObject(json, #name, conf->name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR

  if (!save_json_file(filename, json))
    fprintf(stderr, "Error saving ctx config!\n");
  cJSON_Delete(json);
}

pauli_matrix *load_ctx_configs(const char *dir_name, size_t *out_count) {
  DIR *dir = opendir(dir_name);
  if (!dir)
    return NULL;

  pauli_matrix *list = NULL;
  size_t count = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char path[512]; // build path
    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

    pauli_matrix pm = load_ctx_config(path);

    if (!pm.pauli_rows)
      continue; // check if valid
    pauli_matrix *tmp = realloc(list, (count + 1) * sizeof(pauli_matrix));
    if (!tmp) {
      free_pauli_matrix(&pm);
      break;
    }
    list = tmp;
    list[count++] = pm;
  }
  closedir(dir);
  *out_count = count;
  return list;
}

ctx_conf *load_ctx_configs_info(const char *dir_name, size_t *out_count) {
  DIR *dir = opendir(dir_name);
  if (!dir)
    return NULL;

  ctx_conf *list = NULL;
  size_t count = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char path[512]; // build path
    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

    ctx_conf conf = load_ctx_config_info(path);
    ctx_conf *tmp = realloc(list, (count + 1) * sizeof(ctx_conf));
    list = tmp;
    list[count++] = conf;
  }
  closedir(dir);
  *out_count = count;
  return list;
}

void search_ctx_configs(const char *dir_name, size_t *out_count,
                        search_filters sf, ctx_conf configs_info[128]) {
  ctx_conf *loaded_configs = load_ctx_configs_info(dir_name, out_count);

#define IN_RANGE(val, interval)                                                \
  ((interval).min == -1 || (val) >= (interval).min) &&                         \
      ((interval).max == -1 || (val) <= (interval).max)

  int found = 0;
  for (size_t i = 0; i < *out_count && found < 128; i++) {
    ctx_conf *c = &loaded_configs[i];
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
  free(loaded_configs);
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