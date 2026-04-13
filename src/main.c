#include "arg_handlers.h"

int main(int argc, char *argv[]) {
  cli_args args = parse_args(argc, argv);

  switch (args.cmd) {
  case CMD_RUN:
    handle_run(&args);
    break;
  case CMD_GET:
    handle_get(&args);
    break;
  case CMD_SERVE:
    handle_serve(&args);
    break;
  default:
    ponexit("No commands given.");
    break;
  }

  return 0;
}