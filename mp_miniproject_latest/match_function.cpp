#include "match_function.h"

#include <cstdlib>
#include <exception>
#include <iostream>

#include "shell_function.h"

// buid command table for matching command.
void CommandTable::buildctable() {
  std::string pathstring(path_p);

  // parse the PATH string.
  while (!pathstring.empty()) {
    size_t index = pathstring.find(':');

    if (index != std::string::npos) {
      std::string subpath = pathstring.substr(0, index);
      std::cout << "parsed PATH is: " << subpath << std::endl;
      ctable.push_back(subpath);
      pathstring = pathstring.substr(index + 1);
    }
    else {
      ctable.push_back(pathstring.substr(0));
      break;
    }
  }
}

// match command with directory, and then output processed command.
std::string CommandTable::matchWithDirectory(std::string currDirectory, std::string command) {
  // open the directory.
  DIR * directory = opendir(currDirectory.c_str());
  if (directory == NULL) {
    perror("failed to open directory ");
    closedir(directory);
    return "";
  }

  // look for matching command.
  struct dirent * filename;
  while ((filename = readdir(directory)) != NULL) {
    if (filename->d_type == DT_LNK || filename->d_type == DT_REG) {
      // comapre each filename to command.
      if (!std::strcmp(filename->d_name, command.c_str())) {
        closedir(directory);

        // if matched, return directory + filename.
        std::string output = currDirectory + '/' + command;
        std::cout << "command found. processed command: " << output << std::endl;
        return output;
      }
    }
  }
  closedir(directory);
  return "";
}

// search command under current directory, and output processed command.
std::string CommandTable::searchCurrent(std::string cppcommand) {
  size_t pos = cppcommand.find_last_of('/');
  std::string directory = cppcommand.substr(0, pos);
  std::string command = cppcommand.substr(pos + 1);
  std::cout << "searchCurrent:" << command << std::endl;
  std::string commandpath = matchWithDirectory(directory, command.c_str());

  if (commandpath.empty()) {
    return "";
  }
  return commandpath;
}

/* 
   FUNCTION: match_function(const char * command)
   match input command with functions from PATH. 
   return complete filename with abusolute path.
*/
std::string CommandTable::match_function(std::string command) {
  // check if command has '/', if found, search in current directory.
  if (command.find_first_of('/') != std::string::npos) {
    std::string commandpath = searchCurrent(command);

    // if not found, then output an error.
    if (commandpath.empty()) {
      std::cout << "Command " << command << " not found" << std::endl;
      throw std::runtime_error("failed to match command.\n");
    }
    return commandpath;
  }
  // loop through ctable, and match command in each directory.
  for (size_t i = 0; i < ctable.size(); i++) {
    std::string currDirectory = ctable[i];
    std::string commandpath = matchWithDirectory(currDirectory, command);
    if (!commandpath.empty()) {
      return commandpath;
    }
  }
  // if failed to match anything, report error, and exit.
  std::cout << "failed to match command in PATH." << std::endl;
  std::cout << "Command " << command << " not found" << std::endl;
  throw std::runtime_error("failed to match command.\n");
}
