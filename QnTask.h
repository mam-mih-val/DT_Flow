//
// Created by Lukas Kreis on 29.06.17.
//

#ifndef FLOW_QNTASK_H
#define FLOW_QNTASK_H

#include <vector>
#include <array>
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

#include "QnCorrections/QnCorrectionsManager.h"
#include "DataContainer.h"
#include "EventInfo.h"
#include "ReducedEvent/AliReducedEventInfo.h"

class QnTask {
 public:
  QnTask() = default;
  QnTask(std::array<std::unique_ptr<TFile>, 4> files);
  ~QnTask() = default;
  void Run();
 private:
  /**
   * Initializes task;
   */
  void Initialize();
  /**
   * Processes one event;
   */
  void Process();
  /**
   * Finalizes task. Called after processing is done.
   */
  void Finalize();

 private:
  std::unique_ptr<TFile> in_file_;
  std::unique_ptr<TFile> out_file_;
  std::unique_ptr<TFile> in_calibration_file_;
  std::unique_ptr<TFile> out_calibration_file_;
  std::unique_ptr<TTree> in_tree_;
  std::unique_ptr<TTree> out_tree_;
  TTreeReader tree_reader_;
  TTreeReaderValue<AliReducedEventInfo> event_;
  std::unique_ptr<Qn::DataContainerQn> qn_data_;
  std::unique_ptr<Qn::EventInfoF> qn_eventinfo_f_;
  std::unique_ptr<Qn::EventInfoI> qn_eventinfo_i_;
  QnCorrectionsManager qn_manager_;
};

#endif //FLOW_QNTASK_H
