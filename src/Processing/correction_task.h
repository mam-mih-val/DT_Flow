//
// Created by Lukas Kreis on 29.06.17.
//

#ifndef FLOW_QNTestTask_H
#define FLOW_QNTestTask_H

#include <array>
#include <random>
#include <vector>

#include "CorrectionManager.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "data_tree_var_manager.h"
#include "selector.h"

#include "HADES_constants.h"

namespace Qn {
/**
 * Qn vector analysis TestTask. It is to be configured by the user.
 * @brief TestTask for analysing qn vectors
 */
class CorrectionTask {
public:
  CorrectionTask():
                     out_calibration_file_(new TFile("qn.root", "RECREATE")),
                     out_tree_(new TTree("tree", "tree")),
                     correction_manager_(){};
  CorrectionTask(const std::string& inFile,
      const std::string& incalib,
      const std::string& qn_file="",
      const std::string& efficiency="",
      const std::string& out_file="output.root");
  ~CorrectionTask() = default;
  virtual void Run();
  inline void SetInCalibrationFile(const std::string& file_name ) {
    in_calibration_file_.reset( TFile::Open( file_name.data() ) );
  }
  void SetOutCalibrationFile(const std::shared_ptr<TFile> &out_calibration_file) {
    out_calibration_file_ = out_calibration_file;
  }
  void SetOutFile(const std::string &out_file) {
    out_file_.reset(TFile::Open(out_file.data(), "recreate"));
  }

protected:
  /**
   * Initializing function of Correction Task class. Makes configuration of
   * Correction Manager.
   */
  virtual void Initialize(){};
  void InitializeFull();
  void InitializeFw3s();
  void InitializeFw3x();
  void InitializeRnd();
  void InitializeRndOptimization();
  /**
   * Processes one event;
   */
  virtual void Process();
  void ProgressBar(float progress);

protected:
  std::shared_ptr<TFile> out_file_;
  std::shared_ptr<TFile> in_calibration_file_;
  std::shared_ptr<TFile> out_calibration_file_;
  TTree *out_tree_;
  Qn::CorrectionManager correction_manager_;
  /**
   * Finalizes TestTask. Called after processing is done.
   */
  void Finalize();
};
} // namespace Qn
#endif // FLOW_QNTASK_H
