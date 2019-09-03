#pragma once
#include "Stats.h"
#include "DataContainer.h"
#include "DataContainerHelper.h"
#include "TH1F.h"
#include "TFile.h"
#include "CorrelationHelper.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"

void BuildSystematics(std::string fileName="../../Output/Bs.root")
{
	// std::shared_ptr<TFile>( TFile::Open(fileName.data()) );
	CorrelationHelper helper;
	helper.SetFile(fileName);
	std::vector<std::string> component{"_XX", "YY"};
	vector<std::string> res1Se{"Fw1_Fw2_XX", "Fw1_Fw3_XX", "Fw2_Fw3_XX"};
	vector<std::string> res2Se{"Fw1_Fw2_XX", "Fw2_Fw3_XX", "Fw1_Fw3_XX"};
	vector<std::string> res3Se{"Fw2_Fw3_XX", "Fw1_Fw3_XX", "Fw1_Fw2_XX"};
	std::array<std::vector<std::string>, 3> resConfig;
	resConfig.at(0)=res1Se;
	resConfig.at(1)=res2Se;
	resConfig.at(2)=res3Se;
	std::vector<std::string> resNames{"Fw1_XX", "Fw2_XX", "Fw3_XX"};
	helper.BuildResolution3SE( resConfig, resNames );
	
	vector<std::string> flow1Se{"ProtonMdc_Fw1_XX", "Fw1_XX"};
	vector<std::string> flow2Se{"ProtonMdc_Fw2_XX", "Fw2_XX"};
	vector<std::string> flow3Se{"ProtonMdc_Fw3_XX", "Fw3_XX"};
	std::array<std::vector<std::string>, 3> flowConfig;
	flowConfig.at(0)=flow1Se;
	flowConfig.at(1)=flow2Se;
	flowConfig.at(2)=flow3Se;
	std::vector<std::string> flowNames{"flow_Fw1_XX", "flow_Fw2_XX", "flow_Fw3_XX"};
	std::vector<Qn::DataContainer<Qn::Stats>> v1 = helper.BuildFlow3SE( flowConfig, flowNames );
	
	// std::shared_ptr<TMultiGraph> stack{new TMultiGraph("flow", ";y-y_{beam};v1")};
	TMultiGraph* stack = new TMultiGraph("flow", ";y-y_{beam};v1");
	for( auto flow : v1x )
	{
		flow.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		flow = flow.Rebin({"Centrality", 2, 20, 30});
		flow = flow.Projection({"0_Ycm"});
		graph.push_back( Qn::DataContainerHelper::ToTGraph(flow) );
		stack->Add(graph.back(), "P");
	}
	for( auto flow : v1y )
	{
		flow.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		flow = flow.Rebin({"Centrality", 2, 20, 30});
		flow = flow.Projection({"0_Ycm"});
		graph.push_back( Qn::DataContainerHelper::ToTGraph(flow) );
		stack->Add(graph.back(), "P");
	}
	stack->GetHistogram()->Draw();
	stack->Draw("same");
}