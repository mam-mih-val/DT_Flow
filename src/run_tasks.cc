//
// Created by mikhail on 5/6/20.
//

#include <Processing/task_factory.h>
#include <Processing/task_factory_track_density.h>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "Processing/task_factory.h"
#include "Processing/task_factory_fw3s.h"
#include "Processing/task_factory_rnd.h"

int main(int argc, char **argv) {
  auto start = std::chrono::system_clock::now();
  enum methods{
    kFW3S=0,
    kFWRS,
    kTD
  };
  std::map<std::string, int> method_map{
      std::make_pair( "FW3S", kFW3S ),
      std::make_pair( "FWRS", kFWRS ),
      std::make_pair( "TD", kTD ),
      std::make_pair( "FW3X", 2 ),
      std::make_pair( "FULL", 3 )
  };
  Configuration config;
  float min_signal = 0;
  float max_signal = 999;
  std::string method;
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
      if (flag == "-p") {
        config.pid_code = std::atoi(argv[i + 1]);
        continue;
      }
      if (flag == "-m") {
        method = argv[i + 1];
        continue;
      }
      if( flag == "-t" ){
        config.trigger = argv[i+1];
        continue;
      }
      if( flag == "-e" ) {
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
  std::cout << "method:\t\t" << method << std::endl;
  std::cout << "trigger:\t" << config.trigger << std::endl;
  std::cout << "efficiency:\t" << config.efficiency_file << std::endl;

  TaskFactory* factory;
  switch ( method_map.at(method) ) {
  case kFW3S:
    factory = TaskFactoryFw3s::GetInstance();
    break;
  case kFWRS:
    factory = TaskFactoryRnd::GetInstance();
    break;
  case kTD:
    factory = TaskFactoryTrackDensity::GetInstance();
    break;
  default:
    std::cout<<"Unknown method" << std::endl;
    return 1;
  }
  factory->SetConfig(config);
  factory->RunJob();

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
