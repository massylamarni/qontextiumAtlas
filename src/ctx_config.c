#define _GNU_SOURCE
#include "ctx_config.h"
#include "qtxium_interface.h"
#include "../lib/cJSON.h"

void print_ctx_conf(ctx_conf ctx_conf_i) {
  printf("file_name: %s\n", ctx_conf_i.file_name);
  printf("format: %s\n", ctx_format_to_qtxium[ctx_conf_i.format]);
  printf("qubits_count: %i\n", ctx_conf_i.qubits_count);
  printf("ctx_degree: %i\n", ctx_conf_i.ctx_degree);
  printf("ctx_count: %i\n", ctx_conf_i.ctx_count);
  printf("neg_ctx_count: %i\n", ctx_conf_i.neg_ctx_count);
  printf("best_hamming_distance: %i\n", ctx_conf_i.best_hamming_distance);
}

int is_file_valid(char *file_name) {
  return is_ctx_conf_valid(exec_qtxium(file_name, "assignment"));
}

int is_ctx_conf_valid(ctx_conf ctx_conf_i) {
  return ctx_conf_i.ctx_degree != 0;
}

static int scan_dimension(const char *filename, size_t *row_count_p,
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

ctx_conf load_ctx_conf_info(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) return (ctx_conf){0};

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);

  char *data = malloc(length + 1);
  fread(data, 1, length, file);
  data[length] = '\0';
  fclose(file);

  cJSON *json = cJSON_Parse(data);
  free(data);

  if (!json) return (ctx_conf){0};
  ctx_conf conf;

  strcpy(conf.file_name, cJSON_GetObjectItem(json, "file_name")->valuestring);
  
  conf.format = qtxium_to_ctx_format(cJSON_GetObjectItem(json, "format")->valuestring);

  conf.qubits_count = cJSON_GetObjectItem(json, "qubits_count")->valueint;

  conf.ctx_degree = cJSON_GetObjectItem(json, "ctx_degree")->valueint;

  conf.ctx_count = cJSON_GetObjectItem(json, "ctx_count")->valueint;

  conf.neg_ctx_count = cJSON_GetObjectItem(json, "neg_ctx_count")->valueint;

  conf.best_hamming_distance = cJSON_GetObjectItem(json, "best_hamming_distance")->valueint;

  conf.id = 0;
  conf.dimension = 0;
  conf.observable_count = 0;

  cJSON_Delete(json);
  return conf;
}