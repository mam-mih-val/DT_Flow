#include "Selector.h"


Selector::Selector(DataTreeEvent* _fEvent) 
{
    fEvent = _fEvent;
    hIncorrectEvent = new TH1F("Amount of rejected events on this cut",";code of the cut;counts",cNumOfEventCuts,0,cNumOfEventCuts);
    hIncorrectTracks= new TH1F("Amount of rejected tracks on this cut",";code of the cut;counts",cNumOfTrackCuts,0,cNumOfTrackCuts);
}

Selector::~Selector()
{
    delete hIncorrectEvent;
	delete hIncorrectTracks;
}

Bool_t Selector::IsCorrectEvent(int iPT)
{
    this->CheckEventCuts();

	if( iPT!=-1 )
	{
    	if( !fEvent->GetTrigger(iPT)->GetIsFired() )
    	{
        	return 0;
    	}
	}
    if( iPT == -1 )
	{
		if( !fEvent->GetTrigger(HADES_constants::kPT2)->GetIsFired() && !fEvent->GetTrigger(HADES_constants::kPT3)->GetIsFired() )
		{
			return 0;
		}
	}
    if (  fEvent->GetVertexPositionComponent(2) > 0 || fEvent->GetVertexPositionComponent(2) < -60 )
    {
        return 0;
    }
    Float_t Rx = fEvent->GetVertexPositionComponent(0), Ry = fEvent->GetVertexPositionComponent(1);
    if ( sqrt(Rx*Rx+Ry*Ry) > 3 )
    {
        return 0;
    }
    if ( fEvent->GetVertexQuality() < 0.5 || fEvent->GetVertexQuality() > 40 )
    {
        return 0;
    }
    if ( !fEvent->GetTrigger(HADES_constants::kGoodVertexClust)->GetIsFired() ) 
    {
        return 0;
    }
    if ( ! fEvent->GetTrigger(HADES_constants::kGoodVertexCand)->GetIsFired() )
    {
        return 0;
    }
    if( !fEvent->GetTrigger(HADES_constants::kGoodSTART)->GetIsFired() )
    {
        return 0;
    }
    if( !fEvent->GetTrigger(HADES_constants::kNoPileUpSTART)->GetIsFired() )
    {
        return 0;  
    }
    if( !fEvent->GetTrigger(HADES_constants::kGoodSTARTVETO)->GetIsFired() )
    {
        return 0;
    }
    if( !fEvent->GetTrigger(HADES_constants::kGoodSTARTMETA)->GetIsFired() )
    {
        return 0;
    }
    if( !fEvent->GetTrigger(HADES_constants::kNoVETO)->GetIsFired() )
    {
        return 0;
    }
	if( fEvent->GetPSDEnergy() == 0 )
	{
		return 0;
	}
    return 1;
}

Bool_t Selector::IsCorrectTrack(Int_t idx)
{
    DataTreeTrack* fTrack = fEvent->GetVertexTrack(idx);
    DataTreeTOFHit* fHit = fEvent->GetTOFHit(idx);
    Float_t fTof = fHit->GetTime();
    Float_t fLen = fHit->GetPathLength();
    this->CheckTrackCuts(idx);
    //cout << len/tof/299.792458 << endl;
	Float_t fDCAx = fTrack->GetDCAComponent(0);
	Float_t fDCAy = fTrack->GetDCAComponent(1);
	Float_t fDCA = sqrt( fDCAx*fDCAx + fDCAy*fDCAy );
    if ( fDCA > 15 )
    {
        return 0;
    }
    if ( fHit->GetPositionComponent(0) < -5 || fHit->GetPositionComponent(0) > 5 )
    {
        return 0;
    }
    if ( fHit->GetPositionComponent(1) < -5 || fHit->GetPositionComponent(1) > 5 )
    {
        return 0;
    }    
    if ( fTrack->GetChi2() > 100 )
    {
        return 0;
    }    
    return 1;
}

Bool_t Selector::IsCorrectFwHit(Int_t idx, bool channelSelection, TString signal, float minSignal, float maxSignal)
{
    if( !channelSelection )
    {
        if( fEvent->GetPSDModule(idx)->GetEnergy() < 80.0 )
            return 0;
        short ring = fEvent->GetPSDModule(idx)->GetRing();
        
        if( ring < 1 )
            return 0;
        
        if( ring > 10 )
            return 0;
        
        if( ring <= 5 )
        {
            if( fEvent->GetPSDModule(idx)->GetBeta() < 0.84 )
                return 0;
            if( fEvent->GetPSDModule(idx)->GetBeta() > 1.0 )
                return 0;
        }
        if( ring == 6 || ring == 7 )
        {
            if( fEvent->GetPSDModule(idx)->GetBeta() < 0.85 )
                return 0;
            if( fEvent->GetPSDModule(idx)->GetBeta() > 1.0 )
                return 0;
        }
        if( ring >= 8 )
        {
            if( fEvent->GetPSDModule(idx)->GetBeta() < 0.8 )
                return 0;
            if( fEvent->GetPSDModule(idx)->GetBeta() > 1.0 )
                return 0;
        }
    }
    else
    {
        if( !fEvent->GetPSDModule(idx)->HasPassedCuts() )
            return 0;
    }
    float charge = 0;
    if( signal=="adc" || signal == "ADC" || signal == "Adc" )
        charge = fEvent->GetPSDModule(idx)->GetEnergy();
    if( signal=="z" || signal=="Z" )
        charge = fEvent->GetPSDModule(idx)->GetChargeZ();
    if( charge<=minSignal || charge>maxSignal )
        return 0;
    return 1; 
}

void Selector::CheckEventCuts()
{
    if (  fEvent->GetVertexPositionComponent(2) < 0 || fEvent->GetVertexPositionComponent(2) > -60 )
    {
        hIncorrectEvent->Fill(cVeretexPositionZ);
    }
    Float_t Rx = fEvent->GetVertexPositionComponent(0), Ry = fEvent->GetVertexPositionComponent(1);
    if ( sqrt(Rx*Rx+Ry*Ry) < 3 )
    {
        hIncorrectEvent->Fill(cVeretexPositionXY);
    }
    if ( fEvent->GetTrigger(HADES_constants::kGoodVertexClust)->GetIsFired() ) 
    {
        hIncorrectEvent->Fill(cTriggerVertexClust);
    }
    if ( fEvent->GetTrigger(HADES_constants::kGoodVertexCand)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cTriggerVertexCand);
    }
    if( fEvent->GetTrigger(HADES_constants::kGoodSTART)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cTriggerGoodStart);
    }
    if( fEvent->GetTrigger(HADES_constants::kNoPileUpSTART)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cTriggerNoPileUp);
    }
    if( fEvent->GetTrigger(HADES_constants::kGoodSTARTVETO)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cTriggerGoodStartVeto);
    }
    if( fEvent->GetTrigger(HADES_constants::kGoodSTARTMETA)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cTriggerGoodStartMeta);
    }
    if( fEvent->GetTrigger(HADES_constants::kNoVETO)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cTriggerNoVeto);
    }
    if( fEvent->GetTrigger(HADES_constants::kPT2)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cPT2);
    }
    if( fEvent->GetTrigger(HADES_constants::kPT3)->GetIsFired() )
    {
        hIncorrectEvent->Fill(cPT3);
    }
}

void Selector::CheckTrackCuts(Int_t idx)
{
    DataTreeTrack* fTrack = fEvent->GetVertexTrack(idx);
    DataTreeTOFHit* fHit = fEvent->GetTOFHit(idx);
    Float_t fTof = fHit->GetTime();
    Float_t fLen = fHit->GetPathLength();
    //cout << len/tof/299.792458 << endl;
	Float_t fDCAx = fTrack->GetDCAComponent(0);
	Float_t fDCAy = fTrack->GetDCAComponent(1);
	Float_t fDCA = sqrt( fDCAx*fDCAx + fDCAy*fDCAy );
    if ( fDCA < 15 )
    {
        hIncorrectTracks->Fill(cDCA);
    }
    if ( fHit->GetPositionComponent(0) > -5 || fHit->GetPositionComponent(0) < 5 )
    {
        hIncorrectTracks->Fill(cTrackHitMatchX);
    }
    if ( fHit->GetPositionComponent(1) > -5 || fHit->GetPositionComponent(1) < 5 )
    {
        hIncorrectTracks->Fill(cTrackHitMatchY);
    }    
    if ( fTrack->GetChi2() < 100 )
    {
        hIncorrectTracks->Fill(cChi2);
    }    
}

void Selector::DrawStatistics()
{
    hIncorrectEvent->GetXaxis()->SetBinLabel(cVeretexPositionZ+1,"Vertex on Z");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cVeretexPositionXY+1,"Vertex on X&Y");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerVertexClust+1,"Trigger VertexClust");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerVertexCand+1,"Trigger VertexCand");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerGoodStart+1,"Trigger GoodStart");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerNoPileUp+1,"Trigger NoPileUp");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerGoodStartVeto+1,"Trigger GoodStartVeto");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerGoodStartMeta+1,"Trigger GoodStartMeta");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cTriggerNoVeto+1,"Trigger NoVeto");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cPT2+1,"Trigger PT2");
    hIncorrectEvent->GetXaxis()->SetBinLabel(cPT3+1,"Trigger PT3");
    hIncorrectEvent->GetXaxis()->SetTitle("Passed Events");

    TCanvas* canv = new TCanvas("canv","Events",4000,3000);
    canv->cd();
    gStyle->SetOptStat(0);
    hIncorrectEvent->SetLineWidth(7);
    hIncorrectEvent->Draw();
    canv->SaveAs("../histograms/AmoutOfRejectedEvents.png");

    hIncorrectTracks->GetXaxis()->SetBinLabel(cDCA+1,"DCA");
    hIncorrectTracks->GetXaxis()->SetBinLabel(cTrackHitMatchX+1,"Track-Hit Match on X");
    hIncorrectTracks->GetXaxis()->SetBinLabel(cTrackHitMatchX+1,"Track-Hit Match on X");
    hIncorrectTracks->GetXaxis()->SetBinLabel(cTrackHitMatchY+1,"Track-Hit Match on Y");
    hIncorrectTracks->GetXaxis()->SetBinLabel(cChi2+1,"Chi2");
    hIncorrectTracks->GetXaxis()->SetTitle("Passed Tracks");

    TCanvas* canv1 = new TCanvas("canv1","Tracks",4000,3000);
    // canv1->cd();
    // hIncorrectTracks->SetLineWidth(7);
    // hIncorrectTracks->Draw();
    // canv1->SaveAs("../histograms/AmoutOfRejectedTracks.png");
}

void Selector::SaveStatistics(TString fileName) 
{
    this->DrawStatistics();
    TFile* fFile = new TFile(fileName+"_selector.root","recreate");
    hIncorrectEvent->Write();
    hIncorrectTracks->Write();
    fFile->Close();
}