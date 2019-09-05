#pragma once
#include "TH1F.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"

#define  DATATREE_SHINE
#include "DataTreeEvent.h"

#include "HADES_constants.h"

using std::vector;
using std::cout;
using std::endl;

class Selector
{
public:
	Selector() {};
    Selector(DataTreeEvent* _fEvent) { fEvent=_fEvent; };
    ~Selector() = default;
    bool	IsCorrectEvent();
    bool	IsCorrectTrack(int idx);
    bool	IsCorrectFwHit(int idx);
	void	SetEventAddress(DataTreeEvent* _fEvent) {fEvent=_fEvent;}
	void	SetFwChannelSelection(bool value=false) { fChannelSelection=value; }
	void	SetFwSignalType(std::string value="adc") { fFwSignal=value; }
	void	SetFwSignalRange(float minValue=80.0, float maxValue=80.0) { 
		fMinSignal=minValue; 
		fMaxSignal=maxValue; 
	}

private:
    DataTreeEvent*	fEvent{nullptr};
	bool fChannelSelection=false;
	std::string fFwSignal="adc";
	float fMinSignal=80.0;
	float fMaxSignal=999.0;
};