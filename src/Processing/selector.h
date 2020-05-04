#pragma once
#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"

#define DATATREE_SHINE
#include "DataTreeEvent.h"

#include "Processing/HADES_constants.h"

class Selector {
public:
  static Selector* GetInstance(){
    if( !instance_ )
      instance_ = new Selector();
    return instance_;
  }
  void SetEventPtr( DataTreeEvent *event) { event_ = event; }
  bool IsCorrectEvent();
  bool IsCorrectTrack(int idx);
  bool IsCorrectFwHit(int idx);
  void SetEventAddress(DataTreeEvent *_fEvent) { event_ = _fEvent; }
  void SetFwChannelSelection(bool value = false) { fChannelSelection = value; }
  void SetFwSignalType(std::string value = "adc") { fFwSignal = value; }
  void SetFwSignalRange(float minValue = 80.0, float maxValue = 80.0) {
    min_signal_ = minValue;
    max_signal_ = maxValue;
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
  static Selector* instance_;
  DataTreeEvent *event_{nullptr};
  Selector() = default;
  ~Selector() = default;
  bool fChannelSelection = false;
  std::string fFwSignal = "adc";
  int fTrigger = HADES_constants::kPT3;
  float min_signal_ = 80.0;
  float max_signal_ = 999.0;
  int fPid = 14.0;
};