//
// Created by sikalovnikita on 31.03.18.
//
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

char* replace(const char *old_name, const char *name_to_add) {
  int old_size = strlen(old_name);
  int new_size = old_size + strlen(name_to_add);
  char* new_name = malloc(new_size + 5);
  if (!new_name) {
    perror("memory error: 14");
    return NULL;
  }
  strcat(strcat(strcpy(new_name, old_name), "/"), name_to_add);
  return new_name;
}

void find_recurse(const char *name_of_directory, const char *name_of_file, const char *name_of_user) {
  DIR *dir;
  dir = opendir(name_of_directory);
  struct stat info;
  struct dirent *d;
  if (dir == NULL) {
    perror(name_of_directory);
    return;
  }
  char **files = malloc(1);
  if (!files) {
    perror("memory error: 32");
    return;
  }
  int i = 0;
  while ((d = readdir(dir)) != NULL) {
    files = realloc(files, sizeof(char *) * (i + 1));
    if (files == NULL) {
      perror("memory error: 39");
      continue;
    }
    files[i] = malloc(strlen(d->d_name) + 2);
    if (!files[i]) {
      perror("memory error: 44");
      continue;
    }
    strcpy(files[i], d->d_name);
    i++;
  }
  closedir(dir);
  for (int j = 0; j < i; j++) {
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
    if (!strcmp(files[j], name_of_file)) {
      puts(new_name);
    }
    if (S_ISDIR(info.st_mode) && strcmp(files[j], "..") && strcmp(files[j], ".") && !strcmp(user->pw_name, name_of_user)) {
      find_recurse(new_name, name_of_file, name_of_user);
    }
    free(new_name);
  }
  for (int j = 0; j < i; j++) {
    free(files[j]);
  }
}

int main(int argc, char **argv) {
  if (argc < 6) {
    perror("too few arguments");
    exit(1);
  } else {
    find_recurse(argv[1], argv[3], argv[5]);
  }
}
