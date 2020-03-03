
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

[^ \t\n|><&]*\\[^ \t\n]* {
  
  std::string *result2 = new std::string(yytext);
  std::string copy2 = *result2;
  copy2 = copy2.substr(1, copy2.size()-2);
  *result2 = copy2;
  yylval.cpp_string = result2;
  return WORD;
}


\"[^\n\"]*\" {
  //yylval.cpp_string = new std::string(yytext);
  //char * result = [yyleng];
  //char * cursor = yytext;
  std::string *result = new std::string(yytext);
  std::string copy = *result;
  copy = copy.substr(1, copy.size()-2);
  *result = copy;
  yylval.cpp_string = result;
  return WORD;
} /* quotes */


[^ \t\n|<>][^ \t\n|<>]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}
