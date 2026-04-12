#ifndef QTXIUM_INTERFACE
#define QTXIUM_INTERFACE

#define QONTEXTIUM_DIR "../contextualityDegree"
#define ATLAS_DIR "../qontextiumAtlas"
#define BUFFER_SIZE 256
#define PATH_SIZE 64

#include "ctx_config.h"
#include <fcntl.h>

s_interval parse_interval(const char *s);
search_filters parse_search_filters(int argc, char *argv[]);
ctx_conf_info exec_qtxium(const char *file_name, char *format);
void parse_result(ctx_conf_info *ctx_conf_i, char *buffer);
void init_interface(int argc, char *argv[]);

#endif