#include <cstdio>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "command.hh"
#include "shell.hh"

extern "C" void disp(int sig) {

  //fprintf( stderr, "\nsig:%d  Ouch!\n", sig);
      //if (Command::_currSimpleCommand == NULL) {
      if (Shell::_currentCommand._simpleCommandsArray.size() == 0) {
        printf("\n");
    //printf("myshell>");
        Shell::prompt();
      }
      else {
        printf("\n");
      }
      /*}else {
      printf("\n");
      fflush(stdout);
      }*/

}

extern "C" void zombie(int sig) {
  pid_t pid = wait3(0,0,NULL);
  
  while ((waitpid(-1, NULL, WNOHANG)) > 0) {
    if (Shell::_currentCommand._backgnd) {
      printf("\n[%d] exited.\n", pid);
    }
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
