//
// Created by mikhail on 5/6/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRELATION_TASK_FW3X_H_
#define FLOW_SRC_PROCESSING_CORRELATION_TASK_FW3X_H_
#include "correlation_task.h"
class CorrelationTaskFw3x : public CorrelationTask {
  void Configure(Qn::CorrelationManager &manager) override{
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
};

#endif // FLOW_SRC_PROCESSING_CORRELATION_TASK_FW3X_H_
