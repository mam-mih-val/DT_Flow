//
// Created by mikhail on 3/18/20.
//

#ifndef FLOW_SRC_EFFICIENCY_BUILDER_H_
#define FLOW_SRC_EFFICIENCY_BUILDER_H_

#include <TFile.h>
#include <TH2F.h>
#include <TH1F.h>
#include <vector>
class EfficiencyBuilder {
public:
  explicit EfficiencyBuilder(const std::string& file_name){
    std::shared_ptr<TFile> file{ TFile::Open(file_name.data()) };
    TH2F* h2d;
    TH1F* h1d;
    float pc = 2.5;
    while( pc < 100.0 ){
      std::string histo_name{ "occupancy_map_"+std::to_string(pc) };
      file->GetObject(histo_name.data(), h2d);
      histo_name = "ep_"+std::to_string(pc);
      file->GetObject(histo_name.data(), h1d);
      h2d->Scale( 1./h1d->GetEntries() );
      occupancies_.emplace_back( *h2d );
      pc+=5.0;
    }
    file->Close();
  }
  virtual ~EfficiencyBuilder() = default;
  void Compute(float coeff=27000){
    float percentile{2.5};
    float max_eff=0.98;
    float min_eff=0.7;
    float min_mult=occupancies_.at(3).GetMinimum();
    float max_mult=occupancies_.at(0).GetMaximum();
    coeff=(max_eff-min_eff)/(max_mult-min_mult)/(max_mult-min_mult);
    std::string histo_title{ ";#Delta#phi;#Theta" };
    for( auto occupancy : occupancies_ ){
      std::string histo_name{ "efficiency_map_"+std::to_string(percentile) };
      efficiencies_.emplace_back( histo_name.data(), histo_title.data(), 315,-3.15, 3.15, 87, 0, 1.74 );
      for(short i=1; i<occupancy.GetNbinsX()+1; ++i){
        for(short j=1; j<occupancy.GetNbinsY()+1; ++j){
          float density = occupancy.GetBinContent(i,j);
          float eff = max_eff - coeff*density*density;
          efficiencies_.back().SetBinContent(i,j, eff);
        }
      }
      percentile+=5.0;
    }
  }
  void SaveToFile(std::string file_name){
    std::shared_ptr<TFile> file{ TFile::Open( file_name.data(), "recreate" ) };
    file->cd();
    for( auto occupancy : occupancies_ )
      occupancy.Write();
    for( auto efficiency : efficiencies_ )
      efficiency.Write();
    file->Close();
  }

private:
  std::vector<TH2F> occupancies_;
  std::vector<TH2F> efficiencies_;
};

#endif // FLOW_SRC_EFFICIENCY_BUILDER_H_
