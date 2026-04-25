#include "qtxium_interface.h"
#include "ctx_config.h"
#include <stdio.h>

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

ctx_conf_info exec_qtxium(const char *dir_name, char *format) {
  int pipefd[2];
  pipe(pipefd);
  char buffer[BUFFER_SIZE];
  const char *qtxium_args[5] = {"qontextium", "--import", format, dir_name,
                                NULL};

  ctx_conf_info conf_info = {0};
  conf_info.format = qtxium_to_ctx_format(format);
  strncpy(conf_info.dir_name, dir_name, sizeof(conf_info.dir_name) - 1);

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
    snprintf(file_path, sizeof(file_path), "%s/%s", ATLAS_DIR, dir_name);

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
    parse_result(&conf_info, buffer);
  }
  fclose(stream);
  wait(NULL);

  return conf_info;
}