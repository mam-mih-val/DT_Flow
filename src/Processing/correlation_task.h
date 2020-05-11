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
    std::cout << "Processing Correlation Task" << std::endl;
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
    in_tree_ = this->MakeChain(file_list, "tree");
    reader_.reset(new TTreeReader(in_tree_.get()));
  }

private:
  std::unique_ptr<TTree> in_tree_{nullptr};
  std::shared_ptr<TTreeReader> reader_{nullptr};

  void ProgressBar(float progress){
    int barWidth = 100;
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos)
        std::cout << "=";
      else if (i == pos)
        std::cout << ">";
      else
        std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
  };

  /**
   * Make TChain from file list
   * @param filename name of file containing paths to root files containing the
   * input trees
   * @return Pointer to the TChain
   */
  std::unique_ptr<TTree> MakeChain(const std::string& fileList, const std::string& treename){
    auto file = new TFile(fileList.c_str());
    std::unique_ptr<TTree> chain((TTree *)file->Get("tree"));
    //   chain->AddFile(fileList.c_str());
    return chain;
  }
  bool isSimulation{false};
};