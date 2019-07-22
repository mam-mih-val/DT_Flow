#include <iostream>
#include <iomanip>
#include <chrono>

#include "CorrectionTask.h"

int main(int argc, char **argv) {
  std::cout << "go" << std::endl;
  Qn::CorrectionTask task(argv[1], argv[2]);
  task.Run();
  return 0;
}
