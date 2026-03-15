#ifndef QTXIUM_INTERFACE
#define QTXIUM_INTERFACE

#define QONTEXTIUM_DIR "../../contextualityDegree"
#define ATLAS_DIR "../qontextiumAtlas"
#define BUFFER_SIZE 256
#define ARGS_SIZE 128

#include "ctx_config.h"

void parse_result(ctx_conf *ctx_conf_i, char *buffer);
void init_interface(int argc, char *argv[]);

#endif