//
// Created by mikhail on 4/1/20.
//

#include "CorrectionTask.h"
#include "CorrelationTask.h"
#include "DataContainer.h"
#include "Stats.h"
#include "efficiency_builder.h"
#include <iostream>
#include <string>
#include <utility>
void Correct(const std::string &in_file, const std::string &incalib,
             const std::string &qn_file, const std::string &efficiency,
             const std::string &out_file, std::string method) {
  std::string signal = "adc";
  std::string trigger = "PT2";
  int pid = 14;
  float minSignal = 0;
  float maxSignal = 999;
  bool channelSelection = false;

  Qn::CorrectionTask correction_task(in_file, incalib, qn_file, efficiency,
                                     out_file);
  correction_task.GetSelector()->SetFwChannelSelection(channelSelection);
  correction_task.GetSelector()->SetFwSignalType(signal);
  correction_task.GetSelector()->SetFwSignalRange(minSignal, maxSignal);
  correction_task.GetSelector()->SetTrigger(trigger);
  correction_task.SetParticlePid(pid);
  correction_task.Run(std::move(method));
}
double Decline(std::string file_in) {
  std::unique_ptr<TFile> file{TFile::Open(file_in.data())};
  Qn::DataContainer<Qn::Stats> *flow_x;
  Qn::DataContainer<Qn::Stats> *flow_y;
  file->GetObject("TracksMdc_Full_XX_Ep", flow_x);
  file->GetObject("TracksMdc_Full_YY_Ep", flow_y);
  double result{0.0};
  for (size_t i = 0; i < flow_x->size(); ++i) {
    result += flow_x->At(i).Mean() * flow_x->At(i).Mean() +
              flow_y->At(i).Mean() * flow_y->At(i).Mean();
  }
  return sqrt(result);
}

int main(int argc, char **argv) {
  auto start = std::chrono::system_clock::now();
  bool channelSelection = false;
  std::string signal = "adc";
  std::string method = "FW3S";
  std::string trigger = "PT3";
  std::string all_tracks = "all_tracks_nb.root";
  std::string efficiency_file{};
  std::string qn_file{};

  int pid = 14;
  float minSignal = 0;
  float maxSignal = 999;
  for (int i = 1; i < argc - 2; i += 2) {
    std::string flag = argv[i];
    if (flag == "--signal") {
      signal = argv[i + 1];
      continue;
    }
    if (flag == "--perchannel") {
      channelSelection = std::atoi(argv[i + 1]);
      continue;
    }
    if (flag == "--min") {
      minSignal = std::atof(argv[i + 1]);
      continue;
    }
    if (flag == "--max") {
      maxSignal = std::atof(argv[i + 1]);
      continue;
    }
    if (flag == "--pid") {
      pid = std::atoi(argv[i + 1]);
      continue;
    }
    if (flag == "--method") {
      method = argv[i + 1];
      continue;
    }
    if (flag == "--trigger") {
      trigger = argv[i + 1];
      continue;
    }
    if (flag == "--eff") {
      efficiency_file = argv[i + 1];
      continue;
    }
    if (flag == "--qn") {
      qn_file = argv[i + 1];
      continue;
    }
    if (flag == "--density") {
      all_tracks = argv[i + 1];
      continue;
    }
  }

  std::cout << "Configuration:" << std::endl;
  std::cout << "per channer:\t" << channelSelection << std::endl;
  std::cout << "signal:\t\t" << signal << std::endl;
  std::cout << "min signal:\t" << minSignal << std::endl;
  std::cout << "max signal:\t" << maxSignal << std::endl;
  std::cout << "pid code:\t" << pid << std::endl;
  std::cout << "method:\t\t" << method << std::endl;
  std::cout << "trigger:\t" << trigger << std::endl;
  std::cout << "efficiency:\t" << efficiency_file << std::endl;
  std::cout << "Qn-file:\t" << qn_file << std::endl;

  Correct(argv[argc - 1], "nothing", "", "", "full_0.root", "FULL");
  Correct(argv[argc - 1], "qn.root", "", "", "full_1.root", "FULL");

  double a{0.0};
  double b{30000};
  double accuracy{powf(10, 3)};
  double step{ powf(accuracy, -1) };
  while (fabs((b - a) / 2) > accuracy) {
    double x1 = (a + b - accuracy) / 2;
    double x2 = (a + b + accuracy) / 2;
    double y1 = 0;
    double y2 = 0;
    {
      EfficiencyBuilder builder(all_tracks);
      builder.Compute(x1);
      builder.SaveToFile("efficiency.root");
      Correct(argv[argc - 1], "nothing", "full_1.root", "efficiency.root", "output_0.root",
              "RND_OPT");
      Correct(argv[argc - 1], "qn.root", "full_1.root", "efficiency.root", "output_1.root",
              "RND_OPT");
      CorrelationTask st("output_1.root", "tree");
      st.Run("RND_OPT");
      y1 = Decline("Correlations.root");
    }
    {
      EfficiencyBuilder builder(all_tracks);
      builder.Compute(x2);
      builder.SaveToFile("efficiency.root");
      Correct(argv[argc - 1], "nothing", "full_1.root", "efficiency.root",
              "output_0.root", "RND_OPT");
      Correct(argv[argc - 1], "qn.root", "full_1.root", "efficiency.root",
              "output_1.root", "RND_OPT");
      CorrelationTask st("output_1.root", "tree");
      st.Run("RND_OPT");
      y2 = Decline("Correlations.root");
    }
    std::cout << "err1=" << y1 << " err2=" << y2 << std::endl;
    std::cout << "x1=" << x1 << " x2=" << x2 << std::endl;
    if (y1 > y2) {
      a = x1;
      continue;
    }
    if (y1 < y2) {
      b = x2;
      continue;
    }
  }
  std::cout << " coeff=" << (a + b) / 2.0 << std::endl;
  EfficiencyBuilder builder(all_tracks);
  builder.Compute((a + b) / 2.0);
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}