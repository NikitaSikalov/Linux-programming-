//
// Created by sikalovnikita on 05.04.18.
//
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

//I also reckon that we should check return value from close() == -1, because
//descriptor may not be closed, but the code then will be much more longer
//and nothing hapened if descriptor has not been closed.

int size = 2;

void foo(char *pr, int d0, int d1, int p[size][2]) {
  pid_t id = fork();
  if (!id) {
    if (d0 != 0) {
      dup2(d0, 0);
      close(d0);
    }
    if (d1 != 1) {
      dup2(d1, 1);
      close(d1);
    }
    for (int i = 0; i < size; ++i){
      if (d0 != p[i][0] && d1 != p[i][0]) {
        close(p[i][0]);
      }
      if (d0 != p[i][1] && d1 != p[i][1]) {
        close(p[i][1]);
      }
    }
    execlp(pr, pr, NULL);
    perror(pr);
    exit(1);
  }
}

int main (int argc, char **argv) {
  int p[argc - 2][2];
  size = argc - 2;
  for (int i = 0; i < argc - 2; ++i) {
    if (pipe(p[i]) == -1){
      perror("pipe");
      exit(1);
    }
  }

  for (int i = 1; i < argc; ++i) {
    if (i - 2 == -1) {
      foo(argv[i], 0, p[i - 1][1], p);
      close(p[i - 1][1]);
      continue;
    }
    if (i - 1 == (argc - 2)) {
      foo(argv[i], p[i - 2][0], 1, p);
      close(p[i - 2][0]);
      continue;
    }
    foo(argv[i], p[i - 2][0], p[i - 1][1], p);
    close(p[i - 1][1]);
    close(p[i - 2][0]);
  }
  for (int i = 1; i < argc; ++i) {
    wait(NULL);
  }
  return 0;
}
