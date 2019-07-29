#include "DataTreeVarManager.h"

DataTreeVarManager::DataTreeVarManager(std::string fileName) :
	fChain(new TChain("DataTree")),
	fEvent{new DataTreeEvent},
	fSelector(new Selector)
	{
		fChain->Add(fileName.c_str());
		fChain->SetBranchAddress("DTEvent", &fEvent);
		fSelector->SetEventAddress(fEvent);
		std::cout << "Data Tree Var Manager Initialized. " << fChain->GetEntries() << " events were found." << std::endl;
	}

void DataTreeVarManager::FillEventVariables(double* varContainer)
{
	varContainer[kCentrality]=fEvent->GetCentrality(HADES_constants::kNhitsTOF_RPC_cut);
	for(int idx=0; idx<304; idx++)
	{
		varContainer[kFwModuleRing+idx]=0;
		varContainer[kFwModuleId+idx]=idx;
		varContainer[kFwModuleAdc+idx]=0;
		varContainer[kFwModulePhi+idx]=0;
	}
	Int_t nModules = fEvent->GetNPSDModules();
	for(Int_t idx=0; idx<nModules; idx++) 
	{
		if( !fSelector->IsCorrectFwHit(idx, 0, "adc", 80.0) )
			continue;
		int moduleId = fEvent->GetPSDModule(idx)->GetId();
		varContainer[kFwModuleRing+moduleId]=	(double) fEvent->GetPSDModule(idx)->GetRing();
		varContainer[kFwModuleAdc+moduleId]=	(double) fEvent->GetPSDModule(idx)->GetEnergy();
		varContainer[kFwModulePhi+moduleId]=	(double) fEvent->GetPSDModule(idx)->GetPhi();
	}
}
