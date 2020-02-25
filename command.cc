/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "command.hh"
#include "shell.hh"


Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommandsArray = std::vector<SimpleCommand *>();

    _outFileName = NULL;
    _inFileName = NULL;
    _errFileName = NULL;
    _backgnd = false;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommandsArray.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommandsArray) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommandsArray.clear();

    if ( _outFileName ) {
        delete _outFileName;
    }
    _outFileName = NULL;

    if ( _inFileName ) {
        delete _inFileName;
    }
    _inFileName = NULL;

    if ( _errFileName ) {
        delete _errFileName;
    }
    _errFileName = NULL;

    _backgnd = false;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommandsArray ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFileName?_outFileName->c_str():"default",
            _inFileName?_inFileName->c_str():"default",
            _errFileName?_errFileName->c_str():"default",
            _backgnd?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    //printf("commandArraySize: %d\n", _simpleCommandsArray.size());
    if ( _simpleCommandsArray.size() == 0 ) {
        Shell::prompt();
        return;
    }

    // Print contents of Command data structure
    print();
    int tmpin = dup(0);
    int tmpou = dup(1);
    int tmper = dup(2);

    int ret;

    for (size_t j = 0; j < _simpleCommandsArray.size(); j++) {
      printf("count:%zu\n", j);
      ret = fork();
      size_t argument_size = _simpleCommandsArray[0]->_argumentsArray.size();
      char ** arr = new char *[argument_size + 1];
      size_t i = 0;
      for (i = 0; i < argument_size; i++) {
        arr[i] = strdup(_simpleCommandsArray[0]->_argumentsArray[i]->c_str());

      }
      arr[i] = NULL;
      //printf("arr0 %s, arr1 %s\n", arr[0], arr[1]);
      if (ret == 0) {
      //printf("execute\n");
        execvp(arr[0], arr);
        perror("execvp");
        exit(1);
      //printf("execute\n");
      }
      else if (ret < 0) {
        perror("fork");
        exit(2);
      }
    }
    //printf("clear\n");

    /*int size = _simpleCommandsArray.size();
    for (int i = 0; i < size) {
      for (
    }*/
    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

    if (!_backgnd) {
      printf("!background\n");
      waitpid(ret, NULL, 0);
    }
    // Clear to prepare for next command
    clear();
    // Print new prompt
    Shell::prompt();
    
}

SimpleCommand * Command::_currSimpleCommand;
