//
// Created by mikhail on 3/21/20.
//

#ifndef FLOW_SRC_AVERAGER_H_
#define FLOW_SRC_AVERAGER_H_

#include "efficiency.h"
#include <TFile.h>
#include <TH2F.h>
#include <iostream>
class Averager {
public:
  explicit Averager(const std::string& file_name, std::string efficiency_file="efficiency.root"):
  efficiency_(efficiency_file){
    std::shared_ptr<TFile> file{ TFile::Open(file_name.data()) };
    TH2F* h2d;
    TH1F* h1d;
    for( short i=0; i<20; i++ ){
      std::string histo_name{ "occupancy_map_"+std::to_string(5.0*i+2.5) };
      file->GetObject(histo_name.data(), h2d);
      histo_name = "ep_"+std::to_string(5.0*i+2.5);
      file->GetObject(histo_name.data(), h1d);
      h2d->Scale( 1./h1d->GetEntries() );
      track_densities_.emplace_back( *h2d );
    }
    file->Close();
  }
  virtual ~Averager() = default;
  void Calculate(){
    for(size_t i=0; i<track_densities_.size()-10;++i){
      auto result = CalculateMatrix(i);
      values_.emplace_back(result.at(0));
      errors_.emplace_back(result.at(1));
    }
  }
  std::array<float,2> CalculateMatrix(short idx){
    auto matrix = track_densities_.at(idx);
    float sum_w{0.0};
    float sum_w2{0.0};
    float mean{0.0};
    float mean2{0.0};
    float variance{0.0};
    for(short i=0; i < (short) matrix.GetNbinsX(); ++i) {
      for (short j = 0; j < (short)matrix.GetNbinsY(); ++j) {
        if (matrix.GetBinContent(i, j) == 0.0)
          continue;
        float phi = matrix.GetXaxis()->GetBinCenter(i);
        float theta = matrix.GetYaxis()->GetBinCenter(j);
        float w = matrix.GetBinContent(i,j) / efficiency_.GetEfficiency( idx, phi, theta );;
        mean += w*cosf(phi);
        mean2 += w*w*cosf(phi)*cosf(phi);
        sum_w+=w;
        sum_w2+=w*w;
      }
    }
    mean/=sum_w;
    mean2/=sum_w2;
    variance = sqrtf(mean2/sum_w-mean*mean)*sqrtf(sum_w2/sum_w*sum_w);
    return {mean, variance};
  }
  void Print(){
    float p=2.5;
    for( size_t i=0; i<values_.size(); ++i){
      std::cout << p << "%: mean=" << values_.at(i) << " dev=" << errors_.at(i) << std::endl;
      p+=5.;
    }
  }
  float GetValue(int idx){ return values_.at(idx); }
  std::vector<float> GetValues(){ return  values_; }
private:
  Efficiency efficiency_;
  std::vector<TH2F> track_densities_;
  std::vector<float> values_;
  std::vector<float> errors_;
};

#endif // FLOW_SRC_AVERAGER_H_
