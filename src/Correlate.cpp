#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>

#include "CorrelationTask.h"

int main(int argc, char **argv) {
  using namespace std;
  string filelist{argv[1]};
  CorrelationTask st(filelist, "tree");
  std::cout << "Processing Correlation Task" << std::endl;
  auto start = std::chrono::system_clock::now();
  st.Run();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
