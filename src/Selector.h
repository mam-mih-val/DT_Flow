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
    private:
    DataTreeEvent*	fEvent;
    TH1F*           hRejectedEvents;
    TH1F*           hRejectedTracks;
    TH1F*           hIncorrectEvent;
    TH1F*           hIncorrectTracks;
	bool			bSaveStat = 1;
    enum event_cuts{
        cVeretexPositionZ = 0,
        cVeretexPositionXY, //1
        cTriggerVertexClust, //3
        cTriggerVertexCand, //4
        cTriggerGoodStart, //5
        cTriggerNoPileUp, //6
        cTriggerGoodStartVeto, //7
        cTriggerGoodStartMeta, //8
        cTriggerNoVeto, //9
        cPT2, //10
        cPT3, //11
        cNumOfEventCuts //12
    };
    enum track_cuts{
        cDCA = 0,
        cTrackHitMatchX, //1
        cTrackHitMatchY, //2
        cChi2, //3
        cNumOfTrackCuts //5
    };
    public:
	Selector();
    Selector(DataTreeEvent* _fEvent);
    ~Selector();
    bool	IsCorrectEvent(int iPT = -1);
    bool	IsCorrectTrack(int idx);
    bool	IsCorrectFwHit(int idx, bool channelSelection=0, std::string signal="adc", float minSignal=0.0, float maxSignal=9999.0);
	void	SetEventAddress(DataTreeEvent* _fEvent) {fEvent=_fEvent;}
	void	SetStatOption(bool _bSaveStat = 1) { bSaveStat = _bSaveStat; }
    void    CheckEventCuts();
    void    CheckTrackCuts(int idx);
    void    DrawStatistics();
    void    SaveStatistics(std::string fileName);
};