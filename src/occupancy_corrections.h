//
// Created by mikhail on 3/12/20.
//

#ifndef FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
#define FLOW_SRC_OCCUPANCY_CORRECTIONS_H_

#include "Centrality.h"
#include "DataTreeEvent.h"
#include "Selector.h"
#include <QVector.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2F.h>
#include <memory>
#include <utility>

class OccupancyCorrections {
public:
  explicit OccupancyCorrections(const std::string& file_name_data, bool is_list, std::string file_name_qn) :
  chain_data_(new TChain("DataTree")),
  chain_qn_( new TChain("tree") ),
  event_{new DataTreeEvent},
  selector_(event_),
  centrality_(event_){
    chain_qn_->Add( file_name_qn.data() );
    if( !is_list)
      chain_data_->Add(file_name_data.c_str());
    else{
      std::stringstream list{file_name_data};
      std::string file{};
      if( !file_name_data.empty() ){
        while(std::getline(list,file,',')){
          chain_data_->Add( file.data() );
          std::cout << file << " has been added to sequence" << std::endl;
        }
      }
    }
    chain_data_->SetBranchAddress("DTEvent", &event_);
    chain_qn_->SetBranchAddress("Fw1", &q_vector_);
    for( size_t i=0; i<20; i++ ){
      std::string histo_name{ "occupancy_map_"+std::to_string(5.0*i+2.5) };
      std::string histo_title{ "#delata#phi;#eta" };
      occupancy_maps_.insert( std::make_pair( 5.0*i+2.5, new TH2F( histo_name.data(), histo_title.data(), 315, -3.15, 3.15, 170, 0, 1.7 ) ) );
    }
    std::cout << "Data Tree Var Manager Initialized. " << chain_data_->GetEntries()
              << " events were found." << std::endl;
  }
  void ProcessEvent(){
    float psi = atan2(q_vector_->y(1), q_vector_->x(1));
    size_t n_tracks = event_->GetNVertexTracks();
    for( size_t idx=0; idx<n_tracks; idx++  ){
      if( !selector_.IsCorrectTrack(idx) )
        continue;
      auto p = event_->GetVertexTrack(idx)->GetMomentum();
      occupancy_maps_.at( centrality_.GetCentrality() )->Fill( p.Phi(), p.Eta() );
    }
  }
  void Run(){
    size_t n_events = chain_data_->GetEntries();
    for( size_t i=0; i<n_events; i++ ){
      chain_data_->GetEntry(i);
      chain_qn_->GetEntry(i);
      if( !selector_.IsCorrectEvent() )
        continue;
      ProcessEvent();
    }
  }
private:
  std::map<float, TH2F*> occupancy_maps_;
  std::shared_ptr<TChain> chain_data_;
  std::shared_ptr<TChain> chain_qn_;
  Qn::QVector* q_vector_;
  DataTreeEvent* event_;
  Selector selector_;
  Centrality centrality_;
};

#endif // FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
