//
// Created by Lukas Kreis on 03.08.17.
//
#ifndef FLOW_CORRECTIONINTERFACE_H
#define FLOW_CORRECTIONINTERFACE_H

#include <QnCorrections/QnCorrectionsManager.h>
#include "DataContainer.h"
#include "DetectorGenerator.h"
#include "EventInfo.h"
#include "DataInterface.h"

namespace Qn {
/**
 * Namespace used for internal calculations
 */
namespace Internal {

inline void ClearDataInMap(DetectorMap &map) {
  for (auto &pair : map) {
    std::get<1>(pair.second)->ClearData();
  }
}

inline void ClearDataInMap(std::map<int, std::unique_ptr<Qn::DataContainerQVector>> &map) {
  for (const auto &pair : map) {
    pair.second->ClearData();
  }
}

/**
 * Creates DataContainerQn containing \f$Q_n\f$ Vectors from a DataContainerDataVector.
 * @param map Detector map containing raw data \f$(\phi,w)\f$ with the specified binning.
 * @return DataContainer with the same configuration as the raw data container.
 */
std::map<int, std::unique_ptr<Qn::DataContainerQVector>> MakeQVectorDataContainer(const DetectorMap &map);

/**
 * Fills data to framework for the correction
 * @param[in,out] manager Corrections manager used for the correction of the \f$Q_n\f$ vectors
 * @param map DetectorMap containing the raw data vectors \f$(\phi,w)\f$.
 */
void FillDataToFramework(QnCorrectionsManager &manager, DetectorMap &map);

/**
 * Creates Detectors corresponding to the detectors saved in the DetectorMap.
 * @param[in,out] manager Corrections manager used for the correction of the \f$Q_n\f$ vectors.
 * @param map DetectorMap containing the raw data vectors \f$(\phi,w)\f$.
 * @param set Event variable set to be used for the correction of the detectors.
 */
void AddDetectorsToFramework(QnCorrectionsManager &manager,
                             DetectorMap &map,
                             QnCorrectionsEventClassVariablesSet &set);

/**
 * Gets corrected \f$Q_n\f$ vectors from the QnCorrectionsFramework.
 * @param[in,out] manager Corrections manager used for the correction of the \f$Q_n\f$ vectors.
 * @param[out] map Data container in which the \f$Q_n\f$ vectors are inserted into.
 * @param step correction step of the requested \f$Q_n\f$ vector.
 */
void GetQnFromFramework(QnCorrectionsManager &manager, std::map<int, std::unique_ptr<Qn::DataContainerQVector>> &map, std::string step = "latest");

void SaveToTree(TTree &tree, std::map<int, std::unique_ptr<Qn::DataContainerQVector>> &map);

void SaveToTree(TTree &tree, std::map<int, std::unique_ptr<Qn::DataContainerDataVector>> &map);

void SaveToTree(TTree &tree, std::unique_ptr<Qn::EventInfoF> &eventinfo);
}

namespace Internal {
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
}

}

#endif //FLOW_CORRECTIONINTERFACE_H
