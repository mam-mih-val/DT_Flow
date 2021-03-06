#include <chrono>
#include <iomanip>
#include <iostream>

#include "CorrectionTask.h"

int main(int argc, char **argv) {
  auto start = std::chrono::system_clock::now();
  if (argc < 3) {
    cout << "Error 1: incorrect number of arguments" << endl;
    cout << argc << " arguments were given, minimum 2 is required" << endl;
    return 1;
  }
  bool channelSelection = false;
  std::string signal = "adc";
  std::string method = "FW3S";
  std::string trigger = "PT3";
  std::string efficiency_file{};
  std::string qn_file{};

  int pid = 14;
  float minSignal = 0;
  float maxSignal = 999;
  if (argc > 4) {
    for (int i = 1; i < argc - 2; i += 2) {
      std::string flag = argv[i];
      if (flag == "--signal") {
        signal = argv[i + 1];
        continue;
      }
      if (flag == "--perchannel") {
        channelSelection = std::atoi(argv[i + 1]);
        continue;
      }
      if (flag == "--min") {
        minSignal = std::atof(argv[i + 1]);
        continue;
      }
      if (flag == "--max") {
        maxSignal = std::atof(argv[i + 1]);
        continue;
      }
      if (flag == "--pid") {
        pid = std::atoi(argv[i + 1]);
        continue;
      }
      if (flag == "--method") {
        method = argv[i + 1];
        continue;
      }
      if( flag == "--trigger" ){
        trigger = argv[i+1];
        continue;
      }
      if( flag == "--eff" ){
        efficiency_file = argv[i+1];
        continue;
      }
      if( flag == "--qn" ){
        qn_file = argv[i+1];
        continue;
      }
    }
  }
  std::cout << "Configuration:" << std::endl;
  std::cout << "per channer:\t" << channelSelection << std::endl;
  std::cout << "signal:\t\t" << signal << std::endl;
  std::cout << "min signal:\t" << minSignal << std::endl;
  std::cout << "max signal:\t" << maxSignal << std::endl;
  std::cout << "pid code:\t" << pid << std::endl;
  std::cout << "method:\t\t" << method << std::endl;
  std::cout << "trigger:\t" << trigger << std::endl;
  std::cout << "efficiency:\t" << efficiency_file << std::endl;
  std::cout << "Qn-file:\t" << qn_file << std::endl;


  Qn::CorrectionTask task(argv[argc - 2], argv[argc - 1], qn_file, efficiency_file);
  task.GetSelector()->SetFwChannelSelection(channelSelection);
  task.GetSelector()->SetFwSignalType(signal);
  task.GetSelector()->SetFwSignalRange(minSignal, maxSignal);
  task.GetSelector()->SetTrigger(trigger);
  task.SetParticlePid(pid);

//  task.SetSelectorConfiguration(channelSelection, signal, minSignal, maxSignal,
//                                pid);
  task.Run(method);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
