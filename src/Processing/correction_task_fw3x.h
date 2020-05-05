//
// Created by mikhail on 5/5/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRECTION_TASK_FW3X_H_
#define FLOW_SRC_PROCESSING_CORRECTION_TASK_FW3X_H_
#include "correction_task.h"
namespace Qn {
class CorrectionTaskFw3x : public CorrectionTask {
public:
  void Initialize() override {
    correction_manager_.AddVariable("Centrality", DataTreeVarManager::kCentrality, 1);
    correction_manager_.AddVariable("One", DataTreeVarManager::kOne, 1);
    correction_manager_.AddVariable("Pt", DataTreeVarManager::kMdcPt, 1);
    correction_manager_.AddVariable("Phi", DataTreeVarManager::kMdcPhi, 1);
    correction_manager_.AddVariable("Ycm", DataTreeVarManager::kMdcYcm, 1);
    correction_manager_.AddVariable("Pid", DataTreeVarManager::kMdcPid, 1);
    correction_manager_.AddVariable("Eff", DataTreeVarManager::kMdcEfficiency, 1);
    correction_manager_.AddVariable("FwRing", DataTreeVarManager::kFwModuleRing, 304);
    correction_manager_.AddVariable("FwModuleId", DataTreeVarManager::kFwModuleId, 304);
    correction_manager_.AddVariable("FwAdc", DataTreeVarManager::kFwModuleAdc, 304);
    correction_manager_.AddVariable("FwPhi", DataTreeVarManager::kFwModulePhi, 304);
    correction_manager_.AddVariable("RandomSe", DataTreeVarManager::kRandomSe, 304);
    correction_manager_.AddVariable("moduleX", DataTreeVarManager::kFwModuleX, 304);
    correction_manager_.AddVariable("moduleY", DataTreeVarManager::kFwModuleY, 304);
    std::cout << "Variables added" << std::endl;
    // Correction eventvariables

    correction_manager_.SetEventVariable("Centrality");
    correction_manager_.AddCorrectionAxis({"Centrality", 8, 0, 40});

    Axis pt("Pt", 10, 0.0, 2.0);
    Axis ycm("Ycm", 15, -0.75, 0.75);

    // Configuration of MDC.
    auto MdcConfiguration = [](DetectorConfiguration *config) {
      config->SetNormalization(QVector::Normalization::M);
      auto recenter = new Recentering();
      config->AddCorrectionOnQnVector(recenter);
      auto rescale = new TwistAndRescale();
      rescale->SetApplyTwist(true);
      rescale->SetApplyRescale(true);
      rescale->SetTwistAndRescaleMethod(
          TwistAndRescale::TWRESCALE_doubleHarmonic);
      config->AddCorrectionOnQnVector(rescale);
    };

    // Configuration of FW. Preparing for add axis to qa histograms
    // Producing the function which will configurate the correction Manager

    auto FwConfiguration = [](DetectorConfiguration *config) {
      config->SetNormalization(QVector::Normalization::M);
      auto recenter = new Recentering();
      config->AddCorrectionOnQnVector(recenter);
      auto rescale = new TwistAndRescale();
      rescale->SetApplyTwist(true);
      rescale->SetApplyRescale(true);
      rescale->SetTwistAndRescaleMethod(
          TwistAndRescale::TWRESCALE_doubleHarmonic);
      //          config->AddCorrectionOnQnVector(rescale);
      auto fwChannels = new bool[304];
      auto fwChannelGroups = new int[304];
      for (int i = 0; i < 304; i++) {
        fwChannels[i] = true;
        fwChannelGroups[i] = i;
      }
      config->SetChannelsScheme(fwChannels, fwChannelGroups);
    };
    // u-vectors from MDC
    correction_manager_.AddDetector("TracksMdc", DetectorType::TRACK, "Phi", "Ones", {ycm, pt}, {2});
    correction_manager_.AddCut(
        "TracksMdc", {"Ycm", "Pt"},
        [](const double &y, const double &pt) {
          return
              -0.8 < y && y < 0.8
              && 0.0 < pt && pt < 2.0;
        });
    correction_manager_.SetCorrectionSteps("TracksMdc", MdcConfiguration);

    correction_manager_.AddDetector("MdcQ", DetectorType::TRACK, "Phi", "Ones", {ycm}, {2});
    correction_manager_.AddCut("MdcQ", {"Ycm", "Pt"},
                               [](const double &y, const double &pt) {
                                 return
                                     -0.8 < y && y < 0.8
                                     && 0.0 < pt && pt < 2.0;
                               });
    correction_manager_.SetCorrectionSteps("MdcQ", MdcConfiguration);

    // 3 sub-events method.
    // Each detector builds own Q-vector, which means, you need to add required
    // count of detectors and then configurate their cuts.

    correction_manager_.AddDetector("Fw1", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
    correction_manager_.AddCut("Fw1", {"FwRing"}, [](const double &module) {
      return module >= 1.0 && module <= 5.0;
    });
    correction_manager_.SetCorrectionSteps("Fw1", FwConfiguration);

    correction_manager_.AddDetector("Fw2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
    correction_manager_.AddCut("Fw2", {"FwRing"}, [](const double &module) {
      return module == 6.0 || module == 7.0;
    });
    correction_manager_.SetCorrectionSteps("Fw2", FwConfiguration);

    correction_manager_.AddDetector("Fw3", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
    correction_manager_.AddCut("Fw3", {"FwRing"}, [](const double &module) {
      return module >= 8.0 && module <= 10.0;
    });
    correction_manager_.SetCorrectionSteps("Fw3", FwConfiguration);

    correction_manager_.AddHisto2D("TracksMdc",
                                   {{"Pt", 200, 0., 2.}, {"Ycm", 160, -0.8, 0.8}});
    correction_manager_.AddHisto2D("TracksMdc",
                                   {{"Phi", 126, -3.15, 3.15}, {"Ycm", 160, -0.8, 0.8}});

    correction_manager_.AddHisto2D(
        "Fw1", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
    correction_manager_.AddHisto2D(
        "Fw2", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
    correction_manager_.AddHisto2D(
        "Fw3", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

    correction_manager_.AddEventHisto1D({{"Centrality", 20, 0, 100}});
    correction_manager_.SetTree(out_tree_);
    correction_manager_.Initialize(in_calibration_file_);
    std::cout << "Successfully initialized" << std::endl;
  }
  void Run() override {
    Initialize();
    QnCorrectionsSetTracingLevel(kError);
    std::cout << "Processing..." << std::endl;
    int goodEvents = 0;
    while( !DataTreeVarManager::GetInstance()->Eof() ){
      DataTreeVarManager::GetInstance()->SwitchNextGoodEvent();
      Process();
      goodEvents++;
    }
    std::cout << goodEvents << " good events" << std::endl;
    Finalize();
  }
protected:
  void Process() override {
    correction_manager_.Reset();
    DataTreeVarManager::GetInstance()->FillEventVariables(correction_manager_.GetVariableContainer());
    correction_manager_.ProcessEvent();
    correction_manager_.FillChannelDetectors();
    int trackNumber = DataTreeVarManager::GetInstance()->GetNumberOfTracks();
    for (int i = 0; i < trackNumber; i++) {
      if (!Selector::GetInstance()->IsCorrectTrack(i))
        continue;
      if(!Selector::GetInstance()->IsCorrectPid(i))
        continue;
      DataTreeVarManager::GetInstance()->FillTrackVariables(i,correction_manager_.GetVariableContainer());
      correction_manager_.FillTrackingDetectors();
    }
    correction_manager_.ProcessQnVectors();
  }
};
} // namespace Qn
#endif // FLOW_SRC_PROCESSING_CORRECTION_TASK_FW3X_H_
