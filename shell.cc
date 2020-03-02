#include <cstdio>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "command.hh"
#include "shell.hh"

extern "C" void disp( int sig ) {
  //fprintf( stderr, "\nsig:%d  Ouch!\n", sig);
  if (sig == SIGINT) {
    if (Shell::_currentCommand._argumentsArray.size() == 0) {
      printf("\n");
    //printf("myshell>");
      Shell::prompt();
    }
  }

}

extern "C" void zombie( int sig) {
  pid_t pid = waitpid(-1,NULL,WNOHANG);
  
  while ((waitpid(-1, NULL, WNOHANG)) > 0) {
    printf("\n[%d] exited.", pid);
  }
}

int yyparse(void);

void Shell::prompt() {
  printf("myshell>");
  fflush(stdout);
}

int main() {
  struct sigaction sa;
  sa.sa_handler = disp;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &sa, NULL)) {
    perror("sigaction");
    exit(2);
  }
  
  struct sigaction sa_zombie;
  sa_zombie.sa_handler = zombie;
  sigemptyset(&sa_zombie.sa_mask);
  sa_zombie.sa_flags = SA_RESTART;
  
  if (sigaction(SIGCHLD, &sa_zombie, NULL)) {
    perror("sigaction");
    exit(2);
  }

  if (isatty(0)) {
    Shell::prompt();
  }

  yyparse();

}

Command Shell::_currentCommand;
