#pragma once
#include "DataContainer.h"
#include "Stats.h"
#include "TFile.h"
#include "TList.h"
#include <iostream>

class CorrelationHelper{
	public:
	CorrelationHelper() = default;
	~CorrelationHelper(){ fHeap.clear(); }
	void SetFile(std::string fileName) { fFile.reset(TFile::Open(fileName.data())); }
	void SetFile(std::shared_ptr<TFile> file) { fFile = file; }
	Qn::DataContainer<Qn::Stats>& GetDataContainer(std::string name);
	
	Qn::DataContainer<Qn::Stats> MadeComputations(
		std::vector<std::string> varNames,
		std::function<Qn::DataContainer<Qn::Stats>(std::vector<Qn::DataContainer<Qn::Stats>>)> lambda,
		std::string resultName="result"
	);
	Qn::DataContainer<Qn::Stats> Merge(std::vector<std::string> inputName, std::string outName);
	std::vector<Qn::DataContainer<Qn::Stats>> BuildResolution3SE(
		std::array<std::vector<std::string>,3> varNames,
		std::vector<std::string> ResolutionName
	);
	std::vector<Qn::DataContainer<Qn::Stats>> BuildFlow3SE(
		std::array<std::vector<std::string>,3> varNames,
		std::vector<std::string> flowName
	);

	void PrintHeap() {
		for(auto pair : fHeap)
			std::cout << pair.first << endl;
	}
	protected:
	std::map<std::string, Qn::DataContainer<Qn::Stats> > fHeap;
	std::shared_ptr<TFile> fFile;
};