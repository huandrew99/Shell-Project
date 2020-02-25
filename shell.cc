#include <cstdio>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.hh"

extern "C" void disp( int sig ) {
  fprintf( stderr, "\nsig:%d  Ouch!\n", sig);
  Shell::prompt();
  yyparse();
}

int yyparse(void);

void Shell::prompt() {
  printf("myshell>");
  fflush(stdout);
}

int main() {
  struct sigaction sa;
  sa.sa_handler = disp;
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &sa, NULL)) {
    perror("sigaction");
    exit(2);
  }
  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
