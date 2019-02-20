#ifndef MATCH_FUNCTION_H
#define MATCH_FUNCTION_H

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class CommandTable
{
 private:
  const char * path_p;
  std::vector<std::string> ctable;

  // buid command table for matching command.
  void buildctable();

  // match command with directory, and then output processed command.
  std::string matchWithDirectory(std::string currDirectory, std::string command);
  
  // search command under current directory, and output processed command.
  std::string searchCurrent(std::string cppcommand);

 public:
  // constructor.
  CommandTable() {
    // get PATH using std::getenv.
    path_p = std::getenv("PATH");

    // check if getenv is success, otherwise exit FAILURE.
    if (path_p == NULL) {
      perror("failed to load PATH ");
      exit(EXIT_FAILURE);
    }
    std::cout << "Your PATH is: " << path_p << std::endl;

    // build command table.
    buildctable();
  }

  // match command in PATH, return complete filename with abusolute path.
  std::string match_function(std::string command);
};

#endif /* MATCH_FUNCTION_H */
