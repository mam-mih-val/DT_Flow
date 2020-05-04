//
// Created by mikhail on 3/24/20.
//

#include "Processing/efficiency_builder.h"
#include "averager.h"
#include <string>

int main(int n_args, char **args) {
  if (n_args < 2)
    return 1;
  std::string all_tracks{args[1]};
  std::string proton_tracks{args[2]};

  double a{0.0}; double b{30000};
  double accuracy{powf(10, -3)};
  while(fabs((b-a)/2)>accuracy) {
    double x1 = (a+b-accuracy)/2;
    double x2 = (a+b+accuracy)/2;
    double y1 =0;
    double y2 =0;
    {
      EfficiencyBuilder builder(all_tracks);
      builder.Compute(x1);
      builder.SaveToFile("efficiency.root");
      Averager averager(proton_tracks);
      averager.Calculate();
      auto values = averager.GetValues();
      for (auto value : values)
        y1 += value * value;
      y1 = sqrt(y1);
    }
    {
      EfficiencyBuilder builder(all_tracks);
      builder.Compute(x2);
      builder.SaveToFile("efficiency.root");
      Averager averager(proton_tracks);
      averager.Calculate();
      auto values = averager.GetValues();
      for (auto value : values)
        y2 += value * value;
      y2 = sqrt(y2);
    }
    if( y1 > y2 ){
      a = x1;
      continue;
    }
    if( y1 < y2 ){
      b = x2;
      continue;
    }
  }
  std::cout << std::endl;
  double mean{(a+b)/2};
  std::cout << "min_err=" << " coeff=" << mean << std::endl;
  EfficiencyBuilder builder(all_tracks);
  builder.Compute(mean);
  return 0;
}