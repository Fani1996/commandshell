#ifndef PARSE_H
#define PARSE_H

#include <dirent.h>

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "set_variable.h"

class InputString
{
 private:
  std::string input;
  std::vector<std::string> arguments;

  std::string inputFile;
  std::string outputFile;
  std::string errorFile;

  // skip spaces in the front of the string.
  std::string skipspace(std::string input);

  // count # of '\'.
  size_t count_bslash(std::string substring);

  // process substring to delete '\'.
  std::string process_bslash(std::string substring);

  // process filename.
  std::string processFilename(std::string substr);

  // parse the input string, store into command and vector of arguments.
  void parse(std::string inputstring);

  // check if variable name is valid.
  int checkValid(std::string substr);

  // check if '$' should be escaped.
  bool isEscapeDollar(std::string & inputstr, size_t pos);

  // replace variable in arguments.
  std::string replace(std::string & input, Variable localvar);

  // match the redirection from begin to end, and return the processed filename.
  std::string match_redirection(std::string & input, size_t begin, size_t end);

  // return the count of the occurance of toSearch string.
  size_t findOccurance(std::string & data, std::string & toSearch);

  // check if redirection format is valid.
  bool checkRedirectValid(std::string & input);

  // check if redirection file actually exist.
  bool checkFileExist(std::string & inputfname);

  // set up redirection in inputstring.
  std::string setRedirect(std::string & input);

 public:
  // locate the location of dividing.
  size_t locate_divide(std::string & inputstring, std::string divider, size_t pos) {
    pos = inputstring.find_first_of(divider, pos);
    if (pos != std::string::npos) {
      std::string substring = inputstring.substr(0, pos);
      if (count_bslash(substring) % 2 != 0) {
        return locate_divide(inputstring, divider, pos + 1);
      }
    }

    return pos;
  }

  // setting up input.
  void setinput(std::string inputstring, Variable localvar) {
    input = inputstring;
    // clear previous argument vector.
    arguments.clear();
    inputFile = "";
    outputFile = "";
    errorFile = "";

    // replace the variable.
    input = replace(input, localvar);
    try {
      // set rediction in input string.
      input = setRedirect(input);
      // parse input string to command and arguments.
      parse(input);
    }
    catch (...) {
      throw std::invalid_argument("empty input string.\n");
    }
  }

  std::string getCommand() { return arguments[0]; }

  std::vector<std::string> getArguments() { return arguments; }

  std::string getInfile() { return inputFile; }

  std::string getOutfile() { return outputFile; }

  std::string getErrfile() { return errorFile; }
};

#endif /* PARSE_H */
