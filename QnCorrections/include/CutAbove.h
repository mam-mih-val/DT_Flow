#ifndef QNCORRECTIONS_CUT_ABOVE_H
#define QNCORRECTIONS_CUT_ABOVE_H

/***************************************************************************
 * Package:       FlowVectorCorrections                                    *
 * Authors:       Jaap Onderwaater, GSI, jacobus.onderwaater@cern.ch       *
 *                Ilya Selyuzhenkov, GSI, ilya.selyuzhenkov@gmail.com      *
 *                Víctor González, UCM, victor.gonzalez@cern.ch            *
 *                Contributors are mentioned in the code where appropriate.*
 * Development:   2012-2016                                                *
 * See cxx source for GPL licence et. al.                                  *
 ***************************************************************************/

#include "CutsBase.h"
namespace Qn {
/// \file QnCorrectionsCutAbove.h
/// \brief Lower limit cut class for the Q vector correction framework

/// \class QnCorrectionsCutAbove
/// \brief Lower limit cut class for Q vector correction
///
/// Provides support for cuts based in a lower limit
///
/// Stores the threshold value and pass the var Id to its parent.
/// Implements IsSelected accordingly.
///
/// \author Jaap Onderwaater <jacobus.onderwaater@cern.ch>, GSI
/// \author Ilya Selyuzhenkov <ilya.selyuzhenkov@gmail.com>, GSI
/// \author Víctor González <victor.gonzalez@cern.ch>, UCM
/// \date Jan 22, 2016
class CutAbove : public CutsBase {

 public:
  CutAbove();
  CutAbove(const CutAbove &cut);
  CutAbove(Int_t varId, Float_t threshold);
  virtual ~CutAbove();

  virtual Bool_t IsSelected(const double *variableContainer);
 private:
  Float_t fThreshold;   ///< The value that must be surpassed

/// \cond CLASSIMP
 ClassDef(CutAbove, 1);
/// \endcond
};

/// Check if the actual variable value passes the cut
///
/// \param variableContainer the current variables content addressed by var Id
/// \return kTRUE if the actual value is above the threshold else kFALSE
inline Bool_t CutAbove::IsSelected(const double *variableContainer) {
  if (variableContainer[fVarId] > fThreshold)
    return kTRUE;
  else
    return kFALSE;
}
}
#endif // QNCORRECTIONS_CUT_ABOVE_H
