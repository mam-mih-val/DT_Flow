//
// Created by mikhail on 3/24/20.
//

#include "averager.h"
#include "efficiency_builder.h"
#include <string>

int main(int n_args, char** args){
  if( n_args < 2 )
    return 1;
  std::string all_tracks{ args[1] };
  std::string proton_tracks{ args[2] };
  float err{1.0};
  float delta{+1.0};
  float x{0.0};
  short n_inn{0};
  float step{100000};
  float accuracy{powf(10, -8)};
  while ( step > accuracy ){
    step *= 0.1;
    n_inn=0;
    while( delta > 0.0 || n_inn < 10 ){
      EfficiencyBuilder builder(all_tracks);
      builder.Compute(x);
      builder.SaveToFile("efficiency.root");
      Averager averager(proton_tracks);
      averager.Calculate();
      auto values = averager.GetValues();
      float err_new{0.0};
      for( auto value : values )
        err_new +=value*value;
      err_new = sqrtf(err_new);
      delta= err - err_new;
      if( delta > 0.0 ){
        n_inn=0;
        err=err_new;
      }
      if( delta < 0.0 )
        n_inn++;
      x+=step;
      std::cout << "x=" << x << " err=" << err << std::endl;
    }
    x -= (n_inn+1)*step;
  }
  std::cout << "error=" << err << " parameter=" << x << std::endl;
  return 0;
}