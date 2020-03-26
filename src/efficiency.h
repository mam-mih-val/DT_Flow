//
// Created by mikhail on 3/21/20.
//

#ifndef FLOW_SRC_EFFICIENCY_H_
#define FLOW_SRC_EFFICIENCY_H_

#include <TFile.h>
#include <TH2F.h>
#include <vector>
class Efficiency {
public:
  explicit Efficiency(const std::string& file_name) {
    std::unique_ptr<TFile> file{TFile::Open(file_name.data())};
    float p=2.5;
    TH2F* h2d;
    while(p<100.){
      std::string histo_name{"efficiency_map_"+std::to_string(p)};
      file->GetObject(histo_name.data(), h2d);
      efficiencies_.emplace_back(*h2d);
      p+=5.0;
    }
  }
  float GetEfficiency( float centrality, float phi, float theta ){
    ushort idx = (ushort) (centrality-2.5)/5.0;
    auto efficiency = efficiencies_.at(idx);
    auto bin = efficiency.FindBin(phi, theta);
    return efficiency.GetBinContent(bin);
  }
  float GetEfficiency( int c_bin, float phi, float theta ){
    return efficiencies_.at(c_bin).GetBinContent( efficiencies_.at(c_bin).FindBin(phi, theta) );
  }
private:
  std::vector<TH2F> efficiencies_;
};

#endif // FLOW_SRC_EFFICIENCY_H_
