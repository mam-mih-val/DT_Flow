#ifndef QNCORRECTIONS_CORRECTIONONINPUTDATA_H
#define QNCORRECTIONS_CORRECTIONONINPUTDATA_H

/***************************************************************************
 * Package:       FlowVectorCorrections                                    *
 * Authors:       Jaap Onderwaater, GSI, jacobus.onderwaater@cern.ch       *
 *                Ilya Selyuzhenkov, GSI, ilya.selyuzhenkov@gmail.com      *
 *                Víctor González, UCM, victor.gonzalez@cern.ch            *
 *                Contributors are mentioned in the code where appropriate.*
 * Development:   2012-2016                                                *
 * See cxx source for GPL licence et. al.                                  *
 ***************************************************************************/

/// \file QnCorrectionsCorrectionOnInputData.h
/// \brief Correction steps on input data support within Q vector correction framework
///

#include <TNamed.h>
#include <TList.h>

#include "CorrectionStepBase.h"

namespace Qn {

/// \class QnCorrectionsCorrectionOnInputData
/// \brief Base class for correction steps applied to input data
///
/// \author Jaap Onderwaater <jacobus.onderwaater@cern.ch>, GSI
/// \author Ilya Selyuzhenkov <ilya.selyuzhenkov@gmail.com>, GSI
/// \author Víctor González <victor.gonzalez@cern.ch>, UCM
/// \date Feb 05, 2016

class CorrectionOnInputData : public CorrectionStepBase {
 public:
  friend class DetectorConfigurationChannels;
  CorrectionOnInputData();
  CorrectionOnInputData(const char *name, const char *key);
  virtual ~CorrectionOnInputData();

  /// Informs when the detector configuration has been attached to the framework manager
  /// Basically this allows interaction between the different framework sections at configuration time
  /// Pure virtual function
  virtual void AttachedToFrameworkManager() = 0;
  /// Attaches the needed input information to the correction step
  ///
  /// Pure virtual function
  /// \param list list where the inputs should be found
  /// \return kTRUE if everything went OK
  virtual Bool_t AttachInput(TList *list) = 0;
  /// Perform after calibration histograms attach actions
  /// It is used to inform the different correction step that
  /// all conditions for running the network are in place so
  /// it is time to check if their requirements are satisfied
  ///
  /// Does nothing for the time being
  virtual void AfterInputsAttachActions() {}
  /// Asks for support data structures creation
  ///
  /// Pure virtual function
  virtual void CreateSupportDataStructures() = 0;
  /// Asks for support histograms creation
  ///
  /// Pure virtual function
  /// \param list list where the histograms should be incorporated for its persistence
  /// \return kTRUE if everything went OK
  virtual Bool_t CreateSupportHistograms(TList *list) = 0;
  /// Processes the correction step
  ///
  /// Pure virtual function
  /// \return kTRUE if everything went OK
  virtual Bool_t ProcessCorrections(const double *variableContainer) = 0;
  /// Processes the correction step data collection
  ///
  /// Pure virtual function
  /// \return kTRUE if everything went OK
  virtual Bool_t ProcessDataCollection(const double *variableContainer) = 0;
  /// Include the new corrected Qn vector into the passed list
  ///
  /// Does nothing. Not applicable for corrections on input data
  /// \param list list where the corrected Qn vector should be added
  virtual void IncludeCorrectedQnVector(TList *list) { (void) list; }
  /// Clean the correction to accept a new event
  /// Pure virtual function
  virtual void ClearCorrectionStep() = 0;
  /// Reports if the correction step is being applied
  /// \return FALSE, input data correction step dont make use of this service, yet
  virtual Bool_t IsBeingApplied() const { return kFALSE; }
  /// Report on correction usage
  /// Pure virtual function
  /// Correction step should incorporate its name in calibration
  /// list if it is producing information calibration in the ongoing
  /// step and in the apply list if it is applying correction in
  /// the ongoing step.
  /// \param calibrationList list containing the correction steps producing calibration information
  /// \param applyList list containing the correction steps applying corrections
  /// \return kTRUE if the correction step is being applied
  virtual Bool_t ReportUsage(TList *calibrationList, TList *applyList) = 0;
/// \cond CLASSIMP
 ClassDef(CorrectionOnInputData, 1);
/// \endcond
};
}

#endif // QNCORRECTIONS_CORRECTIONONINPUTDATA_H
