//
// Created by sikalovnikita on 18.03.18.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//1 - template
//2 - replace
//3 ... names of files
//Example of using: ./sed (or ./a.out) "template" "replace" "file 1" file 2" "file 3"

int main(int argc, char **argv) {
  if(argc < 3) {
    perror("there are few arguments");
    exit(1);
  }
  unsigned int size_of_template = strlen(argv[1]), size_of_new_str = strlen(argv[2]);
  FILE *f;
  char *res = (char*)malloc(1);
  res[0] = '\0';
  unsigned int count = 0, current_index = 0;
  for (int i = 3; i < argc; i++) {
    f = fopen(argv[i], "r");
    if (f == NULL) {
      perror(argv[i]);
      continue;
    }
    int c = fgetc(f);
    while (c != EOF) {
      res = realloc(res, current_index + 1);
      if (!res) {
        perror(NULL);
        exit(1);
      }
      res[current_index] = c;
      if (c != argv[1][count]) {
        count = 0;
      }
      if (c == argv[1][count]) {
        count++;
        if (count == size_of_template) {
          res = realloc(res, current_index + size_of_new_str + 2);
          strcpy(res + current_index - count + 1, argv[2]);
          current_index -= (count - size_of_new_str);
          count = 0;
        }
      }
      current_index++;
      c = fgetc(f);
    }
    res = realloc(res, current_index + 1);
    if (!res) {
      perror(NULL);
      exit(1);
    }
    res[current_index] = '\n';
  }
  if (argc == 3) {
    int c = getchar();
    while (c != EOF) {
      res = realloc(res, current_index + 1);
      if (!res) {
        perror(NULL);
        exit(1);
      }
      res[current_index] = c;
      if (c != argv[1][count]) {
        count = 0;
      }
      if (c == argv[1][count]) {
        count++;
        if (count == size_of_template) {
          res = realloc(res, current_index + size_of_new_str + 2);
          strcpy(res + current_index - count + 1, argv[2]);
          current_index -= (count - size_of_new_str);
          count = 0;
        }
      }
      current_index++;
      c = getchar();
    }
  }
  res[current_index] = '\0';
  puts(res);
  free(res);
}

