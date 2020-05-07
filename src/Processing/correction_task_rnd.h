//
// Created by mikhail on 5/4/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRECTION_TASK_RND_H_
#define FLOW_SRC_PROCESSING_CORRECTION_TASK_RND_H_

#include "correction_task.h"
namespace Qn {
class CorrectionTaskRnd : public CorrectionTask {
public:
  void Initialize() override {
    correction_manager_.AddVariable("Centrality", DataTreeVarManager::kCentrality, 1);
    correction_manager_.AddVariable("One", DataTreeVarManager::kOne, 1);
    correction_manager_.AddVariable("Pt", DataTreeVarManager::kMdcPt, 1);
    correction_manager_.AddVariable("Phi", DataTreeVarManager::kMdcPhi, 1);
    correction_manager_.AddVariable("Ycm", DataTreeVarManager::kMdcYcm, 1);
    correction_manager_.AddVariable("Pid", DataTreeVarManager::kMdcPid, 1);
    correction_manager_.AddVariable("1/Eff", DataTreeVarManager::kMdcEfficiency, 1);
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
      config->SetNormalization(QVector::Normalization::NONE);
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
    correction_manager_.AddDetector("TracksMdc", DetectorType::TRACK, "Phi", "1/Eff",
                         {ycm, pt}, {1, 2});
    correction_manager_.AddCut(
        "TracksMdc", {"Ycm", "Pt"},
        [](const double &y, const double &pt) {
          return
              -0.8 < y && y < 0.8 &&
              0.0 < pt && pt < 2.0;
        });
    correction_manager_.SetCorrectionSteps("TracksMdc", MdcConfiguration);

    // Random sub-event method
    correction_manager_.AddDetector("Rs1", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
    correction_manager_.AddCut("Rs1", {"RandomSe"},
                    [](const double &rs) { return rs == 1.00; });
    correction_manager_.SetCorrectionSteps("Rs1", FwConfiguration);

    correction_manager_.AddDetector("Rs2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
    correction_manager_.AddCut("Rs2", {"RandomSe"},
                    [](const double &rs) { return rs == 2.00; });
    correction_manager_.SetCorrectionSteps("Rs2", FwConfiguration);

    correction_manager_.AddDetector("Full", DetectorType::CHANNEL, "FwPhi", "FwAdc", {},
                         {1});
    correction_manager_.AddCut("Full", {"FwAdc"},
                    [](const double &adc) { return adc > 0.0; });
    correction_manager_.SetCorrectionSteps("Full", FwConfiguration);

    correction_manager_.AddHisto2D("TracksMdc",
                        {{"Pt", 200, 0., 2.}, {"Ycm", 160, -0.8, 0.8}});
    correction_manager_.AddHisto2D("TracksMdc",
                        {{"Phi", 126, -3.15, 3.15}, {"Ycm", 160, -0.8, 0.8}});

    correction_manager_.AddHisto2D(
        "Rs1", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
    correction_manager_.AddHisto2D(
        "Rs2", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

    correction_manager_.AddHisto2D(
        "Rs1", {{"moduleX", 50, -1000., 1000.}, {"moduleY", 50, -1000., 1000.}});
    correction_manager_.AddHisto2D(
        "Rs2", {{"moduleX", 50, -1000., 1000.}, {"moduleY", 50, -1000., 1000.}});
    correction_manager_.AddHisto2D(
        "Full", {{"moduleX", 50, -1000., 1000.}, {"moduleY", 50, -1000., 1000.}});

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
    DataTreeVarManager::GetInstance()->Rewind();
    OccupancyCorrections::GetInstance()->Rewind();
    while( !DataTreeVarManager::GetInstance()->Eof() ){
      DataTreeVarManager::GetInstance()->SwitchNextGoodEvent();
      OccupancyCorrections::GetInstance()->SwitchNextEvent();
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
#endif // FLOW_SRC_PROCESSING_CORRECTION_TASK_RND_H_
