#include "selector.h"

bool Selector::IsCorrectEvent() {
  if (!event_) {
    std::cout << "Event pointer is empty" << std::endl;
    return false;
  }
  //   if(!fEvent->GetTrigger(HADES_constants::kPT2)->GetIsFired())
  //          return false;
  if (!event_->GetTrigger(fTrigger)->GetIsFired())
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
  //    if ( !fEvent->GetTrigger(HADES_constants::kGoodTRIGGER)->GetIsFired() )
  //      return false;
  if (!event_->GetTrigger(HADES_constants::kGoodVertexClust)->GetIsFired())
    return false;
  if (!event_->GetTrigger(HADES_constants::kGoodVertexCand)->GetIsFired())
    return false;
  if (!event_->GetTrigger(HADES_constants::kGoodSTART)->GetIsFired())
    return false;
  if (!event_->GetTrigger(HADES_constants::kNoPileUpSTART)->GetIsFired())
    return false;
  if (!event_->GetTrigger(HADES_constants::kGoodSTARTVETO)->GetIsFired())
    return false;
  if (!event_->GetTrigger(HADES_constants::kGoodSTARTMETA)->GetIsFired())
    return false;
  if (!event_->GetTrigger(HADES_constants::kNoVETO)->GetIsFired())
    return false;
  return event_->GetPSDEnergy() != 0;
}

bool Selector::IsCorrectTrack(int idx) {
  DataTreeTrack *track = event_->GetVertexTrack(idx);
  DataTreeTOFHit *hit = event_->GetTOFHit(idx);

  if (track->GetPdgId() <= 6) // Hadrons
    return false;

  float dca_xy = fabs(track->GetDCAComponent(0));
  float dca_z = fabs(track->GetDCAComponent(2));
  if (dca_xy > 10.0)
    return false;
  if (dca_z > 10.0)
    return false;
  if( track->GetChi2() > 100.0 )
    return false;
  return hit->GetSquaredMassError() <= 3.0;
}

bool Selector::IsCorrectFwHit(int idx) {
  if (!fChannelSelection) {
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
  } else {
    if (!event_->GetPSDModule(idx)->HasPassedCuts())
      return false;
  }
  float charge = 0;
  if (fFwSignal == "adc" || fFwSignal == "ADC" || fFwSignal == "Adc")
    charge = event_->GetPSDModule(idx)->GetEnergy();
  if (fFwSignal == "z" || fFwSignal == "Z")
    charge = event_->GetPSDModule(idx)->GetChargeZ();
  if (charge <= min_signal_ || charge > max_signal_)
    return false;
  return true;
}
