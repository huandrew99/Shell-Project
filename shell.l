
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 * NOTICE: This lab is property of Purdue University. You should not for any reason make this code public.
 */

%{

#include <cstring>
#include <unistd.h>
#include "y.tab.hh"


static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

extern int yyleng;

%}

%option noyywrap

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}


">" {
  return GREAT;
}

"<" {
  return LESS;
}

">>" {
  return GREATGREAT;
}

"|" {
  return PIPE;
}

"&" {
  return AMPERSAND;
}

">&" {
  return GREATAMPERSAND;
}

">>&" {
  return GREATGREATAMPERSAND;
}

"2>" {
  return TWOGREAT;
}

[^ \t\n|<>&]*\\[^ \t\n]+ {
  
  //std::string *result2 = new std::string(yytext);

  char * str = yytext;
  char * result2 = new char[strlen(yytext) + 1];
  int index = 0;
  while (1) {
    if (!*str) {
      break;
    }
    if (*str == '\\') {
      str++;
      result2[index] = *str;
      index++;
    }
    else {
      result2[index] = *str;
      index++;
    }
    str++;
  }
  result2[index] = '\0';

  yylval.cpp_string = new std::string(result2);
  return WORD;

} 


\"[^\n\"]*\" {
  std::string *result = new std::string(yytext);
  //char * result = [yyleng];
  //char * cursor = yytext;
  std::string copy = *result;
  copy = copy.substr(1, copy.size()-2);
  *result = copy;

  yylval.cpp_string = result;
  return WORD;
} /* quotes */


\$\([^\n\$]*\) {
  std::string *result = new std::string(yytext);
  std::string copy = *result;
  copy = copy.substr(2, copy.size()-3);
  *result = copy;
  int temin = dup(0);
  int temou = dup(1);

  int pin[2];
  int pout[2];

  pipe(pin);
  pipe(pout);

  write(pin[1], result->c_str(), strlen(result->c_str()));
  write(pin[1], "\n", 1);
  write(pin[1], "exit\n", 5);
  close(pin[1]);
  
  dup2(pin[0], 0);
  close(pin[0]);
  dup2(pout[1], 1);
  close(pout[1]);
  
  int ret = fork();
  if (ret == 0) {
    execvp("/proc/self/exe", NULL);
    perror("execvp");
    _exit(1);
  }
  else if (ret < 0){
    perror("fork");
    exit(2);
  }

  dup2(temin, 0);
  dup2(temou, 1);
  close(temin);
  close(temou);

  char cha;
  char * buffer = new char[5000];
  int read_ret = 0;
  int index = 0;
  while(1) {
    read_ret = read(pout[0], &cha, 1);
    if (read_ret != 1) {
      break;
    }
    //printf("%c", cha);
    if (cha == '\n') {
      buffer[index] = ' ';
      index++;
    }
    else {
      buffer[index] = cha;
      index++;
    }
  }
  buffer[index] = '\0';
  int i;
  for (i = strlen(buffer)-11; i >= 0; i--) {
    myunputc(buffer[i]);
  }



  //yylval.cpp_string = result;
  //return WORD;
} /* subshell */

[^ \t\n|<>][^ \t\n|<>]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}
