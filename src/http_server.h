#include "lib/httpserver/httpserver.h"

int request_target_is(struct http_request_s* request, char const * target);
void chunk_cb(struct http_request_s *request);
void default_routes_hook(struct http_request_s *request, struct http_response_s *response);
void request_handler(struct http_request_s *request);
void routes_hook(struct http_request_s *request, struct http_response_s *response);
