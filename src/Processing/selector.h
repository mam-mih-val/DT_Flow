#ifndef SELECTOR_H
#define SELECTOR_H

#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "DataTreeEvent.h"

#include "HADES_constants.h"

class Selector {
public:
  static Selector* GetInstance(){
    if( !instance_ )
      instance_ = new Selector();
    return instance_;
  }
  Selector( const Selector& ) = delete;
  void operator=(const Selector& ) = delete;
  bool IsCorrectEvent(){
    if (!event_) {
      std::cout << "Selector Error: Event pointer is empty" << std::endl;
      std::abort();
    }
//    auto trigger = event_->GetTrigger(HADES_constants::kNoVETO);
    if (!event_->GetTrigger(trigger_)->GetIsFired())
      return false;

    if (event_->GetVertexPositionComponent(2) > 0 ||
        event_->GetVertexPositionComponent(2) < -60)
      return false;
    Float_t Rx = event_->GetVertexPositionComponent(0),
        Ry = event_->GetVertexPositionComponent(1);
    if (sqrt(Rx * Rx + Ry * Ry) > 3)
      return false;
    if (event_->GetVertexQuality() < 0.5 || event_->GetVertexQuality() > 40)
      return false;

    for( const auto& trigger : triggers_){
      if( !event_->GetTrigger(trigger)->GetIsFired() )
        return false;
    }
    return event_->GetPSDEnergy() != 0;
  }
  bool IsCorrectTrack(int idx){
    DataTreeTrack *track = event_->GetVertexTrack(idx);
    DataTreeTOFHit *hit = event_->GetTOFHit(idx);

    double dca_xy = fabs(track->GetDCAComponent(0));
    double dca_z = fabs(track->GetDCAComponent(2));
    if (dca_xy > 10.0)
      return false;
    if (dca_z > 10.0)
      return false;
    if( track->GetChi2() > 100.0 )
      return false;
    return hit->GetSquaredMassError() <= 3.0;
  }
  bool IsCorrectFwHit(int idx){
    if (event_->GetPSDModule(idx)->GetEnergy() < 80.0)
      return false;
    short ring = event_->GetPSDModule(idx)->GetRing();

    if (ring < 1)
      return false;
    if (ring > 10)
      return false;
    if (ring <= 5) {
      if (event_->GetPSDModule(idx)->GetBeta() < 0.84)
        return false;
      if (event_->GetPSDModule(idx)->GetBeta() > 1.0)
        return false;
      if (event_->GetPSDModule(idx)->GetEnergy() < 80.0)
        return false;
    }
    if (ring == 6 || ring == 7) {
      if (event_->GetPSDModule(idx)->GetBeta() < 0.85)
        return false;
      if (event_->GetPSDModule(idx)->GetBeta() > 1.0)
        return false;
      if (event_->GetPSDModule(idx)->GetEnergy() < 85.0)
        return false;
    }
    if (ring >= 8) {
      if (event_->GetPSDModule(idx)->GetBeta() < 0.80)
        return false;
      if (event_->GetPSDModule(idx)->GetBeta() > 1.0)
        return false;
      if (event_->GetPSDModule(idx)->GetEnergy() < 88.0)
        return false;
    }
    return true;
  }
  bool IsCorrectPid(int idx){
    DataTreeTrack *track = event_->GetVertexTrack(idx);
    return track->GetPdgId() == pid_;
  }
  void SetEventAddress(DataTreeEvent *_fEvent) { event_ = _fEvent; }
  void SetFwSignalRange(float minValue = 80.0, float maxValue = 80.0) {
    min_signal_ = minValue;
    max_signal_ = maxValue;
  }
  void SetTrigger(const std::string& trigger) {
    if(trigger=="PT3"){
      trigger_ =HADES_constants::kPT3;
      return;
    }
    if(trigger=="PT2"){
      trigger_ =HADES_constants::kPT2;
      return;
    }
  }
  void SetPid(int pid) { pid_ = pid; }

private:
  static Selector* instance_;
  Selector() = default;
  ~Selector() = default;
  std::vector<int> triggers_{
      HADES_constants::kGoodVertexClust,
      HADES_constants::kGoodVertexCand,
      HADES_constants::kGoodSTART,
      HADES_constants::kNoPileUpSTART,
      HADES_constants::kGoodSTARTVETO,
      HADES_constants::kGoodSTARTMETA,
      HADES_constants::kNoVETO
  };
  DataTreeEvent *event_{nullptr};
  int trigger_ = HADES_constants::kPT3;
  float min_signal_ = 80.0;
  float max_signal_ = 999.0;
  int pid_ = 14.0;
};

#endif // SELECTOR_H