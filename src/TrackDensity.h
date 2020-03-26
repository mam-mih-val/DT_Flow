//
// Created by mikhail on 3/12/20.
//

#ifndef FLOW_SRC_TRACKDENSITY_H_
#define FLOW_SRC_TRACKDENSITY_H_

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

class TrackDensity {
public:
  explicit TrackDensity(const std::string &file_name_data,
                        std::string file_name_qn)
      : chain_data_(new TChain("DataTree")), chain_qn_(new TChain("tree")),
        event_(new DataTreeEvent),
        q_vector_(new Qn::DataContainer<Qn::QVector>), selector_(event_),
        centrality_(event_) {
    chain_qn_->Add(file_name_qn.data());
    std::cout << file_name_qn << " has been read" << std::endl;
    std::stringstream list{file_name_data};
    std::string file{};
    if (!file_name_data.empty()) {
      while (std::getline(list, file, ',')) {
        chain_data_->Add(file.data());
        std::cout << file << " has been added to sequence" << std::endl;
      }
    }
    chain_data_->SetBranchAddress("DTEvent", &event_);
    chain_qn_->SetBranchAddress("Full", &q_vector_);
    float p=2.5;
    while(p<100.0) {
      std::string histo_name{"occupancy_map_" + std::to_string(p)};
      std::string histo_title{";#Delta#phi;#Theta"};
      occupancy_maps_.emplace_back(histo_name.data(), histo_title.data(), 315,-3.15, 3.15, 85, 0, 1.7);
      histo_name = "ep_" + std::to_string(p);
      histo_title = ";#Psi^{EP};counts";
      ep_maps_.emplace_back(histo_name.data(), histo_title.data(), 315, -3.15, 3.15);
      p+=5.0;
    }
    std::cout << chain_data_->GetEntries() << " events were found."
              << std::endl;
    std::cout << chain_qn_->GetEntries() << " qvectors were found."
              << std::endl;
  }
  void ProcessEvent() {
    float psi = atan2f(q_vector_->At(0).y(1), q_vector_->At(0).x(1));
    try {
      ep_maps_.at(centrality_.GetCentralityClass()).Fill(psi);
      size_t n_tracks = event_->GetNVertexTracks();
      for (size_t idx = 0; idx < n_tracks; idx++) {
        if (!selector_.IsCorrectTrack(idx))
          continue;
//        if (event_->GetVertexTrack(idx)->GetPdgId() != pid_code_)
//          continue;
        auto p = event_->GetVertexTrack(idx)->GetMomentum();
        float d_phi = p.Phi()-psi;
//        if( ( (-0.25 < p.Rapidity()-Y_BEAM/2 && p.Rapidity() -Y_BEAM/2 < -0.15) ||
//              (0.15 < p.Rapidity()-Y_BEAM/2 && p.Rapidity()-Y_BEAM/2 < 0.25) ) &&
//              0.2 < p.Pt() && p.Pt() < 0.3 )
        if( -TMath::Pi() <= d_phi && d_phi <= TMath::Pi() )
          occupancy_maps_.at(centrality_.GetCentralityClass() ).Fill(d_phi, p.Theta());
        if( d_phi < -TMath::Pi() )
          occupancy_maps_.at(centrality_.GetCentralityClass() ).Fill(d_phi+2*TMath::Pi(), p.Theta());
        if( d_phi > TMath::Pi() )
          occupancy_maps_.at(centrality_.GetCentralityClass() ).Fill(d_phi-2*TMath::Pi(), p.Theta());
      }
    } catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
      std::cout << "Centrality is not correct:" << std::endl;
      std::cout << "Centrality: " << centrality_.GetCentrality() << "%"
                << std::endl;
      return;
    }
  }
  void Run() {
    size_t n_events = chain_data_->GetEntries();
    size_t n_qvectors = chain_qn_->GetEntries();
    size_t j = 0;
    for (size_t i = 0; i < n_events && j < n_qvectors; i++) {
      chain_data_->GetEntry(i);
      if (!selector_.IsCorrectEvent())
        continue;
      chain_qn_->GetEntry(j);
      ProcessEvent();
      j++;
    }
    std::shared_ptr<TFile> file{TFile::Open("pavel.root", "recreate")};
    WriteToFile(file);
    file->Close();
  }
  void WriteToFile(std::shared_ptr<TFile> file) {
    file->cd();
    for (auto ep_map : ep_maps_)
      ep_map.Write();
    for (auto occupancy_map : occupancy_maps_)
      occupancy_map.Write();
  }
  Selector &GetSelector() { return selector_; }
  void SetPidCode(unsigned short pid_code) { pid_code_ = pid_code; }

private:
  unsigned short pid_code_{14};
  std::vector<TH2F> occupancy_maps_;
  std::vector<TH1F> ep_maps_;
  std::shared_ptr<TChain> chain_data_;
  std::shared_ptr<TChain> chain_qn_;
  Qn::DataContainer<Qn::QVector> *q_vector_;
  DataTreeEvent *event_;
  Selector selector_;
  Centrality centrality_;

  const double T = 1.23;  // AGeV
  const double M = 0.938; // GeV
  const double GAMMA = (T + M) / M;
  const double BETA = sqrt(1 - (M * M) / (M + T) / (M + T));
  const double PZ = M * BETA * GAMMA;
  const double E = T + M;
  const double Y_BEAM = 0.5 * log((E + PZ) / (E - PZ));
};

#endif // FLOW_SRC_TRACKDENSITY_H_
