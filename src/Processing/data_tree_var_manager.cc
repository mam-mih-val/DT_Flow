#include "data_tree_var_manager.h"

#include <random>

DataTreeVarManager* DataTreeVarManager::instance_=nullptr;

void DataTreeVarManager::FillEventVariables(double *varContainer) {
//  varContainer[kCentrality]=fEvent->GetCentrality5pc(HADES_constants::kNhitsTOF_RPC_cut);
  varContainer[kCentrality] = Centrality::GetInstance()->GetCentrality5pc();
  for (int idx = 0; idx < 304; idx++) {
    varContainer[kFwModuleId + idx] = (double)idx;
    varContainer[kFwModuleRing + idx] = -1.0;
    varContainer[kFwModuleAdc + idx] = 0.0;
    varContainer[kFwModulePhi + idx] = 0.0;
    varContainer[kRandomSe + idx] = 0.0;
    varContainer[kFwModuleX + idx] = 0.0;
    varContainer[kFwModuleY + idx] = 0.0;
  }
  Int_t nModules = event_->GetNPSDModules();
  std::vector<int> moduleList;
  for (Int_t idx = 0; idx < nModules; idx++) {
    if (!Selector::GetInstance()->IsCorrectFwHit(idx))
      continue;
    int moduleId = event_->GetPSDModule(idx)->GetId();
    varContainer[kFwModuleRing + moduleId] =
        (double)event_->GetPSDModule(idx)->GetRing();
    varContainer[kFwModuleAdc + moduleId] =
        (double)event_->GetPSDModule(idx)->GetEnergy();
    varContainer[kFwModulePhi + moduleId] =
        (double)event_->GetPSDModule(idx)->GetPhi();
    varContainer[kFwModuleX + idx] =
        (double)event_->GetPSDModule(idx)->GetPositionComponent(0);
    varContainer[kFwModuleY + idx] =
        (double)event_->GetPSDModule(idx)->GetPositionComponent(1);
    moduleList.push_back(moduleId);
  }
  shuffle(moduleList.begin(), moduleList.end(), std::mt19937(std::random_device()()));
  for (int idx = 0; idx < moduleList.size(); idx++)
    varContainer[kRandomSe + moduleList.at(idx)] = idx % 2 == 0 ? 1.0 : 2.0;
}

void DataTreeVarManager::FillEp(double *varContainer){
  double Qx=0;
  double Qy=0;
  for( int i=0; i<304; i++ ){
    Qx+=varContainer[kFwModuleAdc+i]*cos(varContainer[kFwModulePhi+i]);
    Qy+=varContainer[kFwModuleAdc+i]*sin(varContainer[kFwModulePhi+i]);
  }
  psi_ep_=atan2f( Qy, Qx );
  try{
  std::cout << psi_ep_ - OccupancyCorrections::GetInstance()->GetPsiEp() << std::endl << std::endl;
  } catch(const std::exception &e){
    return;
  }
}

void DataTreeVarManager::FillTrackVariables(int idx, double *varContainer) {
  auto p = event_->GetVertexTrack(idx)->GetMomentum();
  varContainer[kOne] = 1.0;
  varContainer[kMdcPt] = p.Pt();
  varContainer[kMdcYcm] = p.Rapidity() - Y_BEAM / 2;
  varContainer[kMdcPhi] = p.Phi();
  varContainer[kMdcPid] = event_->GetVertexTrack(idx)->GetPdgId();
  float efficiency{OccupancyCorrections::GetInstance()->GetEfficiency(
      Centrality::GetInstance()->GetCentralityClass5pc(), p.Phi(), p.Theta())};
  if( efficiency == 0 )
    varContainer[kMdcEfficiency] = 0;
  else
    varContainer[kMdcEfficiency] = 1./efficiency;
}