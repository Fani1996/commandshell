#include <fstream>
#include <iostream>

#include "match_function.h"
#include "shell_function.h"

int main() {
  Shell shell;

  do {
    // read input from stdin.
    int read_signal = -1;
    try {
      read_signal = shell.read_input();
    }
    catch (...) {
      continue;
    }

    // if input is empty, then do nothing.
    if (read_signal == -1) {
      continue;
    }

    // if match exit, then break the loop.
    if (read_signal == -2) {
      break;
    }

    // loop through all the commands.
    shell.loopCommand();

  } while (true);
}
