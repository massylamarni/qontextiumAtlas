#ifndef HTTP_SERVER
#define HTTP_SERVER

#include "lib/httpserver/httpserver.h"
#include <stdbool.h>
#include "localdb.h"
#include "qtxium_interface.h"
#include "arg_handlers.h"
#include "lib/cjson/cJSON.h"

int request_target_is(struct http_request_s *request, char const *target);
void chunk_cb(struct http_request_s *request);
void default_routes_hook(struct http_request_s *request,
                         struct http_response_s *response);
void routes_hook(struct http_request_s *request,
                 struct http_response_s *response);
void request_handler(struct http_request_s *request);
void response_error(struct http_request_s *request,
                    struct http_response_s *response, int status,
                    const char *msg);
int parse_run_request(struct http_request_s *request,
                      struct http_response_s *response, cli_args *args,
                      cJSON *body);
int parse_get_request(struct http_request_s *request,
                      struct http_response_s *response, cli_args *args,
                      cJSON *body);
void route_run(struct http_request_s *request,
               struct http_response_s *response);
void route_get(struct http_request_s *request,
               struct http_response_s *response);

#endif