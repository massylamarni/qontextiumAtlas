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

void init_interface(int argc, char *argv[]) {
  if (argc > 5) {
    printf("Usage: %s <qtxium_args>\n", argv[0]);
    _exit(1);
  }

  int pipefd[2];
  pipe(pipefd);
  char buffer[BUFFER_SIZE];
  char qtxium_args[ARGS_SIZE];
  ctx_conf ctx_conf_1;
  strcpy(ctx_conf_1.file_name, argv[3]);
  ctx_conf_1.format = qtxium_to_ctx_format(argv[2]);

  pid_t pid = fork();
  if (pid == 0) {
    close(pipefd[0]);               // close read end
    dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe
    dup2(pipefd[1], STDERR_FILENO); // redirect stderr to pipe
    close(pipefd[1]);

    chdir(QONTEXTIUM_DIR);
    snprintf(qtxium_args, sizeof(qtxium_args), "%s %s %s/%s", argv[1], argv[2],
             ATLAS_DIR, argv[3]);
    printf("Executing cmd: ./qontextium %s\n", qtxium_args);
    fflush(stdout);

    execl("./qontextium", "qontextium", qtxium_args, NULL);
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

  print_ctx_conf(ctx_conf_1);

  wait(NULL);
}