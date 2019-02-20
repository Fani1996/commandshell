#ifndef SET_VARIABLE_H
#define SET_VARIABLE_H

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

class Variable
{
 private:
  std::map<std::string, std::string> localVars;
  void set_helper(std::string key, std::string value);

 public:
  std::string lookupVal(std::string key);
  void setVar(std::vector<std::string> arguments);
  void exportVar(std::vector<std::string> arguments);
  void incVar(std::vector<std::string> arguments);
};

#endif /* SET_VARIABLE_H */
