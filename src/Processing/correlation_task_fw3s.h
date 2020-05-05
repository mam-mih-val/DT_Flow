//
// Created by mikhail on 5/5/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRELATION_TASK_FW3S_H_
#define FLOW_SRC_PROCESSING_CORRELATION_TASK_FW3S_H_

#include "correlation_task.h"
class CorrelationTaskFw3s : public CorrelationTask {
  void Configure(Qn::CorrelationManager &manager) override {
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
};

#endif // FLOW_SRC_PROCESSING_CORRELATION_TASK_FW3S_H_
