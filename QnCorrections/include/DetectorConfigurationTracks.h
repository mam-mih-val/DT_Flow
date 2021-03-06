#ifndef QNCORRECTIONS_DETECTORCONFTRACKS_H
#define QNCORRECTIONS_DETECTORCONFTRACKS_H

/***************************************************************************
 * Package:       FlowVectorCorrections                                    *
 * Authors:       Jaap Onderwaater, GSI, jacobus.onderwaater@cern.ch       *
 *                Ilya Selyuzhenkov, GSI, ilya.selyuzhenkov@gmail.com      *
 *                Víctor González, UCM, victor.gonzalez@cern.ch            *
 *                Contributors are mentioned in the code where appropriate.*
 * Development:   2012-2016                                                *
 * See cxx source for GPL licence et. al.                                  *
 ***************************************************************************/

/// \file QnCorrectionsDetectorConfigurationTracks.h
/// \brief Track detector configuration class for Q vector correction framework
///

#include "CorrectionDataVector.h"
#include "DetectorConfiguration.h"
#include "CorrectionLog.h"
namespace Qn {
class CorrectionProfileComponents;

/// \class QnCorrectionsDetectorConfigurationTracks
/// \brief Track detector configuration within Q vector correction framework
///
/// A track detector within the Q vector correction framework is defined
/// as one for which its data vectors only involve azimuthal angles and a
/// potential weight. Apart from that no other input data calibration is
/// available.
///
/// \author Jaap Onderwaater <jacobus.onderwaater@cern.ch>, GSI
/// \author Ilya Selyuzhenkov <ilya.selyuzhenkov@gmail.com>, GSI
/// \author Víctor González <victor.gonzalez@cern.ch>, UCM
/// \date Feb 08, 2016

class DetectorConfigurationTracks :
    public DetectorConfiguration {
 public:
  friend class CorrectionStepBase;
  friend class CorrectionDetector;
  DetectorConfigurationTracks();
  DetectorConfigurationTracks(const char *name,
                                           EventClassVariablesSet *eventClassesVariables,
                                           Int_t nNoOfHarmonics,
                                           Int_t *harmonicMap = NULL);
  virtual ~DetectorConfigurationTracks();

  /// Get if the detector configuration is own by a tracking detector
  /// \return TRUE, this is tracking detector configuration
  virtual Bool_t GetIsTrackingDetector() const { return kTRUE; }

  virtual void AttachCorrectionsManager(CorrectionCalculator *manager);

  virtual void CreateSupportDataStructures();
  virtual Bool_t CreateSupportHistograms(TList *list);
  virtual Bool_t CreateQAHistograms(TList *list);
  virtual Bool_t CreateNveQAHistograms(TList *list);
  virtual Bool_t AttachCorrectionInputs(TList *list);
  virtual void AfterInputsAttachActions();

  virtual Bool_t ProcessCorrections(const double *variableContainer);
  virtual Bool_t ProcessDataCollection(const double *variableContainer);
  virtual Bool_t AddDataVector(const double *variableContainer,
                               Double_t phi,
                               Double_t weight = 1.0,
                               Int_t channelId = -1);

  virtual void BuildQnVector();
  virtual void IncludeQnVectors(TList *list);
  virtual void FillOverallInputCorrectionStepList(TList *list) const;
  virtual void FillOverallQnVectorCorrectionStepList(TList *list) const;
  virtual void ReportOnCorrections(TList *steps, TList *calib, TList *apply) const;

  /// Checks if the current content of the variable bank applies to
  /// the detector configuration
  /// \param variableContainer pointer to the variable content bank
  /// \return kTRUE if the current content applies to the configuration
  virtual Bool_t IsSelected(const double *variableContainer) {
    return ((fCuts!=NULL) ? fCuts->IsSelected(variableContainer) : kTRUE);
  }
  /// wrong call for this class invoke base class behavior
  virtual Bool_t IsSelected(const double *variableContainer, Int_t nChannel) {
    (void) variableContainer;
    (void) nChannel;
    QnCorrectionsFatal(Form("You have reached base member %s. This means you have instantiated a base class or\n" \
    "you are using a channelized detector configuration but passing no channel number. FIX IT, PLEASE.",
                            "QnCorrectionsDetectorConfigurationBase::IsSelected()"));
    return kFALSE;
  }

  virtual void ClearConfiguration();

 private:
  /* QA section */
  void FillQAHistograms(const double *variableContainer);
  static const char
      *szQAQnAverageHistogramName; ///< name and title for plain Qn vector components average QA histograms
  CorrectionProfileComponents *fQAQnAverageHistogram; //!<! the plain average Qn components QA histogram

/// \cond CLASSIMP
 ClassDef(DetectorConfigurationTracks, 2);
/// \endcond
};

/// New data vector for the detector configuration.
/// A check is made to see if the current variable bank content passes
/// the associated cuts. If so, the data vector is stored.
/// \param variableContainer pointer to the variable content bank
/// \param phi azimuthal angle
/// \param weight the weight associated to the data vector. For track detector is usually one.
/// \param id the Id associated to the data vector. For track detector configurations could represent the track id.
/// \return kTRUE if the data vector was accepted and stored
inline Bool_t DetectorConfigurationTracks::AddDataVector(
    const double *variableContainer, Double_t phi, Double_t weight, Int_t id) {
  if (IsSelected(variableContainer)) {
    /// add the data vector to the bank
    new(fDataVectorBank->ConstructedAt(fDataVectorBank->GetEntriesFast()))
        CorrectionDataVector(id, phi, weight);
    return kTRUE;
  }
  return kFALSE;
}

/// Clean the configuration to accept a new event
///
/// Transfers the order to the Q vector correction steps and
/// cleans the own Q vector and the input data vector bank
/// for accepting the next event.
inline void DetectorConfigurationTracks::ClearConfiguration() {
  /* transfer the order to the Q vector corrections */
  for (Int_t ixCorrection = 0; ixCorrection < fQnVectorCorrections.GetEntries(); ixCorrection++) {
    fQnVectorCorrections.At(ixCorrection)->ClearCorrectionStep();
  }
  /* clean the own Q vectors */
  fPlainQnVector.Reset();
  fPlainQ2nVector.Reset();
  fCorrectedQnVector.Reset();
  fCorrectedQ2nVector.Reset();
  /* and now clear the the input data bank */
  fDataVectorBank->Clear("C");
}

/// Builds Qn vectors before Q vector corrections but
/// considering the chosen calibration method.
/// Remember, this configuration does not have a channelized
/// approach so, the built Q vectors are the ones to be used for
/// subsequent corrections.
inline void DetectorConfigurationTracks::BuildQnVector() {
  fTempQnVector.Reset();
  fTempQ2nVector.Reset();

  for (Int_t ixData = 0; ixData < fDataVectorBank->GetEntriesFast(); ixData++) {
    CorrectionDataVector *dataVector = static_cast<CorrectionDataVector *>(fDataVectorBank->At(ixData));
    fTempQnVector.Add(dataVector->Phi(), dataVector->Weight());
    fTempQ2nVector.Add(dataVector->Phi(), dataVector->Weight());
  }
  /* check the quality of the Qn vector */
  fTempQnVector.CheckQuality();
  fTempQ2nVector.CheckQuality();
  fTempQnVector.Normalize(fQnNormalizationMethod);
  fTempQ2nVector.Normalize(fQnNormalizationMethod);
  fPlainQnVector.Set(&fTempQnVector, kFALSE);
  fPlainQ2nVector.Set(&fTempQ2nVector, kFALSE);
  fCorrectedQnVector.Set(&fTempQnVector, kFALSE);
  fCorrectedQ2nVector.Set(&fTempQ2nVector, kFALSE);
}

/// Ask for processing corrections for the involved detector configuration
///
/// The request is transmitted to the Q vector correction steps.
/// The first not applied correction step breaks the loop and kFALSE is returned
/// \return kTRUE if all correction steps were applied
inline Bool_t DetectorConfigurationTracks::ProcessCorrections(const double *variableContainer) {
  /* first we build the Q vector with the chosen calibration */
  BuildQnVector();

  /* then we transfer the request to the Q vector correction steps */
  /* the loop is broken when a correction step has not been applied */
  for (Int_t ixCorrection = 0; ixCorrection < fQnVectorCorrections.GetEntries(); ixCorrection++) {
    if (fQnVectorCorrections.At(ixCorrection)->ProcessCorrections(variableContainer))
      continue;
    else
      return kFALSE;
  }
  /* all correction steps were applied */
  return kTRUE;
}

/// Ask for processing corrections data collection for the involved detector configuration
/// Fill own QA histogram information and then
/// the request is transmitted to the Q vector correction steps.
/// The first not applied correction step breaks the loop and kFALSE is returned
/// \return kTRUE if all correction steps were applied
inline Bool_t DetectorConfigurationTracks::ProcessDataCollection(const double *variableContainer) {

  /* fill QA information */
  FillQAHistograms(variableContainer);

  /* we transfer the request to the Q vector correction steps */
  /* the loop is broken when a correction step has not been applied */
  for (Int_t ixCorrection = 0; ixCorrection < fQnVectorCorrections.GetEntries(); ixCorrection++) {
    if (fQnVectorCorrections.At(ixCorrection)->ProcessDataCollection(variableContainer))
      continue;
    else
      return kFALSE;
  }
  /* all correction steps were applied */
  return kTRUE;
}
}
#endif // QNCORRECTIONS_DETECTORCONFTRACKS_H
