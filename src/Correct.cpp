#include <iostream>
#include <iomanip>
#include <chrono>

#include "CorrectionTask.h"

int main(int argc, char **argv) {
	auto start = std::chrono::system_clock::now();
	if(argc<3)
    {
        cout << "Error 1: incorrect number of arguments" << endl;
        cout << argc << " arguments were given, minimum 2 is required" << endl;
        return 1;
    }
    bool channelSelection= false;
    std::string signal="adc";
    int pid=14;
    float minSignal=0;
    float maxSignal=999;
    if(argc>4)
    {
        for(int i=1; i<argc-2; i+=2)
        {
            std::string flag=argv[i];
            if(flag=="--signal")
            {
                signal=argv[i+1];
                continue;
            }
            if(flag=="--perchannel")
            {
                channelSelection=std::atoi(argv[i+1]);
                continue;
            }
            if(flag=="--min")
            {
                minSignal=std::atof(argv[i+1]);
                continue;
            }
            if(flag=="--max")
            {
                maxSignal=std::atof(argv[i+1]);
                continue;
            }
            if(flag=="--pid")
            {
                pid=std::atoi(argv[i+1]);
                continue;
            }
        }
    }
	Qn::CorrectionTask task(argv[ argc-2 ], argv[ argc-1 ]);
	task.SetSelectorConfiguration( channelSelection, signal, minSignal, maxSignal, pid);
	task.Run();
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
	return 0;
}
