#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "CorrelationTask.h"

int main(int argc, char **argv) {
  using namespace std;
  std::string method = "FW3S";
  if (argc > 2) {
    for (int i = 1; i < argc - 2; i += 2) {
      std::string flag = argv[i];

      if (flag == "--method") {
        method = argv[i + 1];
        continue;
      }
    }
  }
  std::cout << "method:\t" << method << std::endl;
  string filelist{argv[argc-1]};
  CorrelationTask st(filelist, "tree");
  std::cout << "Processing Correlation Task" << std::endl;
  auto start = std::chrono::system_clock::now();
  st.Run(method);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
