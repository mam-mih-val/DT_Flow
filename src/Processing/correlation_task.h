#pragma once

#include "TChain.h"
#include "TTreeReader.h"

#include <CorrelationManager.h>

class CorrelationTask {
public:
  CorrelationTask() = default;
  virtual void Configure(Qn::CorrelationManager &manager){};
  void Run(){
    int nEvents = in_tree_->GetEntries();
    Qn::CorrelationManager manager(reader_, nEvents);
    in_tree_->LoadTree(0); // prevents weird TTree errors
    Configure(manager);
    reader_->SetEntry(0); //  first entry needed to access configuration of
    //  DataContainers in the input file
    manager.Initialize();
    in_tree_->LoadTree(0); // prevents weird TTree errors
    std::cout << "Processing..." << std::endl;
    long long events = 0;
    do {
      manager.Process();
      events++;
    } while (reader_->Next());
    manager.Finalize();
    std::cout << std::endl
              << "number of analyzed events: " << events << std::endl;
  };
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