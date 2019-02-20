#include "shell_function.h"

#include <unistd.h>

#include <cstring>
#include <iostream>

// read input from stdin, and return std::string.
int Shell::read_input() {
  std::cout << "myShell:" << currDir << " $ ";
  if (std::getline(std::cin, inputstring).eof()) {
    std::cout << "EOF reached" << std::endl;
    return -2;
  }

  // if simply hit ENTER with no input, do nothing.
  if (inputstring.empty()) {
    return -1;
  }
  return 0;
}

// read inputstring and set up corresponding value.
void Shell::setInput(std::string inputstr) {
  // setting up input.
  try {
    input.setinput(inputstr, localvar);
  }
  catch (...) {
    throw std::invalid_argument("invalid inputstring format.\n");
  }

  // transfer value into shell class.
  command = input.getCommand();
  arguments = input.getArguments();

  infile = input.getInfile();
  outfile = input.getOutfile();
  errfile = input.getErrfile();

  std::cout << "\n### INPUT command: " << command << std::endl;
  if (command.compare("exit") == 0) {
    isExit = true;
  }
}

void Shell::setIOInput() {
  // check input file.
  if (!infile.empty()) {
    int filein = -1;
    if ((filein = open(infile.c_str(), O_RDONLY)) != -1) {
      dup2(filein, STDIN_FILENO);
    }
    else {
      std::cout << "failed to open input file." << std::endl;
      throw std::runtime_error("failed to open input file.\n");
    }
  }
}

void Shell::setIOError() {
  // check error file.
  if (!errfile.empty()) {
    int fileerr = -1;
    if ((fileerr = open(errfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) != -1) {
      dup2(fileerr, STDERR_FILENO);
    }
    else {
      std::cout << "failed to open error file." << std::endl;
      throw std::runtime_error("failed to open error file.\n");
    }
  }
}

void Shell::setIOOutput() {
  // check output file.
  if (!outfile.empty()) {
    int fileout = -1;
    if ((fileout = open(outfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) != -1) {
      dup2(fileout, STDOUT_FILENO);
    }
    else {
      std::cout << "failed to open output file." << std::endl;
      throw std::runtime_error("failed to open output file.\n");
    }
  }
}

// set up the pipe.
void Shell::setupPipe(){
  // read end.
  if (pipefd[0][0] != -1) {
    close(pipefd[0][1]);
    dup2(pipefd[0][0], 0);
  }
  // write end.
  if (pipefd[1][0] != -1) {
    close(pipefd[1][0]);
    dup2(pipefd[1][1], 1);
  }
}

// close the pipe.
void Shell::closePipe(){
  // close current pipe.
  if (pipefd[0][0] != -1) {
    close(pipefd[0][0]);
    close(pipefd[0][1]);
  }
  // assign current output pipe as next command's input pipe.
  pipefd[0][0] = pipefd[1][0];
  pipefd[0][1] = pipefd[1][1];
}

// parse '|' to split multiple command.
int Shell::splitCommand(std::string & inputstr) {
  size_t begin = 0, end = begin;
  pid_t child_pid = -1;
  pipefd[0][0] = -1,  pipefd[0][1] = -1;

  while (end != std::string::npos) {
    // split the inputstring using '|'.
    end = inputstr.find("|", begin);
    if (end == inputstr.length() - 1) {
      std::cout << "pipe error: invalid pip format." << std::endl;
      throw std::invalid_argument("invalid pipe format.\n");
    }
    std::string commandstring = inputstr.substr(begin, end - begin);
    std::cout << "\nSPLITTED: " << commandstring << std::endl;

    // use commandstring to set up arguments.
    try {
      setInput(commandstring);
    }
    catch (...) {
      throw std::invalid_argument("invalid inputstring format.\n");
    }
    // if the last command, close write end.
    if (end == std::string::npos) {
      pipefd[1][0] = -1;
      pipefd[1][1] = -1;
    }
    // check pipe error, adopted from pipe man page.
    else if (pipe(pipefd[1]) == -1) {
      perror("pipe error");
      continue;
    }

    if (isExit) {
      this->~Shell();
      std::exit(EXIT_SUCCESS);
    }

    // execute the command.
    try {
      child_pid = execute();
    }
    catch (...) {
      return -1;
    }
    // close pipe.
    closePipe();
    // execute next command.
    begin = end + 1;
  }

  return child_pid;
}

// matching preset functions, including 'cd', 'set' and 'export'.
bool Shell::match_preset() {
  // match with 'cd'.
  if (command.compare("cd") == 0) {
    std::string temp = currDir;
    try {
      changeDirectory();
    }
    catch (std::runtime_error) {
      // cd error occurred, fatal error.
      throw std::runtime_error("cd error occurred.\n");
    }
  }
  // match with 'set'.
  else if (command.compare("set") == 0) {
    localvar.setVar(arguments);
  }
  // match with 'export'.
  else if (command.compare("export") == 0) {
    localvar.exportVar(arguments);
    // update the env.
    extern char ** environ;
    globalEnv = environ;
  }
  // match with 'inc'.
  else if (command.compare("inc") == 0) {
    localvar.incVar(arguments);
  }
  // if nothing matched, return false, and continue seraching in PATH.
  else {
    return false;
  }

  return true;
}

// execute the command, with its arguments.
pid_t Shell::execute() {
  // check if command is empty.
  if (command == "") {
    std::cout << "empty command." << std::endl;
    throw std::invalid_argument("empty command.");
  }

  // if match with preset function, run them in parent process.
  try {
    if (match_preset() == true) {
      return -1;
    }
  }
  catch (...) {
    throw std::runtime_error("match_preset error.");
  }

  // try to match the function with the command.
  std::string comm;
  try {
    comm = ctable.match_function(command);
  }
  catch (...) {
    throw std::runtime_error("match_preset error.");
  }

  // otherwise, fork new child process.
  pid_t child_pid = fork();
  // check if failed to fork.
  if (child_pid == -1) {
    perror("failed to fork ");
    exitWhenFailure();
  }
  // if fork success.
  if (child_pid == 0) {
    // setting up pipe.
    setupPipe();
    // setting up I/O.
    try {
      setIOInput();
      setIOError();
      setIOOutput();
    }
    catch (...) {
      std::cout << "failed to setting IO." << std::endl;
      exitWhenFailure();
    }

    // set up arguments for execve.
    char ** argv = new char *[arguments.size() + 1];
    for (int idx = 0; idx < (int)arguments.size(); idx++) {
      argv[idx] = (char *)arguments[idx].c_str();
    }
    // The argv include a null pointer at the end of the array.
    argv[arguments.size()] = (char *)0;

    // run the child program.
    execve(comm.c_str(), argv, globalEnv);
    // if return, means error occurs.
    delete[] argv;
    this->~Shell();
    perror("execve error ");
    exit(EXIT_FAILURE);
  }

  return child_pid;
}

void Shell::printStatus(pid_t child_pid) {
  int wstatus;
  pid_t w;

  // using waitpid to print status, adopted from man page.
  if (child_pid > 0) {
    do {
      if ((w = waitpid(child_pid, &wstatus, WUNTRACED | WCONTINUED)) == -1) {
        perror("failed when doing waitpid.\n");
        exit(EXIT_FAILURE);
      }

      if (WIFEXITED(wstatus)) {
        printf("Program exited with status %d\n", WEXITSTATUS(wstatus));
      }
      else if (WIFSIGNALED(wstatus)) {
        printf("Program was killed by signal %d\n", WTERMSIG(wstatus));
      }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }
}

void Shell::exitWhenFailure() {
  this->~Shell();
  exit(EXIT_FAILURE);
}

void Shell::changeDirectory() {
  // arguments has NULL pointer in the back.
  std::cout << "cd method matched." << std::endl;

  const char * path;
  // cd with no path. change to the home directory.
  if (arguments.size() == 1) {
    path = getenv("HOME");
  }
  // cd with a path. change to that directory.
  else if (arguments.size() == 2) {
    path = arguments[1].c_str();
  }
  // cd with too many path at same time.
  else {
    std::cout << "cd error: too many arguments to call." << std::endl;
    return;
  }

  // actual operaion to change directory.
  if (chdir(path) == 0) {
    char * currpath = get_current_dir_name();
    currDir = currpath;
    free(currpath);
  }
  else {
    perror("failed to open directory ");
    throw std::runtime_error("cd error: failed to change directory.\n");
  }

  // set new directory to be PWD.
  if ((setenv("PWD", currDir.c_str(), 1)) != 0) {
    perror("failed to set directory ");
    throw std::runtime_error("cd error: failed to export PWD.\n");
  }
}

void Shell::loopCommand() {
  pid_t initpid = -1;
  try {
    initpid = splitCommand(inputstring);
    // child process forked, deal with output signal.
    if (initpid > 0) {
      printStatus(initpid);
    }
  }
  // if error occurs, catch error and print status.
  catch (std::runtime_error) {
    printStatus(initpid);
    return;
  }
  // if invalid input string, ignore.
  catch (std::invalid_argument) {
    return;
  }
}
