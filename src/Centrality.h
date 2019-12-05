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
  float	GetCentralityClass(){
    auto TOFRPChits = event_->GetCentralityEstimator(HADES_constants::kNhitsTOF_RPC_cut);
    auto bin = centrality_percentile_->FindBin(TOFRPChits);
    return centrality_percentile_->GetBinContent(bin);
  };
  float GetCentrality(){
    auto TOFRPChits = event_->GetCentralityEstimator(HADES_constants::kNhitsTOF_RPC_cut);
    auto bin = centrality_percentile_->FindBin(TOFRPChits);
    return 5.0*centrality_percentile_->GetBinContent(bin)-2.5;
  }
  int	GetNumClasses(){
    return centrality_percentile_->GetNbinsX()-1;
  };
  void	LoadCentralityPercentile(){
    Double_t xAxis1[12] =  {0, 39, 60, 74, 88, 104, 121, 140, 160, 182, 250, 280};
    centrality_percentile_ = new TH1F("hTOFRPCGlau_5percent_fixedCuts__1","hTOFRPCGlau_5percent_fixedCuts",11, xAxis1);
    centrality_percentile_->SetBinContent(1,10);
    centrality_percentile_->SetBinContent(2,9);
    centrality_percentile_->SetBinContent(3,8);
    centrality_percentile_->SetBinContent(4,7);
    centrality_percentile_->SetBinContent(5,6);
    centrality_percentile_->SetBinContent(6,5);
    centrality_percentile_->SetBinContent(7,4);
    centrality_percentile_->SetBinContent(8,3);
    centrality_percentile_->SetBinContent(9,2);
    centrality_percentile_->SetBinContent(10,1);
    centrality_percentile_->SetBinError(2,5.043633);
    centrality_percentile_->SetBinError(3,4.9067);
    centrality_percentile_->SetBinError(4,5.181467);
    centrality_percentile_->SetBinError(5,4.911733);
    centrality_percentile_->SetBinError(6,4.9759);
    centrality_percentile_->SetBinError(7,4.896367);
    centrality_percentile_->SetBinError(8,4.923133);
    centrality_percentile_->SetBinError(9,4.918733);
    centrality_percentile_->SetBinError(10,5.023);
    centrality_percentile_->SetEntries(11);
  };

private:
  Centrality() = default;
  TFile* file_;
  TH1F* centrality_percentile_;
  DataTreeEvent* event_;
};