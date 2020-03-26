#pragma once

#include "TH1F.h"
#include "TFile.h"

#include "HADES_constants.h"

#define  DATATREE_SHINE
#include "DataTreeEvent.h"
#include <string>
#include <iostream>

class Centrality
{
public:
  Centrality(DataTreeEvent *event)
      : event_(event) {
    LoadCentralityPercentile();
  }
  ~Centrality() = default;;
  int GetCentralityClass(){
    auto TOFRPChits = event_->GetCentralityEstimator(HADES_constants::kNhitsTOF_RPC_cut);
    auto bin = centrality_percentile_->FindBin(TOFRPChits);
    return static_cast<int>(centrality_percentile_->GetBinContent(bin) - 1);
  };
  float GetCentrality(){
    auto TOFRPChits = event_->GetCentralityEstimator(HADES_constants::kNhitsTOF_RPC_cut);
    auto bin = centrality_percentile_->FindBin(TOFRPChits);
    return 5.0*GetCentralityClass()+2.5;
  }
  int	GetNumClasses(){
    return centrality_percentile_->GetNbinsX()-1;
  };
  void	LoadCentralityPercentile(){
    Double_t xAxis[17] = {0, 10, 17, 23, 29, 36, 45, 55, 68, 82, 99, 117, 136, 157, 180, 312, 499};
    centrality_percentile_ = new TH1F("TOFRPC_5pc_fixedCuts__1","TOFRPC_5pc_fixedCuts",16, xAxis);
    centrality_percentile_->SetBinContent(1,15);
    centrality_percentile_->SetBinContent(2,14);
    centrality_percentile_->SetBinContent(3,13);
    centrality_percentile_->SetBinContent(4,12);
    centrality_percentile_->SetBinContent(5,11);
    centrality_percentile_->SetBinContent(6,10);
    centrality_percentile_->SetBinContent(7,9);
    centrality_percentile_->SetBinContent(8,8);
    centrality_percentile_->SetBinContent(9,7);
    centrality_percentile_->SetBinContent(10,6);
    centrality_percentile_->SetBinContent(11,5);
    centrality_percentile_->SetBinContent(12,4);
    centrality_percentile_->SetBinContent(13,3);
    centrality_percentile_->SetBinContent(14,2);
    centrality_percentile_->SetBinContent(15,1);
    centrality_percentile_->SetBinError(1,1.8301);
    centrality_percentile_->SetBinError(2,5.29551);
    centrality_percentile_->SetBinError(3,5.07632);
    centrality_percentile_->SetBinError(4,4.68075);
    centrality_percentile_->SetBinError(5,4.83154);
    centrality_percentile_->SetBinError(6,5.30302);
    centrality_percentile_->SetBinError(7,4.84388);
    centrality_percentile_->SetBinError(8,5.20623);
    centrality_percentile_->SetBinError(9,4.82995);
    centrality_percentile_->SetBinError(10,5.22526);
    centrality_percentile_->SetBinError(11,4.99358);
    centrality_percentile_->SetBinError(12,4.83847);
    centrality_percentile_->SetBinError(13,4.94002);
    centrality_percentile_->SetBinError(14,5.02256);
    centrality_percentile_->SetBinError(15,5.08179);
    centrality_percentile_->SetBinError(16,0.00100254);
    centrality_percentile_->SetEntries(16);
  };

private:
  Centrality() = default;
  TFile* file_;
  TH1F* centrality_percentile_;
  DataTreeEvent* event_;
};