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
  // --------------------------------- Scalar Product ---------------------------------
  auto QxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1);
  };
  auto QxQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1);
  };
  auto QyQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1);
  };
  auto QyQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1);
  };
  auto uxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / qn.at(0).mag(1);
  };
  auto uxQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1) / qn.at(0).mag(1);
  };
  auto uyQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1) / qn.at(0).mag(1);
  };
  auto uyQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / qn.at(0).mag(1);
  };
  // --------------------------------- Second Harmonic ---------------------------------
  auto uxQxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uxQyQySp = [](const std::vector<Qn::QVector> &qn) {
    return - qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  auto uxQxQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  auto uxQyQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uyQxQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  auto uyQyQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uyQxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return - qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uyQyQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  // --------------------------------- Event plane ---------------------------------
  auto QxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto QxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto QyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto QyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto uxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto uxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto uyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  auto uyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / ( qn.at(0).mag(1) * qn.at(1).mag(1) );
  };
  // --------------------------------- Second Harmonic ---------------------------------
  auto uxQxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uxQyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return - qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uxQxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).y(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uxQyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).x(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uyQxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).y(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uyQyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).x(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uyQxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return - qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).x(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };
  auto uyQyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).y(1) / ( qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1) );
  };


  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 10, 0, 50});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP, 100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors(" Fw1, Fw2, Fw3");
  manager.AddQVectors("Rs1, Rs2");
  manager.AddQVectors("Full");
  manager.AddQVectors("TracksMdcPt, TracksMdcYcm");

  std::vector<std::string> Q3Se{"Fw1", "Fw2", "Fw3"};
  std::vector<std::string> QRnd{"Rs1", "Rs2"};
  std::vector<std::string> u_vector{"TracksMdcPt", "TracksMdcYcm"};
  std::vector<std::string> method{"Sp", "Ep"};

  /**
   * Correlations of all detectors vs PsiRP
   */
  std::string components [] = {"XX", "YY", "XY", "YX"};

  std::vector<std::string> Q3Se_1{Q3Se.at(1), Q3Se.at(2), Q3Se.at(0)};

  for (int i = 0; i < Q3Se.size(); i++)
  {
    for(auto meth : method){
      auto Q1 = Q3Se.at(i);
      auto Q2 = Q3Se_1.at(i);

      auto QxQx = meth == "Sp" ? QxQxSp : QxQxEp;
      auto QxQy = meth == "Sp" ? QxQySp : QxQyEp;
      auto QyQx = meth == "Sp" ? QyQxSp : QyQxEp;
      auto QyQy = meth == "Sp" ? QyQySp : QyQyEp;

      auto uxQx = meth == "Sp" ? uxQxSp : uxQxEp;
      auto uxQy = meth == "Sp" ? uxQySp : uxQyEp;
      auto uyQx = meth == "Sp" ? uyQxSp : uyQxEp;
      auto uyQy = meth == "Sp" ? uyQySp : uyQyEp;

      auto uxQxQx = meth == "Sp" ? uxQxQxSp : uxQxQxEp;
      auto uxQyQy = meth == "Sp" ? uxQyQySp : uxQyQyEp;
      auto uxQxQy = meth == "Sp" ? uxQxQySp : uxQxQyEp;
      auto uxQyQx = meth == "Sp" ? uxQyQxSp : uxQyQxEp;

      auto uyQxQx = meth == "Sp" ? uyQxQxSp : uyQxQxEp;
      auto uyQyQy = meth == "Sp" ? uyQyQySp : uyQyQyEp;
      auto uyQxQy = meth == "Sp" ? uyQxQySp : uyQxQyEp;
      auto uyQyQx = meth == "Sp" ? uyQyQxSp : uyQyQxEp;

      manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_"+meth, Q1 + ", " + Q2, QxQx);
      manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_XY_"+meth, Q1 + ", " + Q2, QxQy);
      manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XY_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YX_"+meth, Q1 + ", " + Q2, QyQx);
      manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YX_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_"+meth, Q1 + ", " + Q2, QyQy);
      manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

      for( auto u : u_vector )
      {
        // First harmonic
        manager.AddCorrelation(u + "_" + Q1 + "_XX_"+meth, u + ", " + Q1, uxQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_XX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_XY_"+meth, u + ", " + Q1, uxQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_XY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YX_"+meth, u + ", " + Q1, uyQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_YX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YY_"+meth, u + ", " + Q1, uyQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_YY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        // Second harmonic
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_"+meth, u + ", " + Q1+", " + Q2, uxQxQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_"+meth, u + ", " + Q1+", " + Q2, uxQyQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYX_"+meth, u + ", " + Q1+", " + Q2, uxQyQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXY_"+meth, u + ", " + Q1+", " + Q2, uxQxQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXX_"+meth, u + ", " + Q1+", " + Q2, uyQxQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYY_"+meth, u + ", " + Q1+", " + Q2, uyQyQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_"+meth, u + ", " + Q1+", " + Q2, uyQyQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_"+meth, u + ", " + Q1+", " + Q2, uyQxQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      }
    }
  }

  for (auto meth : method )
  {
    auto Q1 = QRnd.at(0);
    auto Q2 = QRnd.at(1);

    auto QxQx = meth == "Sp" ? QxQxSp : QxQxEp;
    auto QxQy = meth == "Sp" ? QxQySp : QxQyEp;
    auto QyQx = meth == "Sp" ? QyQxSp : QyQxEp;
    auto QyQy = meth == "Sp" ? QyQySp : QyQyEp;

    auto uxQx = meth == "Sp" ? uxQxSp : uxQxEp;
    auto uxQy = meth == "Sp" ? uxQySp : uxQyEp;
    auto uyQx = meth == "Sp" ? uyQxSp : uyQxEp;
    auto uyQy = meth == "Sp" ? uyQySp : uyQyEp;

    manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_"+meth, Q1 + ", " + Q2, QxQx);
    manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_XY_"+meth, Q1 + ", " + Q2, QxQy);
    manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XY_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_YX_"+meth, Q1 + ", " + Q2, QyQx);
    manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YX_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_"+meth, Q1 + ", " + Q2, QyQy);
    manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY_"+meth, {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

    for( auto u : u_vector )
    {
      manager.AddCorrelation(u + "_" + Q1 + "_XX_"+meth, u + ", " + Q1, uxQx);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_XX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_XY_"+meth, u + ", " + Q1, uxQy);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_XY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_YX_"+meth, u + ", " + Q1, uyQx);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_YX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_YY_"+meth, u + ", " + Q1, uyQy);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_YY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

      manager.AddCorrelation(u + "_" + Q2 + "_XX_"+meth, u + ", " + Q2, uxQx);
      manager.SetRefQinCorrelation(u + "_" + Q2 + "_XX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_XY_"+meth, u + ", " + Q2, uxQy);
      manager.SetRefQinCorrelation(u + "_" + Q2 + "_XY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_YX_"+meth, u + ", " + Q2, uyQx);
      manager.SetRefQinCorrelation(u + "_" + Q2 + "_YX_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_YY_"+meth, u + ", " + Q2, uyQy);
      manager.SetRefQinCorrelation(u + "_" + Q2 + "_YY_"+meth, {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

    }
  }

  for( auto u : u_vector )
  {
    manager.AddCorrelation(u + "_" + "Full" + "_XX_Ep", u + ", " + "Full", uxQxEp);
    manager.SetRefQinCorrelation(u + "_" + "Full" + "_XX_Ep", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(u + "_" + "Full" + "_XY_Ep", u + ", " + "Full", uxQyEp);
    manager.SetRefQinCorrelation(u + "_" + "Full" + "_XY_Ep", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(u + "_" + "Full" + "_YX_Ep", u + ", " + "Full", uyQxEp);
    manager.SetRefQinCorrelation(u + "_" + "Full" + "_YX_Ep", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(u + "_" + "Full" + "_YY_Ep", u + ", " + "Full", uyQyEp);
    manager.SetRefQinCorrelation(u + "_" + "Full" + "_YY_Ep", {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
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
