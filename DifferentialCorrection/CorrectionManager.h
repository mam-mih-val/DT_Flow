//
// Created by Lukas Kreis on 16.01.18.
//

#ifndef FLOW_CORRECTIONMANAGER_H
#define FLOW_CORRECTIONMANAGER_H

#include <string>
#include <map>
#include <utility>
#include <QnCorrections/QnCorrectionsManager.h>
#include "Detector.h"
#include "VariableManager.h"
#include "Base/Axis.h"
#include "EventInfo.h"
#include "DifferentialCorrection/Interface/DataFiller.h"
namespace Qn {
class CorrectionManager {
 public:

  CorrectionManager() : event_variables_(new Qn::EventInfoF()) {}

  void AddVariable(const std::string &name, const int number) { var_manager_.AddVariable(name, number); }

  void AddCorrectionAxis(const Qn::Axis &variable) { qncorrections_axis_.push_back(variable); }

  void SetEventVariable(const std::string &name) {
    event_variables_->AddVariable(name);
  }

  void SetEventCut() {};

  void AddDetector(const std::string &name,
                   DetectorType type,
                   const std::vector<Qn::Axis> &axes,
                   int nchannels = 0) {
    std::vector<int> enums;
    enums.reserve(axes.size());
    for (const auto &axis : axes) {
      enums.push_back(var_manager_.FindNum(axis.Name()));
    }
    Detector detector(type,axes,enums,nchannels);
    detectors_.insert(std::make_pair(name, std::move(detector)));
  }

  void SetCorrectionSteps(const std::string &name,
                          std::function<void(QnCorrectionsDetectorConfigurationBase *config)> config) {
    detectors_.at(name).SetConfig(std::move(config));
  }

  void CreateDetectors() {
    int nbinsrunning = 0;
    for (auto &pair : detectors_) {
      auto &detector = pair.second;
      for (int ibin = 0; ibin < detector.GetDataContainer()->size(); ++ibin) {
        auto globalid = nbinsrunning + ibin;
        auto frameworkdetector = detector.GenerateDetector(pair.first, globalid, ibin, qncorrections_varset_);
        qncorrections_manager_.AddDetector(frameworkdetector);
      }
      nbinsrunning += detector.GetDataContainer()->size();
    }
  }

  void FillDataToFramework() {
    int nbinsrunning = 0;
    for (auto &pair : detectors_) {
      auto &detector = pair.second.GetDataContainer();
      int ibin = 0;
      for (const std::vector<DataVector> &bin : *detector) {
        auto detectorid = nbinsrunning + ibin;
        ++ibin;
        int idata = 0;
        for (const auto &data : bin) {
          qncorrections_manager_.AddDataVector(detectorid, data.phi, data.weight, idata);
          ++idata;
        }
      }
      nbinsrunning += detector->size();
    }
  }

  void GetQnFromFramework(const std::string &step) {
    int nbinsrunning = 0;
    for (auto &pair : detectors_) {
      auto &detector = pair.second.GetQnDataContainer();
      auto ibin = 0;
      for (auto &bin : *detector) {
        auto name = (pair.first + std::to_string(ibin)).data();
        ++ibin;
        auto vector = qncorrections_manager_.GetDetectorQnVector(name, step.c_str(), step.c_str());
        if (!vector) continue;
        auto method =
            qncorrections_manager_.FindDetector(name)->FindDetectorConfiguration(name)->GetQVectorNormalizationMethod();
        QVector temp(GetNormalization(method), *vector);
        bin = temp;
      }
      nbinsrunning += detector->size();
    }
  }

  void SaveQVectorsToTree(TTree &tree) {
    for (auto &pair : detectors_) {
      tree.Branch(pair.first.data(), pair.second.GetQnDataContainer().get());
    }
  }

  void SaveEventVariablesToTree(TTree &tree) {
    event_variables_->SetToTree(tree);
  }

  void FillDataToFramework(Qn::Differential::Interface::DataFiller filler) {
    for (auto &detector : detectors_) {
      filler.Fill(detector.first, detector.second);
    }
  }

  void SaveCorrectionHistograms() {
   qncorrections_manager_.GetOutputHistogramsList()->Write(qncorrections_manager_.GetOutputHistogramsList()->GetName(), TObject::kSingleKey);
   qncorrections_manager_.GetQAHistogramsList()->Write(qncorrections_manager_.GetQAHistogramsList()->GetName(), TObject::kSingleKey);
  }

  float *GetVariableContainer() { return qncorrections_manager_.GetDataContainer(); }

  void Initialize(std::shared_ptr<TFile> &in_calibration_file_) {
    CalculateCorrectionAxis();
    CreateDetectors();
    qncorrections_manager_.SetCalibrationHistogramsList(in_calibration_file_.get());
    qncorrections_manager_.SetShouldFillQAHistograms();
    qncorrections_manager_.SetShouldFillOutputHistograms();
    qncorrections_manager_.InitializeQnCorrectionsFramework();
    qncorrections_manager_.SetCurrentProcessListName("correction");
  }

  void Process() {
    FillDataToFramework();
    for (auto &event_var : *event_variables_) {
      event_var.second.SetValue(qncorrections_manager_.GetDataContainer()[var_manager_.FindNum(event_var.first)]);
    }
    qncorrections_manager_.ProcessEvent();
    GetQnFromFramework("latest");
  }

  void Finalize() { qncorrections_manager_.FinalizeQnCorrectionsFramework(); }

  void Reset() {
    qncorrections_manager_.ClearEvent();
    for (auto &det : detectors_) {
      det.second.ClearData();
    }
    event_variables_->Reset();
  }

 private:
  int kMaxCorrectionArrayLength = 200;

  void CalculateCorrectionAxis() {
    qncorrections_varset_ = new QnCorrectionsEventClassVariablesSet(qncorrections_axis_.size());
    for (const auto &axis : qncorrections_axis_) {
      double axisbins[kMaxCorrectionArrayLength];
      auto nbins = axis.size();
      for (int ibin = 0; ibin < nbins+1; ++ibin) {
        axisbins[ibin] = *(axis.begin() + ibin);
      }
      auto variable =
          new QnCorrectionsEventClassVariable(var_manager_.FindNum(axis.Name()), axis.Name().data(), nbins, axisbins);
      qncorrections_varset_->Add(variable);
    }
  }
/**
 * Get Normalization from Qn::CorrectionsQnVector framework
 * @param method normalization method.
 * @return corresponding correlation.
 */
inline Qn::QVector::Normalization GetNormalization(QnCorrectionsQnVector::QnVectorNormalizationMethod method) {
  if (method == QnCorrectionsQnVector::QnVectorNormalizationMethod::QVNORM_noCalibration)
    return Qn::QVector::Normalization::NOCALIB;
  if (method == QnCorrectionsQnVector::QnVectorNormalizationMethod::QVNORM_QoverM)
    return Qn::QVector::Normalization::QOVERM;
  if (method == QnCorrectionsQnVector::QnVectorNormalizationMethod::QVNORM_QoverSqrtM)
    return Qn::QVector::Normalization::QOVERSQRTM;
  if (method == QnCorrectionsQnVector::QnVectorNormalizationMethod::QVNORM_QoverQlength)
    return Qn::QVector::Normalization::QOVERNORMQ;
  return Qn::QVector::Normalization::NOCALIB;
}

  QnCorrectionsEventClassVariablesSet *qncorrections_varset_;
  std::unique_ptr<Qn::EventInfoF> event_variables_;
  std::vector<Qn::Axis> qncorrections_axis_;
  QnCorrectionsManager qncorrections_manager_;
  VariableManager var_manager_;
  std::map<std::string, Detector> detectors_;
};
}

#endif //FLOW_CORRECTIONMANAGER_H
