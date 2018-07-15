//
// Created by sikalovnikita on 23.04.18.
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <stdbool.h>
#define step 10000

int threadNum = 0;
long long int *simple_numbers = NULL;
long long int N = 0;
long long int size = 0;
long long int capacity = 0;
long long int from = 1;
long long int to;
bool *current_numbrs;
int delta = 0;

pthread_mutex_t mut;

void *f(void *p) {
  int k = *(int *)p;

  for (long long int i = from + delta * k, n = 0; n <= delta && i <= to; i++, n++) {
    current_numbrs[i % step] = 1;
    for (int j = 0; j < size && simple_numbers[j] * simple_numbers[j] <= i; ++j) {
      if (i % simple_numbers[j] == 0) {
        current_numbrs[i % step] = 0;
        break;
      }
    }
  }
}

long long int* get_first_step_simple_numbers(long long int *simple_numbers) {
  for (int i = 2; i < step && i <= N; ++i) {
    int flag = 1;
    for (int j = 0; j < size && simple_numbers[j] * simple_numbers[j] <= i; ++j) {
      if (i % simple_numbers[j] == 0) {
        flag = 0;
        break;
      }
    }
    if (flag) {
      if (size == capacity) {
        capacity *= 2;
        simple_numbers = realloc(simple_numbers, sizeof(long long int) * capacity);
        if(!simple_numbers) {
          perror(NULL);
          exit(1);
        }
      }
      simple_numbers[size] = i;
      printf("%lld\n", simple_numbers[size]);
      size++;
    }
  }
  return simple_numbers;
}

int main(int argc, const char *argv[]) {
  if (argc == 3) {
    N = atoll(argv[2]);
  } else {
    N = LLONG_MAX;
  }
  threadNum = atoi(argv[1]);
  current_numbrs = (bool*)malloc(sizeof(bool) * (step + 1));
  delta = step / threadNum;

  simple_numbers = (long long int*)malloc(1);
  capacity = 1;
  size = 0;
  simple_numbers = get_first_step_simple_numbers(simple_numbers);

  int *k = (int*)malloc(threadNum * sizeof(int));
  for (int i = 0; i < threadNum; ++i) {
    k[i] = i;
  }

  pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * threadNum);
  for (long long int a = step; a <= N; a += step) {
    from = a;
    to = a + step - 1;
    if (to > N) {
      to = N;
    }

    for (int i = 0; i < threadNum; ++i) {
      pthread_create(threads + i, NULL, f, k + i);
    }

    void *p;
    for (int i = 0; i < threadNum; ++i) {
      pthread_join(threads[i], &p);
    }

    for (long long int j = from; j <= to && j <= N; j++) {
      if (size == capacity) {
        capacity *= 2;
        simple_numbers = realloc(simple_numbers, sizeof(long long int) * capacity);
        if(!simple_numbers) {
          perror(NULL);
          exit(1);
        }
      }
      if (current_numbrs[j % step] == 1) {
        simple_numbers[size] = j;
        printf("%lld\n", j);
        size++;
      }
    }
  }
  free(simple_numbers);
  free(current_numbrs);
  return 0;
}
