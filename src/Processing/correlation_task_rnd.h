//
// Created by mikhail on 5/6/20.
//

#ifndef FLOW_SRC_PROCESSING_CORRELATION_TASK_RND_H_
#define FLOW_SRC_PROCESSING_CORRELATION_TASK_RND_H_
#include "correlation_task.h"

class CorrelationTaskRnd : public CorrelationTask {
  void Configure( Qn::CorrelationManager &manager ) override {
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
    auto uxQy = [](const std::vector<Qn::QVector> &qn) {
      return qn.at(0).x(1) * qn.at(1).y(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
    };
    auto uyQx = [](const std::vector<Qn::QVector> &qn) {
      return qn.at(0).y(1) * qn.at(1).x(1) / (qn.at(0).mag(1) * qn.at(1).mag(1));
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
    manager.AddCorrelation("TracksMdc_Full_XY_Ep", "TracksMdc,Full", uxQy);
    manager.SetRefQinCorrelation("TracksMdc_Full_XX_Ep",
                                 {Qn::Weight::OBSERVABLE, Qn::Weight::REFERENCE});
    manager.AddCorrelation("TracksMdc_Full_YX_Ep", "TracksMdc,Full", uyQx);
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
};

#endif // FLOW_SRC_PROCESSING_CORRELATION_TASK_RND_H_
