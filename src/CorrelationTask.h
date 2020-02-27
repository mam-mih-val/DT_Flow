#pragma once

#include "TChain.h"
#include "TTreeReader.h"

#include <CorrelationManager.h>

class CorrelationTask {
 public:
  CorrelationTask() = default;
  CorrelationTask(std::string filelist, std::string treename);

  void Configure(Qn::CorrelationManager &manager);
  void ConfigureEp(Qn::CorrelationManager &manager);
  void ConfigureRnd(Qn::CorrelationManager &manager);
  void ConfigureFw3s(Qn::CorrelationManager &manager);
  void ConfigureFw3x(Qn::CorrelationManager &manager);
  void Run(std::string method="FW3S");

 private:
  std::unique_ptr<TTree> in_tree_;
  std::shared_ptr<TTreeReader> reader_;

  void ProgressBar(float progress);


  /**
   * Make TChain from file list
   * @param filename name of file containing paths to root files containing the input trees
   * @return Pointer to the TChain
   */
  std::unique_ptr<TTree> MakeChain(std::string fileList, std::string treename);
  bool isSimulation{false};
};