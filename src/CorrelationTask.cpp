#include <utility>

//
// Created by Lukas Kreis on 13.11.17.
//

#include "CorrelationTask.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TTreeReaderValue.h"

using std::string;

CorrelationTask::CorrelationTask(std::string filelist, std::string treename) :
    in_tree_(this->MakeChain(std::move(filelist), "tree")),
    reader_(new TTreeReader(in_tree_.get())) {}

void CorrelationTask::Configure(Qn::CorrelationManager &manager) 
{
  auto XY = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1);
  };
  auto YX = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1);
  };
  auto XX = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1);
  };
  auto YY = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1);
  };

  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 10, 0, 50});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP, 100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors("Fw1, Fw2, Fw3");
  manager.AddQVectors("ProtonMdc");
  
  std::vector<string> Q_vector{"Fw1", "Fw2", "Fw3"};
  std::vector<string> u_vector{"ProtonMdc"};
  
  /**
   * Correlations of all detectors vs PsiRP
   */
  string components [] = {"XX", "YY", "XY", "YX"};
  
  for (int idx1 = 0; idx1 < Q_vector.size(); idx1++)
  {
    auto Q1 = Q_vector.at (idx1);
    
    for (int idx2 = idx1 + 1; idx2 < Q_vector.size(); idx2++)
    {
		auto Q2 = Q_vector.at (idx2);
		manager.AddCorrelation(Q1 + "_" + Q2 + "_XX", Q1 + ", " + Q2, XX);
		manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
		manager.AddCorrelation(Q1 + "_" + Q2 + "_YY", Q1 + ", " + Q2, YY);
		manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
		manager.AddCorrelation(Q1 + "_" + Q2 + "_XY", Q1 + ", " + Q2, XY);
		manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
		manager.AddCorrelation(Q1 + "_" + Q2 + "_YX", Q1 + ", " + Q2, YX);
		manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    }
	for(auto &u : u_vector)
	{
		manager.AddCorrelation(u + "_" + Q1 + "_XX", u + ", " + Q1, XX);
		manager.SetRefQinCorrelation(u + "_" + Q1 + "_XX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
		manager.AddCorrelation(u + "_" + Q1 + "_YY", u + ", " + Q1, YY);
		manager.SetRefQinCorrelation(u + "_" + Q1 + "_YY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
		manager.AddCorrelation(u + "_" + Q1 + "_XY", u + ", " + Q1, XY);
		manager.SetRefQinCorrelation(u + "_" + Q1 + "_XY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
		manager.AddCorrelation(u + "_" + Q1 + "_YX", u + ", " + Q1, YX);
		manager.SetRefQinCorrelation(u + "_" + Q1 + "_YX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
	}
  }
}

void CorrelationTask::Run() {
  int nEvents = in_tree_->GetEntries();
  Qn::CorrelationManager fManager(reader_, nEvents);
  in_tree_->LoadTree(0); // prevents weird TTree errors
  this->Configure(fManager);
  int events = 1;
  reader_->SetEntry(0); //  first entry needed to access configuration of DataContainers in the input file
  fManager.Initialize();
  in_tree_->LoadTree(0); // prevents weird TTree errors
  std::cout << "Processing..." << std::endl;
  int eventsteps = nEvents / 100;
  events = 0;
  do
  {
    events++;
    fManager.Process();
    if (eventsteps > 1 && events % eventsteps == 0) {
      float progress = events / (float) nEvents;
      ProgressBar(progress);
    }
  }
  while(reader_->Next());
  fManager.Finalize();
  std::cout << std::endl << "number of analyzed events: " << events << std::endl;
}

std::unique_ptr<TTree> CorrelationTask::MakeChain(std::string fileList, std::string treename) {
  auto file = new TFile(fileList.c_str());
  std::unique_ptr<TTree> chain((TTree*) file->Get("tree"));
//   chain->AddFile(fileList.c_str());
  return chain;
}

void CorrelationTask::ProgressBar(float progress) {
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
