#ifndef QNCORRECTIONS_CUT_SETBIT_H
#define QNCORRECTIONS_CUT_SETBIT_H

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
/// \file QnCorrectionsCutSetBit.h
/// \brief Bit setting cut class support for the Q vector correction framework

/// \class QnCorrectionsCutSetBit
/// \brief Bit setting cut class for Q vector correction
///
/// Provides support for cuts based in the setting or not
/// of a concrete bit within the actual value of a variable.
/// The selected bit is passed as an integer from 0 up to 31.
///
/// Stores the desired bit and its (un)set condition and pass
/// the var Id to its parent. Implements IsSelected accordingly.
///
/// \author Jaap Onderwaater <jacobus.onderwaater@cern.ch>, GSI
/// \author Ilya Selyuzhenkov <ilya.selyuzhenkov@gmail.com>, GSI
/// \author Víctor González <victor.gonzalez@cern.ch>, UCM
/// \date Jan 25, 2016
class CutSetBit : public CutsBase {

 public:
  CutSetBit();
  CutSetBit(const CutSetBit &cut);
  CutSetBit(Int_t varId, Int_t bitNo, Bool_t set);
  virtual ~CutSetBit();

  virtual Bool_t IsSelected(const double *variableContainer);
 private:
  UInt_t fBitMask;   ///< The mask to apply to the variable value
  UInt_t fExpectedResult; ///< The expected masked result to pass the cut

/// \cond CLASSIMP
 ClassDef(CutSetBit, 1);
/// \endcond
};

/// Check if the actual variable value passes the cut
///
/// \param variableContainer the current variables content addressed by var Id
/// \return kTRUE if the actual value is below the threshold else kFALSE
inline Bool_t CutSetBit::IsSelected(const double *variableContainer) {
  if ((UInt_t(variableContainer[fVarId]) & fBitMask)!=fExpectedResult)
    return kFALSE;
  else
    return kTRUE;
}
}
#endif // QNCORRECTIONS_CUT_SETBIT_H
