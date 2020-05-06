//
// Created by mikhail on 5/6/20.
//

#include <chrono>
#include <iomanip>
#include <iostream>

#include "Processing/task_factory.h"

int main(int argc, char **argv) {
  auto start = std::chrono::system_clock::now();

  std::map<std::string, int> method_map{
      std::make_pair( "FW3S", 0 ),
      std::make_pair( "FWRS", 1 ),
      std::make_pair( "FW3X", 2 ),
      std::make_pair( "FULL", 3 )
  };
  Configuration config;
  float min_signal = 0;
  float max_signal = 999;
  if (argc > 1) {
    for (int i = 1; i < argc; i += 2) {
      std::string flag = argv[i];
      if (flag == "--min") {
        min_signal = std::atof(argv[i + 1]);
        continue;
      }
      if (flag == "--max") {
        max_signal = std::atof(argv[i + 1]);
        continue;
      }
      if (flag == "--pid") {
        config.pid_code = std::atoi(argv[i + 1]);
        continue;
      }
      if (flag == "--method") {
        config.method = method_map.at(argv[i + 1]);
        continue;
      }
      if( flag == "--trigger" ){
        config.trigger = argv[i+1];
        continue;
      }
      if( flag == "--eff" ) {
        config.efficiency_file = argv[i + 1];
        continue;
      }
      if( flag == "-i" ){
        config.input_data = argv[i+1];
        continue;
      }
    }
  }
  std::cout << "Configuration:" << std::endl;
  std::cout << "min signal:\t" << min_signal << std::endl;
  std::cout << "max signal:\t" << max_signal << std::endl;
  std::cout << "input data:\t" << config.input_data << std::endl;
  std::cout << "pid code:\t" << config.pid_code << std::endl;
  std::cout << "method:\t\t" << config.method << std::endl;
  std::cout << "trigger:\t" << config.trigger << std::endl;
  std::cout << "efficiency:\t" << config.efficiency_file << std::endl;

  TaskFactory::GetInstance()->SetConfig(config);
  TaskFactory::GetInstance()->RunJob();

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
