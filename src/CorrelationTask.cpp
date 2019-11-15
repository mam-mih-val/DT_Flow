#include <utility>

//
// Created by Lukas Kreis on 13.11.17.
//

#include "CorrelationTask.h"

#include "TCanvas.h"
#include "TFile.h"
#include "TTreeReaderValue.h"
#include <cmath>

using std::string;

CorrelationTask::CorrelationTask(std::string filelist, std::string treename) :
    in_tree_(this->MakeChain(std::move(filelist), "tree")),
    reader_(new TTreeReader(in_tree_.get())) {}

void CorrelationTask::Configure(Qn::CorrelationManager &manager) 
{
  auto QxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1);
  };
  auto QyQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1);
  };
  auto QxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto QyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };

  auto uxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / qn.at(0).mag(1);
  };
  auto uyQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / qn.at(0).mag(1);
  };
  auto uxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / (qn.at(0).mag(1)*qn.at(1).mag(1));
  };
  auto uyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1)*qn.at(1).mag(1));
  };
  auto uxQxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1)*qn.at(1).mag(1));
  };

  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 10, 0, 50});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP, 100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors(" Fw1Sp, Fw2Sp, Fw3Sp");
  manager.AddQVectors(" Fw1Ep, Fw2Ep, Fw3Ep");
  manager.AddQVectors("Rs1Ep, Rs2Ep");
  manager.AddQVectors("Rs1Sp, Rs2Sp");
  manager.AddQVectors("Full");
  manager.AddQVectors("TracksMdcPtMr");

  std::vector<std::string> Q3Se{"Fw1", "Fw2", "Fw3"};
  std::vector<std::string> QRnd{"Rs1", "Rs2"};
  std::vector<std::string> method{ "Sp", "Ep" };

  std::vector<std::string> u_vector{"TracksMdcPtMr"};

  /**
   * Correlations of all detectors vs PsiRP
   */
  std::string components [] = {"XX", "YY", "XY", "YX"};

  std::vector<std::string> Q3Se_1{Q3Se.at(1), Q3Se.at(2), Q3Se.at(0)};

  for (int i = 0; i < Q3Se.size(); i++)
  {
    for( auto meth : method )
    {
      auto Q1 = Q3Se.at(i)+meth;
      auto Q2 = Q3Se_1.at(i)+meth;
      auto QxQx = meth == "Sp" ? QxQxSp : QxQxEp;
      auto QyQy = meth == "Sp" ? QyQySp : QyQyEp;

      auto uxQx = meth == "Sp" ? uxQxSp : uxQxEp;
      auto uyQy = meth == "Sp" ? uyQySp : uyQyEp;

      manager.AddCorrelation(Q1 + "_" + Q2 + "_XX", Q1 + ", " + Q2, QxQx);
      manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YY", Q1 + ", " + Q2, QyQy);
      manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      
      for( auto u : u_vector )
      {
        manager.AddCorrelation(u + "_" + Q1 + "_XX", u + ", " + Q1, uxQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_XX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YY", u + ", " + Q1, uyQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_YY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      }
    }
  }

  for (auto meth : method )
  {
    auto Q1 = QRnd.at(0) + meth;
    auto Q2 = QRnd.at(1) + meth;
    auto QxQx = meth == "Sp" ? QxQxSp : QxQxEp;
    auto QyQy = meth == "Sp" ? QyQySp : QyQyEp;

    auto uxQx = meth == "Sp" ? uxQxSp : uxQxEp;
    auto uyQy = meth == "Sp" ? uyQySp : uyQyEp;

    manager.AddCorrelation(Q1 + "_" + Q2 + "_XX", Q1 + ", " + Q2, QxQx);
    manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_YY", Q1 + ", " + Q2, QyQy);
    manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

    for( auto u : u_vector )
    {
      manager.AddCorrelation(u + "_" + Q1 + "_XX", u + ", " + Q1, uxQx);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_XX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_YY", u + ", " + Q1, uyQy);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_YY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

      manager.AddCorrelation(u + "_" + Q2 + "_XX", u + ", " + Q2, uxQx);
      manager.SetRefQinCorrelation(u + "_" + Q2 + "_XX", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_YY", u + ", " + Q2, uyQy);
      manager.SetRefQinCorrelation(u + "_" + Q2 + "_YY", {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
    }
  }

  for( auto u : u_vector )
  {
    manager.AddCorrelation(u + "_" + "Full" + "_XX", u + ", " + "Full", uxQxEp);
    manager.SetRefQinCorrelation(u + "_" + "Full" + "_XX", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(u + "_" + "Full" + "_YY", u + ", " + "Full", uyQyEp);
    manager.SetRefQinCorrelation(u + "_" + "Full" + "_YY", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
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
      // ProgressBar(progress);
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
