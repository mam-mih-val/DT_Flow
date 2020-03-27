#include "DataTreeVarManager.h"

DataTreeVarManager::DataTreeVarManager(std::string fileName, std::string file_qn, std::string file_efficiency)
    : fChain(new TChain("DataTree")), fEvent{new DataTreeEvent},
      fSelector(new Selector), fCentrality(new Centrality(fEvent)),
      corrections_( file_qn, file_efficiency ){
  std::stringstream list{fileName};
  std::string file{};
  if( !fileName.empty() ){
    while(std::getline(list,file,',')){
      fChain->Add( file.data() );
      std::cout << file << " has been added to sequence" << std::endl;
    }
  }
  fChain->SetBranchAddress("DTEvent", &fEvent);
  fSelector->SetEventAddress(fEvent);
  std::cout << "Data Tree Var Manager Initialized. " << fChain->GetEntries()
            << " events were found." << std::endl;
}

void DataTreeVarManager::FillEventVariables(double *varContainer) {
//  varContainer[kCentrality]=fEvent->GetCentrality(HADES_constants::kNhitsTOF_RPC_cut);
  varContainer[kCentrality] = fCentrality->GetCentrality();
  float file_cent = fEvent->GetCentrality(HADES_constants::kNhitsTOF_RPC_cut);
  float my_cent = fCentrality->GetCentrality();
  float cent = 2.5+5.0*fCentrality->GetCentralityClass();
  for (int idx = 0; idx < 304; idx++) {
    varContainer[kFwModuleId + idx] = (double)idx;
    varContainer[kFwModuleRing + idx] = -1.0;
    varContainer[kFwModuleAdc + idx] = 0.0;
    varContainer[kFwModulePhi + idx] = 0.0;
    varContainer[kRandomSe + idx] = 0.0;
    varContainer[kFwModuleX + idx] = 0.0;
    varContainer[kFwModuleY + idx] = 0.0;
  }
  Int_t nModules = fEvent->GetNPSDModules();
  std::vector<int> moduleList;
  for (Int_t idx = 0; idx < nModules; idx++) {
    if (!fSelector->IsCorrectFwHit(idx))
      continue;
    int moduleId = fEvent->GetPSDModule(idx)->GetId();
    varContainer[kFwModuleRing + moduleId] =
        (double)fEvent->GetPSDModule(idx)->GetRing();
    varContainer[kFwModuleAdc + moduleId] =
        (double)fEvent->GetPSDModule(idx)->GetEnergy();
    varContainer[kFwModulePhi + moduleId] =
        (double)fEvent->GetPSDModule(idx)->GetPhi();
    varContainer[kFwModuleX + idx] =
        (double)fEvent->GetPSDModule(idx)->GetPositionComponent(0);
    varContainer[kFwModuleY + idx] =
        (double)fEvent->GetPSDModule(idx)->GetPositionComponent(1);
    moduleList.push_back(moduleId);
  }
  random_shuffle(moduleList.begin(), moduleList.end());
  for (int idx = 0; idx < moduleList.size(); idx++)
    varContainer[kRandomSe + moduleList.at(idx)] = idx % 2 == 0 ? 1.0 : 2.0;
}

void DataTreeVarManager::FillTrackVariables(int idx, double *varContainer) {
  auto p = fEvent->GetVertexTrack(idx)->GetMomentum();
  varContainer[kOne] = 1.0;
  varContainer[kMdcPt] = p.Pt();
  varContainer[kMdcYcm] = p.Rapidity() - Y_BEAM / 2;
  varContainer[kMdcPhi] = p.Phi();
  varContainer[kMdcPid] = fEvent->GetVertexTrack(idx)->GetPdgId();
  float efficiency{corrections_.GetEfficiency(fCentrality->GetCentralityClass(), p.Phi(), p.Theta())};
  if( efficiency == 0 )
    varContainer[kMdcEfficiency] = 0;
  else
    varContainer[kMdcEfficiency] = 1./efficiency;
}
