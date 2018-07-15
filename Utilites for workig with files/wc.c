//
// Created by sikalovnikita on 22.03.18.
//
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//This task has done only for 1-byte characters (2 points)

int main(int argc, char **argv) {
  FILE *f;
  int c, prev_c;
  unsigned int total_strings = 0, total_words = 0, total_size = 0;
  for (int i = 1; i < argc; i++) {
    unsigned int count_of_strings = 0, count_of_words = 0, size = 0;
    unsigned int new_word = 1;
    f = fopen(argv[i], "r");
    if (f == NULL) {
      perror(argv[i]);
      continue;
    }
    c = fgetc(f);
    prev_c = ' ';
    while (c != EOF) {
      size++;
      if (!isspace(c)) {
        new_word = 0;
      } else {
        if (c == '\n') {
          count_of_strings++;
        }
        if (new_word == 0) {
          count_of_words++;
        }
        new_word = 1;
      }
      prev_c = c;
      c = fgetc(f);
    }
    if (!isspace(prev_c)) {
      count_of_words++;
    }
    printf("%d %d %d %s\n", count_of_strings, count_of_words, size, argv[i]);
    total_size += size;
    total_words += count_of_words;
    total_strings += count_of_strings;
    fclose(f);
  }
  if (argc >= 3) {
    printf("%d %d %d %s\n", total_strings, total_words, total_size, "total");
  }

  if (argc == 1) {
    unsigned int new_word = 1;
    c = getchar();
    unsigned int count_of_strings = 0, count_of_words = 0, size = 0;
    prev_c = ' ';
    while (c != EOF) {
      size++;
      if (!isspace(c)) {
        new_word = 0;
      } else {
        if (c == '\n') {
          count_of_strings++;
        }
        if (new_word == 0) {
          count_of_words++;
        }
        new_word = 1;
      }
      prev_c = c;
      c = getchar();
    }
    if (!isspace(prev_c)) {
      count_of_words++;
    }
    printf("%d %d %d\n", count_of_strings, count_of_words, size);
  }
}
