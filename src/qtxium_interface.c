// #define _POSIX_C_SOURCE 200809L
#include "ctx_config.h"
#include <stdio.h>
#define _GNU_SOURCE
#include "qtxium_interface.h"

s_interval parse_interval(const char *s) {
  s_interval iv = {-1, -1};
  char *colon = strchr(s, ':');
  if (colon) {
    if (colon != s)
      iv.min = atoi(s);
    if (*(colon + 1) != '\0')
      iv.max = atoi(colon + 1);
  } else {
    iv.min = iv.max = atoi(s);
  }
  return iv;
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

void parse_result(ctx_conf *ctx_conf_i, char *buffer) {
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

ctx_conf exec_qtxium(char *file_name, char *format) {
  int pipefd[2];
  pipe(pipefd);
  char buffer[BUFFER_SIZE];
  char *qtxium_args[5] = {"qontextium", "--import", format, file_name, NULL};

  ctx_conf ctx_conf_1 = {0};
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

void init_interface(int argc, char *argv[]) {
  if (argc >= 2 && strcmp(argv[1], "get") == 0) {
    search_filters sf = parse_search_filters(argc - 2, argv + 2);
    ctx_conf configs_info[128] = {0};
    size_t out_count = 0;
    search_ctx_configs("ctxs/jbatch", &out_count, sf, configs_info);
    if (out_count == 0) {
      printf("No results found !\n");
    } else {
      printf("Found %lu config(s)\n", out_count);
      for (int i = 0; i < out_count; i++) {
        printf("-------------------\n");
        print_ctx_conf(configs_info[i]);
      }
    }
  } else if (argc == 3) {
    ctx_conf ctx_conf_1 = exec_qtxium(argv[2], argv[1]);
    if (is_ctx_conf_valid(ctx_conf_1)) {
      print_ctx_conf(ctx_conf_1);
    } else {
      printf("Invalid config !\n");
    }
  } else {
    printf("Usage: %s <format> <file_path>\n", argv[0]);
    printf("Or: %s get [<filter>=<min>:<max>...]\n", argv[0]);
    _exit(1);
  }
}