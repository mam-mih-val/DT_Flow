//
// Created by mikhail on 3/21/20.
//

#ifndef FLOW_SRC_EFFICIENCY_H_
#define FLOW_SRC_EFFICIENCY_H_

#include <TFile.h>
#include <TH2F.h>
#include <TMath.h>
#include <iostream>
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
    float efficiency{0.98};
    if( -3.15 < phi < 3.15 ){
      int bin_x{ efficiencies_.at(c_bin).GetXaxis()->FindBin(phi) };
      int bin_y{ efficiencies_.at(c_bin).GetYaxis()->FindBin(theta) };
      efficiency = efficiencies_.at(c_bin).GetBinContent( bin_x, bin_y );
    }
    if( phi < -3.15 )
      efficiency = efficiencies_.at(c_bin).GetBinContent( efficiencies_.at(c_bin).FindBin(phi+2*TMath::Pi(), theta) );
    if( phi > 3.15 )
      efficiency = efficiencies_.at(c_bin).GetBinContent( efficiencies_.at(c_bin).FindBin(phi-2*TMath::Pi(), theta) );

    return efficiency;
  }
private:
  std::vector<TH2F> efficiencies_;
};

#endif // FLOW_SRC_EFFICIENCY_H_
