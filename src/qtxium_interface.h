#ifndef QTXIUM_INTERFACE
#define QTXIUM_INTERFACE

#define QONTEXTIUM_DIR "../contextualityDegree"
#define ATLAS_DIR "../qontextiumAtlas"
#define BUFFER_SIZE 256
#define PATH_SIZE 64

#include "ctx_config.h"
#include <fcntl.h>

void parse_result(ctx_conf_info *ctx_conf_i, char *buffer);
ctx_conf_info exec_qtxium(const char *dir_name, char *format);

#endif