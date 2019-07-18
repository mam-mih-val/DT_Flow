//
// Created by Lukas Kreis on 29.06.17.
//
#include "CorrectionTask.h"

#include <memory>
#include <iostream>

namespace Qn {

CorrectionTask::CorrectionTask(std::string inFile, std::string incalib) :
	out_file_(new TFile("output.root", "RECREATE")),
	in_calibration_file_(new TFile(incalib.data(), "READ")),
	out_calibration_file_(new TFile("qn.root", "RECREATE")),
	out_tree_(nullptr),
	fManager(),
	write_tree_(true) 
{
	fChain = new TChain("DataTree");
    fChain->Add(inFile.data());
    cout << fChain->GetEntries() << " events found" << endl;
    fChain->SetBranchAddress("DTEvent", &fEvent);
	fSelector = new Selector(fEvent);
	out_file_->cd();
	out_tree_ = new TTree("tree", "tree");
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
	std::cout << "Analyzed " << events2read << " events." << std::endl;
	Finalize();
}

void CorrectionTask::Initialize() {
	// Add Variables to variable manager needed for filling
	fManager.AddVariable("Centrality", kCentrality, 1);
	fManager.AddVariable("FwRing", kFwModuleRing, 1);
	fManager.AddVariable("FwModuleId", kFwModuleId, 1);
	fManager.AddVariable("FwAdc", kFwModuleAdc, 1);
	fManager.AddVariable("FwPhi", kFwModulePhi, 1);

	//Correction eventvariables
	fManager.SetEventVariable("Centrality");
	fManager.AddCorrectionAxis({"Centrality", 9, 0, 45});

	//Configuration of FW. Preparing for add axis to qa histograms
	Axis Centrality("Centrality", {0.0,, 5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0});
  
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
	};
	auto Is1se = [](double ring){ return ring>0 && ring <=4; }
	auto Is2se = [](double ring){ return ring==5 || ring ==6; }
	auto Is3se = [](double ring){ return ring>=7 && ring <=9; }
	// 3 sub-events method.
	// Each detector builds own Q-vector, which means, you need to add required count of detectors and then configurate their cuts.  
	fManager.AddDetector("Fw1", DetectorType::CHANNEL, "FwPhi", "FwAdc", {Centrality}, {1});
	fManager.AddCut("Fw1", {"FwRing"}, Is1se);
	fManager.SetCorrectionSteps("Fw1", FwConfiguration);

	fManager.AddDetector("Fw2", DetectorType::CHANNEL, "FwPhi", "FwAdc", {Centrality}, {1});
	fManager.AddCut("Fw2", {"FwRing"}, Is2se);
	fManager.SetCorrectionSteps("Fw2", FwConfiguration);

	fManager.AddDetector("Fw3", DetectorType::CHANNEL, "FwPhi", "FwAdc", {Centrality}, {1});
	fManager.AddCut("Fw3", {"FwRing"}, Is3se);
	fManager.SetCorrectionSteps("Fw3", FwConfiguration);
  
	fManager.SetTree(out_tree_);
	fManager.Initialize(in_calibration_file_);
}

void CorrectionTask::Process() {
	fManager.Reset();
	auto varContainer = fManager.GetVariableContainer(); 
	varContainer[kCentrality]=fEvent->GetCentrality(HADES_constants::kNhitsTOF_RPC_cut);
	Int_t nModules = fEvent->GetNPSDModules();

	for(Int_t idx=0; idx<nModules; idx++) 
	{
		varContainer[kFwModuleRing]=fEvent->GetPSDModule(idx)->GetRing();
		varContainer[kFwModuleId]=fEvent->GetPSDModule(idx)->GetId();
		varContainer[kFwModuleAdc]=fEvent->GetPSDModule(idx)->GetEnergy();
		varContainer[kFwModulePhi]=fEvent->GetPSDModule(idx)->GetPhi();
		fManager.FillTrackingDetectors();
	}
	fManager.ProcessQnVectors();
}

void CorrectionTask::Finalize() {
  fManager.Finalize();
  fManager.SaveOutput(out_calibration_file_, out_file_);
}
}
