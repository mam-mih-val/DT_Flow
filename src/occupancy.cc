//
// Created by mikhail on 3/12/20.
//

#include "occupancy_corrections.h"
#include <cstdlib>
#include <iostream>
int main(int n_args, char **args){
  std::string trigger = "PT3";
  unsigned short pid = 14;
  if (n_args > 4) {
    for (int i = 1; i < n_args - 2; i += 2) {
      std::string flag = args[i];
      if (flag == "--pid") {
        pid = std::atoi(args[i + 1]);
        continue;
      }
      if( flag == "--trigger" ){
        trigger = args[i+1];
        continue;
      }
    }
  }
  std::cout << "Configuration:" << std::endl;
  std::cout << "pid code:\t" << pid << std::endl;
  std::cout << "trigger:\t" << trigger << std::endl;

  OccupancyCorrections occupancy( args[n_args - 2], args[n_args - 1] );
  occupancy.GetSelector().SetTrigger(trigger);
  occupancy.SetPidCode(pid);
  occupancy.Run();
  return 0;
}