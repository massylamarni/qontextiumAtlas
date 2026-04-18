#include <stdio.h>
#define HTTPSERVER_IMPL
#include "http_server.h"

#define RESPONSE "Pong"

struct http_server_s *poll_server;

int request_target_is(struct http_request_s *request, char const *target) {
  http_string_t url = http_request_target(request);
  int len = strlen(target);
  return len == url.len && memcmp(url.buf, target, url.len) == 0;
}

void chunk_cb(struct http_request_s *request) {
  int *chunk_count = http_request_userdata(request);
  (*chunk_count)++;
  struct http_response_s *response = http_response_init();
  http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
  if (*chunk_count < 3) {
    http_respond_chunk(request, response, chunk_cb);
  } else {
    http_response_header(response, "Foo-Header", "bar");
    http_respond_chunk_end(request, response);
  }
}

void default_routes_hook(struct http_request_s *request,
                         struct http_response_s *response) {
  if (request_target_is(request, "/echo")) {
    http_string_t body = http_request_body(request);
    http_response_body(response, body.buf, body.len);
  } else if (request_target_is(request, "/host")) {
    http_string_t ua = http_request_header(request, "Host");
    http_response_body(response, ua.buf, ua.len);
  } else if (request_target_is(request, "/poll")) {
    while (http_server_poll(poll_server) > 0)
      ;
    http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
  } else if (request_target_is(request, "/empty")) {
    // No Body
  } else if (request_target_is(request, "/chunked")) {
    http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
    http_respond_chunk(request, response, chunk_cb);
    return;
  } else if (request_target_is(request, "/headers")) {
    int iter = 0, i = 0;
    http_string_t key, val;
    char buf[512];
    while (http_request_iterate_headers(request, &key, &val, &iter)) {
      i += snprintf(buf + i, 512 - i, "%.*s: %.*s\n", key.len, key.buf, val.len,
                    val.buf);
    }
    http_response_body(response, buf, i);
  } else {
    http_response_body(response, RESPONSE, sizeof(RESPONSE) - 1);
  }
  http_respond(request, response);
}

void routes_hook(struct http_request_s *request,
                 struct http_response_s *response) {
  if (request_target_is(request, "/")) {
    const char *body = "C server";
    http_response_body(response, body, strlen(body));
    http_respond(request, response);
  } else if (request_target_is(request, "/run")) {
    route_run(request, response);
  } else if (request_target_is(request, "/get")) {
    route_get(request, response);
  } else {
    default_routes_hook(request, response);
  }
}

void request_handler(struct http_request_s *request) {
  http_request_connection(request, HTTP_CLOSE);
  struct http_response_s *response = http_response_init();
  http_response_status(response, 200);
  http_response_header(response, "Content-Type", "text/plain");
  routes_hook(request, response);
}

void response_error(struct http_request_s *request,
                    struct http_response_s *response, int status,
                    const char *msg) {
  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "error", msg);
  char *body = cJSON_PrintUnformatted(root);

  http_response_status(response, status);
  http_response_header(response, "Content-Type", "application/json");
  http_response_body(response, body, strlen(body));
  http_respond(request, response);

  free(body);
  cJSON_Delete(root);
}

static cJSON *ctx_conf_info_to_json(const ctx_conf_info *conf_info) {
  cJSON *json = cJSON_CreateObject();
  char uuid_str[37];
  uuid_unparse(conf_info->id, uuid_str);
  cJSON_AddStringToObject(json, "id", uuid_str);
  cJSON_AddStringToObject(json, "format", ctx_format_to_qtxium[conf_info->format]);
#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name) cJSON_AddNumberToObject(json, #name, conf_info->name);
#define _X_STR(name) cJSON_AddStringToObject(json, #name, conf_info->name);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
  return json;
}

static cJSON *pauli_matrix_to_json(const pauli_matrix *pm) {
  char *buf = NULL;
  size_t size = 0;
  FILE *stream = open_memstream(&buf, &size);
  fprint_pauli_matrix(stream, pm);
  fclose(stream);
  cJSON *json = cJSON_CreateString(buf);
  free(buf);
  return json;
}

int parse_run_request(struct http_request_s *request,
                      struct http_response_s *response, cli_args *args,
                      cJSON *body) {
  if (!body) {
    response_error(request, response, 400, "Invalid JSON body");
    return 0;
  }

  cJSON *j_format = cJSON_GetObjectItemCaseSensitive(body, "format");
  cJSON *j_path = cJSON_GetObjectItemCaseSensitive(body, "path");
  if (!cJSON_IsString(j_format) || !cJSON_IsString(j_path)) {
    cJSON_Delete(body);
    response_error(request, response, 400, "'format' and 'path' are required");
    return 0;
  }

  args->cmd = CMD_RUN;
  args->format = j_format->valuestring;
  args->path = j_path->valuestring;

  cJSON *j_saveas = cJSON_GetObjectItemCaseSensitive(body, "saveas");
  cJSON *j_pick = cJSON_GetObjectItemCaseSensitive(body, "pick");
  if (cJSON_IsString(j_saveas))
    args->saveas = j_saveas->valuestring;
  if (cJSON_IsString(j_pick))
    args->pick = j_pick->valuestring;

  return 1;
}

int parse_get_request(struct http_request_s *request,
                      struct http_response_s *response, cli_args *args,
                      cJSON *body) {
  if (!body) {
    response_error(request, response, 400, "Invalid JSON body");
    return 0;
  }
  cJSON *j_show = cJSON_GetObjectItemCaseSensitive(body, "show_conf");
  if (cJSON_IsTrue(j_show))
    args->show_conf = true;

  cJSON *j_filters = cJSON_GetObjectItemCaseSensitive(body, "filters");
  if (cJSON_IsArray(j_filters)) {
    int n = cJSON_GetArraySize(j_filters);
    char *filter_argv[64];
    int filter_argc = 0;
    for (int i = 0; i < n && filter_argc < 64; i++) {
      cJSON *item = cJSON_GetArrayItem(j_filters, i);
      if (cJSON_IsString(item))
        filter_argv[filter_argc++] = item->valuestring;
    }
    args->filters = parse_search_filters(filter_argc, filter_argv);
  }

  return 1;
}

void route_run(struct http_request_s *request,
               struct http_response_s *response) {
  struct http_string_s raw = http_request_body(request);
  cJSON *body = cJSON_ParseWithLength(raw.buf, raw.len);
  cli_args args = {0};
  if (!parse_run_request(request, response, &args, body))
    return;
  ctx_conf_info conf_info = exec_qtxium(args.path, args.format);
  if (!is_ctx_conf_info_valid(conf_info)) {
    cJSON_Delete(body);
    response_error(request, response, 422, "Invalid config");
    return;
  }

  cJSON *root = cJSON_CreateObject();

  if (args.pick) {
    // TODO: populate once print_ctx_conf_attr is implemented
  } else {
    if (args.saveas) {
      pauli_matrix pm = load_ctx_config(args.path);
      add_ctx_config(&conf_info, &pm, args.saveas);
    }
  }
  cJSON *resp_json = ctx_conf_info_to_json(&conf_info);
  char *resp_body = cJSON_PrintUnformatted(resp_json);
  http_response_status(response, 200);
  http_response_header(response, "Content-Type", "application/json");
  http_response_body(response, resp_body, strlen(resp_body));
  http_respond(request, response);

  free(resp_body);
  cJSON_Delete(root);
  cJSON_Delete(body);
}

void route_get(struct http_request_s *request,
               struct http_response_s *response) {
  struct http_string_s raw = http_request_body(request);
  cJSON *body = cJSON_ParseWithLength(raw.buf, raw.len);

  cli_args args = {0};
  if (!parse_get_request(request, response, &args, body))
    return;

  pauli_matrix pms[128] = {0};
  ctx_conf_info conf_infos[128] = {0};
  size_t out_count = 0;

  search_ctx_configs(CTX_CONF_INFO_DIR, &out_count, args.filters, conf_infos,
                     pms);

  cJSON *root = cJSON_CreateObject();
  cJSON *results = cJSON_CreateArray();
  cJSON_AddNumberToObject(root, "count", (double)out_count);
  cJSON_AddItemToObject(root, "results", results);
    
  for (size_t i = 0; i < out_count; i++) {

  char uuid_str[37];
  uuid_unparse(conf_infos[i].id, uuid_str);
  printf("%s\n", uuid_str);
  fflush(stdout);
    cJSON *entry = ctx_conf_info_to_json(&conf_infos[i]);
    if (args.show_conf)
      cJSON_AddItemToObject(entry, "ctx_conf", pauli_matrix_to_json(&pms[i]));
    cJSON_AddItemToArray(results, entry);
  }

  char *resp_body = cJSON_PrintUnformatted(root);
  http_response_status(response, out_count > 0 ? 200 : 404);
  http_response_header(response, "Content-Type", "application/json");
  http_response_body(response, resp_body, strlen(resp_body));
  http_respond(request, response);

  free(resp_body);
  cJSON_Delete(root);
  if (body)
    cJSON_Delete(body);
}
