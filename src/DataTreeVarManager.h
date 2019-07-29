#pragma once

#include <iostream>
#include <memory>

#include <TChain.h>
#include "Selector.h"
#include "DataTreeEvent.h"

#include "HADES_constants.h"

class DataTreeVarManager{
	protected:
	std::shared_ptr<TChain> fChain;
	std::shared_ptr<Selector> fSelector;
	DataTreeEvent* fEvent;
	DataTreeVarManager();
	public:
	enum Vars{
		kCentrality=0,	//0
		kFwModuleRing, 	//1
		kFwModuleId=kFwModuleRing+304,	//2
		kFwModuleAdc=kFwModuleId+304,	//3
		kFwModulePhi=kFwModuleAdc+304,	//4
		kNumberOfVars=kFwModulePhi+304	//5
	};
	DataTreeVarManager(std::string fileName);
	~DataTreeVarManager() = default;
	DataTreeEvent*	Event() {return fEvent;}
	void			FillEventVariables(double* varContainer);
	int 			GetNumberOfEvents() {return fChain->GetEntries();}
	bool			IsGoodEvent(){ return fSelector->IsCorrectEvent(); }
	void			SwitchEvent(int idx) {fChain->GetEntry(idx); }
};