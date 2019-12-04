#include "Selector.h"

bool Selector::IsCorrectEvent()
{
	if( !fEvent )
	{
		std::cout << "Event pointer is empty" << std::endl;
		return false;
	}
	 if( !fEvent->GetTrigger(HADES_constants::kPT2)->GetIsFired() && !fEvent->GetTrigger(HADES_constants::kPT3)->GetIsFired() )
	 	return false;
    if (  fEvent->GetVertexPositionComponent(2) > 0 || fEvent->GetVertexPositionComponent(2) < -60 )
        return false;
    Float_t Rx = fEvent->GetVertexPositionComponent(0), Ry = fEvent->GetVertexPositionComponent(1);
    if ( sqrt(Rx*Rx+Ry*Ry) > 3 )
        return false;
    if ( fEvent->GetVertexQuality() < 0.5 || fEvent->GetVertexQuality() > 40 )
        return false;
    if ( !fEvent->GetTrigger(HADES_constants::kGoodVertexClust)->GetIsFired() ) 
        return false;
    if ( ! fEvent->GetTrigger(HADES_constants::kGoodVertexCand)->GetIsFired() )
        return false;
    if( !fEvent->GetTrigger(HADES_constants::kGoodSTART)->GetIsFired() )
        return false;
    if( !fEvent->GetTrigger(HADES_constants::kNoPileUpSTART)->GetIsFired() )
        return false;  
    if( !fEvent->GetTrigger(HADES_constants::kGoodSTARTVETO)->GetIsFired() )
        return false;
    if( !fEvent->GetTrigger(HADES_constants::kGoodSTARTMETA)->GetIsFired() )
        return false;
    if( !fEvent->GetTrigger(HADES_constants::kNoVETO)->GetIsFired() )
        return false;
	if( fEvent->GetPSDEnergy() == 0 )
		return false;
    return true;
}

bool Selector::IsCorrectTrack(int idx)
{
    DataTreeTrack* fTrack = fEvent->GetVertexTrack(idx);
    DataTreeTOFHit* fHit = fEvent->GetTOFHit(idx);
    Float_t fTof = fHit->GetTime();
    Float_t fLen = fHit->GetPathLength();
	Float_t fDCAx = fTrack->GetDCAComponent(0);
	Float_t fDCAy = fTrack->GetDCAComponent(1);
	Float_t fDCA = sqrt( fDCAx*fDCAx + fDCAy*fDCAy );
    if ( fDCA > 15 )
        return false;
    if ( fHit->GetPositionComponent(0) < -5 || fHit->GetPositionComponent(0) > 5 )
        return false;
    if ( fHit->GetPositionComponent(1) < -5 || fHit->GetPositionComponent(1) > 5 )
        return false;
    if ( fTrack->GetChi2() > 100 )
        return false;
    if( fTrack->GetPdgId() != fPid )
        return false;
    return true;
}

bool Selector::IsCorrectFwHit(int idx)
{
    if( !fChannelSelection )
    {
        if( fEvent->GetPSDModule(idx)->GetEnergy() < 80.0 )
            return false;
        short ring = fEvent->GetPSDModule(idx)->GetRing();
        
        if( ring < 1 )
            return false;
        if( ring > 10 )
            return false;
        if( ring <= false )
        {
            if( fEvent->GetPSDModule(idx)->GetBeta() < 0.84 )
                return false;
            if( fEvent->GetPSDModule(idx)->GetBeta() > 1.2 )
                return false;
        }
        if( ring == 6 || ring == 7 )
        {
            if( fEvent->GetPSDModule(idx)->GetBeta() < 0.85 )
                return false;
            if( fEvent->GetPSDModule(idx)->GetBeta() > 1.2 )
                return false;
        }
        if( ring >= 8 )
        {
            if( fEvent->GetPSDModule(idx)->GetBeta() < 0.8 )
                return false;
            if( fEvent->GetPSDModule(idx)->GetBeta() > 1.2 )
                return false;
        }
    }
    else
    {
        if( !fEvent->GetPSDModule(idx)->HasPassedCuts() )
            return false;
    }
    float charge = 0;
    if( fFwSignal=="adc" || fFwSignal == "ADC" || fFwSignal == "Adc" )
        charge = fEvent->GetPSDModule(idx)->GetEnergy();
    if( fFwSignal=="z" || fFwSignal=="Z" )
        charge = fEvent->GetPSDModule(idx)->GetChargeZ();
    if( charge<=fMinSignal || charge>fMaxSignal )
        return false;
    return true; 
}
