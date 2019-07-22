//
// Created by Lukas Kreis on 29.06.17.
//
#include "CorrectionTask.h"

#include <memory>
#include <iostream>

namespace Qn {

CorrectionTask::CorrectionTask(std::string inFile, std::string incalib) :
    out_file_(new TFile("output.root", "RECREATE")),
    in_calibration_file_(new TFile(incalib.data(), "READ")),
    out_calibration_file_(new TFile("qn.root", "RECREATE")),
    out_tree_(nullptr),
    manager_(),
    write_tree_(true) 
{
  femtoReader = new StFemtoDstReader(inFile.c_str());
  femtoReader->Init();
  femtoReader->SetStatus("*",0);
  femtoReader->SetStatus("Event",1);
  femtoReader->SetStatus("Track",1);
  if( !femtoReader->chain() ) {
    std::cout << "No chain has been found." << std::endl;
  }
  out_file_->cd();
  out_tree_ = new TTree("tree", "tree");
}

void CorrectionTask::Run() {
  Initialize();
  QnCorrectionsSetTracingLevel(kError);
  std::cout << "Processing..." << std::endl;
  
  Long64_t events2read = femtoReader->chain()->GetEntries();

  for(Long64_t iEvent=0; iEvent<events2read; iEvent++) {
    if( !femtoReader->readFemtoEvent(iEvent) ) break;
    StFemtoDst *dst = femtoReader->femtoDst();
    event = dst->event();
    if( !event )  break;
    if ( !isGoodEvent(event) ) continue;
    Process();
  }
  
  std::cout << "Analyzed " << events2read << " events." << std::endl;
  Finalize();
}

Bool_t CorrectionTask::isGoodEvent(StFemtoEvent *const &event)
{
  if (!event) return false;
  if (event == nullptr) return false;

  if (event->primaryVertex().Perp() > cutVtxR) return false;
  if (TMath::Abs(event->primaryVertex().Z()) > cutVtxZEnergy.at(energy)) return false;

  if ((energy == 200.) && TMath::Abs(event->primaryVertex().Z() - event->vpdVz()) > cutVpdVz) return false;

  return true;
}

Bool_t CorrectionTask::isGoodTrack(StFemtoTrack *const &track, Float_t _energy, TVector3 pVtx)
{
  if (!track) return false;
  if (!track->isPrimary()) return false;
  if (track->nHitsFit() < cutNhits) return false;
  if (track->nHitsPoss() <= cutNhitsPoss) return false;
  if ((Double_t) track->nHitsFit()/track->nHitsPoss() < cutNhitsRatio) return false;
  if (TMath::Abs(track->eta()) >= cutEta) return false;
  if (track->pt() <= cutPtMin.at(_energy)) return false;
  if (track->ptot() > cutPMax) return false;
  if (track->gDCA(pVtx).Mag() >= cutDCA.at(_energy)) return false;
  return true;
}

void CorrectionTask::Initialize() {
  // Add Variables to variable manager needed for filling
  manager_.AddVariable("Centrality", VAR::kCentrality, 1);
  manager_.AddVariable("Vx", VAR::kVx, 1);
  manager_.AddVariable("Vy", VAR::kVy, 1);
  manager_.AddVariable("Vz", VAR::kVz, 1);
  manager_.AddVariable("Pt", VAR::kPt, 1);
  manager_.AddVariable("Eta", VAR::kEta, 1);
  manager_.AddVariable("Phi", VAR::kPhi, 1);
  manager_.AddVariable("Dca", VAR::kDca, 1);
  manager_.AddVariable("Pid", VAR::kPid, 1);
  manager_.AddVariable("BBCwestPhi", VAR::kBBCwestPhi, 16);
  manager_.AddVariable("BBCeastPhi", VAR::kBBCeastPhi, 16);
  manager_.AddVariable("BBCwestADC", VAR::kBBCwestADC, 16);
  manager_.AddVariable("BBCeastADC", VAR::kBBCeastADC, 16);

  //Correction eventvariables
  manager_.SetEventVariable("Centrality");
  manager_.AddCorrectionAxis({"Centrality", 20, 0, 100});

  //Config TPC
  Axis pt("Pt", {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1., 1.25, 1.5, 1.75, 2.0, 2.25, 2.5});
  Axis etaEast("Eta", 5, -1., 0.);
  Axis etaWest("Eta", 5, 0., 1.);

//  auto cut_eta = [](const double &eta) { return -1. < eta && eta < 1.; };
  
  //Config of TPC corrections
  auto confTPC = [](DetectorConfiguration *config) {
    config->SetNormalization(QVector::Normalization::M);
    auto recenter = new Recentering();
    config->AddCorrectionOnQnVector(recenter);
    auto rescale = new TwistAndRescale();
    rescale->SetApplyTwist(true);
    rescale->SetApplyRescale(true);
    rescale->SetTwistAndRescaleMethod(TwistAndRescale::TWRESCALE_doubleHarmonic);
    config->AddCorrectionOnQnVector(rescale);
  };
  
  // TPC pT-dependence
  manager_.AddDetector("pion_east_pT", DetectorType::TRACK, "Phi", "Pt", {pt}, {1, 2, 3, 4});
  manager_.AddCut("pion_east_pT", {"Eta", "Dca", "Pid"}, [](const double &eta, const double &dca, const double &pid) { return pid == 211 && -1. < eta && eta < 0. && dca < 1.; });
  manager_.SetCorrectionSteps("pion_east_pT", confTPC);
  
  manager_.AddDetector("pion_west_pT", DetectorType::TRACK, "Phi", "Pt", {pt}, {1, 2, 3, 4});
  manager_.AddCut("pion_west_pT", {"Eta", "Dca", "Pid"}, [](const double &eta, const double &dca, const double &pid) { return pid == 211 && 0. < eta && eta < 1. && dca < 1.; });
  manager_.SetCorrectionSteps("pion_west_pT", confTPC);
  
  // TPC eta-dependence
  manager_.AddDetector("pion_east_eta", DetectorType::TRACK, "Phi", "Pt", {etaEast}, {1, 2, 3, 4});
  manager_.AddCut("pion_east_eta", {"Eta", "Dca", "Pid"}, [](const double &eta, const double &dca, const double &pid) { return pid == 211 && -1 < eta && eta < 0. && dca < 1.; });
  manager_.SetCorrectionSteps("pion_east_eta", confTPC);
  
  manager_.AddDetector("pion_west_eta", DetectorType::TRACK, "Phi", "Pt", {etaWest}, {1, 2, 3, 4});
  manager_.AddCut("pion_west_eta", {"Eta", "Dca", "Pid"}, [](const double &eta, const double &dca, const double &pid) { return pid == 211 && 0. < eta && eta < 1. && dca < 1.; });
  manager_.SetCorrectionSteps("pion_west_eta", confTPC);

  //TPC integrated
  manager_.AddDetector("TPCeast", DetectorType::TRACK, "Phi", "Pt", {}, {1, 2, 3, 4});
  manager_.AddCut("TPCeast", {"Pt", "Eta"}, [](const double &pt, const double &eta) { return pt < 2. && eta < -0.05; });
  manager_.SetCorrectionSteps("TPCeast", confTPC);
  
  manager_.AddDetector("TPCwest", DetectorType::TRACK, "Phi", "Pt", {}, {1, 2, 3, 4});
  manager_.AddCut("TPCwest", {"Pt", "Eta"}, [](const double &pt, const double &eta) { return pt < 2. && eta > 0.05; });
  manager_.SetCorrectionSteps("TPCwest", confTPC);
  
  std::vector <std::string> TPC_subevents = {"pion_east_pT", "pion_west_pT", "pion_east_eta", "pion_west_eta", "TPCeast", "TPCwest"};
  for (auto &sub : TPC_subevents)
  {
    manager_.AddHisto2D(sub, {{"Pt", 50, 0., 10.}, {"Phi", 50, 0, 2*TMath::Pi()}});
    manager_.AddHisto2D(sub, {{"Eta", 50, -1., 1.}, {"Phi", 50, 0, 2*TMath::Pi()}});
    manager_.AddHisto2D(sub, {{"Eta", 50, -1., 1.}, {"Pt", 50, 0., 10.}});
  }
  
  //Config of BBC corrections
  auto confBBC = [](DetectorConfiguration *config) {
    config->SetNormalization(QVector::Normalization::M);
    auto recenter = new Recentering();
//    recenter->SetApplyWidthEqualization(true);
    config->AddCorrectionOnQnVector(recenter);
    auto BBCchannels = new bool[16];
    auto BBCchannelGroups = new int[16];
    for (int ich = 0; ich < 16; ich++) {
      BBCchannels[ich] = true;
      BBCchannelGroups[ich] = ich/1;
    }
    config->SetChannelsScheme(BBCchannels, BBCchannelGroups);
//    auto equal = new GainEqualization();
//    equal->SetEqualizationMethod(GainEqualization::Method::AVERAGE);
//    equal->SetUseChannelGroupsWeights(true);
//    config->AddCorrectionOnInputData(equal);
  };

  //BBC
  auto cut_ADC = [](double &adc) {
    return adc > -0.1;
  };
  
  manager_.AddDetector("BBCwest", DetectorType::CHANNEL, "BBCwestPhi", "BBCwestADC", {}, {1, 2, 3, 4});
  manager_.AddCut("BBCwest", {"BBCwestADC"}, cut_ADC);
  manager_.SetCorrectionSteps("BBCwest", confBBC);
  
  manager_.AddDetector("BBCeast", DetectorType::CHANNEL, "BBCeastPhi", "BBCeastADC", {}, {1, 2, 3, 4});
  manager_.AddCut("BBCeast", {"BBCeastADC"}, cut_ADC);
  manager_.SetCorrectionSteps("BBCeast", confBBC);
  
  std::vector <std::string> BBC_subevents = {"BBCwest", "BBCeast"};
  for (auto &sub : BBC_subevents)
  {
//    manager_.AddHisto1D(sub, {{"BBCwestChannels", 16, 0, 16}}, "BBCwestADC");
//    manager_.AddHisto1D(sub, {{"BBCwestChannels", 16, 0, 16}}, "BBCwestPhi");
//    manager_.AddHisto2D(sub, {{"BBCwestPhi", 100, -TMath::Pi(), TMath::Pi()}, {"BBCwestChannel", 16, 0, 16}}, "BBCwestADC");
  }                      
  
  manager_.AddEventHisto1D({{"Centrality", 20, 0, 100}});
  manager_.AddEventHisto1D({{"Vz", 200, -30, 30}});
  manager_.AddEventHisto2D({{"Vx", 200, -0.5, 0.5}, {"Vy", 200, -0.5, 0.5}});
  
  manager_.SetTree(out_tree_);
  manager_.Initialize(in_calibration_file_);
}

void CorrectionTask::Process() {
  manager_.Reset();
  StFemtoEvent *event = dst->event();
  VAR::FillEventInfo(event, manager_.GetVariableContainer());
  manager_.ProcessEvent();
  manager_.FillChannelDetectors();
  
  Int_t nTracks = dst->numberOfTracks();
  TVector3 pVtx = event->primaryVertex();

  for(Int_t iTrk=0; iTrk<nTracks; iTrk++) {
    StFemtoTrack *femtoTrack = dst->track(iTrk);
    if (!femtoTrack) continue;
    if ( !femtoTrack->isPrimary() ) continue;
    if (!isGoodTrack(femtoTrack, energy, pVtx)) continue;
    if ( femtoTrack->isTofTrack() ) {}
    VAR::FillTrackInfo(femtoTrack, pVtx, manager_.GetVariableContainer());
    manager_.FillTrackingDetectors();
  }
  manager_.ProcessQnVectors();
}

void CorrectionTask::Finalize() {
  femtoReader->Finish();
  manager_.Finalize();
  manager_.SaveOutput(out_calibration_file_, out_file_);
}
}
