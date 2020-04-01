//
// Created by mikhail on 3/19/20.
//

#ifndef FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
#define FLOW_SRC_OCCUPANCY_CORRECTIONS_H_

#include "efficiency.h"
#include <DataContainer.h>
#include <TChain.h>
class OccupancyCorrections {
public:
  explicit OccupancyCorrections(const std::string& file_qn, const std::string& file_efficiency) :
  efficiency_(file_efficiency), qn_chain_("tree"),
  q_vector_{new Qn::DataContainer<Qn::QVector>}{
    qn_chain_.AddFile(file_qn.data());
    qn_chain_.SetBranchAddress("Full", &q_vector_);
  }
  virtual ~OccupancyCorrections() = default;
  void SwitchEvent(long long idx){
    if( idx >= qn_chain_.GetEntries() )
      return;
    qn_chain_.GetEntry(idx);
  }
  float GetEfficiency(int c_bin, float phi, float theta){
    try {
      float psi = atan2f(q_vector_->At(0).y(1), q_vector_->At(0).x(1));
      return efficiency_.GetEfficiency( c_bin, phi-psi, theta );
    }catch (const std::exception &e) {
//      std::cout << e.what() << std::endl;
      return 0.98;
    }
  }
private:
  Efficiency efficiency_;
  TChain qn_chain_;
  Qn::DataContainer<Qn::QVector> *q_vector_;

};

#endif // FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
