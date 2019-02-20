#include "set_variable.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

// helper function: make the rest of arguments into string.
std::string argu2string(std::vector<std::string> arguments) {
  std::string output;

  // loop through arguments and combine.
  for (size_t i = 2; i < arguments.size(); i++) {
    output += arguments[i];

    if (i != arguments.size() - 1) {
      output += " ";
    }
  }

  return output;
}

// helper function: check if set variable name is valid.
bool isKeyValid(std::string key) {
  // check each char if is alphabet, number and '_'.
  for (size_t idx = 0; idx < key.length(); idx++) {
    if (!isalnum(key[idx]) && key[idx] != '_') {
      return false;
    }
  }

  return true;
}

// helper function: check if string contains only num.
bool isnumvalue(std::string key) {
  // check each char if is number.
  for (size_t i = 0; i < key.length(); i++) {
    if (!isdigit(key[i])) {
      return false;
    }
  }

  return true;
}

// helper function: translate value to string.
template<typename T>
std::string toString(T val) {
  std::stringstream stream;
  stream << val;
  return stream.str();
}

// helper function: translate string to value.
template<typename T>
T fromString(const std::string & string) {
  std::istringstream ss(string);
  T output;
  ss >> output;
  return output;
}

// helper function: look up value in system env.
std::string lookupSys(std::string key) {
  char * sysVar = getenv(key.c_str());
  if (sysVar != NULL) {
    return (std::string)sysVar;
  }
  return "";
}

// look up value in map for corresponding key.
std::string Variable::lookupVal(std::string key) {
  std::map<std::string, std::string>::iterator it = localVars.find(key);

  // first find key in system env.
  std::string sysVal;
  if ((sysVal = lookupSys(key)) != "") {
    return sysVal;
  }

  // find the value, return it.
  if (it != localVars.end()) {
    return it->second;
  }

  // find nothing, return empty.
  return "";
}

// helper function: first check validness of key, and set value in sys or localVars.
void Variable::set_helper(std::string key, std::string value) {
  // check validness of key.
  if (isKeyValid(key) == true) {
    // first check if key is in sys env.
    std::string sysVal;
    if ((sysVal = lookupSys(key)) != "") {
      // if found, update the value in sys env instead.
      if ((setenv(key.c_str(), value.c_str(), 1)) != 0) {
        throw std::runtime_error("export var error: set operation failed.\n");
      }
    }
    // if not found, update value in localVars.
    else {
      localVars[key] = value;
    }
  }

  // if key is invalid, output an error.
  else {
    std::cout << "set var error: key contains invalid characters." << std::endl;
    return;
  }
}

// set variable into localVars.
void Variable::setVar(std::vector<std::string> arguments) {
  if (arguments.size() < 3) {
    std::cout << "set var error: not enough arguments." << std::endl;
    return;
  }

  // get the key and the value.
  std::string key = arguments[1];
  std::string value = argu2string(arguments);

  std::cout << "original value of " << arguments[1] << ": " << lookupVal(arguments[1]) << std::endl;

  // set_helper to set the value to the key.
  set_helper(key, value);

  std::cout << "current value of  " << arguments[1] << ": " << lookupVal(arguments[1]) << std::endl;
}

// export variable to system env.
void Variable::exportVar(std::vector<std::string> arguments) {
  if (arguments.size() != 2) {
    std::cout << "export var error: invalid arguments number." << std::endl;
    return;
  }

  // look up value in the map.
  std::string key = arguments[1];
  if (isKeyValid(key) == false) {
    std::cout << "export var error: key contains invalid characters." << std::endl;
    return;
  }

  // try to look up the value, if not found, do nothing.
  std::string value = lookupVal(key);
  if (value == "") {
    std::cout << "export var error: no such key." << std::endl;
    return;
  }

  // export variable to system env.
  if ((setenv(key.c_str(), value.c_str(), 1)) != 0) {
    throw std::runtime_error("export var error: set operation failed.\n");
  }
}

// increase variable's value.
void Variable::incVar(std::vector<std::string> arguments) {
  if (arguments.size() != 2) {
    std::cout << "inc var error: invalid arguments number." << std::endl;
    return;
  }

  std::string key = arguments[1];

  // try to look up the value, if not found, do nothing.
  std::string value = lookupVal(key);
  if (value == "") {
    return;
  }

  // modify the value and store back to string.
  if (isnumvalue(value)) {
    int num = fromString<int>(value);
    value = toString<int>(num + 1);
  }
  else {
    value = toString<int>(1);
  }

  // then put the value back.
  set_helper(key, value);
}
