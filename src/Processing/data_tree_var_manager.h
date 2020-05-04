#pragma once

#include <iostream>
#include <memory>

#include "DataTreeEvent.h"
#include "Processing/centrality.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "selector.h"
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
    chain_->SetBranchAddress("DTEvent", &fEvent);
    std::cout << "Data Tree Var Manager Initialized. " << chain_->GetEntries()
              << " events were found." << std::endl;
  }
  DataTreeEvent *Event() { return fEvent; }
  void FillEventVariables(double *varContainer);
  void FillTrackVariables(int idx, double *varContainer);
  int GetNumberOfEvents() { return chain_->GetEntries(); }
  int GetNumberOfTracks() { return fEvent->GetNVertexTracks(); }
  void SwitchEvent(int idx) { chain_->GetEntry(idx); }
  void SwitchNextEvent(){
    try {
      chain_->GetEntry(position);
      position++;
    } catch (const std::exception& e) {
      std::cout << e.what() << std::endl;
      abort();
    }
  }
  bool Eof(){
    position < chain_->GetEntries();
  }
  void SwitchNextGoodEvent(){
    auto selector = Selector::GetInstance();
    while( !selector->IsCorrectEvent() )
      if( !Eof() )
        SwitchNextEvent();
      else
        return;
  }
  void FillEp(double *varContainer);

protected:
  static DataTreeVarManager* instance_;
  std::shared_ptr<TChain> chain_;
  DataTreeEvent *fEvent;
//  OccupancyCorrections corrections_;
  float psi_ep_{0.0};
  DataTreeVarManager() : chain_(new TChain("DataTree")),
                         fEvent{new DataTreeEvent}
                         {};
  virtual ~DataTreeVarManager() = default;
  long long position=0;
  const double T = 1.23;  // AGeV
  const double M = 0.938; // GeV
  const double GAMMA = (T + M) / M;
  const double BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
  const double PZ = M * BETA * GAMMA;
  const double E = T + M;
  const double Y_BEAM = 0.5 * log((E + PZ) / (E - PZ));
};