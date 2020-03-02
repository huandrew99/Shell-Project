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
#include <sys/stat.h>
#include <fcntl.h>

#include "command.hh"
#include "shell.hh"

extern char **environ;

Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommandsArray = std::vector<SimpleCommand *>();

    _outFileName = NULL;
    _inFileName = NULL;
    _errFileName = NULL;
    _append = false;
    _backgnd = false;
    _outCount = 0;

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
    _append = false;
    _backgnd = false;
    _outCount = 0;
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
        if (isatty(0)) {
          Shell::prompt();
        }
        return;
    }

    if ( !strcmp(_simpleCommandsArray[0]->_argumentsArray[0]->c_str(), "exit") ) {
      printf("Goodbye!!\n");
      exit(1);
    }

    //printf("out: %d", _outCount);
    if (_outCount > 1) {
      printf("Ambiguous output redirect.\n");
      clear();
      if (isatty(0)) {
        Shell::prompt();
      }
      return;
    }

    // Print contents of Command data structure
    //print();
    int tmpin = dup(0);
    int tmpou = dup(1);
    int tmper = dup(2);

    int infd;
    int erfd;

    if (_inFileName) {
      infd = open(_inFileName->c_str(), O_RDONLY);
      if (infd < 0) {
        perror("open");
        exit(1);
      }
      //printf("inF\n");
    }
    else {
      infd = dup(tmpin);
    }
    if (_errFileName) {
      if (_append) {
        erfd = open(_errFileName->c_str(), O_WRONLY|O_CREAT|O_APPEND, 0664);
        if (erfd < 0) {
          perror("open");
          exit(1);
        }
      }
      else {
        erfd = open(_errFileName->c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0664);
        if (erfd < 0) {
          perror("open");
          exit(1);
        }
      }
    }
    else {
      erfd = dup(tmper);
    }
    int ret;
    size_t simpleNum = _simpleCommandsArray.size();
    //printf("size%zu\n", simpleNum);
    int oufd;
    for (size_t j = 0; j < simpleNum; j++) {

      //printf("count:%zu\n", j);
      dup2(infd, 0);
      close(infd);
      dup2(erfd, 2);
      close(erfd);

      if (strcmp(_simpleCommandsArray[j]->_argumentsArray[0]->c_str(), "setenv") == 0) {
        setenv(_simpleCommandsArray[j]->_argumentsArray[1]->c_str(), _simpleCommandsArray[j]->_argumentsArray[2]->c_str(), true);
        clear();
        Shell::prompt();
        return;
      }

      if (strcmp(_simpleCommandsArray[j]->_argumentsArray[0]->c_str(), "unsetenv") == 0) {
        unsetenv(_simpleCommandsArray[j]->_argumentsArray[1]->c_str());
        clear();
        Shell::prompt();
        return;
      }

      if (strcmp(_simpleCommandsArray[j]->_argumentsArray[0]->c_str(), "source") == 0) {
        clear();
        Shell::prompt();
        return;
      }

      if (strcmp(_simpleCommandsArray[j]->_argumentsArray[0]->c_str(), "cd") == 0) {
        chdir(_simpleCommandsArray[j]->_argumentsArray[1]->c_str());

      }

      if (j == simpleNum - 1) {
        if (_outFileName) {
          if (_append) {
            //printf("outappend\n");
            oufd = open(_outFileName->c_str(), O_WRONLY|O_CREAT|O_APPEND, 0664);
            if (oufd < 0) {
              perror("open");
              exit(1);
            }
          }
          else {
            //printf("Outnotappend\n");
            oufd = open(_outFileName->c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0664);
            if (oufd < 0) {
              perror("open");
              exit(1);
            }
          }
        }
        else {
          oufd = dup(tmpou);
        }
        
      }
      else {
        //printf("pipe\n");
        int fdpipe[2];
        pipe(fdpipe);
        oufd = fdpipe[1];
        infd = fdpipe[0];
      }

      dup2(oufd, 1);
      close(oufd);

      ret = fork();

      size_t argument_size = _simpleCommandsArray[j]->_argumentsArray.size();
      char ** arr = new char *[argument_size + 1];
      size_t i = 0;
      // convert vectors to string array
      for (i = 0; i < argument_size; i++) {
        arr[i] = strdup(_simpleCommandsArray[j]->_argumentsArray[i]->c_str());

      }
      arr[i] = NULL;
      
      /*for (size_t k = 0; k < argument_size; k++) {
        printf("arr[%zu] %s\n", k, arr[k]);
      }*/
      if (ret == 0) {
      //printf("execute\n");
        if (!strcmp(_simpleCommandsArray[j]->_argumentsArray[0]->c_str(), "printenv")) {
          char **p=environ;
          while (*p != NULL) {
            printf("%s\n", *p);
            p++;
          }
          fflush(stdout);
          _exit(1);
        }
        execvp(arr[0], arr);
        fprintf(stderr, "%s: command not found\n", arr[0]);
        //perror("execvp");
        _exit(1);
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
    
    dup2(tmpin, 0);
    dup2(tmpou, 1);
    dup2(tmper, 2);
    close(tmpin);
    close(tmpou);
    close(tmper);

    if (!_backgnd) {
      //printf("!background\n");
      waitpid(ret, NULL, 0);
    }
    // Clear to prepare for next command
    clear();
    // Print new prompt
    if (isatty(0)) {
      Shell::prompt();
    }
    
}

SimpleCommand * Command::_currSimpleCommand;
