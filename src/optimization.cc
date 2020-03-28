//
// Created by mikhail on 3/24/20.
//

#include "averager.h"
#include "efficiency_builder.h"
#include <string>

int main(int n_args, char **args) {
  if (n_args < 2)
    return 1;
  std::string all_tracks{args[1]};
  std::string proton_tracks{args[2]};
  std::vector<float> err{1.0};
  std::vector<float> coeffs{0.0};
  float step{1000};
  double accuracy{powf(10, -7)};
  for(size_t i = 0; i < 100; ++i) {
    EfficiencyBuilder builder(all_tracks);
    float x = step*i;
    builder.Compute(x);
    builder.SaveToFile("efficiency.root");
    Averager averager(proton_tracks);
    averager.Calculate();
    auto values = averager.GetValues();
    float y=0;
    for(auto value:values)
      y+=value*value;
    y=sqrt(y);
    coeffs.push_back(x);
    err.push_back(y);
  }
  float min{1.0};
  float point{0.0};
  for( size_t i=0; i<err.size(); ++i ){
    if( min > err.at(i) ){
      min=err.at(i);
      point=coeffs.at(i);
    }
  }
  std::cout << std::endl;
  float mean{0.0};
  std::cout << "min_err=" << min << " coeff=" << point << std::endl;
  EfficiencyBuilder builder(all_tracks);
  builder.Compute(point);
  return 0;
}