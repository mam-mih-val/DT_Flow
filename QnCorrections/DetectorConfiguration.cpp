/**************************************************************************************************
 *                                                                                                *
 * Package:       FlowVectorCorrections                                                           *
 * Authors:       Jaap Onderwaater, GSI, jacobus.onderwaater@cern.ch                              *
 *                Ilya Selyuzhenkov, GSI, ilya.selyuzhenkov@gmail.com                             *
 *                Víctor González, UCM, victor.gonzalez@cern.ch                                   *
 *                Contributors are mentioned in the code where appropriate.                       *
 * Development:   2012-2016                                                                       *
 *                                                                                                *
 * This file is part of FlowVectorCorrections, a software package that corrects Q-vector          *
 * measurements for effects of nonuniform detector acceptance. The corrections in this package    *
 * are based on publication:                                                                      *
 *                                                                                                *
 *  [1] "Effects of non-uniform acceptance in anisotropic flow measurements"                      *
 *  Ilya Selyuzhenkov and Sergei Voloshin                                                         *
 *  Phys. Rev. C 77, 034904 (2008)                                                                *
 *                                                                                                *
 * The procedure proposed in [1] is extended with the following steps:                            *
 * (*) alignment correction between subevents                                                     *
 * (*) possibility to extract the twist and rescaling corrections                                 *
 *      for the case of three detector subevents                                                  *
 *      (currently limited to the case of two “hit-only” and one “tracking” detectors)            *
 * (*) (optional) channel equalization                                                            *
 * (*) flow vector width equalization                                                             *
 *                                                                                                *
 * FlowVectorCorrections is distributed under the terms of the GNU General Public License (GPL)   *
 * (https://en.wikipedia.org/wiki/GNU_General_Public_License)                                     *
 * either version 3 of the License, or (at your option) any later version.                        *
 *                                                                                                *
 **************************************************************************************************/

/// \file QnCorrectionsDetectorConfigurationBase.cxx
/// \brief Implementation of the base detector configuration class within Q vector correction framework

#include "DetectorConfiguration.h"
#include "CorrectionLog.h"

/// \cond CLASSIMP
ClassImp(Qn::DetectorConfiguration);
/// \endcond
namespace Qn {
const char *DetectorConfiguration::szPlainQnVectorName = "plain";

/// Default constructor
DetectorConfiguration::DetectorConfiguration() : TNamed(),
                                                 fPlainQnVector(), fPlainQ2nVector(),
                                                 fCorrectedQnVector(), fCorrectedQ2nVector(),
                                                 fTempQnVector(), fTempQ2nVector(),
                                                 fQnVectorCorrections() {
  fDetector = NULL;
  fCorrectionsManager = NULL;
  fCuts = NULL;
  fDataVectorBank = NULL;
  fQnNormalizationMethod = CorrectionQnVector::Normalization::NONE;
  fEventClassVariables = NULL;
  fPlainQ2nVector.SetHarmonicMultiplier(2);
  fCorrectedQ2nVector.SetHarmonicMultiplier(2);
  fTempQ2nVector.SetHarmonicMultiplier(2);
}

/// Normal constructor
/// \param name the name of the detector configuration
/// \param eventClassesVariables the set of event classes variables
/// \param nNoOfHarmonics the number of harmonics that must be handled
/// \param harmonicMap an optional ordered array with the harmonic numbers
DetectorConfiguration::DetectorConfiguration(const char *name,
                                             EventClassVariablesSet *eventClassesVariables,
                                             Int_t nNoOfHarmonics,
                                             Int_t *harmonicMap) :
    TNamed(name, name),
    fPlainQnVector(szPlainQnVectorName, nNoOfHarmonics, harmonicMap),
    fPlainQ2nVector(Form("%s2n", szPlainQnVectorName), nNoOfHarmonics, harmonicMap),
    fCorrectedQnVector(szPlainQnVectorName, nNoOfHarmonics, harmonicMap),
    fCorrectedQ2nVector(Form("%s2n", szPlainQnVectorName), nNoOfHarmonics, harmonicMap),
    fTempQnVector("temp", nNoOfHarmonics, harmonicMap),
    fTempQ2nVector("temp2n", nNoOfHarmonics, harmonicMap),
    fQnVectorCorrections() {

  fDetector = NULL;
  fCorrectionsManager = NULL;
  fCuts = NULL;
  fDataVectorBank = NULL;
  fQnNormalizationMethod = CorrectionQnVector::Normalization::NONE;
  fEventClassVariables = eventClassesVariables;
  fPlainQ2nVector.SetHarmonicMultiplier(2);
  fCorrectedQ2nVector.SetHarmonicMultiplier(2);
  fTempQ2nVector.SetHarmonicMultiplier(2);
}

/// Default destructor
/// Releases the memory which was taken or passed
DetectorConfiguration::~DetectorConfiguration() {
  if (fDataVectorBank!=NULL) {
    delete fDataVectorBank;
  }
  if (fCuts!=NULL) {
    delete fCuts;
  }
}

/// Incorporates the passed correction to the set of Q vector corrections
/// \param correctionOnQn the correction to add
void DetectorConfiguration::AddCorrectionOnQnVector(CorrectionOnQvector *correctionOnQn) {
  correctionOnQn->SetConfigurationOwner(this);
  fQnVectorCorrections.AddCorrection(correctionOnQn);
}

/// Incorporates the passed correction to the set of input data corrections
///
/// Interface declaration function.
/// Default behavior. Base class should not be instantiated.
/// Run time error to support debugging.
///
/// \param correctionOnInputData the correction to add
void DetectorConfiguration::AddCorrectionOnInputData(CorrectionOnInputData *correctionOnInputData) {
  (void) correctionOnInputData;
  QnCorrectionsFatal(Form("You have reached base member %s. This means you have instantiated a base class or\n"
                          "you are using a non channelized detector configuration to calibrate input data. FIX IT, PLEASE.",
                          "QnCorrectionsDetectorConfigurationBase::AddCorrectionOnInputData()"));
}

/// Get the corrected Qn vector from the step previous to the one given
/// If not previous step the plain Qn vector is returned.
/// The user is not able to modify it.
/// \param correctionOnQn the correction to find its predecessor corrected Qn vector
/// \return the corrected Qn vector from the correction step predecessor or the plain Qn vector
const CorrectionQnVector *DetectorConfiguration::GetPreviousCorrectedQnVector(CorrectionOnQvector *correctionOnQn) const {
  if (fQnVectorCorrections.GetPrevious(correctionOnQn)!=NULL)
    return fQnVectorCorrections.GetPrevious(correctionOnQn)->GetCorrectedQnVector();
  else
    return &fPlainQnVector;
}

/// Check if a concrete correction step is bein applied on this detector configuration
/// It is not enough having the correction step configured or collecting data. To
/// get an affirmative answer the correction step must be being applied.
/// Transfers the request to the set of Qn vector corrections.
/// \param step the name of the correction step
/// \return TRUE if the correction step is being applied
Bool_t DetectorConfiguration::IsCorrectionStepBeingApplied(const char *step) const {

  return fQnVectorCorrections.IsCorrectionStepBeingApplied(step);
}

/// Activate the processing for the passed harmonic
/// \param harmonic the desired harmonic number to activate
void DetectorConfiguration::ActivateHarmonic(Int_t harmonic) {
  fPlainQnVector.ActivateHarmonic(harmonic);
  fCorrectedQnVector.ActivateHarmonic(harmonic);
  fPlainQ2nVector.ActivateHarmonic(harmonic);
  fCorrectedQ2nVector.ActivateHarmonic(harmonic);
  fTempQnVector.ActivateHarmonic(harmonic);
  fTempQ2nVector.ActivateHarmonic(harmonic);
}

}
