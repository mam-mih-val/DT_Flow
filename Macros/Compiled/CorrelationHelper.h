#pragma once
#include "DataContainer.h"
#include "Stats.h"
#include "TFile.h"
#include <iostream>

class CorrelationHelper{
	public:
	CorrelationHelper() = default;
	~CorrelationHelper(){
		fHeap.clear();
	}
	void SetFile(std::string fileName) { fFile.reset(TFile::Open(fileName.data())); }
	void SetFile(std::shared_ptr<TFile> file) { fFile = file; }
	Qn::DataContainer<Qn::Stats> GetDataContainer(std::string name){ 
		Qn::DataContainer<Qn::Stats> empty;
		if( fHeap.count(name) !=0 ) 
			return fHeap.at(name); 
		else
			return empty;
	}
	Qn::DataContainer<Qn::Stats> MadeComputations(
	std::vector<std::string> varNames,
	std::function<Qn::DataContainer<Qn::Stats>(std::vector<Qn::DataContainer<Qn::Stats>>)> lambda,
	std::string resultName="result"
	)
	{
		std::vector<Qn::DataContainer<Qn::Stats>> arg;
		Qn::DataContainer<Qn::Stats>* ptr{0};
		for( auto name : varNames )
		{
			if( fHeap.count(name) == 0 )
			{
				fFile->GetObject(name.data(), ptr);
				if( !ptr )
				{
					std::cout << "No such variable: " << name << std::endl;
					continue;
				}
				arg.push_back(*ptr);
				fHeap.insert(make_pair( name, arg.back() ));
				ptr=nullptr;
			}
			else
				arg.push_back( fHeap.at(name) );
		}
		auto result = lambda(arg);
		fHeap.insert(make_pair(resultName, result));
		return result;
	}
	std::vector<Qn::DataContainer<Qn::Stats>> BuildResolution3SE(
		std::array<std::vector<std::string>,3> varNames,
		std::vector<std::string> ResolutionName
	)
	{
		auto R = []( std::vector<Qn::DataContainer<Qn::Stats>> corr ){
			Qn::DataContainer<Qn::Stats> result_;
			if( corr.size() != 3 )
			{
				std::cout << "Arguments amount error" << std::endl;
				return result_;
			}
			result_ = Sqrt( ( corr.at(0) * corr.at(1) ) / ( corr.at(2)*2 ) );
			return result_;
		};
		std::vector<Qn::DataContainer<Qn::Stats>> result;
		int i=0;
		for( auto strVec : varNames )
		{
			result.push_back( MadeComputations(strVec, R, ResolutionName.at(i)) );
			i++;
		}
		return result;
	}
	std::vector<Qn::DataContainer<Qn::Stats>> BuildFlow3SE(
		std::array<std::vector<std::string>,3> varNames,
		std::vector<std::string> flowName
	)
	{
		auto flow = []( std::vector<Qn::DataContainer<Qn::Stats>> corr ){
			Qn::DataContainer<Qn::Stats> result_;
			if( corr.size() != 2 )
			{
				std::cout << "Arguments amount error" << std::endl;
				return result_;
			}
			result_ = corr.at(0)/corr.at(1);
			return result_;
		};
		std::vector<Qn::DataContainer<Qn::Stats>> result;
		int i=0;
		for( auto strVec : varNames )
		{
			result.push_back( MadeComputations(strVec, flow, flowName.at(i)) );
			i++;
		}
		return result;
	}
	void PrintHeap() {
		for(auto pair : fHeap)
			std::cout << pair.first << endl;
	}
	protected:
	std::map<std::string, Qn::DataContainer<Qn::Stats> > fHeap;
	std::shared_ptr<TFile> fFile;
};