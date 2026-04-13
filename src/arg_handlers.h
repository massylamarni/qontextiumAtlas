#include "ctx_config.h"

typedef enum {
    CMD_NONE,
    CMD_RUN,
    CMD_GET,
    CMD_SERVE,
} cmd_type;

typedef struct {
    cmd_type cmd;
    // --run
    char *format;
    char *path;
    char *saveas;
    char *pick;
    // --get
    search_filters filters;
    bool show_conf;
    // --serve
    int port;
} cli_args;

cli_args parse_args(int argc, char *argv[]);
void handle_run(cli_args *args);
void handle_get(cli_args *args);
void handle_serve(cli_args *args);
void ponexit(const char *fmt, ...);