//
// Created by mikhail on 3/19/20.
//

#include "efficiency_builder.h"
#include <string>
int main( int n_args, char **args ){
  if( n_args < 2 )
    return 1;
  std::string file_in{ args[1] };
  std::string file_out{ args[2] };

  EfficiencyBuilder efficiency(file_in);
  efficiency.Compute();
  efficiency.SaveToFile(file_out);
  return 0;
}