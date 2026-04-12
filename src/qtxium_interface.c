// #define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "qtxium_interface.h"
#include "ctx_config.h"
#include <limits.h>
#include <stdio.h>

s_interval parse_interval(const char *val) {
  s_interval interval = {-1, -1};

  char *colon = strchr(val, ':');
  if (!colon) {
    // min == max
    interval.min = atoi(val);
    interval.max = interval.min;
    return interval;
  }

  char min_str[32], max_str[32];
  size_t min_len = colon - val;
  strncpy(min_str, val, min_len);
  min_str[min_len] = '\0';
  strncpy(max_str, colon + 1, sizeof(max_str) - 1);

  if (min_str[0] == '\0')
    interval.min = -1;
  else if (min_str[0] == '-')
    interval.min = INT_MIN;
  else if (min_str[0] == '+')
    interval.min = INT_MAX;
  else
    interval.min = atoi(min_str);

  if (max_str[0] == '\0')
    interval.max = -1;
  else if (max_str[0] == '+')
    interval.max = INT_MAX;
  else if (max_str[0] == '-')
    interval.max = INT_MIN;
  else
    interval.max = atoi(max_str);

  return interval;
}

search_filters parse_search_filters(int argc, char *argv[]) {
  search_filters sf = init_search_filters();

  for (int i = 0; i < argc; i++) {
    char *eq = strchr(argv[i], '=');
    if (!eq) {
      fprintf(stderr, "Ignored unknown arg: %s\n", argv[i]);
      continue;
    }

    *eq = '\0'; /* split key / value in-place */
    const char *key = argv[i];
    const char *val = eq + 1;

    if (!strcmp(key, "id"))
      sf.id = atoi(val);
#define X(kind, type, name) _X_##kind(name)
#define _X_INT(name)                                                           \
  else if (!strcmp(key, #name)) sf.name = parse_interval(val);
#define _X_STR(name)                                                           \
  else if (!strcmp(key, #name)) strncpy(sf.name, val, sizeof(sf.name) - 1);
#include "ctx_config_dynamic_attr.def"
#undef X
#undef _X_INT
#undef _X_STR
    else
      fprintf(stderr, "Unknown filter key: %s\n", key);

    *eq = '='; /* restore argv */
  }
  return sf;
}

void parse_result(ctx_conf_info *ctx_conf_i, char *buffer) {
  if (strstr(buffer, "number of qubits:") != NULL) {
    sscanf(buffer, " number of qubits: %d", &ctx_conf_i->qubits_count);
  }
  if (strstr(buffer, "Contextuality degree found:") != NULL) {
    sscanf(buffer, "Contextuality degree found: %d", &ctx_conf_i->ctx_degree);
  }
  if (strstr(buffer, "number of contexts:") != NULL) {
    sscanf(buffer, "number of contexts:%d", &ctx_conf_i->ctx_count);
  }
  if (strstr(buffer, "number of negative contexts:") != NULL) {
    sscanf(buffer, "number of negative contexts:%d",
           &ctx_conf_i->neg_ctx_count);
  }
  if (strstr(buffer, "best Hamming distance found:") != NULL) {
    sscanf(buffer, "best Hamming distance found: %d",
           &ctx_conf_i->best_hamming_distance);
  }
}

ctx_conf_info exec_qtxium(const char *file_name, char *format) {
  int pipefd[2];
  pipe(pipefd);
  char buffer[BUFFER_SIZE];
  const char *qtxium_args[5] = {"qontextium", "--import", format, file_name,
                                NULL};

  ctx_conf_info ctx_conf_1 = {0};
  ctx_conf_1.format = qtxium_to_ctx_format(format);
  strncpy(ctx_conf_1.file_name, file_name, sizeof(ctx_conf_1.file_name) - 1);

  pid_t pid = fork();
  if (pid == 0) {
    close(pipefd[0]);               // close read end
    dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe
    dup2(pipefd[1], STDERR_FILENO); // redirect stderr to pipe
    close(pipefd[1]);

    // Redirect stdin to /dev/null to prevent blocking on input
    int devnull = open("/dev/null", O_RDONLY);
    if (devnull != -1) {
      dup2(devnull, STDIN_FILENO);
      close(devnull);
    }

    chdir(QONTEXTIUM_DIR);
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s/%s", ATLAS_DIR, file_name);

    printf("Executing cmd: ./qontextium %s %s %s\n", qtxium_args[1],
           qtxium_args[2], file_path);
    fflush(stdout);

    execl("./qontextium", qtxium_args[0], qtxium_args[1], qtxium_args[2],
          file_path, NULL);
    perror("exec failed");
    _exit(1);
  }
  close(pipefd[1]); // close write end

  FILE *stream = fdopen(pipefd[0], "r");
  while (fgets(buffer, sizeof(buffer), stream) != NULL) {
    // printf("%s", buffer);
    parse_result(&ctx_conf_1, buffer);
  }
  fclose(stream);
  wait(NULL);

  return ctx_conf_1;
}