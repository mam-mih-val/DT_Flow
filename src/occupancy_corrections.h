//
// Created by mikhail on 3/12/20.
//

#ifndef FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
#define FLOW_SRC_OCCUPANCY_CORRECTIONS_H_

#include "Centrality.h"
#include "DataTreeEvent.h"
#include "Selector.h"
#include <DataContainer.h>
#include <QVector.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2F.h>
#include <TProfile2D.h>
#include <memory>
#include <utility>

class OccupancyCorrections {
public:
  explicit OccupancyCorrections(const std::string& file_name_data, std::string file_name_qn) :
  chain_data_(new TChain("DataTree")),
  chain_qn_( new TChain("tree") ),
  event_(new DataTreeEvent),
  q_vector_(new Qn::DataContainer<Qn::QVector>),
  selector_(event_),
  centrality_(event_),
  ep_histo_(new TH1F("psi_ep", ";#Psi^{EP};counts", 315, -3.15, 3.15)){
    chain_qn_->Add( file_name_qn.data() );
    std::cout << file_name_qn << " has been read" << std::endl;
    std::stringstream list{file_name_data};
    std::string file{};
    if( !file_name_data.empty() ){
      while(std::getline(list,file,',')){
        chain_data_->Add( file.data() );
        std::cout << file << " has been added to sequence" << std::endl;
      }
    }
    chain_data_->SetBranchAddress("DTEvent", &event_);
    chain_qn_->SetBranchAddress("Full", &q_vector_);
    for( size_t i=0; i<20; i++ ){
      std::string histo_name{ "occupancy_map_"+std::to_string(5.0*i+2.5) };
      std::string histo_title{ ";#Delta#phi;#Theta" };
      occupancy_maps_.insert( std::make_pair( 5.0*i+2.5, new TH2F( histo_name.data(), histo_title.data(), 315, -3.15, 3.15, 170, 0, 1.7 ) ) );
    }
    std::cout << chain_data_->GetEntries()
              << " events were found." << std::endl;
    std::cout << chain_qn_->GetEntries()
              << " qvectors were found." << std::endl;
  }
  void ProcessEvent(){
    float psi = atan2f(q_vector_->At(0).y(1), q_vector_->At(0).x(1));
    ep_histo_->Fill(psi);
    size_t n_tracks = event_->GetNVertexTracks();
    for( size_t idx=0; idx<n_tracks; idx++  ){
      if( !selector_.IsCorrectTrack(idx) )
        continue;
      if( event_->GetVertexTrack(idx)->GetPdgId() != pid_code_ )
        continue;
      auto p = event_->GetVertexTrack(idx)->GetMomentum();
      try {
        occupancy_maps_.at( (float) centrality_.GetCentrality() )->Fill( p.Phi()-psi, p.Theta() );
      } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        std::cout << "Centrality is not correct:" << std::endl;
        std::cout << "Centrality: " << centrality_.GetCentrality() << "%" << std::endl;
        return;
      }
    }
  }
  void Run(){
    size_t n_events = chain_data_->GetEntries();
    size_t n_qvectors = chain_qn_->GetEntries();
    size_t j=0;
    for( size_t i=0; i<n_events && j<n_qvectors; i++ ){
      chain_data_->GetEntry(i);
      if( !selector_.IsCorrectEvent() )
        continue;
      chain_qn_->GetEntry(j);
      ProcessEvent();
      j++;
    }
    std::shared_ptr<TFile> file{ TFile::Open( "pavel.root", "recreate" ) };
    WriteToFile(file);
    file->Close();
  }
  void WriteToFile( std::shared_ptr<TFile> file ){
    file->cd();
    ep_histo_->Write();
    for( auto occupancy_map : occupancy_maps_ )
      occupancy_map.second->Write();
  }
  Selector &GetSelector() { return selector_; }
  void SetPidCode(unsigned short pid_code) { pid_code_ = pid_code; }

private:
  unsigned short pid_code_{14};
  std::map<float, TH2F*> occupancy_maps_;
  std::shared_ptr<TH1F> ep_histo_;
  std::shared_ptr<TChain> chain_data_;
  std::shared_ptr<TChain> chain_qn_;
  Qn::DataContainer<Qn::QVector>* q_vector_;
  DataTreeEvent* event_;
  Selector selector_;
  Centrality centrality_;
};

#endif // FLOW_SRC_OCCUPANCY_CORRECTIONS_H_
