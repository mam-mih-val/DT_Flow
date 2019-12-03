//
// Created by Lukas Kreis on 29.06.17.
//
#include "CorrectionTask.h"

#include <memory>
#include <iostream>

namespace Qn {

CorrectionTask::CorrectionTask(std::string inFile, std::string incalib) :
	out_file_(new TFile("output.root", "RECREATE")),
	in_calibration_file_(new TFile(incalib.c_str(), "READ")),
	out_calibration_file_(new TFile("qn.root", "RECREATE")),
	fManager(),
	write_tree_(true) ,
	fVarManager(new DataTreeVarManager(inFile))
{
	out_file_->cd();
	out_tree_= new TTree("tree", "tree");
}

void CorrectionTask::Run() 
{
	Initialize();
	QnCorrectionsSetTracingLevel(kError);
	std::cout << "Processing..." << std::endl;
	int numEvents = fVarManager->GetNumberOfEvents();
	int goodEvents=0;
	for(int idx=0; idx<numEvents; idx++) 
	{
		fVarManager->SwitchEvent(idx);
		if( !fVarManager->IsGoodEvent() )
			continue;
		Process();
		float progress = (float) idx / (float) numEvents;
		// this->ProgressBar(progress);
		goodEvents++;
	}
	std::cout << goodEvents << " good events" << std::endl;
	Finalize();
}

void CorrectionTask::SetSelectorConfiguration(bool perChannel, std::string signal, float min, float max, int pid)
{
	fVarManager->GetSelector()->SetFwSignalType(signal);
	fVarManager->SetSignal(signal == "adc" ? DataTreeVarManager::Signals::kAdc : DataTreeVarManager::Signals::kChargeZ);
	fVarManager->GetSelector()->SetFwChannelSelection(perChannel);
	fVarManager->GetSelector()->SetFwSignalRange(min, max);
	fVarManager->GetSelector()->SetPid(pid);
}

void CorrectionTask::Initialize() {
	// Add Variables to variable manager needed for filling
	fManager.AddVariable("Centrality", DataTreeVarManager::kCentrality, 1);
	fManager.AddVariable("One", DataTreeVarManager::kOne, 1);
	fManager.AddVariable("Pt", DataTreeVarManager::kMdcPt, 1);
	fManager.AddVariable("Phi", DataTreeVarManager::kMdcPhi, 1);
	fManager.AddVariable("Ycm", DataTreeVarManager::kMdcYcm, 1);
	fManager.AddVariable("Pid", DataTreeVarManager::kMdcPid, 1);
	fManager.AddVariable("FwRing", DataTreeVarManager::kFwModuleRing, 304);
	fManager.AddVariable("FwModuleId", DataTreeVarManager::kFwModuleId, 304);
	fManager.AddVariable("FwAdc", DataTreeVarManager::kFwModuleAdc, 304);
	fManager.AddVariable("FwPhi", DataTreeVarManager::kFwModulePhi, 304);
	fManager.AddVariable("RandomSe", DataTreeVarManager::kRandomSe, 304);
	fManager.AddVariable("moduleX", DataTreeVarManager::kFwModuleX, 304);
	fManager.AddVariable("moduleY", DataTreeVarManager::kFwModuleY, 304);
	std::cout << "Variables added" << std::endl;
	//Correction eventvariables
	
	fManager.SetEventVariable("Centrality");
	fManager.AddCorrectionAxis({"Centrality", 20, 0, 100});

	Axis pt("Pt", 16, 0.2, 1.8);
	Axis ycm("Ycm", 16, -0.8, 0.8);
	
	// Configuration of MDC.

	auto MdcConfiguration = [](DetectorConfiguration *config)
	{
          config->SetNormalization(QVector::Normalization::M);
          auto recenter = new Recentering();
          config->AddCorrectionOnQnVector(recenter);
          auto rescale = new TwistAndRescale();
          rescale->SetApplyTwist(true);
          rescale->SetApplyRescale(true);
          rescale->SetTwistAndRescaleMethod(TwistAndRescale::TWRESCALE_doubleHarmonic);
          //config->AddCorrectionOnQnVector(rescale);
	};

	//Configuration of FW. Preparing for add axis to qa histograms
	//Producing the function which will configurate the correction Manager

	auto FwConfiguration = [](DetectorConfiguration *config)
	{
          config->SetNormalization(QVector::Normalization::M);
          auto recenter = new Recentering();
          config->AddCorrectionOnQnVector(recenter);
          auto rescale = new TwistAndRescale();
          rescale->SetApplyTwist(true);
          rescale->SetApplyRescale(true);
          rescale->SetTwistAndRescaleMethod(TwistAndRescale::TWRESCALE_doubleHarmonic);
//          config->AddCorrectionOnQnVector(rescale);
          auto fwChannels=new bool[304];
          auto fwChannelGroups=new int[304];
          for(int i=0; i<304; i++){
            fwChannels[i]=true;
            fwChannelGroups[i]=i;
          }
          config->SetChannelsScheme(fwChannels, fwChannelGroups);
	};

	// u-vectors from MDC
	fManager.AddDetector("TracksMdcPt", DetectorType::TRACK, "Phi", "Ones", {pt}, {1, 2, 3});
	fManager.AddCut("TracksMdcPt", {"Ycm"}, [](const double &y){ return -0.25 < y && y < -0.15; });
	fManager.SetCorrectionSteps("TracksMdcPt", MdcConfiguration);

	fManager.AddDetector("TracksMdcYcm", DetectorType::TRACK, "Phi", "Ones", {ycm}, {1, 2, 3});
	fManager.AddCut("TracksMdcYcm", {"Pt"}, [](const double &pt){ return 0.80 < pt && pt < 0.85; });
	fManager.SetCorrectionSteps("TracksMdcYcm", MdcConfiguration);

	// 3 sub-events method.
	// Each detector builds own Q-vector, which means, you need to add required count of detectors and then configurate their cuts.
	fManager.AddDetector("Fw1", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw1", {"FwRing"}, [](const double &module) { return module >= 0.0 && module <= 4.0; });
	fManager.SetCorrectionSteps("Fw1", FwConfiguration);

	fManager.AddDetector("Fw2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw2", {"FwRing"}, [](const double &module) { return module == 5.0 || module == 6.0; });
	fManager.SetCorrectionSteps("Fw2", FwConfiguration);

	fManager.AddDetector("Fw3", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw3", {"FwRing"}, [](const double &module) { return module >= 7.0 && module <= 9.0; });
	fManager.SetCorrectionSteps("Fw3", FwConfiguration);

	// Random sub-event method
	fManager.AddDetector("Rs1", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Rs1", {"RandomSe"}, [](const double &rs){ return rs == 1.00; });
	fManager.SetCorrectionSteps("Rs1", FwConfiguration);

	fManager.AddDetector("Rs2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Rs2", {"RandomSe"}, [](const double &rs) { return rs == 2.00; });
	fManager.SetCorrectionSteps("Rs2", FwConfiguration);

	fManager.AddDetector("Full", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
        fManager.AddCut("Full", {"FwAdc"}, [](const double &adc) { return adc > 0.0; });
	fManager.SetCorrectionSteps("Full", FwConfiguration);


	fManager.AddHisto2D("Fw1", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	fManager.AddHisto2D("Fw2", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	fManager.AddHisto2D("Fw3", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

	fManager.AddHisto2D("Rs1", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	fManager.AddHisto2D("Rs2", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

	fManager.AddHisto2D("Rs1", {{"moduleX", 50, -1000., 1000.}, {"moduleY", 50, -1000., 1000.}});
	fManager.AddHisto2D("Rs2", {{"moduleX", 50, -1000., 1000.}, {"moduleY", 50, -1000., 1000.}});
	fManager.AddHisto2D("Full", {{"moduleX", 50, -1000., 1000.}, {"moduleY", 50, -1000., 1000.}});
	fManager.AddEventHisto1D({{"Centrality", 20, 0, 100}});
	fManager.SetTree(out_tree_);
	fManager.Initialize(in_calibration_file_);
	std::cout << "Successfully initialized" << std::endl;
}

void CorrectionTask::Process() {
	fManager.Reset();
	fVarManager->FillEventVariables(fManager.GetVariableContainer());
	fManager.ProcessEvent();
	fManager.FillChannelDetectors();
	int trackNumber = fVarManager->GetNumberOfTracks();
	for(int i=0; i<trackNumber; i++)
	{
		if( !fVarManager->IsGoodTrack(i) )
			continue;
		fVarManager->FillTrackVariables( i, fManager.GetVariableContainer() );
		fManager.FillTrackingDetectors();
	}
	fManager.ProcessQnVectors();
}

void CorrectionTask::Finalize() {
	fManager.Finalize();
	fManager.SaveOutput(out_calibration_file_, out_file_);
	// std::cout << "Successfully Finalized." << std::endl;
}

void CorrectionTask::ProgressBar(float progress)
{
	int barWidth = 100;
	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
	if (i < pos) std::cout << "=";
	else if (i == pos) std::cout << ">";
	else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
}
}