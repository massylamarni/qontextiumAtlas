#include "ctx_config_formats.h"

const ctx_format qtxium_to_ctx_format(const char *s) {
  for (int i = 0; i <= GRAPH; i++) {
    if (strcmp(s, ctx_format_to_qtxium[i]) == 0)
      return (ctx_format)i;
  }
  return (ctx_format)-1;
}
