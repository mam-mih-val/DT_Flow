//
// Created by mikhail on 5/5/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRECTION_TASK_FULL_H_
#define FLOW_SRC_PROCESSING_CORRECTION_TASK_FULL_H_
#include "correction_task.h"
namespace Qn {
class CorrectionTaskFull : public CorrectionTask {
public:
  void Initialize() override {
    correction_manager_.AddVariable("Centrality", DataTreeVarManager::kCentrality, 1);
    correction_manager_.AddVariable("One", DataTreeVarManager::kOne, 1);
    correction_manager_.AddVariable("FwRing", DataTreeVarManager::kFwModuleRing, 304);
    correction_manager_.AddVariable("FwModuleId", DataTreeVarManager::kFwModuleId, 304);
    correction_manager_.AddVariable("FwAdc", DataTreeVarManager::kFwModuleAdc, 304);
    correction_manager_.AddVariable("FwPhi", DataTreeVarManager::kFwModulePhi, 304);
    correction_manager_.AddVariable("moduleX", DataTreeVarManager::kFwModuleX, 304);
    correction_manager_.AddVariable("moduleY", DataTreeVarManager::kFwModuleY, 304);
    std::cout << "Variables added" << std::endl;
    // Correction eventvariables

    correction_manager_.SetEventVariable("Centrality");
    correction_manager_.AddCorrectionAxis({"Centrality", 20, 0, 100});

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
    correction_manager_.AddDetector("Full", DetectorType::CHANNEL, "FwPhi", "FwAdc", {},
                                    {1});
    correction_manager_.AddCut("Full", {"FwAdc"},
                               [](const double &adc) { return adc > 0.0; });
    correction_manager_.SetCorrectionSteps("Full", FwConfiguration);

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
    correction_manager_.ProcessQnVectors();
  }
};
} // namespace Qn
#endif // FLOW_SRC_PROCESSING_CORRECTION_TASK_FULL_H_
