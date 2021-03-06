#ifndef QNCORRECTIONS_PROFILECORRCOMPHARM_H
#define QNCORRECTIONS_PROFILECORRCOMPHARM_H

/// \file QnCorrectionsProfileCorrelationComponentsHarmonics.h
/// \brief Correlation components based set of profiles with harmonic support for the Q vector correction framework

#include "CorrectionHistogramBase.h"
namespace Qn {
/// \class QnCorrectionsProfileCorrelationComponentsHarmonics
/// \brief Base class for the correlation components based set of profiles
///
/// Provides profile histograms for storing component, XX, XY, YX, YY, based
/// information for a set of predefined harmonics defined at creation
/// time. The user can select the harmonic addressing procedure so that
/// it will possible to ask for just one harmonic support and assign
/// to it any desired number.
///
/// As any histogram derived from QnCorrectionsHistogramBase the set
/// of variables that identify the different event classes has to
/// be passed to the constructor together with the required number of
/// harmonics and an optional harmonic expected numbering scheme.
/// Of course,  the base name and base title for the different
/// histograms has also to be provided.
///
/// The harmonic map passed should contain an ordered array with
/// as many items as requested harmonics that provides the external
/// number to be used for request the corresponding harmonic.
/// Requesting five harmonics without maps is equivalent to pass
/// {1,2,3,4,5} as map. Requesting just support for the harmonic
/// four will require a map {4}.
///
/// \author Jaap Onderwaater <jacobus.onderwaater@cern.ch>, GSI
/// \author Ilya Selyuzhenkov <ilya.selyuzhenkov@gmail.com>, GSI
/// \author Víctor González <victor.gonzalez@cern.ch>, UCM
/// \date Jan 19, 2016
class CorrectionProfileCorrelationComponentsHarmonics : public CorrectionHistogramBase {
 public:
  CorrectionProfileCorrelationComponentsHarmonics();
  CorrectionProfileCorrelationComponentsHarmonics(
      const char *name,
      const char *title,
      EventClassVariablesSet &ecvs,
      Option_t *option = "");
  virtual ~CorrectionProfileCorrelationComponentsHarmonics();

  Bool_t CreateCorrelationComponentsProfileHistograms(TList *histogramList,
                                                      Int_t nNoOfHarmonics,
                                                      Int_t *harmonicMap = NULL);
  virtual Bool_t AttachHistograms(TList *histogramList);
  /// wrong call for this class invoke base class behavior
  virtual Bool_t AttachHistograms(TList *histogramList,
                                  const Bool_t *bUsedChannel,
                                  const Int_t *nChannelGroup) {
    return CorrectionHistogramBase::AttachHistograms(histogramList,
                                                        bUsedChannel,
                                                        nChannelGroup);
  }

  virtual Long64_t GetBin(const double *variableContainer);
  /// wrong call for this class invoke base class behavior
  virtual Long64_t GetBin(const double *variableContainer, Int_t nChannel) {
    return CorrectionHistogramBase::GetBin(variableContainer,
                                              nChannel);
  }
  virtual Bool_t BinContentValidated(Long64_t bin);
  virtual Float_t GetXXBinContent(Int_t harmonic, Long64_t bin);
  virtual Float_t GetXYBinContent(Int_t harmonic, Long64_t bin);
  virtual Float_t GetYXBinContent(Int_t harmonic, Long64_t bin);
  virtual Float_t GetYYBinContent(Int_t harmonic, Long64_t bin);
  virtual Float_t GetXXBinError(Int_t harmonic, Long64_t bin);
  virtual Float_t GetXYBinError(Int_t harmonic, Long64_t bin);
  virtual Float_t GetYXBinError(Int_t harmonic, Long64_t bin);
  virtual Float_t GetYYBinError(Int_t harmonic, Long64_t bin);

  virtual void FillXX(Int_t harmonic, const double *variableContainer, Float_t weight);
  virtual void FillXY(Int_t harmonic, const double *variableContainer, Float_t weight);
  virtual void FillYX(Int_t harmonic, const double *variableContainer, Float_t weight);
  virtual void FillYY(Int_t harmonic, const double *variableContainer, Float_t weight);

  /// wrong call for this class invoke base class behavior
  virtual Float_t GetXXBinContent(Long64_t bin) { return CorrectionHistogramBase::GetXXBinContent(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetXYBinContent(Long64_t bin) { return CorrectionHistogramBase::GetXYBinContent(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetYXBinContent(Long64_t bin) { return CorrectionHistogramBase::GetYXBinContent(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetYYBinContent(Long64_t bin) { return CorrectionHistogramBase::GetYYBinContent(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetXXBinError(Long64_t bin) { return CorrectionHistogramBase::GetXXBinError(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetXYBinError(Long64_t bin) { return CorrectionHistogramBase::GetXYBinError(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetYXBinError(Long64_t bin) { return CorrectionHistogramBase::GetYXBinError(bin); }
  /// wrong call for this class invoke base class behavior
  virtual Float_t GetYYBinError(Long64_t bin) { return CorrectionHistogramBase::GetYYBinError(bin); }

  /// wrong call for this class invoke base class behavior
  virtual void FillXX(const double *variableContainer, Float_t weight) {
    return CorrectionHistogramBase::FillXX(variableContainer,
                                              weight);
  }
  /// wrong call for this class invoke base class behavior
  virtual void FillXY(const double *variableContainer, Float_t weight) {
    return CorrectionHistogramBase::FillXY(variableContainer,
                                              weight);
  }
  /// wrong call for this class invoke base class behavior
  virtual void FillYX(const double *variableContainer, Float_t weight) {
    return CorrectionHistogramBase::FillYX(variableContainer,
                                              weight);
  }
  /// wrong call for this class invoke base class behavior
  virtual void FillYY(const double *variableContainer, Float_t weight) {
    return CorrectionHistogramBase::FillYY(variableContainer,
                                              weight);
  }

 private:
  THnF **fXXValues;            //!<! XX component histogram for each requested harmonic
  THnF **fXYValues;            //!<! XY component histogram for each requested harmonic
  THnF **fYXValues;            //!<! YX component histogram for each requested harmonic
  THnF **fYYValues;            //!<! YY component histogram for each requested harmonic
  UInt_t fXXharmonicFillMask;  //!<! keeps track of harmonic XX component filled values
  UInt_t fXYharmonicFillMask;  //!<! keeps track of harmonic XY component filled values
  UInt_t fYXharmonicFillMask;  //!<! keeps track of harmonic YX component filled values
  UInt_t fYYharmonicFillMask;  //!<! keeps track of harmonic YY component filled values
  UInt_t fFullFilled;          //!<! mask for the fully filled condition
  THnI *fEntries;             //!<! Cumulates the number on each of the event classes
  /// \cond CLASSIMP
 ClassDef(CorrectionProfileCorrelationComponentsHarmonics, 1);
  /// \endcond
};
}
#endif
