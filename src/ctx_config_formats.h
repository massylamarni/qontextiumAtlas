#ifndef CTX_CONFIG_FORMATS
#define CTX_CONFIG_FORMATS

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef enum { PAULI_MATRIX, HYPERGRAM, GRAPH } ctx_format;
typedef enum { PAULI_X, PAULI_Y, PAULI_Z } pauli_operator;

typedef struct {
  pauli_operator *ops;
  size_t n_qubits;
} pauli_row;

typedef struct {
  pauli_row *pauli_rows;
  size_t row_count;
  size_t col_count;
} pauli_matrix;

typedef struct {
  int **obs;
} hypergram;

typedef struct {
  int **matrix;
} anticom_matrix;

static const char ctx_format_to_qtxium[3][16] = {
    [PAULI_MATRIX] = "assignment", [HYPERGRAM] = "hypergram", [GRAPH] = "gram"};

const ctx_format qtxium_to_ctx_format(const char *s);
char pauli_to_char(pauli_operator op);
pauli_operator char_to_pauli(char c);
void fprint_pauli_matrix(FILE *f, const pauli_matrix *pm);
void free_pauli_matrix(pauli_matrix *pm);

/* TODO */

void pauli_to_hypergram(pauli_matrix pm, hypergram hg);
void hypergram_to_pauli(hypergram hg, pauli_matrix pm);
void pauli_to_am(hypergram hg, anticom_matrix am);
void hypergram_to_am(hypergram hg, anticom_matrix am);

#endif