//
// Created by sikalovnikita on 12.05.18.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>

int sock, listener, round = 0;

struct player{
  char addr[14];
  char login[12];
  int hp;
  int in_game;
  char msg[100];
  int modifided_now;
  int id_of_process;
  int sock;
  int can_heal;
};

struct player* people, *person = NULL;
int capacity = 100;
int* size;
int id_of_judge;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void catch() {
  shutdown(sock, SHUT_RDWR);
  //shutdown(listener, SHUT_RDWR);
  close(sock);
  ///close(listener);
  shm_unlink("people");
  shm_unlink("size");
  exit(0);
}

void catch_for_main_process() {
  for(int i = 0; i < (*size); ++i) {
    if(people[i].in_game == 1) {
      kill(people[i].id_of_process, SIGINT);
    }
  }
  shutdown(listener, SHUT_RDWR);
  close(listener);
  kill(id_of_judge, SIGINT);
  exit(0);
}

void send_msg() {
  if(strlen(person->msg) != 0) {
    send(sock, person->msg, strlen(person->msg), 0);
    person->msg[0] = '\0';
  }
  if(person->in_game == 0) {
    for(int i = 0; i < (*size); ++i) {
      if(people[i].in_game == 1) {
        strcat(people[i].msg, "Somebody is died. R.I.P.\n");
        kill(people[i].id_of_process, SIGUSR1);
      }
    }
    catch();
  }
}

void start_of_round() {
  round = 1;
  person->can_heal = 1;
  char msg[] = "Round starts!\n";
  send(sock, msg, sizeof(msg), 0);
}

void end_of_round() {
  round = 0;
  char msg[] = "Round ends!\n";
  send(sock, msg, sizeof(msg), 0);
}

void sig_work() {
  struct sigaction *sig = malloc(sizeof(struct sigaction));
  sig->sa_handler = catch;
  sigaction(SIGINT, sig, NULL);

  struct sigaction *up_msg = malloc(sizeof(struct sigaction));
  up_msg->sa_handler = send_msg;
  up_msg->sa_flags = SA_RESTART;
  sigaction(SIGUSR1, up_msg, NULL);

  struct sigaction *announce_start = malloc(sizeof(struct sigaction));
  announce_start->sa_handler = start_of_round;
  announce_start->sa_flags = SA_RESTART;
  sigaction(SIGUSR2, announce_start, NULL);

  struct sigaction *announce_end = malloc(sizeof(struct sigaction));
  announce_end->sa_handler = end_of_round;
  announce_end->sa_flags = SA_RESTART;
  sigaction(SIGCONT, announce_end, NULL);
}

char* itoa(int a) {
  char* str = malloc(10);
  int tmp = a % 10, i = 0;
  if(a == 0) {
    str[0] = '0';
    str[1] = '\0';
    return str;
  }
  while(a != 0) {
    str[i] = tmp + '0';
    a /= 10;
    i++;
    tmp = a % 10;
  }
  str[i] = '\0';
  int k = strlen(str);
  for(int i = 0; i < (k / 2); ++i) {
    int temp = str[i];
    str[i] = str[k - i - 1];
    str[k - i - 1] = temp;
  }
  return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct player* find_person(char log[10]) {
  for(int i = 0; i < (*size); ++i) {
    if (!strcmp(people[i].login, log)) {
      return &people[i];
    }
  }
  return NULL;
}

struct player* add_player(char addr[14]) {
  strcpy(people[*size].addr, addr);
  people[*size].hp = 100;
  people[*size].in_game = 0;
  people[*size].msg[0] = '\0';
  people[*size].login[0] = '\0';
  people[*size].modifided_now = 0;
  people[*size].sock = sock;
  people[*size].id_of_process = getpid();
  (*size)++;
  return &people[*size - 1];
}

void create_judge() {
  id_of_judge = fork();
  if(!id_of_judge) {
    while(1) {
      sleep(20);
      for(int i = 0; i < (*size); ++i) {
        if(people[i].in_game == 1) {
          kill(people[i].id_of_process, SIGUSR2);
        }
      }
      sleep(20);
      for(int i = 0; i < (*size); ++i) {
        if(people[i].in_game == 1) {
          kill(people[i].id_of_process, SIGCONT);
        }
      }
    }
  }
}

void lock(struct player* person) {
  while (person->modifided_now == 1) {
    sleep(1);
  }
  person->modifided_now = 1;
}

void unlock(struct player* person) {
  person->modifided_now = 0;
}

int main() {
  struct sigaction* parent_sig = malloc(sizeof(struct sigaction));
  parent_sig->sa_handler = catch_for_main_process;
  sigaction(SIGINT, parent_sig, NULL);

  char sep[] = "\r\n ";
  int fd_people = shm_open("people", O_RDWR | O_CREAT, 0600);
  ftruncate(fd_people, capacity * sizeof(struct player));
  people = mmap(NULL, capacity * sizeof(struct player), PROT_READ | PROT_WRITE, MAP_SHARED, fd_people, 0);


  int fd_size = shm_open("size", O_RDWR | O_CREAT, 0600);
  ftruncate(fd_size, capacity * sizeof(int));
  size = mmap(NULL, capacity * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_size, 0);
  *size = 0;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  create_judge();
  struct sockaddr_in addr;
  char buf[1024];
  int bytes_read;
  listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  int opt=1;
  setsockopt (listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));

  if(listener < 0) {
    perror("socket");
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3425);
  addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(2);
  }
  listen(listener, 5);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  while (1) {
    struct sockaddr *tmp_addr = malloc(sizeof(struct sockaddr));
    socklen_t *tmp = malloc(sizeof(socklen_t));
    sock = accept(listener, tmp_addr, tmp);
    puts("New person in game");

    if (!fork()) {
      sig_work();
      char message[] = "login:\n";
      send(sock, message, sizeof(message), 0);
      person = add_player(tmp_addr->sa_data);
      recv(sock, buf, 1024, 0);
      char* login = strtok(buf, sep);
      strncpy(person->login, login, 10);
      person->in_game = 1;
      while (1) {
        int len = recv(sock, buf, 1024, 0);
        if (len == 0) {
          person->in_game = 0;
          catch();
        }
        buf[len] = '\0';
        char *sub_str = strtok(buf, sep);
        if (sub_str != NULL && !strcmp(sub_str, "who")) {
          for (int i = 0; i < (*size); ++i) {
            if (people[i].in_game == 1) {
              send(sock, people[i].login, 12, 0);
              send(sock, " ", 2, 0);
              send(sock, itoa(people[i].hp), 10, 0);
              send(sock, "\n", 2, 0);
            }
          }
        }
        if (sub_str != NULL && !strcmp(sub_str, "wall")) {
          sub_str = strtok(NULL, sep);
          for (int i = 0; i < (*size); ++i) {
            if (people[i].in_game == 1 && &people[i] != person) {
              lock(&people[i]);
              strcat(strcat(people[i].msg, person->login), " shouts: ");
            }
          }
          while (sub_str != NULL) {
            for (int i = 0; i < (*size); ++i) {
              if (people[i].in_game == 1 && &people[i] != person) {
                strcat(strcat(people[i].msg, sub_str), " ");
              }
            }
            sub_str = strtok(NULL, sep);
          }
          for (int i = 0; i < (*size); ++i) {
            if (people[i].in_game == 1 && &people[i] != person) {
              strcat(people[i].msg, "\n");
              kill(people[i].id_of_process, SIGUSR1);
              unlock(&people[i]);
            }
          }
        }
        if (sub_str != NULL && !strcmp(sub_str, "say")) {
          sub_str = strtok(NULL, sep);
          struct player *reciever = find_person(sub_str);
          if (reciever == NULL) {
            char msg[] = "there is no such person in the game\n";
            send(sock, msg, sizeof(msg), 0);
          } else {
            sub_str = strtok(NULL, sep);
            lock(reciever);
            strcat(strcat(reciever->msg, person->login), " say: ");
            while (sub_str != NULL) {
              strcat(strcat(reciever->msg, sub_str), " ");
              sub_str = strtok(NULL, sep);
            }
            strcat(reciever->msg, "\n");
            kill(reciever->id_of_process, SIGUSR1);
            unlock(reciever);
          }
        }
        if (sub_str != NULL && !strcmp(sub_str, "kill")) {
          if (round == 1) {
            sub_str = strtok(NULL, sep);
            struct player *opponent = find_person(sub_str);
            if (opponent == NULL) {
              char msg[] = "there is no such person in the game\n";
              send(sock, msg, sizeof(msg), 0);
            } else {
              srand(time(NULL) + getpid());
              int n = rand() % 10 + 1;
              lock(opponent);
              opponent->hp -= n;
              strcat(strcat(opponent->msg, person->login), " attacks you!\n");
              if (opponent->hp <= 0) {
                strcat(opponent->msg, "You are killed\n");
                opponent->in_game = 0;
                char msg[100];
                msg[0] = '\0';
                strcat(strcat(msg, opponent->login), " is died!\n");
                send(sock, msg, strlen(msg), 0);
              }
              kill(opponent->id_of_process, SIGUSR1);
              unlock(opponent);
            }
          } else {
            char msg[] = "Round has not started yet!\n";
            send(sock, msg, sizeof(msg), 0);
          }
        }
        if (sub_str != NULL && !strcmp(sub_str, "heal")) {
          if (round == 1) {
            sub_str = strtok(NULL, sep);
            struct player *opponent = find_person(sub_str);
            if (opponent == NULL) {
              char msg[] = "There is no such person in the game\n";
              send(sock, msg, sizeof(msg), 0);
            } else {
              if (person->can_heal == 1) {
                srand(time(NULL) + getpid());
                int n = rand() % 10 + 1;
                lock(opponent);
                opponent->hp += n;
                unlock(opponent);
                person->can_heal = 0;
              } else {
                char msg[] = "You can't heal in this round\n";
                send(sock, msg, sizeof(msg), 0);
              }
            }
          } else {
            char msg[] = "Round has not started yet!\n";
            send(sock, msg, sizeof(msg), 0);
          }
        }
      }
    } else {
      close(sock);
    }
  }
}
