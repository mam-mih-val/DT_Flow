//
// Created by mikhail on 3/21/20.
//

#include "averager.h"
#include <string>
int main(int n_args, char **args){
  if( n_args < 2 )
    return 1;
  std::string file_in{ args[1] };

  Averager averager(file_in);
  averager.Calculate();
  averager.Print();
  return 0;
}