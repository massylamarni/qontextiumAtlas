// #define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "qtxium_interface.h"

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
  if (argc != 3) {
    printf("Usage: %s <format> <file_path>\n", argv[0]);
    _exit(1);
  }

  ctx_conf ctx_conf_1 = exec_qtxium(argv[2], argv[1]);
  if (is_ctx_conf_valid(ctx_conf_1)) {
    print_ctx_conf(ctx_conf_1);
  } else {
    printf("Invalid config !\n");
  }
}