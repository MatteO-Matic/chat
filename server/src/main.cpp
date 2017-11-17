#include "Server.h"
#include <exception>
#include <iostream>

int main()
{
  try {
    Server s(3333);
    s.run();
  } catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }
}

