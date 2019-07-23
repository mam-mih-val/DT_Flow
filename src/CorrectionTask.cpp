//
// Created by Lukas Kreis on 29.06.17.
//
#include "CorrectionTask.h"

#include <memory>
#include <iostream>

namespace Qn {

CorrectionTask::CorrectionTask(std::string inFile, std::string incalib) :
	out_file_(new TFile("output.root", "RECREATE")),
	// in_calibration_file_(new TFile(incalib.c_str(), "READ")),
	in_calibration_file_{new TFile(incalib.c_str(), "READ")},
	out_calibration_file_(new TFile("qn.root", "RECREATE")),
	out_tree_(new TTree("tree", "tree")),
	fManager(),
	write_tree_(true) ,
	fEvent{nullptr}
{
	fChain = new TChain("DataTree");
    fChain->Add(inFile.c_str());
	std::cout << "File read. " << fChain->GetEntries() << " events found." << endl;
    fChain->SetBranchAddress("DTEvent", &fEvent);
	std::cout << "Branch read successfully" << std::endl;
	fSelector = new Selector(fEvent);
	out_file_->cd();
	std::cout << "Construction of Qn::CorrectionTask ended successfully" << endl;
}

void CorrectionTask::Run() 
{
	Initialize();
	QnCorrectionsSetTracingLevel(kError);
	std::cout << "Processing..." << std::endl;
	
	Long64_t numEvents = fChain->GetEntries();
	for(Long64_t idx=0; idx<numEvents; idx++) 
	{
		fChain->GetEntry(idx);
		if( !fSelector->IsCorrectEvent() )
			continue;
		Process();
	}
	Finalize();
}

void CorrectionTask::Initialize() {
	cout << "Initialization began" << endl;
	// Add Variables to variable manager needed for filling
	fManager.AddVariable("Centrality", kCentrality, 1);
	fManager.AddVariable("FwRing", kFwModuleRing, 304);
	fManager.AddVariable("FwModuleId", kFwModuleId, 304);
	fManager.AddVariable("FwAdc", kFwModuleAdc, 304);
	fManager.AddVariable("FwPhi", kFwModulePhi, 304);
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
		// auto rescale = new TwistAndRescale();
		// rescale->SetApplyTwist(true);
		// rescale->SetApplyRescale(true);
		// rescale->SetTwistAndRescaleMethod(TwistAndRescale::TWRESCALE_doubleHarmonic);
		// config->AddCorrectionOnQnVector(rescale);
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
	fManager.AddCut("Fw1", {"FwRing"}, [](const double ring){ return ring>0.0 && ring <=4.0; });
	fManager.SetCorrectionSteps("Fw1", FwConfiguration);

	fManager.AddDetector("Fw2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw2", {"FwRing"}, [](const double ring){ return ring>=5.0 && ring <=6.0; });
	fManager.SetCorrectionSteps("Fw2", FwConfiguration);

	fManager.AddDetector("Fw3", DetectorType::CHANNEL, "FwPhi", "FwAdc", {}, {1});
	fManager.AddCut("Fw3", {"FwRing"}, [](const double ring){ return ring>=7.0 && ring <=9.0; });
	fManager.SetCorrectionSteps("Fw3", FwConfiguration);
  
	fManager.AddHisto2D("Fw1", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	fManager.AddHisto2D("Fw2", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	fManager.AddHisto2D("Fw3", {{"FwAdc", 100, 0., 1000.}, {"FwModuleId", 304, 0., 304.}} );
	
	fManager.AddEventHisto1D({{"Centrality", 20, 0, 100}});
	
	fManager.SetTree(out_tree_);
	fManager.Initialize(in_calibration_file_);
}

void CorrectionTask::Process() {
	fManager.Reset();
	auto varContainer = fManager.GetVariableContainer(); 
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
		// std::cout << " *** " << std::endl;
		// for(int i=0; i<kNumberOfVars; i++)
		// 	std::cout << i << ": " << varContainer[i] << std::endl;
	}
	fManager.ProcessEvent();
	fManager.FillChannelDetectors();
	fManager.ProcessQnVectors();
}

void CorrectionTask::Finalize() {
	fManager.Finalize();
	fManager.SaveOutput(out_calibration_file_, out_file_);
	std::cout << "Successfully Finalized." << std::endl;
}
}
