#ifndef DATA_TREE_VAR_MANAGER_H
#define DATA_TREE_VAR_MANAGER_H

#include <iostream>
#include <memory>

#include "DataTreeEvent.h"
#include "centrality.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "selector.h"
#include "centrality.h"
#include <TChain.h>
#include <TH2F.h>

#include "HADES_constants.h"
#include "occupancy_corrections.h"

class DataTreeVarManager {
public:
  enum Signals { kAdc = 0, kChargeZ, kNumOfSignals };
  enum Vars {
    kCentrality = 0, // 0
    kOne,
    kMdcPt,
    kMdcYcm,
    kMdcPhi,
    kMdcPid,
    kMdcEfficiency,
    kFwModuleRing,                     // 1
    kFwModuleId = kFwModuleRing + 304, // 2
    kFwModuleAdc = kFwModuleId + 304,  // 3
    kFwModulePhi = kFwModuleAdc + 304, // 4
    kRandomSe = kFwModulePhi + 304,
    kFwModuleX = kRandomSe + 304,
    kFwModuleY = kFwModuleX + 304,
    kNumberOfVars = kFwModuleY + 304 // 5
  };
  static DataTreeVarManager* GetInstance(){
      if( !instance_ )
        instance_ = new DataTreeVarManager();
      return instance_;
  };
  void OpenFiles( const std::string& file_list ){
    std::stringstream list{file_list};
    std::string file{};
    if( !file_list.empty() ){
      while(std::getline(list,file,',')){
        chain_->Add( file.data() );
        std::cout << file << " has been added to sequence" << std::endl;
      }
    }
    chain_->SetBranchAddress("DTEvent", &event_);
    std::cout << "Data Tree Var Manager Initialized. " << chain_->GetEntries()
              << " events were found." << std::endl;
  }
  DataTreeEvent *Event() { return event_; }
  void FillEventVariables(double *varContainer);
  void FillTrackVariables(int idx, double *varContainer);
  int GetNumberOfEvents() { return chain_->GetEntries(); }
  int GetNumberOfTracks() { return event_->GetNVertexTracks(); }
  void SwitchEvent(int idx) {
    if( idx >= chain_->GetEntries() )
      return;
    chain_->GetEntry(idx);
  }
  void Rewind(){
    position_ =0;
    chain_->GetEntry(position_);
  }
  void SwitchNextEvent(){
    if( Eof() )
      return;
    chain_->GetEntry(position_);
    position_+=1;
  }
  void SwitchNextGoodEvent(){
    auto selector = Selector::GetInstance();
    do{
      if( Eof() )
        return;
      SwitchNextEvent();
    }while( !selector->IsCorrectEvent() );
  }
  void SwitchOnSelector(){
    Selector::GetInstance()->SetEventAddress(event_);
  }
  void SwitchOnCentrality(){
    Centrality::GetInstance()->SetEventAddress(event_);
  }
  bool Eof(){
    return position_ >= chain_->GetEntries();
  }
  void FillEp(double *varContainer);

protected:
  static DataTreeVarManager* instance_;
  std::shared_ptr<TChain> chain_;
  DataTreeEvent *event_;
  float psi_ep_{0.0};
  DataTreeVarManager() : chain_(new TChain("DataTree")), event_{new DataTreeEvent}
                         {};
  virtual ~DataTreeVarManager() = default;
  long long position_ =0;
  const double T = 1.23;  // AGeV
  const double M = 0.938; // GeV
  const double GAMMA = (T + M) / M;
  const double BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
  const double PZ = M * BETA * GAMMA;
  const double E = T + M;
  const double Y_BEAM = 0.5 * log((E + PZ) / (E - PZ));
};

#endif //DATA_TREE_VAR_MANAGER_H