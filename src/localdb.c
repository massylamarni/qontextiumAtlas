#include "localdb.h"
#include "ctx_config.h"
#include <errno.h>
#include <stdio.h>

pauli_matrix load_ctx_config(const char *dir_name) {
  size_t row_count, col_count, n_qubits;
  pauli_matrix pm = {0};

  if (!scan_dimension(dir_name, &row_count, &col_count, &n_qubits))
    return (pauli_matrix){0};

  FILE *f = fopen(dir_name, "r");
  if (!f) {
    perror(dir_name);
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
                dir_name, n_qubits, strlen(token));
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
                  token[i], dir_name, r, c);
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
      char entry_base[FILE_NAME_SIZE];
      dir_name_cpy(entry->d_name, entry_base);
      bool found = false;
      for (size_t i = 0; i < dir_names_count; i++) {
        char requested_base[FILE_NAME_SIZE];
        dir_name_cpy(dir_names[i], requested_base);
        if (strcmp(entry_base, requested_base) == 0) {
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

ctx_conf_info load_ctx_config_info(const char *dir_name) {
  cJSON *json = NULL;
  if (!load_json_file(dir_name, &json))
    return (ctx_conf_info){0};

  ctx_conf_info conf_info = {0};
  cJSON *item = NULL;

  item = cJSON_GetObjectItem(json, "id");
  if (item && item->valuestring) {
    if (uuid_parse(item->valuestring, conf_info.id) != 0)
      fprintf(stderr, "Invalid UUID: %s\n", item->valuestring);
  }
  item = cJSON_GetObjectItem(json, "format");
  if (item && item->valuestring)
    conf_info.format = qtxium_to_ctx_format(item->valuestring);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name)                                                           \
  item = cJSON_GetObjectItem(json, #name);                                     \
  if (item)                                                                    \
    conf_info.name = item->valueint;
#define _X_STR(name)                                                           \
  item = cJSON_GetObjectItem(json, #name);                                     \
  if (item && item->valuestring)                                               \
    strncpy(conf_info.name, item->valuestring, sizeof(conf_info.name) - 1);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR

  cJSON_Delete(json);
  return conf_info;
}

ctx_conf_info *load_ctx_configs_info(const char *dir_name, size_t *out_count) {
  DIR *dir = opendir(dir_name);
  if (!dir) {
    *out_count = 0;
    return NULL;
  }

  ctx_conf_info *list = NULL;
  size_t count = 0;

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char path[512]; // build path
    snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

    ctx_conf_info conf = load_ctx_config_info(path);
    dir_name_cpy(entry->d_name, conf.dir_name);
    ctx_conf_info *tmp = realloc(list, (count + 1) * sizeof(ctx_conf_info));
    list = tmp;
    list[count++] = conf;
  }
  closedir(dir);
  *out_count = count;
  return list;
}

void save_ctx_config_info(const char *dir_name, const ctx_conf_info *conf) {
  cJSON *json = cJSON_CreateObject();

  char uuid_str[37];
  uuid_unparse(conf->id, uuid_str);
  cJSON_AddStringToObject(json, "id", uuid_str);
  cJSON_AddStringToObject(json, "format", ctx_format_to_qtxium[conf->format]);

#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) cJSON_AddNumberToObject(json, #name, conf->name);
#define _X_STR(name) cJSON_AddStringToObject(json, #name, conf->name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR

  if (!save_json_file(dir_name, json))
    fprintf(stderr, "Error saving ctx config!\n");
  cJSON_Delete(json);
}

void get_new_id(unsigned char *id) { uuid_generate(id); }

void get_new_name(char *name, ctx_conf_info conf_info) {
  snprintf(name, 32, "ctx_%s", conf_info.id);
}

void dir_name_cpy(const char *src_dir_name, char *dest_dir_name) {
  size_t sizeof_src_dir_name = sizeof(src_dir_name);
  if (sizeof_src_dir_name == 0)
    return;
  strncpy(dest_dir_name, src_dir_name, sizeof_src_dir_name - 1);
  dest_dir_name[sizeof_src_dir_name - 1] = '\0';
  char *dot = strrchr(dest_dir_name, '.');
  if (dot != NULL)
    *dot = '\0';
}