#include "ctx_config_formats.h"
#include "qtxium_interface.h"

int main(int argc, char *argv[]) {
  init_interface(argc, argv);
  pauli_matrix pm = load_ctx_config("ctxs/ctx1.txt");
  print_pauli_matrix(pm);
  
  if (pm.pauli_rows != NULL) {
    free_pauli_matrix(&pm);
  }

  return 0;
}