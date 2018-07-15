//
// Created by sikalovnikita on 23.03.18.
//
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
//Task has done on points 4 (only recursive way with information)

char* replace(const char *old_name, const char *name_to_add) {
  int old_size = strlen(old_name);
  int new_size = old_size + strlen(name_to_add);
  char* new_name = malloc(new_size + 5);
  if (!new_name) {
    perror("memory error: 16");
    return NULL;
  }
  strcat(strcat(strcpy(new_name, old_name), "/"), name_to_add);
  return new_name;
}

void f(const char *name_of_directory) {
    DIR *dir;
    dir = opendir(name_of_directory);
    struct stat info;
    struct dirent *d;
    if (dir == NULL) {
      perror(name_of_directory);
      return;
    }
    char **files = malloc(1);
    if (!files){
      perror("memory error: 34");
      return;
    }
    int i = 0;
    while ((d = readdir(dir)) != NULL) {
      files = realloc(files, sizeof(char *) * (i + 1));
      if (files == NULL) {
        perror("memory error: 41");
        continue;
      }
      files[i] = malloc(strlen(d->d_name) + 2);
      if (!files[i]) {
        perror("memory error: 46");
        continue;
      }
      strcpy(files[i], d->d_name);
      i++;
    }
    closedir(dir);
    printf("%s:\n", name_of_directory);
    for (int j = 0; j < i; ++j) {
      char *new_name = replace(name_of_directory, files[j]);
      if (new_name == NULL) {
        continue;
      }
      stat(new_name, &info);
      struct passwd *user = getpwuid(info.st_uid);
      if (user == NULL) {
        perror(new_name);
        continue;
      }
      printf("%c", (S_ISDIR(info.st_mode) != 0 ? 'd' : '-'));
      printf("%c%c%c", ((info.st_mode & S_IRUSR) == S_IRUSR ? 'r' : '-'), ((info.st_mode & S_IWUSR) == S_IWUSR ? 'w' : '-'), ((info.st_mode & S_IXUSR) == S_IXUSR ? 'x' : '-'));
      printf("%c%c%c", ((info.st_mode & S_IRGRP) == S_IRGRP ? 'r' : '-'), ((info.st_mode & S_IWGRP) == S_IWGRP ? 'w' : '-'), ((info.st_mode & S_IXGRP) == S_IXGRP ? 'x' : '-'));
      printf("%c%c%c", ((info.st_mode & S_IROTH) == S_IROTH ? 'r' : '-'), ((info.st_mode & S_IWOTH) == S_IWOTH ? 'w' : '-'), ((info.st_mode & S_IXOTH) == S_IXOTH ? 'x' : '-'));
      printf(" %d %s %5d", info.st_nlink, user->pw_name, info.st_size);
      //printf("%s", ctime(&info.st_ctime));
      printf(" %s\n", files[j]);
    }
    printf("\n");
    for (int j = 0; j < i; j++) {
      char *new_name = replace(name_of_directory, files[j]);
      if (new_name == NULL) {
        continue;
      }
      stat(new_name, &info);
      if (S_ISDIR(info.st_mode) && strcmp(files[j], "..") && strcmp(files[j], ".")) {
        f(new_name);
      }
      free(new_name);
    }
    for (int j = 0; j < i; j++) {
      free(files[j]);
    }
}

int main(int argc, char **argv) {
  if (argc == 1) {
    f(".");
  } else {
    for (int i = 1; i < argc; ++i) {
      f(argv[i]);
    }
  }
}
