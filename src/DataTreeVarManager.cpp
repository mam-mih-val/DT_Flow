#include "DataTreeVarManager.h"

DataTreeVarManager::DataTreeVarManager(std::string fileName) :
	fChain(new TChain("DataTree")),
	fEvent{new DataTreeEvent},
	fSelector(new Selector),
        fCentrality(new Centrality(fEvent))
	{
		fChain->Add(fileName.c_str());
		fChain->SetBranchAddress("DTEvent", &fEvent);
		fSelector->SetEventAddress(fEvent);
		std::cout << "Data Tree Var Manager Initialized. " << fChain->GetEntries() << " events were found." << std::endl;
	}

void DataTreeVarManager::FillEventVariables(double* varContainer)
{
	varContainer[kCentrality]=fEvent->GetCentrality(HADES_constants::kNhitsTOF_RPC_cut);
//	varContainer[kCentrality]=fEvent->GetCentrality();
	for(int idx=0; idx<304; idx++)
	{
		varContainer[kFwModuleId+idx]=(double)	idx;
		varContainer[kFwModuleRing+idx]=		-1.0;
		varContainer[kFwModuleAdc+idx]=			0.0;
		varContainer[kFwModulePhi+idx]=			0.0;
		varContainer[kRandomSe+idx]=			0.0;
		varContainer[kFwModuleX+idx]=			0.0;
		varContainer[kFwModuleY+idx]=			0.0;
	}
	Int_t nModules = fEvent->GetNPSDModules();
	std::vector<int> moduleList;
	for(Int_t idx=0; idx<nModules; idx++) 
	{
		if( !fSelector->IsCorrectFwHit(idx) )
			continue;
		int moduleId = fEvent->GetPSDModule(idx)->GetId();
		varContainer[kFwModuleRing+moduleId]=	(double) fEvent->GetPSDModule(idx)->GetRing();
		varContainer[kFwModuleAdc + moduleId] = (double) fEvent->GetPSDModule(idx)->GetEnergy();
		varContainer[kFwModulePhi+moduleId]=	(double) fEvent->GetPSDModule(idx)->GetPhi();
		varContainer[kFwModuleX+idx]=			(double) fEvent->GetPSDModule(idx)->GetPositionComponent(0);
		varContainer[kFwModuleY+idx]=			(double) fEvent->GetPSDModule(idx)->GetPositionComponent(1);
		moduleList.push_back(moduleId);
	}
	random_shuffle( moduleList.begin(), moduleList.end() );
	for( int idx = 0; idx<moduleList.size(); idx++ )
		varContainer[kRandomSe + moduleList.at(idx)] = idx % 2 == 0 ? 1.0 : 2.0;
}

void DataTreeVarManager::FillTrackVariables(int idx, double* varContainer)
{
	double BETA = sqrt( 1.0 - 0.938*0.938/1.23/1.23 );
	TVector3 b{0,0,-BETA}; 
	auto p = fEvent->GetVertexTrack(idx)->GetMomentum();
	p.Boost(b);
	varContainer[kOne]=1.0;
	varContainer[kMdcPt]=p.Pt();
	varContainer[kMdcYcm]=p.Rapidity();
	varContainer[kMdcPhi]=p.Phi();
	varContainer[kMdcPid]=fEvent->GetVertexTrack(idx)->GetPdgId();
}
