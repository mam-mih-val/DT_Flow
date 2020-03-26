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
    for( short i=0; i<20; i++ ){
      std::string histo_name{ "occupancy_map_"+std::to_string(5.0*i+2.5) };
      file->GetObject(histo_name.data(), h2d);
      histo_name = "ep_"+std::to_string(5.0*i+2.5);
      file->GetObject(histo_name.data(), h1d);
      h2d->Scale( 1./h1d->GetEntries() );
      occupancies_.emplace_back( *h2d );
    }
    file->Close();
  }
  virtual ~EfficiencyBuilder() = default;
  void Compute(float coeff=4424.42){
    float percentile{2.5};
    float max_eff=0.98;
    std::string histo_title{ ";#Delta#phi;#Theta" };
    for( auto occupancy : occupancies_ ){
      std::string histo_name{ "efficiency_map_"+std::to_string(percentile) };
      efficiencies_.emplace_back( histo_name.data(), histo_title.data(), 315, -3.15, 3.15, 85, 0.0, 1.7 );
      for(short i=0; i<occupancy.GetNbinsX(); ++i){
        for(short j=0; j<occupancy.GetNbinsY(); ++j){
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
