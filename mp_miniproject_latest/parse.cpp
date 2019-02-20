#include "parse.h"

// ARGUMENTS.
// skip spaces in the front of the string.
std::string InputString::skipspace(std::string input) {
  while (isspace(input[0])) {
    input = input.substr(1);
  }
  return input;
}

// count # of '\'.
size_t InputString::count_bslash(std::string substring) {
  if (substring.size() == 0)
    return 0;

  size_t count = 0;
  size_t index = substring.size() - 1;
  while (substring[index] == '\\') {
    count += 1;
    index--;
  }
  return count;
}

// process substring to delete '\'.
std::string InputString::process_bslash(std::string substring) {
  std::cout << "\nprocessing_bslash: " << substring << std::endl;

  std::string temp;
  for (size_t i = 0; i < substring.length(); i++) {
    // push characters that are not '\'.
    if (substring[i] != '\\') {
      temp.push_back(substring[i]);
    }

    // if input contains '\\', then should keep one '\'.
    if (i != substring.length() - 1 && substring[i] == '\\' && substring[i + 1] == '\\') {
      temp.push_back(substring[i]);
      i++;
    }
  }

  std::cout << "processed_bslash: " << temp << "\n" << std::endl;
  return temp;
}

// process filename.
std::string InputString::processFilename(std::string substr) {
  substr = skipspace(substr);
  reverse(substr.begin(), substr.end());

  substr = skipspace(substr);
  reverse(substr.begin(), substr.end());

  return substr;
}

// parse the input string, store into command and vector of arguments.
void InputString::parse(std::string inputstring) {
  std::cout << "BEFORE PARSING: " << inputstring << std::endl;
  if (inputstring.size() == 0) {
    std::cout << "parse error: empty input string." << std::endl;
    throw std::invalid_argument("parse error: empty inputstring.");
  }

  // define whitespace.
  std::string whitespace = " \t\n\v\f\r";
  std::string temp = processFilename(inputstring);

  size_t begin = 0;
  size_t end = locate_divide(temp, whitespace, begin);

  // push argument into vector.
  if (end != std::string::npos) {
    std::string processed = process_bslash(temp.substr(begin, end - begin));
    arguments.push_back(processed);
    parse(temp.substr(end + 1));
  }
  else {
    std::string processed = process_bslash(temp.substr(begin));
    arguments.push_back(processed);
    return;
  }
}

// VARIABLES.
// check if variable name is valid.
int InputString::checkValid(std::string substr) {
  for (int idx = 1; idx < (int)substr.length(); idx++) {
    if (!(isalnum(substr[idx]) || substr[idx] == '_')) {
      return idx;
    }
  }
  return -1;
}

// check if '$' should be escaped.
bool InputString::isEscapeDollar(std::string & inputstr, size_t pos) {
  // check if matched with '\$...'
  if (pos == 1 && inputstr[pos - 1] == '\\') {
    return true;
  }

  // check if matched with '...\$...'
  if (pos > 1 && inputstr[pos - 1] == '\\' && inputstr[pos - 2] != '\\') {
    return true;
  }

  return false;
}

// replace variable in arguments.
std::string InputString::replace(std::string & input, Variable localvar) {
  size_t begin = 0;

  // if found $ in string, then replace variable in map.
  while ((begin = input.find('$', begin)) != std::string::npos) {
    // if contians '\$', then skip this variable.
    if (isEscapeDollar(input, begin)) {
      begin++;
      continue;
    }

    size_t end = input.find('$', begin + 1);
    int nonvar = begin + checkValid(input.substr(begin));

    // if between two '$', exist invalid character, then replace end with nonvar.
    if ((size_t)nonvar != begin - 1 && end > (size_t)nonvar) {
      end = nonvar;
    }

    // parse keystring.
    std::string keystring;
    if (end == std::string::npos) {
      keystring = input.substr(begin + 1);
    }
    else {
      keystring = input.substr(begin + 1, end - 1 - begin);
    }
    std::cout << "keystring: " << keystring << std::endl;

    // match key with value.
    std::string value = localvar.lookupVal(keystring);
    std::cout << "varstring: " << value << std::endl;

    // if cannot find a match, then do not replace.
    if (value == "") {
      begin++;
      continue;
    }

    // replace variable with value.
    input.replace(begin, end - begin, value);
    std::cout << "input replaced to: " << input << std::endl;

    begin = begin + value.length();
  }
  return input;
}

// REDIRECTION.
// match the redirection part of input from begin to end, and return the processed filename.
std::string InputString::match_redirection(std::string & input, size_t begin, size_t end) {
  // find the end of valid filename.
  if (end != std::string::npos) {
    end = input.find_first_of("<> ", end + 1);
  }

  // if not found, set to the end of input.
  if (end == std::string::npos) {
    end = input.length();
  }

  // parse out the filename and process it.
  std::string filename = input.substr(begin + 1, end - begin - 1);
  filename = processFilename(filename);

  // erase the corresponding part of redirection.
  input.erase(begin, end - begin);
  return filename;
}

// return the count of the occurance of toSearch string.
size_t InputString::findOccurance(std::string & data, std::string & toSearch) {
  size_t count = 0;
  size_t pos = data.find(toSearch);

  // repeat till end is reached.
  while (pos != std::string::npos) {
    // inc count.
    count++;

    // get the next occurrence from the current position.
    pos = data.find(toSearch, pos + toSearch.size());
  }
  return count;
}

// check if redirection format is valid.
bool InputString::checkRedirectValid(std::string & input) {
  std::string errstr = " 2>";
  size_t in_count = std::count(input.begin(), input.end(), '<');
  size_t err_count = findOccurance(input, errstr);

  // if input redirection and error redirection occurs more than one, return false.
  if (in_count > 1 || err_count > 1) {
    return false;
  }

  // if output redirection occurs more than one, check if one of them is error redirection first.
  size_t out_count = std::count(input.begin(), input.end(), '>');
  if (out_count > 2) {
    return false;
  }
  else if (out_count == 2 && err_count != 1) {
    return false;
  }

  // else return valid.
  return true;
}

// check if redirection file actually exist.
bool InputString::checkFileExist(std::string & inputfname) {
  std::string fdir;
  std::string fname;
  // split fdir and fname.
  size_t pos = inputfname.find_last_of('/');
  if (pos != std::string::npos) {
    fdir = inputfname.substr(0, pos);
    fname = inputfname.substr(pos + 1);
  }
  else {
    fdir = "./";
    fname = inputfname;
  }
  // open the directory.
  DIR * directory = opendir(fdir.c_str());
  if (directory == NULL) {
    perror("failed to open directory ");
    closedir(directory);
    return false;
  }
  // look for matching command.
  struct dirent * filename;
  while ((filename = readdir(directory)) != NULL) {
    if (filename->d_type == DT_LNK || filename->d_type == DT_REG) {
      // if matched, means filename exists.      
      if (fname.compare(filename->d_name) == 0) {
        closedir(directory);
        return true;
      }
    }
  }
  std::cout << "cannot find redirection file" << std::endl;
  closedir(directory);
  return false;
}

// set up redirection in inputstring.
std::string InputString::setRedirect(std::string & input) {
  inputFile = "", outputFile = "", errorFile = "";
  // check if input string is valid.
  if (!checkRedirectValid(input)) {
    std::cout << "redirection error: invalid format." << std::endl;
    throw std::invalid_argument("redirection error: invalid format.\n");
  }
  // if valid, setting up redirection.
  size_t begin = 0, end = 0;

  // find '<'.
  begin = input.find("<");
  if (begin != std::string::npos) {
    end = input.find_first_not_of(" ", begin + 1);
    std::string filename = match_redirection(input, begin, end);
    std::cout << "input filename: " << filename << "." << std::endl;
    // check if got valid input filename.
    if (filename.empty()) {
      std::cout << "detect redirection, but got empty inputfile." << std::endl;
      throw std::invalid_argument("invalid inputfile.");
    }
    // check if input file exists.
    if (!checkFileExist(filename)) {
      throw std::invalid_argument("invalid inputfile.");
    }
    inputFile = filename;
    std::cout << "input becomes: " << input << std::endl;
  }

  // find ' 2>'.
  begin = input.find(" 2>");
  if (begin != std::string::npos) {
    end = input.find_first_not_of(" ", begin + 3);
    std::string filename = match_redirection(input, begin + 3, end);
    std::cout << "error filename: " << filename << std::endl;
    // check if got valid input filename.
    if (filename.empty()) {
      std::cout << "detect redirection, but got empty errorfile." << std::endl;
      throw std::invalid_argument("invalid inputfile.");
    }
    errorFile = filename;
    // erase the error redirection.
    input.erase(begin, 3);
    std::cout << "input becomes: " << input << std::endl;
  }

  // find '>'.
  begin = input.find(">");
  if (begin != std::string::npos) {
    end = input.find_first_not_of(" ", begin + 1);
    std::string filename = match_redirection(input, begin, end);
    std::cout << "output filename: " << filename << "." << std::endl;
    // check if got valid input filename.
    if (filename.empty()) {
      std::cout << "detect redirection, but got empty outputfile." << std::endl;
      throw std::invalid_argument("invalid inputfile.");
    }
    outputFile = filename;
    std::cout << "input becomes: " << input << std::endl;
  }

  input = processFilename(input);
  return input;
}
