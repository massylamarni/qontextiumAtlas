#ifndef LOCALDB
#define LOCALDB

#include "ctx_config.h"
#include <uuid/uuid.h>

#define CTX_CONF_DIR "localdb/ctx"
#define CTX_CONF_INFO_DIR "localdb/md"

pauli_matrix load_ctx_config(const char *dir_name);
pauli_matrix *load_ctx_configs(const char *dir_name, size_t *out_count,
                               const char **dir_names,
                               size_t dir_names_count);
void save_ctx_config(const char *dir_name, const pauli_matrix *pm);

ctx_conf_info load_ctx_config_info(const char *dir_name);
ctx_conf_info *load_ctx_configs_info(const char *dir_name, size_t *out_count);
void save_ctx_config_info(const char *dir_name, const ctx_conf_info *conf);
void get_new_id(unsigned char *id);
void get_new_name(char *name, ctx_conf_info conf_info);

/* TODO */
void delete_ctx_configs(unsigned int *ids);
void update_ctx_configs(unsigned int *ids, const char *src_dir_name);
void is_ctx_config_present(const char *dir_name);

#endif