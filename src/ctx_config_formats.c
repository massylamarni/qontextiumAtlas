#include "ctx_config_formats.h"
#include <stdlib.h>

const ctx_format qtxium_to_ctx_format(const char *s) {
  for (int i = 0; i <= GRAPH; i++) {
    if (strcmp(s, ctx_format_to_qtxium[i]) == 0)
      return (ctx_format)i;
  }
  return (ctx_format)-1;
}

char pauli_to_char(pauli_operator op) {
  switch (op) {
  case PAULI_I:
    return 'I';
  case PAULI_X:
    return 'X';
  case PAULI_Y:
    return 'Y';
  case PAULI_Z:
    return 'Z';
  default:
    return '?';
  }
}

int pauli_to_int(pauli_operator op) {
  switch (op) {
  case PAULI_I:
    return 0;
  case PAULI_X:
    return 1;
  case PAULI_Y:
    return 2;
  case PAULI_Z:
    return 3;
  default:
    return -1;
  }
}

pauli_operator char_to_pauli(char c) {
  switch (c) {
  case 'I':
    return PAULI_I;
  case 'X':
    return PAULI_X;
  case 'Y':
    return PAULI_Y;
  case 'Z':
    return PAULI_Z;
  default:
    return -1;
  }
}

void fprint_pauli_matrix(FILE *f, const pauli_matrix *pm) {
  for (size_t r = 0; r < pm->row_count; r++) {
    for (size_t c = 0; c < pm->col_count; c++) {
      pauli_row *ps = &pm->pauli_rows[r * pm->col_count + c];
      for (size_t q = 0; q < ps->n_qubits; q++) {
        fputc(pauli_to_char(ps->ops[q]), f);
      }
      if (c < pm->col_count - 1) {
        fputc(',', f);
      }
    }
    fputc('\n', f);
  }
}

void free_pauli_matrix(pauli_matrix *m) {
  for (size_t i = 0; i < m->row_count * m->col_count; i++) {
    free(m->pauli_rows[i].ops);
  }
  free(m->pauli_rows);
}

int **alloc_matrix(size_t rows, size_t cols) {
    int **m = malloc(rows * sizeof(int *));
    for (size_t i = 0; i < rows; i++) {
        m[i] = calloc(cols, sizeof(int)); 
    }
    return m;
}

void pauli_to_hypergram(pauli_matrix pm, hypergram hg){
    hg.n_edges = pm.row_count;
    hg.n_vertices = pm.col_count;
    hg.obs = alloc_matrix(hg.n_edges, hg.n_vertices);

    for (size_t i = 0; i < pm.row_count; i++) {
        for (size_t j = 0; j < pm.col_count; j+=pm.pauli_rows[i].n_qubits) {
            int id_qubits = 1;
            int multi = 1;
            for (size_t k = pm.pauli_rows[i].n_qubits-1; k > -1; k--) {
                pauli_operator op = pm.pauli_rows[i].ops[j+k];
                id_qubits += multi*pauli_to_int(op);
                multi *= 4;
            }
            hg.obs[i][j] = id_qubits;
        }
    }
}
