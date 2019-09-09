#pragma once
#include "DataContainer.h"
#include "DataContainerHelper.h"
#include "Stats.h"
#include "Axis.h"
#include "TFile.h"
#include "TList.h"
#include "TH1F.h"
#include <iostream>

class CorrelationHelper{
	public:
	CorrelationHelper() = default;
	~CorrelationHelper(){ fHeap.clear(); }
	void SetFile(std::string fileName) { fFile.reset(TFile::Open(fileName.data())); }
	void SetFile(std::shared_ptr<TFile> file) { fFile = file; }
	void SetFile(TFile* file) { fFile.reset(file); }
	Qn::DataContainer<Qn::Stats>& GetDataContainer(std::string name);
	
	Qn::DataContainer<Qn::Stats> MakeComputations(
		std::vector<std::string> varNames,
		std::function<Qn::DataContainer<Qn::Stats>(std::vector<Qn::DataContainer<Qn::Stats>>)> lambda,
		std::string resultName="result"
	);
	Qn::DataContainer<Qn::Stats> Merge(std::vector<std::string> inputName, std::string outName);
	
	std::vector<Qn::DataContainer<Qn::Stats>> BuildResolution3Se(
		std::vector<std::string> corrNames,
		std::vector<std::string> resNames
	);
	
	std::vector<Qn::DataContainer<Qn::Stats>> BuildFlow3Se(
		std::vector<std::string> unNames,
		std::vector<std::string> resNames,
		std::vector<std::string> flowNames
	);

	Qn::DataContainer<Qn::Stats> BuildRatio(
		std::vector<std::string> corr,
		std::string resultName
	);
	TH1F* GetTh1f( std::string name );
	std::vector<TH1F*> GetVectorTh1f( std::vector<std::string> names );
	void PrintHeap() {
		for(auto pair : fHeap)
			std::cout << pair.first << endl;
	}
	void SaveToFile(std::string fileName){
		TFile* file = new TFile( fileName.data(),"recreate" );
		file->cd();
		for(auto &container : fHeap)
		{
			container.second.Write(container.first.data());
		}
	}
	protected:
	std::map<std::string, Qn::DataContainer<Qn::Stats> > fHeap;
	std::shared_ptr<TFile> fFile;
};