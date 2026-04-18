#include "arg_handlers.h"
#include "ctx_config.h"
#include "ctx_config_formats.h"
#include "localdb.h"
#include "qtxium_interface.h"
#include "http_server.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cli_args parse_args(int argc, char *argv[]) {
  cli_args args = {0};

  if (argc < 2)
    return args;

  if (strcmp(argv[1], "--run") == 0) {
    if (argc < 4)
      ponexit("Usage: [--run <format> <path>] [--saveas <author_name>]? [--pick <attr>]?");
    args.cmd = CMD_RUN;
    args.format = argv[2];
    args.path = argv[3];

    for (int i = 4; i < argc; i++) {
      if (strcmp(argv[i], "--saveas") == 0 && i + 1 < argc)
        args.saveas = argv[++i];
      else if (strcmp(argv[i], "--pick") == 0 && i + 1 < argc)
        args.pick = argv[++i];
      else
        ponexit("Unknown flag: %s", argv[i]);
    }
  } else if (strcmp(argv[1], "--get") == 0) {
    args.cmd = CMD_GET;

    int i = 2;
    char *filter_argv[64];

    int filter_argc = 0;
    while (i < argc && argv[i][0] != '-')
      filter_argv[filter_argc++] = argv[i++];

    args.filters = parse_search_filters(filter_argc, filter_argv);

    for (; i < argc; i++) {
      if (strcmp(argv[i], "--conf") == 0)
        args.show_conf = true;
      else
        ponexit("Unknown flag: %s", argv[i]);
    }
  } else if (strcmp(argv[1], "--serve") == 0) {
    if (argc < 3)
      ponexit("Usage: --serve <port>");
    args.cmd = CMD_SERVE;
    args.port = atoi(argv[2]);
  } else {
    ponexit("Unknown command: %s\nUsage: [--get [<filter> = <max>:<min>]*] [--conf]? (INT_MIN = -, INT_MAX = +)", argv[1]);
  }

  return args;
}

void handle_run(cli_args *args) {
  ctx_conf_info conf_info = exec_qtxium(args->path, args->format);
  if (!is_ctx_conf_info_valid(conf_info)) {
    ponexit("Invalid config!");
  }

  if (args->pick) {
    // TODO: print_ctx_conf_attr(conf, args->pick);
    return;
  }

  if (args->saveas) {
    pauli_matrix pm = load_ctx_config(args->path);
    fprint_pauli_matrix(stdout, &pm);

    add_ctx_config(&conf_info, &pm, args->saveas);
  }

  fprint_ctx_conf_info(stdout, conf_info);
}

void handle_get(cli_args *args) {
  pauli_matrix pms[128] = {0};
  ctx_conf_info conf_infos[128] = {0};
  size_t out_count = 0;
  search_ctx_configs(CTX_CONF_INFO_DIR, &out_count, args->filters, conf_infos,
                     pms);

  if (out_count == 0) {
    printf("No results found.\n");
    return;
  }

  printf("Found %zu config(s)\n", out_count);
  for (size_t i = 0; i < out_count; i++) {
    if (args->show_conf) {
      printf("~~~ Config %lu ~~~\n", i + 1);
      fprint_pauli_matrix(stdout, &pms[i]);
    }
    printf("~~~ Config %lu infos ~~~\n", i + 1);
    fprint_ctx_conf_info(stdout, conf_infos[i]);
  }
}

void handle_serve(cli_args *args) {
  struct http_server_s *server = http_server_init(args->port, request_handler);
  printf("Server running on http://localhost:%i\n", args->port);
  http_server_listen(server);
}

void ponexit(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  exit(1);
}