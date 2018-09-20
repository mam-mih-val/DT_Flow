// Virtual class for analysis cuts
// Author: Ionut-Cristian Arsene (iarsene@cern.ch)
//   10/09/2015

#ifndef ALIREDUCEDINFOCUT_H
#define ALIREDUCEDINFOCUT_H

#include <TNamed.h>

//_________________________________________________________________________
class AliReducedInfoCut : public TNamed {

 public:
  AliReducedInfoCut();
  AliReducedInfoCut(const Char_t* name, const Char_t* title);
  virtual ~AliReducedInfoCut();
  
  virtual Bool_t IsSelected(TObject* obj) {return kTRUE;};
  virtual Bool_t IsSelected(TObject* obj, double* values) {return kTRUE;};
  virtual Bool_t IsSelected(double* values) {return kTRUE;};
  
 protected: 
   
  AliReducedInfoCut(const AliReducedInfoCut &c);
  AliReducedInfoCut& operator= (const AliReducedInfoCut &c);
  
  ClassDef(AliReducedInfoCut,2);
};

#endif