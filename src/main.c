#include "ctx_config.h"
#include "ctx_config_formats.h"
#include "qtxium_interface.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  init_interface(argc, argv);
  pauli_matrix pm = load_ctx_config("ctxs/ctx1.txt");
  fprint_pauli_matrix(stdout, &pm);
  save_ctx_config("ctxs/new_ctx1.txt", &pm);

  ctx_conf conf = load_ctx_config_info("ctxs/ctx1.json");
  print_ctx_conf(conf);
  save_ctx_config_info("ctxs/new_ctx1.json", &conf);
  free_pauli_matrix(&pm);

  size_t out_count = 0;
  pauli_matrix *pms = load_ctx_configs("ctxs/batch", &out_count);

  for (int i = 0; i < out_count; i++) {
    printf("-------------------\n");
    fprint_pauli_matrix(stdout, &pms[i]);
  }

  ctx_conf *configs_info = load_ctx_configs_info("ctxs/jbatch", &out_count);
  for (int i = 0; i < out_count; i++) {
    printf("-------------------\n");
    print_ctx_conf(configs_info[i]);
  }

  return 0;
}