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
    out_file_( new TFile("output.root", "RECREATE") ),
//    out_calibration_file_(new TFile("qn.root", "RECREATE")),
    correction_manager_(){
    out_tree_ = new TTree("tree", "tree");
  };

  virtual ~CorrectionTask() = default;
  virtual void Run(){};
  inline void SetOutCalibrationFile(const std::string& file_name ) {
    out_calibration_file_.reset( TFile::Open( file_name.data(), "recreate" ) );
  }
  inline void SetInCalibrationFile(const std::string& file_name ) {
    in_calibration_file_.reset( TFile::Open( file_name.data(), "read" ) );
  }
  void SetOutCalibrationFile(const std::shared_ptr<TFile> &out_calibration_file) {
    out_calibration_file_ = out_calibration_file;
  }
  void SetOutFile(const std::string &out_file) {
    out_file_.reset(TFile::Open(out_file.data(), "recreate"));
    out_tree_ = new TTree("tree", "tree");
  }
protected:
  /**
   * Initializing function of Correction Task class. Makes configuration of
   * Correction Manager.
   */
  virtual void Initialize(){};
  /**
   * Processes one event;
   */
  virtual void Process(){};
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
  void Finalize(){
    correction_manager_.Finalize();
    correction_manager_.SaveOutput(out_calibration_file_, out_file_);
  };
  std::shared_ptr<TFile> out_file_;
  std::shared_ptr<TFile> in_calibration_file_;
  std::shared_ptr<TFile> out_calibration_file_;
  TTree *out_tree_;
  CorrectionManager correction_manager_;
  /**
   * Finalizes TestTask. Called after processing is done.
   */
};
} // namespace Qn
#endif // FLOW_QNTASK_H
