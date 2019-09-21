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
		this->ProgressBar(progress);
		goodEvents++;
	}
	std::cout << goodEvents << " good events" << std::endl;
	Finalize();
}

void CorrectionTask::SetFwConfiguration(bool perChannel, std::string signal, float min, float max)
{
	fVarManager->GetSelector()->SetFwSignalType(signal);
	fVarManager->SetSignal(signal == "adc" ? DataTreeVarManager::Signals::kAdc : DataTreeVarManager::Signals::kChargeZ);
	fVarManager->GetSelector()->SetFwChannelSelection(perChannel);
	fVarManager->GetSelector()->SetFwSignalRange(min, max);
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

	Axis pt("Pt", 20, 0., 2.0);
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
		// config->AddCorrectionOnQnVector(rescale);
	};

	//Configuration of FW. Preparing for add axis to qa histograms
	//Producing the function which will configurate the correction Manager

	auto FwConfiguration = [](DetectorConfiguration *config) 
	{
		config->SetNormalization(QVector::Normalization::M);
		auto recenter = new Recentering();
		config->AddCorrectionOnQnVector(recenter);
		auto fwChannels=new bool[304];
		auto fwChannelGroups=new int[304];
		for(int i=0; i<304; i++)
		{
			fwChannels[i]=true;
			fwChannelGroups[i]=i;
		}
		config->SetChannelsScheme(fwChannels, fwChannelGroups);
	};

	// u-vectors from MDC
	fManager.AddDetector("TracksMdcYcm", DetectorType::TRACK, "Phi", "Ones", {ycm}, {1});
	fManager.AddCut("TracksMdcYcm", {"Pid", "Pt"}, [](const double &pid, const double &pt){ return pid > 13.99 && pid < 14.01 && pt > 0.80 && pt < 0.85 ; });
	fManager.SetCorrectionSteps("TracksMdcYcm", MdcConfiguration);
	
	fManager.AddDetector("TracksMdcPt", DetectorType::TRACK, "Phi", "Ones", {pt}, {1});
	fManager.AddCut("TracksMdcPt", {"Pid", "Ycm"}, [](const double &pid, const double &y){ return pid > 13.99 && pid < 14.01 && y>-0.25 && y<-0.15; });
	fManager.SetCorrectionSteps("TracksMdcPt", MdcConfiguration);

	// 3 sub-events method.
	// Each detector builds own Q-vector, which means, you need to add required count of detectors and then configurate their cuts.  
	fManager.AddDetector("Fw1Sp", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Fw1Sp", {"FwRing"}, [](const double &module) { return module >= 0.0 && module <= 4.0; });
	fManager.SetCorrectionSteps("Fw1Sp", FwConfiguration);

	fManager.AddDetector("Fw1Ep", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Fw1Ep", {"FwRing"}, [](const double &module) { return module >= 0.0 && module <= 4.0; });
	fManager.SetCorrectionSteps("Fw1Ep", FwConfiguration);

	fManager.AddDetector("Fw2Sp", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Fw2Sp", {"FwRing"}, [](const double &module) { return module == 5.0 || module == 6.0; });
	fManager.SetCorrectionSteps("Fw2Sp", FwConfiguration);

	fManager.AddDetector("Fw2Ep", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Fw2Ep", {"FwRing"}, [](const double &module) { return module == 5.0 || module == 6.0; });
	fManager.SetCorrectionSteps("Fw2Ep", FwConfiguration);

	fManager.AddDetector("Fw3Sp", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Fw3Sp", {"FwRing"}, [](const double &module) { return module >= 7.0 && module <= 9.0; });
	fManager.SetCorrectionSteps("Fw3Sp", FwConfiguration);

	fManager.AddDetector("Fw3Ep", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Fw3Ep", {"FwRing"}, [](const double &module) { return module >= 7.0 && module <= 9.0; });
	fManager.SetCorrectionSteps("Fw3Ep", FwConfiguration);

	fManager.AddDetector("Rs1Ep", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Rs1Ep", {"RandomSe"}, [](const double &rs){ return rs == 1.00; });
	fManager.SetCorrectionSteps("Rs1Ep", FwConfiguration);

	fManager.AddDetector("Rs1Sp", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Rs1Sp", {"RandomSe"}, [](const double &rs) { return rs == 1.00; });
	fManager.SetCorrectionSteps("Rs1Sp", FwConfiguration);

	fManager.AddDetector("Rs2Ep", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Rs2Ep", {"RandomSe"}, [](const double &rs) { return rs == 2.00; });
	fManager.SetCorrectionSteps("Rs2Ep", FwConfiguration);

	fManager.AddDetector("Rs2Sp", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.AddCut("Rs2Sp", {"RandomSe"}, [](const double &rs) { return rs == 2.00; });
	fManager.SetCorrectionSteps("Rs2Sp", FwConfiguration);

	fManager.AddDetector("Full", DetectorType::CHANNEL, "FwPhi", "Ones", {}, {1});
	fManager.SetCorrectionSteps("Full", FwConfiguration);

	// fManager.AddHisto2D("TracksMdc", {{"Ycm", 100, -0.8, 0.8}, {"Pt", 100, 0., 1.5}} );
	
	fManager.AddHisto2D("Fw1Sp", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	fManager.AddHisto2D("Fw1Ep", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	
	fManager.AddHisto2D("Fw2Sp", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	fManager.AddHisto2D("Fw2Ep", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

	fManager.AddHisto2D("Fw3Sp", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	fManager.AddHisto2D("Fw3Ep", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

	fManager.AddHisto2D("Rs1Sp", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	fManager.AddHisto2D("Rs1Ep", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

	fManager.AddHisto2D("Rs2Sp", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});
	fManager.AddHisto2D("Rs2Ep", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}});

	fManager.AddHisto2D("Rs1Ep", {{"moduleX", 3000, -3000., 3000.}, {"moduleY", 3000, -3000., 3000.}});
	fManager.AddHisto2D("Rs2Ep", {{"moduleX", 3000, -3000., 3000.}, {"moduleY", 3000, -3000., 3000.}} );

	fManager.AddHisto2D("Rs1Sp", {{"moduleX", 3000, -3000., 3000.}, {"moduleY", 3000, -3000., 3000.}});
	fManager.AddHisto2D("Rs2Sp", {{"moduleX", 3000, -3000., 3000.}, {"moduleY", 3000, -3000., 3000.}});

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