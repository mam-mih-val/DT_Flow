//
// Created by Lukas Kreis on 29.06.17.
//

#ifndef FLOW_QNTestTask_H
#define FLOW_QNTestTask_H

#include <vector>
#include <array>
#include <random>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"

#include "StFemtoDstReader.h"
#include "StFemtoDst.h"
#include "StFemtoEvent.h"
#include "StFemtoTrack.h"

#include "FemtoDstVarManager.h"
#include "CorrectionManager.h"


#define VAR FemtoDstVarManager

namespace Qn {
/**
 * Qn vector analysis TestTask. It is to be configured by the user.
 * @brief TestTask for analysing qn vectors
 */
class CorrectionTask {
 public:
  CorrectionTask(std::string filelist, std::string incalib);
  ~CorrectionTask() = default;
  void Run();

 private:
  /**
   * Initializes TestTask;
   */
  void Initialize();
  /**
   * Processes one event;
   */
  void Process();
  /**
   * Finalizes TestTask. Called after processing is done.
   */
  void Finalize();
  
  Bool_t isGoodEvent(StFemtoEvent *const &event);
  Bool_t isGoodTrack(StFemtoTrack *const &track, Float_t _energy, TVector3 pVtx);
  
 protected:
  std::shared_ptr<TFile> out_file_;
  std::shared_ptr<TFile> in_calibration_file_;
  std::shared_ptr<TFile> out_calibration_file_;
  StFemtoDst *dst;
  StFemtoEvent *event;
  StFemtoDstReader* femtoReader;
  TTree *out_tree_;
  TTreeReader tree_reader_;
  Qn::CorrectionManager manager_;
  bool write_tree_;
  
  // Used constants (global)
  const Int_t energy = 200.;

  // Used constants for the event cut
  const std::map<Float_t, Double_t> cutVtxZEnergy = {{7.7, 70.}, {11.5, 50.}, {19.6, 70}, {27., 70.}, {39., 40.}, {62., 40.}, {200., 30.}};
  const Double_t cutVtxR = 2.;
  const Double_t cutVpdVz = 3.;

  // Used constants for the track cut
  const std::map<Float_t, Double_t> cutDCA = {{7.7, 1.}, {11.5, 1.}, {19.6, 1.}, {27., 1.}, {39., 1.}, {62., 1.}, {200., 3.}};
  const Double_t cutEta = 1.;
  const Int_t    cutNhits = 15;
  const Double_t cutNhitsPoss = 0.;
  const Double_t cutNhitsRatio = 0.51;
  const std::map<Float_t, Double_t> cutPtMin = {{7.7, 0.2}, {11.5, 0.2}, {19.6, 0.2}, {27., 0.2}, {39., 0.2}, {62., 0.2}, {200., 0.15}};
  const Double_t cutPMax = 10.;

};
}
#endif //FLOW_QNTASK_H
