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

CorrelationTask::CorrelationTask(std::string filelist, std::string treename)
    : in_tree_(this->MakeChain(std::move(filelist), "tree")),
      reader_(new TTreeReader(in_tree_.get())) {}

void CorrelationTask::Configure(Qn::CorrelationManager &manager) {
  // --------------------------------- Scalar Product
  // ---------------------------------
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
  // --------------------------------- Second Harmonic
  // ---------------------------------
  auto uxQxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uxQyQySp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1) / qn.at(0).mag(2);
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
    return -qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uyQyQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  // --------------------------------- Third Harmonic
  // ---------------------------------
  auto uxQxQxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(3) * qn.at(1).x(1) * qn.at(2).x(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uxQyQyQxSp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).y(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uxQyQxQySp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).y(1) * qn.at(2).x(1) * qn.at(3).y(1) /
           qn.at(0).mag(3);
  };
  auto uxQxQyQySp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).x(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uyQyQxQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).y(1) * qn.at(2).x(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uyQxQyQxSp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).x(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uyQxQxQySp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).x(1) * qn.at(2).x(1) * qn.at(3).y(1) /
           qn.at(0).mag(3);
  };
  auto uyQyQyQySp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).y(3) * qn.at(1).y(1) * qn.at(2).y(1) * qn.at(3).y(1) /
           qn.at(0).mag(3);
  };
  // --------------------------------- Event plane
  // ---------------------------------
  auto QxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto QxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto QyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto QyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto uxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto uxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto uyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto uyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  // --------------------------------- Second Harmonic
  // ---------------------------------
  auto uxQxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uxQyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uxQxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uxQyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).x(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uyQxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uyQyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).x(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uyQxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).x(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uyQyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  // --------------------------------- Third Harmonic
  // ---------------------------------
  auto uxQxQxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(3) * qn.at(1).x(1) * qn.at(2).x(1) * qn.at(3).x(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uxQyQyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).y(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uxQyQxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).y(1) * qn.at(2).x(1) * qn.at(3).y(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uxQxQyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).x(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uyQyQxQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).y(1) * qn.at(2).x(1) * qn.at(3).x(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uyQxQyQxEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).x(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uyQxQxQyEp = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).x(1) * qn.at(2).x(1) * qn.at(3).y(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };
  auto uyQyQyQyEp = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).y(3) * qn.at(1).y(1) * qn.at(2).y(1) * qn.at(3).y(1) /
           (qn.at(0).mag(3) * qn.at(1).mag(1) * qn.at(2).mag(1) *
            qn.at(3).mag(1));
  };

  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 10, 0, 50});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP,
                              100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors(" Fw1, Fw2, Fw3");
  manager.AddQVectors("Rs1, Rs2");
  manager.AddQVectors("Mdc1, Mdc2");
  manager.AddQVectors("Full");
  manager.AddQVectors("TracksMdcPt, TracksMdcYcm");

  std::vector<std::string> Q3Se{"Fw1", "Fw2", "Fw3"};
  std::vector<std::string> QRnd{"Rs1", "Rs2"};
  std::vector<std::string> QMdc{"Mdc1", "Mdc2", "Full"};
  std::vector<std::string> u_vector{"TracksMdcPt", "TracksMdcYcm"};
  std::vector<std::string> method{"Sp", "Ep"};

  /**
   * Correlations of all detectors vs PsiRP
   */
  std::string components[] = {"XX", "YY", "XY", "YX"};

  std::vector<std::string> Q3Se_1{Q3Se.at(1), Q3Se.at(2), Q3Se.at(0)};

  for (int i = 0; i < Q3Se.size(); i++) {
    for (auto meth : method) {
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

      manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_" + meth, Q1 + ", " + Q2,
                             QxQx);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_XX_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_XY_" + meth, Q1 + ", " + Q2,
                             QxQy);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_XY_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YX_" + meth, Q1 + ", " + Q2,
                             QyQx);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_YX_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_" + meth, Q1 + ", " + Q2,
                             QyQy);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_YY_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

      for (auto u : u_vector) {
        // First harmonic
        manager.AddCorrelation(u + "_" + Q1 + "_XX_" + meth, u + ", " + Q1,
                               uxQx);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_XX_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_XY_" + meth, u + ", " + Q1,
                               uxQy);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_XY_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YX_" + meth, u + ", " + Q1,
                               uyQx);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_YX_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YY_" + meth, u + ", " + Q1,
                               uyQy);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_YY_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        // Second harmonic
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_" + meth,
                               u + ", " + Q1 + ", " + Q2, uxQxQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_" + meth,
                               u + ", " + Q1 + ", " + Q2, uxQyQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYX_" + meth,
                               u + ", " + Q1 + ", " + Q2, uxQyQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYX_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXY_" + meth,
                               u + ", " + Q1 + ", " + Q2, uxQxQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXY_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});

        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXX_" + meth,
                               u + ", " + Q1 + ", " + Q2, uyQxQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXX_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYY_" + meth,
                               u + ", " + Q1 + ", " + Q2, uyQyQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYY_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_" + meth,
                               u + ", " + Q1 + ", " + Q2, uyQyQx);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_" + meth,
                               u + ", " + Q1 + ", " + Q2, uyQxQy);
        manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_" + meth,
                                     {Qn::Weight::OBSERVABLE,
                                      Qn::Weight::REFERENCE,
                                      Qn::Weight::REFERENCE});
      }
    }
  }
  // Third harmonic
  for (auto u : u_vector) {
    std::string Q1 = Q3Se.at(0);
    std::string Q2 = Q3Se.at(1);
    std::string Q3 = Q3Se.at(2);
    for (auto meth : method) {
      auto uxQxQxQx = meth == "Sp" ? uxQxQxQxSp : uxQxQxQxEp;
      auto uxQyQyQx = meth == "Sp" ? uxQyQyQxSp : uxQyQyQxEp;
      auto uxQyQxQy = meth == "Sp" ? uxQyQxQySp : uxQyQxQyEp;
      auto uxQxQyQy = meth == "Sp" ? uxQxQyQySp : uxQxQyQyEp;

      auto uyQyQxQx = meth == "Sp" ? uyQyQxQxSp : uyQyQxQxEp;
      auto uyQxQyQx = meth == "Sp" ? uyQxQyQxSp : uyQxQyQxEp;
      auto uyQxQxQy = meth == "Sp" ? uyQxQxQySp : uyQxQxQyEp;
      auto uyQyQyQy = meth == "Sp" ? uyQyQyQySp : uyQyQyQyEp;

      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XXXX_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uxQxQxQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XXXX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XYYX_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uxQyQyQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XYYX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XYXY_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uxQyQxQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XYXY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XXYY_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uxQxQyQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_XXYY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});

      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YYXX_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uyQyQxQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YYXX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YXYX_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uyQxQyQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YXYX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YXXY_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uyQxQxQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YXXY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YYYY_" +
                                 meth,
                             u + ", " + Q1 + ", " + Q2 + ", " + Q3, uyQyQyQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_" + Q2 + "_" + Q3 + "_YYYY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE,
           Qn::Weight::REFERENCE});
    }
  }

  std::vector<std::string> QMdc_1{QMdc.at(1), QMdc.at(2), QMdc.at(0)};

  for (int i = 0; i < Q3Se.size(); i++) {
    for (auto meth : method) {
      auto Q1 = QMdc.at(i);
      auto Q2 = QMdc_1.at(i);

      auto QxQx = meth == "Sp" ? QxQxSp : QxQxEp;
      auto QxQy = meth == "Sp" ? QxQySp : QxQyEp;
      auto QyQx = meth == "Sp" ? QyQxSp : QyQxEp;
      auto QyQy = meth == "Sp" ? QyQySp : QyQyEp;

      auto uxQx = meth == "Sp" ? uxQxSp : uxQxEp;
      auto uxQy = meth == "Sp" ? uxQySp : uxQyEp;
      auto uyQx = meth == "Sp" ? uyQxSp : uyQxEp;
      auto uyQy = meth == "Sp" ? uyQySp : uyQyEp;

      manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_" + meth, Q1 + ", " + Q2,
                             QxQx);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_XX_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_XY_" + meth, Q1 + ", " + Q2,
                             QxQy);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_XY_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YX_" + meth, Q1 + ", " + Q2,
                             QyQx);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_YX_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_" + meth, Q1 + ", " + Q2,
                             QyQy);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_YY_" + meth,
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

      for (auto u : u_vector) {
        // First harmonic
        manager.AddCorrelation(u + "_" + Q1 + "_XX_" + meth, u + ", " + Q1,
                               uxQx);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_XX_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_XY_" + meth, u + ", " + Q1,
                               uxQy);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_XY_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YX_" + meth, u + ", " + Q1,
                               uyQx);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_YX_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
        manager.AddCorrelation(u + "_" + Q1 + "_YY_" + meth, u + ", " + Q1,
                               uyQy);
        manager.SetRefQinCorrelation(
            u + "_" + Q1 + "_YY_" + meth,
            {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      }
    }
  }

  for (auto meth : method) {
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

    manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_" + meth, Q1 + ", " + Q2, QxQx);
    manager.SetRefQinCorrelation(
        Q1 + "_" + Q2 + "_XX_" + meth,
        {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_XY_" + meth, Q1 + ", " + Q2, QxQy);
    manager.SetRefQinCorrelation(
        Q1 + "_" + Q2 + "_XY_" + meth,
        {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_YX_" + meth, Q1 + ", " + Q2, QyQx);
    manager.SetRefQinCorrelation(
        Q1 + "_" + Q2 + "_YX_" + meth,
        {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_" + meth, Q1 + ", " + Q2, QyQy);
    manager.SetRefQinCorrelation(
        Q1 + "_" + Q2 + "_YY_" + meth,
        {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

    for (auto u : u_vector) {
      manager.AddCorrelation(u + "_" + Q1 + "_XX_" + meth, u + ", " + Q1, uxQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_XX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_XY_" + meth, u + ", " + Q1, uxQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_XY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_YX_" + meth, u + ", " + Q1, uyQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_YX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_YY_" + meth, u + ", " + Q1, uyQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q1 + "_YY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

      manager.AddCorrelation(u + "_" + Q2 + "_XX_" + meth, u + ", " + Q2, uxQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q2 + "_XX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_XY_" + meth, u + ", " + Q2, uxQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q2 + "_XY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_YX_" + meth, u + ", " + Q2, uyQx);
      manager.SetRefQinCorrelation(
          u + "_" + Q2 + "_YX_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q2 + "_YY_" + meth, u + ", " + Q2, uyQy);
      manager.SetRefQinCorrelation(
          u + "_" + Q2 + "_YY_" + meth,
          {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
    }
  }
}

void CorrelationTask::ConfigureRnd(Qn::CorrelationManager &manager) {
  // ------------------------ Event plane ------------------------
  auto QxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto QyQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto uxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto uyQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
  };
  auto u2xQ2x = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) *
           (qn.at(1).x(1) * qn.at(1).x(1) - qn.at(1).y(1) * qn.at(1).y(1)) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(1).mag(1));
  };
  auto u2yQ2y = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * 2 * qn.at(1).x(1) * qn.at(1).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(1).mag(1));
  };
  auto uxQxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uxQyQy = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uyQxQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).y(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };
  auto uyQyQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).x(1) /
           (qn.at(0).mag(2) * qn.at(1).mag(1) * qn.at(2).mag(1));
  };

  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 8, 0, 40});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP,
                              100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors("Rs1, Rs2");
  manager.AddQVectors("Full");
  manager.AddQVectors("TracksMdc");

  std::vector<std::string> QRnd{"Rs1", "Rs2"};
  std::vector<std::string> Full{"Full"};
  std::vector<std::string> u_vector{"TracksMdc"};
  std::vector<std::string> method{"Sp", "Ep"};

  auto Q1 = QRnd.at(0);
  auto Q2 = QRnd.at(1);
  std::string u = "TracksMdc";
  // First harmonic
  manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_Ep", Q1 + ", " + Q2, QxQx);
  manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_XX_Ep",
                               {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
  manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_Ep", Q1 + ", " + Q2, QyQy);
  manager.SetRefQinCorrelation(Q1 + "_" + Q2 + "_YY_Ep",
                               {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

  manager.AddCorrelation(u + "_" + Q1 + "_XX_Ep", u + ", " + Q1, uxQx);
  manager.SetRefQinCorrelation(
      u + "_" + Q1 + "_XX_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  manager.AddCorrelation(u + "_" + Q1 + "_YY_Ep", u + ", " + Q1, uyQy);
  manager.SetRefQinCorrelation(
      u + "_" + Q1 + "_YY_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  manager.AddCorrelation(u + "_" + Q2 + "_XX_Ep", u + ", " + Q2, uxQx);
  manager.SetRefQinCorrelation(
      u + "_" + Q2 + "_XX_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  manager.AddCorrelation(u + "_" + Q2 + "_YY_Ep", u + ", " + Q2, uyQy);
  manager.SetRefQinCorrelation(
      u + "_" + Q2 + "_YY_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

  manager.AddCorrelation("TracksMdc_Full_XX_Ep", "TracksMdc,Full", uxQx);
  manager.SetRefQinCorrelation("TracksMdc_Full_XX_Ep",
                               {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  manager.AddCorrelation("TracksMdc_Full_YY_Ep", "TracksMdc,Full", uyQy);
  manager.SetRefQinCorrelation("TracksMdc_Full_YY_Ep",
                               {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  // Second harmonic
  manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_Ep",
                         u + ", " + Q1 + ", " + Q2, uxQxQx);
  manager.SetRefQinCorrelation(
      u + "_" + Q1 + "_" + Q2 + "_XXX_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

  manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_Ep",
                         u + ", " + Q1 + ", " + Q2, uxQyQy);
  manager.SetRefQinCorrelation(
      u + "_" + Q1 + "_" + Q2 + "_XYY_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

  manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_Ep",
                         u + ", " + Q1 + ", " + Q2, uyQxQy);
  manager.SetRefQinCorrelation(
      u + "_" + Q1 + "_" + Q2 + "_YXY_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

  manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_Ep",
                         u + ", " + Q1 + ", " + Q2, uyQyQx);
  manager.SetRefQinCorrelation(
      u + "_" + Q1 + "_" + Q2 + "_YYX_Ep",
      {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

  manager.AddCorrelation("TracksMdc2_Full2_XX_Ep", "TracksMdc,Full", u2xQ2x);
  manager.SetRefQinCorrelation("TracksMdc2_Full2_XX_Ep",
                               {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

  manager.AddCorrelation("TracksMdc2_Full2_YY_Ep", "TracksMdc,Full", u2yQ2y);
  manager.SetRefQinCorrelation("TracksMdc2_Full2_YY_Ep",
                               {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
}

void CorrelationTask::ConfigureFw3s(Qn::CorrelationManager &manager) {
  auto QxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1);
  };
  auto QyQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1);
  };
  auto uxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1) / qn.at(0).mag(1);
  };
  auto uyQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1) / qn.at(0).mag(1);
  };
  // --------------------------------- Second Harmonic
  // ---------------------------------
  auto uxQxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  auto uxQyQy = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  auto uyQxQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).y(1) / qn.at(0).mag(2);
  };
  auto uyQyQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).x(1) / qn.at(0).mag(2);
  };
  // --------------------------------- Third Harmonic
  // ---------------------------------
  /*
  auto uxQxQxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(3) * qn.at(1).x(1) * qn.at(2).x(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uxQyQyQx = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).y(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uxQyQxQy = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).y(1) * qn.at(2).x(1) * qn.at(3).y(1) /
           qn.at(0).mag(3);
  };
  auto uxQxQyQy = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(3) * qn.at(1).x(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uyQyQxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).y(1) * qn.at(2).x(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uyQxQyQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).x(1) * qn.at(2).y(1) * qn.at(3).x(1) /
           qn.at(0).mag(3);
  };
  auto uyQxQxQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(3) * qn.at(1).x(1) * qn.at(2).x(1) * qn.at(3).y(1) /
           qn.at(0).mag(3);
  };
  auto uyQyQyQy = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).y(3) * qn.at(1).y(1) * qn.at(2).y(1) * qn.at(3).y(1) /
           qn.at(0).mag(3);
  };
*/
  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 8, 0, 40});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP,
                              100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors(" Fw1, Fw2, Fw3, MdcQ");
  manager.AddQVectors("TracksMdc");

  std::vector<std::string> Q3Se{"MdcQ", "Fw1", "Fw2", "Fw3"};
  std::string u{"TracksMdc"};
  /**
   * Correlations of all detectors vs PsiRP
   */
  for (size_t i = 0; i < Q3Se.size(); i++) {
    auto Q1 = Q3Se.at(i);
    for (size_t j = i + 1; j < Q3Se.size(); j++) {
      auto Q2 = Q3Se.at(j);
      manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_Sp", Q1 + ", " + Q2, QxQx);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_XX_Sp",
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

      manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_Sp", Q1 + ", " + Q2, QyQy);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_YY_Sp",
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    }
    manager.AddCorrelation(u + "_" + Q1 + "_XX_Sp", u + ", " + Q1, uxQx);
    manager.SetRefQinCorrelation(
        u + "_" + Q1 + "_XX_Sp",
        {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

    manager.AddCorrelation(u + "_" + Q1 + "_YY_Sp", u + ", " + Q1, uyQy);
    manager.SetRefQinCorrelation(
        u + "_" + Q1 + "_YY_Sp",
        {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  }
  manager.AddCorrelation("MdcQ_MdcQ_XX_Sp", "MdcQ, MdcQ", QxQx);
  manager.SetRefQinCorrelation("MdcQ_MdcQ_XX_Sp",
                               {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

  manager.AddCorrelation("MdcQ_MdcQ_YY_Sp", "MdcQ, MdcQ", QyQy);
  manager.SetRefQinCorrelation("MdcQ_MdcQ_YY_Sp",
                               {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
  Q3Se = {"Fw1", "Fw2", "Fw3"};
  for (size_t i = 0; i < Q3Se.size(); i++) {
    auto Q1 = Q3Se.at(i);
    // Second harmonic
    for (size_t j = i + 1; j < Q3Se.size(); j++) {
      auto Q2 = Q3Se.at(j);
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_Sp",
                             u + ", " + Q1 + ", " + Q2, uxQxQx);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XXX_Sp",
                                   {Qn::Weight::OBSERVABLE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_Sp",
                             u + ", " + Q1 + ", " + Q2, uxQyQy);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_XYY_Sp",
                                   {Qn::Weight::OBSERVABLE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});

      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_Sp",
                             u + ", " + Q1 + ", " + Q2, uyQyQx);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YYX_Sp",
                                   {Qn::Weight::OBSERVABLE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});
      manager.AddCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_Sp",
                             u + ", " + Q1 + ", " + Q2, uyQxQy);
      manager.SetRefQinCorrelation(u + "_" + Q1 + "_" + Q2 + "_YXY_Sp",
                                   {Qn::Weight::OBSERVABLE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});
      /*
              for (size_t k = i + 2; k < Q3Se.size(); k++) {
                auto Q3 = Q3Se.at(k);
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XXXX_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uxQxQxQx);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XXXX_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XYYX_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uxQyQyQx);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XYYX_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XYXY_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uxQyQxQy);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XYXY_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XXYY_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uxQxQyQy);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_XXYY_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YYXX_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uyQyQxQx);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YYXX_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YXYX_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uyQxQyQx);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YXYX_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YXXY_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uyQxQxQy);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YXXY_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
                manager.AddCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YYYY_Sp",
                    u + ", " + Q1 + ", " + Q1 + ", " + Q3, uyQyQyQy);
                manager.SetRefQinCorrelation(
                    u + "_" + Q1 + "_" + Q1 + "_" + Q3 + "_YYYY_Sp",
                    {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE,
                     Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
              }
              */
    }

  }
}

void CorrelationTask::ConfigureFw3x(Qn::CorrelationManager &manager) {
  auto QxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(1) * qn.at(1).x(1);
  };
  auto QyQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(1) * qn.at(1).y(1);
  };

  // ----------------- Second Harmonic -----------------
  auto QxQxQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(1) * qn.at(2).x(1);
  };
  auto QxQyQy = [](const std::vector<Qn::QVector> &qn) {
    return -qn.at(0).x(2) * qn.at(1).y(1) * qn.at(2).y(1);
  };
  auto QyQxQy = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).x(1) * qn.at(2).y(1);
  };
  auto QyQyQx = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(1) * qn.at(2).x(1);
  };

  auto u2xQ2x = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).x(2) * qn.at(1).x(2) / qn.at(0).mag(2);
  };
  auto u2yQ2y = [](const std::vector<Qn::QVector> &qn) {
    return qn.at(0).y(2) * qn.at(1).y(2) / qn.at(0).mag(2);
  };

  manager.SetOutputFile("Correlations.root");
  manager.AddEventVariable({"Centrality", 8, 0, 40});
  manager.ConfigureResampling(Qn::Sampler::Method::BOOTSTRAP,
                              100); // BOOTSTRAP, SUBSAMPLING

  manager.AddQVectors(" Fw1, Fw2, Fw3, MdcQ");
  manager.AddQVectors("TracksMdc");

  std::vector<std::string> Q3Se{"Fw1", "Fw2", "Fw3"};
  std::string u{"TracksMdc"};
  /**
   * Correlations of all detectors vs PsiRP
   */
  for (size_t i = 0; i < Q3Se.size(); i++) {
    auto Q1 = Q3Se.at(i);
    for (size_t j = i + 1; j < Q3Se.size(); j++) {
      auto Q2 = Q3Se.at(j);
      manager.AddCorrelation(Q1 + "_" + Q2 + "_XX_Sp", Q1 + ", " + Q2, QxQx);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_XX_Sp",
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});

      manager.AddCorrelation(Q1 + "_" + Q2 + "_YY_Sp", Q1 + ", " + Q2, QyQy);
      manager.SetRefQinCorrelation(
          Q1 + "_" + Q2 + "_YY_Sp",
          {Qn::Weight::REFERENCE, Qn::Weight::REFERENCE});
    }
  }

  manager.AddCorrelation("TracksMdc2_MdcQ2_XX_Sp", "TracksMdc, MdcQ", u2xQ2x);
  manager.SetRefQinCorrelation("TracksMdc2_MdcQ2_XX_Sp",
                               {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
  manager.AddCorrelation("TracksMdc2_MdcQ2_YY_Sp", "TracksMdc, MdcQ", u2yQ2y);
  manager.SetRefQinCorrelation("TracksMdc2_MdcQ2_YY_Sp",
                               {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});

  Q3Se = {"Fw1", "Fw2", "Fw3"};
  std::string Q{ "MdcQ" };
  for (size_t i = 0; i < Q3Se.size(); i++) {
    auto Q1 = Q3Se.at(i);
    // Second harmonic
    for (size_t j = i + 1; j < Q3Se.size(); j++) {
      auto Q2 = Q3Se.at(j);
      manager.AddCorrelation(Q + "_" + Q1 + "_" + Q2 + "_XXX_Sp",
                             Q + ", " + Q1 + ", " + Q2, QxQxQx);
      manager.SetRefQinCorrelation(Q + "_" + Q1 + "_" + Q2 + "_XXX_Sp",
                                   {Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q + "_" + Q1 + "_" + Q2 + "_XYY_Sp",
                             Q + ", " + Q1 + ", " + Q2, QxQyQy);
      manager.SetRefQinCorrelation(Q + "_" + Q1 + "_" + Q2 + "_XYY_Sp",
                                   {Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});

      manager.AddCorrelation(Q + "_" + Q1 + "_" + Q2 + "_YYX_Sp",
                             Q + ", " + Q1 + ", " + Q2, QyQyQx);
      manager.SetRefQinCorrelation(Q + "_" + Q1 + "_" + Q2 + "_YYX_Sp",
                                   {Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});
      manager.AddCorrelation(Q + "_" + Q1 + "_" + Q2 + "_YXY_Sp",
                             Q + ", " + Q1 + ", " + Q2, QyQxQy);
      manager.SetRefQinCorrelation(Q + "_" + Q1 + "_" + Q2 + "_YXY_Sp",
                                   {Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE,
                                    Qn::Weight::REFERENCE});
    }
  }
}

void CorrelationTask::Run(std::string method) {
  int nEvents = in_tree_->GetEntries();
  Qn::CorrelationManager fManager(reader_, nEvents);
  in_tree_->LoadTree(0); // prevents weird TTree errors
  if (method == "FW3S")
    this->ConfigureFw3s(fManager);
  if (method == "FW3X")
    this->ConfigureFw3x(fManager);
  if (method == "RND")
    this->ConfigureRnd(fManager);
  int events = 1;
  reader_->SetEntry(0); //  first entry needed to access configuration of
                        //  DataContainers in the input file
  fManager.Initialize();
  in_tree_->LoadTree(0); // prevents weird TTree errors
  std::cout << "Processing..." << std::endl;
  int eventsteps = nEvents / 100;
  events = 0;
  do {
    events++;
    fManager.Process();
    if (eventsteps > 1 && events % eventsteps == 0) {
      float progress = events / (float)nEvents;
      // ProgressBar(progress);
    }
  } while (reader_->Next());
  fManager.Finalize();
  std::cout << std::endl
            << "number of analyzed events: " << events << std::endl;
}

std::unique_ptr<TTree> CorrelationTask::MakeChain(std::string fileList,
                                                  std::string treename) {
  auto file = new TFile(fileList.c_str());
  std::unique_ptr<TTree> chain((TTree *)file->Get("tree"));
  //   chain->AddFile(fileList.c_str());
  return chain;
}

void CorrelationTask::ProgressBar(float progress) {
  int barWidth = 100;
  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();
}
