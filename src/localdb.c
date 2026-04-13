// #define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "localdb.h"
#include "ctx_config.h"
#include <errno.h>
#include <stdio.h>

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

pauli_matrix *load_ctx_configs(const char *dir_name, size_t *out_count,
                               const char **dir_names, size_t dir_names_count) {
  DIR *dir = opendir(dir_name);
  if (!dir) {
    fprintf(stderr, "opendir failed for %s: %s\n", dir_name, strerror(errno));
    return NULL;
  }

  pauli_matrix *list = NULL;
  size_t count = 0;
  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    if (dir_names != NULL) {
      bool found = false;
      for (size_t i = 0; i < dir_names_count; i++) {
        if (strcmp(entry->d_name, dir_names[i]) == 0) {
          found = true;
          break;
        }
      }
      if (!found)
        continue;
    }

    char path[512];
    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

    pauli_matrix pm = load_ctx_config(path);
    if (!pm.pauli_rows)
      continue;

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

void save_ctx_config(const char *dir_name, const pauli_matrix *pm) {
  FILE *f = fopen(dir_name, "w");
  if (!f)
    return;

  fprint_pauli_matrix(f, pm);

  fclose(f);
}

ctx_conf_info load_ctx_config_info(const char *filename) {
  cJSON *json = NULL;
  if (!load_json_file(filename, &json))
    return (ctx_conf_info){0};

  ctx_conf_info conf = {0};
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

ctx_conf_info *load_ctx_configs_info(const char *dir_name, size_t *out_count) {
  DIR *dir = opendir(dir_name);
  if (!dir)
    return NULL;

  ctx_conf_info *list = NULL;
  size_t count = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char path[512]; // build path
    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

    ctx_conf_info conf = load_ctx_config_info(path);
    ctx_conf_info *tmp = realloc(list, (count + 1) * sizeof(ctx_conf_info));
    list = tmp;
    list[count++] = conf;
  }
  closedir(dir);
  *out_count = count;
  return list;
}

void save_ctx_config_info(const char *filename, const ctx_conf_info *conf) {
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

void get_new_id(unsigned char *id) { uuid_generate(id); }

void get_new_name(char *name, ctx_conf_info conf_info) {
  snprintf(name, 32, "ctx_%s", conf_info.id);
}