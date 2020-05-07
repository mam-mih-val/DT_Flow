//
// Created by mikhail on 3/19/20.
//

#ifndef FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
#define FLOW_SRC_OCCUPANCY_CORRECTIONS_H_

#include "Processing/efficiency.h"
#include <DataContainer.h>
#include <TChain.h>
class OccupancyCorrections {
public:
  static OccupancyCorrections* GetInstance(){
    if(!instance_)
      instance_=new OccupancyCorrections();
    return instance_;
  }
  void SetQnFile( std::string file_qn_name ){
    qn_chain_->AddFile(file_qn_name.data());
    qn_chain_->SetBranchAddress("Full", &q_vector_);
  }
  void SetEfficiencyFile( std::string file_efficiency_name ){
    efficiency_->SetFile(file_efficiency_name);
  }
  void SwitchEvent(long long idx){
    if( idx >= qn_chain_->GetEntries() )
      return;
    qn_chain_->GetEntry(idx);
  }
  void SwitchNextEvent(){
    SwitchEvent(position_);
    position_++;
  }
  void Rewind(){
    position_=0;
    SwitchEvent(position_);
  }
  bool Eof(){
    return position_ >= qn_chain_->GetEntries();
  }
  float GetEfficiency(int c_bin, float phi, float theta){
    try {
      float psi = GetPsiEp();
      return efficiency_->GetEfficiency( c_bin, phi-psi, theta );
    }catch (const std::exception &e) {
//      std::cout << e.what() << std::endl;
      return 0.98;
    }
  }
  float GetPsiEp(){
    return atan2f(q_vector_->At(0).y(1), q_vector_->At(0).x(1));
  }
private:
  static OccupancyCorrections* instance_;
  OccupancyCorrections() :qn_chain_{new TChain("tree")},
                          q_vector_{new Qn::DataContainer<Qn::QVector>},
                          efficiency_{new Efficiency}{};
  ~OccupancyCorrections() = default;
  long long position_=0;
  Efficiency* efficiency_{nullptr};
  TChain* qn_chain_{nullptr};
  Qn::DataContainer<Qn::QVector> *q_vector_{nullptr};

};

#endif // FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
