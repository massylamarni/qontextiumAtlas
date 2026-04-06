#include "ctx_config.h"
#include "ctx_config_formats.h"
#include "qtxium_interface.h"

int main(int argc, char *argv[]) {
  init_interface(argc, argv);
  pauli_matrix pm = load_ctx_config("ctxs/ctx1.txt");
  fprint_pauli_matrix(stdout, &pm);
  save_ctx_config("ctxs/new_ctx1.txt", &pm);

  ctx_conf conf = load_ctx_conf_info("ctxs/ctx1.json");
  print_ctx_conf(conf);
  save_ctx_conf_info("ctxs/new_ctx1.json", &conf);
  free_pauli_matrix(&pm);

  return 0;
}