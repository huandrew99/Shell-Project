
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 * NOTICE: This lab is property of Purdue University. You should not for any reason make this code public.
 */

%code requires 
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE LESS GREATGREAT PIPE AMPERSAND GREATAMPERSAND GREATGREATAMPERSAND TWOGREAT

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"

void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
  pipe_list iomodifier_list background_opt NEWLINE {
    /* printf("   Yacc: Execute command\n"); */
    Shell::_currentCommand.execute();
  }
  | NEWLINE { Shell::_currentCommand.execute(); }
  | error NEWLINE { yyerrok;  }
  ;

command_and_args:
  command_word argument_list {
    /* Command::_currSimpleCommand->print(); */
    Shell::_currentCommand.
    insertSimpleCommand( Command::_currSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
    /* printf("   Yacc: insert argument \"%s\"\n", $1->c_str()); */
    Command::_currSimpleCommand->insertArgument( $1 );\
  }
  ;

pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
  ;

command_word:
  WORD {
    /* printf("   Yacc: insert command \"%s\"\n", $1->c_str()); */
    Command::_currSimpleCommand = new SimpleCommand();
    Command::_currSimpleCommand->insertArgument( $1 );
  }
  ;

iomodifier_opt:
  GREAT WORD {
    /* printf("   Yacc: insert output \"%s\"\n", $2->c_str()); */
    Shell::_currentCommand._outFileName = $2;
    Shell::_currentCommand._outCount = Shell::_currentCommand._outCount + 1;
  }
  | GREATGREAT WORD {
    /* printf("   Yacc: insert output append \"%s\"\n", $2->c_str()); */
    Shell::_currentCommand._append = true;
    Shell::_currentCommand._outFileName = $2;
    Shell::_currentCommand._outCount = Shell::_currentCommand._outCount + 1;
  }
  | GREATGREATAMPERSAND WORD {
    /* printf("   Yacc: insert output >>& \"%s\"\n", $2->c_str()); */
    Shell::_currentCommand._append = true;
    Shell::_currentCommand._outFileName = $2;
    std::string *arg = new std::string;
    *arg = *($2);
    Shell::_currentCommand._errFileName = arg;
    Shell::_currentCommand._outCount = Shell::_currentCommand._outCount + 1;
  }
  | LESS WORD {
    /* printf("   Yacc: insert input \"%s\"\n", $2->c_str()); */
    Shell::_currentCommand._inFileName = $2;
  }
  | GREATAMPERSAND WORD {
    /* printf("   Yacc: insert output >& \"%s\"\n", $2->c_str()); */
    Shell::_currentCommand._outFileName = $2;
    std::string *arg = new std::string;
    *arg = *($2);
    Shell::_currentCommand._errFileName = arg;
    Shell::_currentCommand._outCount = Shell::_currentCommand._outCount + 1;
  }
  | TWOGREAT WORD {
    /* printf("   Yacc: insert stderr >& \"%s\"\n", $2->c_str()); */
    Shell::_currentCommand._errFileName = $2;
  }

  ;

iomodifier_list:
  iomodifier_list iomodifier_opt
  | /* can be empty */
  ;

background_opt:
  AMPERSAND {
  Shell::_currentCommand._backgnd = true;
  }
  | /* can be empty */
  ;

%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
