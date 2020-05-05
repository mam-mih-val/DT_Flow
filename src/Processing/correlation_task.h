#pragma once

#include "TChain.h"
#include "TTreeReader.h"

#include <CorrelationManager.h>

class CorrelationTask {
public:
  CorrelationTask() = default;
  CorrelationTask(std::string filelist, std::string treename);
  virtual void Configure(Qn::CorrelationManager &manager);
  void ConfigureEp(Qn::CorrelationManager &manager);
  void ConfigureRnd(Qn::CorrelationManager &manager);
  void ConfigureRndOptimization(Qn::CorrelationManager &manager);
  void ConfigureFw3s(Qn::CorrelationManager &manager);
  void ConfigureFw3x(Qn::CorrelationManager &manager);
  void Run(std::string method = "FW3S");
  void AddFiles(const std::string &file_list) {
    in_tree_ = this->MakeChain(std::move(file_list), "tree");
    reader_.reset(new TTreeReader(in_tree_.get()));
  }

private:
  std::unique_ptr<TTree> in_tree_{nullptr};
  std::shared_ptr<TTreeReader> reader_{nullptr};

  void ProgressBar(float progress);

  /**
   * Make TChain from file list
   * @param filename name of file containing paths to root files containing the
   * input trees
   * @return Pointer to the TChain
   */
  std::unique_ptr<TTree> MakeChain(std::string fileList, std::string treename);
  bool isSimulation{false};
};