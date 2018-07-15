//
// Created by sikalovnikita on 22.03.18.
//
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  FILE *f;
  char *s = (char*)malloc(1);
  for (int i = 1; i < argc; i++) {
    f = fopen(argv[i], "r");
    if (f == NULL) {
      perror("file is not opened");
    }
    int j = 0;
    int c = fgetc(f);
    while (c != EOF) {
      if (c < 128 && c >= 32 && c != '\n' || c == 9) {
        s[j] = c;
        j++;
        s = realloc(s, j + 1);
        if (!s) {
          perror(NULL);
          exit(1);
        }
      }
      if (c == '\n') {
        s[j] = '\0';
        if (j >= 4) {
          puts(s);
        }
        j = 0;
      }
      c = fgetc(f);
    }
  }

  if (argc == 1) {
    int j = 0;
    int c = getchar();
    while (c != EOF) {
      if (c < 128 && c >= 32 && c != '\n' || c == 9) {
        s[j] = c;
        j++;
        s = realloc(s, j + 1);
        if (!s) {
          perror(NULL);
          exit(1);
        }
      }
      if (c == '\n') {
        s[j] = '\0';
        if (j >= 4) {
          puts(s);
        }
        j = 0;
      }
      c = getchar();
    }
  }
}


