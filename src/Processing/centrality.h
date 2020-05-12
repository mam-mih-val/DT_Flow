
#ifndef CENTRALITY_H
#define CENTRALITY_H

#include "TH1F.h"
#include "TFile.h"

#include "HADES_constants.h"

#include <iostream>
#include <string>
#include "DataTreeEvent.h"

class Centrality
{
public:
  static Centrality* GetInstance(){
    if(!instance_)
      instance_ = new Centrality();
    return instance_;
  }
  void SetEventAddress(DataTreeEvent *event){
    event_ = event;
  }
  int GetCentralityClass5pc(){
    if (!event_) {
      std::cout << "Centrality Error: Event pointer is empty" << std::endl;
      std::abort();
    }
    auto TOFRPChits = event_->GetCentralityEstimator(HADES_constants::kNhitsTOF_RPC_cut);
    auto bin = centrality_5pc_->FindBin(TOFRPChits);
    return static_cast<int>(centrality_5pc_->GetBinContent(bin) - 1);
  };
  int GetCentralityClass10pc(){
    if (!event_) {
      std::cout << "Centrality Error: Event pointer is empty" << std::endl;
      std::abort();
    }
    auto TOFRPChits = event_->GetCentralityEstimator(HADES_constants::kNhitsTOF_RPC_cut);
    auto bin = centrality_10pc_->FindBin(TOFRPChits);
    return static_cast<int>(centrality_10pc_->GetBinContent(bin) - 1);
  };
  float GetCentrality5pc(){
    return 5.0*GetCentralityClass5pc()+2.5;
  }
  float GetCentrality10pc(){
    return 10.0*GetCentralityClass10pc()+5.0;
  }
  int GetNumClasses5pc(){
    return centrality_5pc_->GetNbinsX()-1;
  };
  void LoadCentrality5pc(){
    Double_t xAxis[12] = {0, 39, 60, 74, 88, 104, 121, 140, 160, 182, 250, 280};
    centrality_5pc_ = new TH1F("hTOFRPCGlau_5percent_fixedCuts__1","hTOFRPCGlau_5percent_fixedCuts",11, xAxis);
    centrality_5pc_->SetBinContent(1,10);
    centrality_5pc_->SetBinContent(2,9);
    centrality_5pc_->SetBinContent(3,8);
    centrality_5pc_->SetBinContent(4,7);
    centrality_5pc_->SetBinContent(5,6);
    centrality_5pc_->SetBinContent(6,5);
    centrality_5pc_->SetBinContent(7,4);
    centrality_5pc_->SetBinContent(8,3);
    centrality_5pc_->SetBinContent(9,2);
    centrality_5pc_->SetBinContent(10,1);

    centrality_5pc_->SetBinError(2,5.043633);
    centrality_5pc_->SetBinError(3,4.9067);
    centrality_5pc_->SetBinError(4,5.181467);
    centrality_5pc_->SetBinError(5,4.911733);
    centrality_5pc_->SetBinError(6,4.9759);
    centrality_5pc_->SetBinError(7,4.896367);
    centrality_5pc_->SetBinError(8,4.923133);
    centrality_5pc_->SetBinError(9,4.918733);
    centrality_5pc_->SetBinError(10,5.023);
    centrality_5pc_->SetEntries(11);
  };
  void LoadCentrality10pc(){
    Double_t xAxis1[10] = {0, 10, 23, 36, 55, 82, 117, 157, 312, 499};

    centrality_10pc_ = new TH1F("TOFRPC_10pc_fixedCuts__1","TOFRPC_10pc_fixedCuts",9, xAxis1);
    centrality_10pc_->SetBinContent(1,8);
    centrality_10pc_->SetBinContent(2,7);
    centrality_10pc_->SetBinContent(3,6);
    centrality_10pc_->SetBinContent(4,5);
    centrality_10pc_->SetBinContent(5,4);
    centrality_10pc_->SetBinContent(6,3);
    centrality_10pc_->SetBinContent(7,2);
    centrality_10pc_->SetBinContent(8,1);
    centrality_10pc_->SetBinError(1,1.8301);
    centrality_10pc_->SetBinError(2,10.3718);
    centrality_10pc_->SetBinError(3,9.5123);
    centrality_10pc_->SetBinError(4,10.1469);
    centrality_10pc_->SetBinError(5,10.0362);
    centrality_10pc_->SetBinError(6,10.2188);
    centrality_10pc_->SetBinError(7,9.77849);
    centrality_10pc_->SetBinError(8,10.1044);
    centrality_10pc_->SetBinError(9,0.00100254);
    centrality_10pc_->SetEntries(9);
  };
private:
  Centrality(){
    LoadCentrality5pc();
    LoadCentrality10pc();
  };
  ~Centrality() = default;
  static Centrality* instance_;
  TH1F* centrality_5pc_;
  TH1F* centrality_10pc_;
  DataTreeEvent* event_;
};

#endif // CENTRALITY_H