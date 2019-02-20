#ifndef SHELL_FUNCTION_H
#define SHELL_FUNCTION_H

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <vector>

#include "match_function.h"
#include "parse.h"
#include "set_variable.h"

class Shell
{
 private:
  std::string inputstring;
  std::string currDir;
  char ** globalEnv;

  std::string command;
  std::vector<std::string> arguments;

  std::string infile;
  std::string outfile;
  std::string errfile;

  InputString input;
  CommandTable ctable;
  Variable localvar;
  bool isExit;

  // name adopted from pipe man page.
  int pipefd[2][2];

  // from inputstring, parse IO, command and arguments, and set shell's variable.
  void setInput(std::string inputstr);

  // set up the file IO.
  void setIOInput();
  void setIOError();
  void setIOOutput();

  // set up and close the pipe.
  void setupPipe();
  void closePipe();

  // split the inputstring into multiple command when detect pipe '|'.
  int splitCommand(std::string & inputstr);

  // match command with preset functions (cd, set, export, inc).
  bool match_preset();

  // execute command.
  pid_t execute();

  // print out the return status for executing the command.
  void printStatus(pid_t child_pid);

  // function implementation for 'cd'.
  void changeDirectory();

  // function for properly exit when detect error.
  void exitWhenFailure();

 public:
  Shell() : isExit(false) {
    // set up global env.
    extern char ** environ;
    globalEnv = environ;

    // set up current directory.
    currDir = std::getenv("PWD");
  }

  // read input from stdin, and set up value in shell accordingly.
  int read_input();

  // loop through all commands.
  void loopCommand();
};

#endif /* SHELL_FUNCTION_H */
