#pragma once
#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"

#define DATATREE_SHINE
#include "DataTreeEvent.h"

#include "HADES_constants.h"

using std::cout;
using std::endl;
using std::vector;

class Selector {
public:
  Selector() = default;
  ;
  explicit Selector(DataTreeEvent *_fEvent) { fEvent = _fEvent; };
  ~Selector() = default;
  bool IsCorrectEvent();
  bool IsCorrectTrack(int idx);
  bool IsCorrectFwHit(int idx);
  void SetEventAddress(DataTreeEvent *_fEvent) { fEvent = _fEvent; }
  void SetFwChannelSelection(bool value = false) { fChannelSelection = value; }
  void SetFwSignalType(std::string value = "adc") { fFwSignal = value; }
  void SetFwSignalRange(float minValue = 80.0, float maxValue = 80.0) {
    fMinSignal = minValue;
    fMaxSignal = maxValue;
  }
  void SetTrigger(const std::string& trigger) {
    if(trigger=="PT3"){
      fTrigger=HADES_constants::kPT3;
      return;
    }
    if(trigger=="PT2"){
      fTrigger=HADES_constants::kPT2;
      return;
    }
  }

private:
  DataTreeEvent *fEvent{nullptr};
  bool fChannelSelection = false;
  std::string fFwSignal = "adc";
  int fTrigger = HADES_constants::kPT3;
  float fMinSignal = 80.0;
  float fMaxSignal = 999.0;
  int fPid = 14.0;
};