#pragma once

#include <iostream>
#include <memory>

#include <TChain.h>
#include "Selector.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "DataTreeEvent.h"
#include "Centrality.h"

#include "HADES_constants.h"

class DataTreeVarManager{
	protected:
	int fSignal=0;
	std::shared_ptr<TChain> fChain;
	std::shared_ptr<Selector> fSelector;
	DataTreeEvent* fEvent;
	Centrality* fCentrality;
	DataTreeVarManager() = default;
	int fCurrentTrackIdx;
	public:
	enum Signals{
		kAdc=0,
		kChargeZ,
		kNumOfSignals
	};
	enum Vars{
		kCentrality=0,	//0
		kOne,
		kMdcPt,
		kMdcYcm,
		kMdcPhi,
		kMdcPid,
		kFwModuleRing, 	//1
		kFwModuleId=kFwModuleRing+304,	//2
		kFwModuleAdc=kFwModuleId+304,	//3
		kFwModulePhi=kFwModuleAdc+304,	//4
		kRandomSe=kFwModulePhi+304,
		kFwModuleX=kRandomSe+304,
		kFwModuleY=kFwModuleX+304,
		kNumberOfVars=kFwModuleY+304	//5
	};
	DataTreeVarManager(std::string fileName);
	~DataTreeVarManager() = default;
	DataTreeEvent*	Event() {return fEvent;}
	void			FillEventVariables(double* varContainer);
	void			FillTrackVariables(int idx, double* varContainer);
	int 			GetNumberOfEvents() { return fChain->GetEntries(); }
	int				GetNumberOfTracks() { return fEvent->GetNVertexTracks(); }
	bool			IsGoodEvent(){ return fSelector->IsCorrectEvent(); }
	bool			IsGoodTrack(int idx){ return fSelector->IsCorrectTrack(idx); }
	void			SwitchEvent(int idx) { fChain->GetEntry(idx); }
	void			SetSignal(int signal = DataTreeVarManager::Signals::kAdc) { fSignal=signal; }
	std::shared_ptr<Selector> GetSelector(){ return fSelector; }
};