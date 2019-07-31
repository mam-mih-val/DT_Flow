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
	for(int idx=0; idx<numEvents; idx++) 
	{
		fVarManager->SwitchEvent(idx);
		if( !fVarManager->IsGoodEvent() )
			continue;
		Process();
	}
	Finalize();
}

void CorrectionTask::Initialize() {
	// Add Variables to variable manager needed for filling
	fManager.AddVariable("Centrality", DataTreeVarManager::kCentrality, 1);
	fManager.AddVariable("FwRing", DataTreeVarManager::kFwModuleRing, 304);
	fManager.AddVariable("FwModuleId", DataTreeVarManager::kFwModuleId, 304);
	fManager.AddVariable("FwAdc", DataTreeVarManager::kFwModuleAdc, 304);
	fManager.AddVariable("FwPhi", DataTreeVarManager::kFwModulePhi, 304);
	std::cout << "Variables added" << std::endl;
	//Correction eventvariables
	
	fManager.SetEventVariable("Centrality");
	fManager.AddCorrectionAxis({"Centrality", 10, 0, 50});


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
		config->AddCorrectionOnQnVector(rescale);
		auto fwChannels=new bool[304];
		auto fwChannelGroups=new int[304];
		for(int i=0; i<304; i++)
		{
			fwChannels[i]=true;
			fwChannelGroups[i]=i;
		}
		config->SetChannelsScheme(fwChannels, fwChannelGroups);
	};
	// 3 sub-events method.
	// Each detector builds own Q-vector, which means, you need to add required count of detectors and then configurate their cuts.  
	fManager.AddDetector("Fw1", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw1", {"FwModuleId", "FwAdc"}, [](const double &module, const double &adc){ return module>0.0 && module <144.0 && adc>10.0; });
	fManager.SetCorrectionSteps("Fw1", FwConfiguration);

	fManager.AddDetector("Fw2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw2", {"FwModuleId"}, [](const double &module){ return module>143.0 && module < 208.0; });
	fManager.SetCorrectionSteps("Fw2", FwConfiguration);

	fManager.AddDetector("Fw3", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw3", {"FwModuleId"}, [](const double &module){ return module>207.0 && module < 304.0; });
	fManager.SetCorrectionSteps("Fw3", FwConfiguration);
  
	fManager.AddHisto2D("Fw1", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	fManager.AddHisto2D("Fw2", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	fManager.AddHisto2D("Fw3", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	
	fManager.AddEventHisto1D({{"Centrality", 10, 0, 50}});
	
	fManager.SetTree(out_tree_);
	fManager.Initialize(in_calibration_file_);
	std::cout << "Successfully initialized" << std::endl;
}

void CorrectionTask::Process() {
	fManager.Reset();
	fVarManager->FillEventVariables(fManager.GetVariableContainer());
	fManager.ProcessEvent();
	fManager.FillChannelDetectors();
	fManager.FillTrackingDetectors();
	fManager.ProcessQnVectors();
}

void CorrectionTask::Finalize() {
	fManager.Finalize();
	fManager.SaveOutput(out_calibration_file_, out_file_);
	std::cout << "Successfully Finalized." << std::endl;
}
}
