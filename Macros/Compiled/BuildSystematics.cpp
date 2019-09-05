#pragma once
#include "Stats.h"
#include "DataContainer.h"
#include "DataContainerHelper.h"
#include "TH1F.h"
#include "TFile.h"
#include "CorrelationHelper.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TPad.h"

void BuildSystematics( std::string fileName="../../Output/Bs.root" )
{
	CorrelationHelper helper;
	helper.SetFile(fileName);
	std::vector<std::string> components = {"_XX", "_YY"};
	std::vector<std::string> flowName = {"flow_Fw1", "flow_Fw2", "flow_Fw3"};
	for( auto component : components )
	{
		vector<std::string> res1Se{"Fw1_Fw2"+component, "Fw1_Fw3"+component, "Fw2_Fw3"+component};
		vector<std::string> res2Se{"Fw1_Fw2"+component, "Fw2_Fw3"+component, "Fw1_Fw3"+component};
		vector<std::string> res3Se{"Fw2_Fw3"+component, "Fw1_Fw3"+component, "Fw1_Fw2"+component};
		std::array<std::vector<std::string>, 3> resConfig;
		resConfig.at(0)=res1Se;
		resConfig.at(1)=res2Se;
		resConfig.at(2)=res3Se;
		std::vector<std::string> resNames{"Fw1"+component, "Fw2"+component, "Fw3"+component};
		helper.BuildResolution3SE( resConfig, resNames );
		
		vector<std::string> flow1Se{"ProtonMdc_Fw1"+component, "Fw1"+component};
		vector<std::string> flow2Se{"ProtonMdc_Fw2"+component, "Fw2"+component};
		vector<std::string> flow3Se{"ProtonMdc_Fw3"+component, "Fw3"+component};
		std::array<std::vector<std::string>, 3> flowConfig;
		flowConfig.at(0)=flow1Se;
		flowConfig.at(1)=flow2Se;
		flowConfig.at(2)=flow3Se;
		std::vector<std::string> flowNames{
			flowName.at(0)+component, 
			flowName.at(1)+component, 
			flowName.at(2)+component
		};
		helper.BuildFlow3SE( flowConfig, flowNames );
	}
	Qn::DataContainer<Qn::Stats> integral; // Integrated Flow
	std::vector<Qn::DataContainer<Qn::Stats>> flowDiffOnSe; // Flow diffetiated on sub-events
	std::vector<Qn::DataContainer<Qn::Stats>> flowDiffOnComp; // Flow diffetiated on x-y components
	std::array< std::vector< std::string >, 3> listDiffOnSe; // list of strings for merging by coponents to estimate difference between SE
	std::array< std::vector< std::string >, 2> listDiffOnComp; // list of strings for merging by SE to estimate difference between components
	std::vector< std::string > listIntegrated; // list of strings for merging by SE to estimate difference between components
	for( int i=0; i<flowName.size(); i++ )
	{
		for(int j=0; j<components.size(); j++)
		{
			std::string name = flowName.at(i)+components.at(j);
			listDiffOnSe.at(i).push_back( name );
			listDiffOnComp.at(j).push_back( name );
			listIntegrated.push_back(name);
		}
	}
	integral = helper.Merge( listIntegrated, "Integral" );
	std::vector<std::string> strSe = { "Fw1", "Fw2", "Fw3" };
	std::vector<std::string> strComp = { "X", "Y" };
	{
		int i=0;
		for( auto list : listDiffOnSe )
		{
			flowDiffOnSe.push_back(helper.Merge( list, strSe.at(i) ));
			i++;
		}
		i=0;
		for( auto list : listDiffOnComp )
		{
			flowDiffOnComp.push_back( helper.Merge( list, strComp.at(i) ) );
			i++;
		}
	}
	for( auto &flow : flowDiffOnSe )
	{
		flow = flow.Rebin({"Centrality", 2, 20, 30});
		flow = flow.Projection({"0_Ycm"});
	}
	for( auto &flow : flowDiffOnComp )
	{
		flow = flow.Rebin({"Centrality", 2, 20, 30});
		flow = flow.Projection({"0_Ycm"});
	}
	integral = integral.Rebin({"Centrality", 2, 20, 30});
	integral = integral.Projection({"0_Ycm"});
	integral.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );

	vector<TMultiGraph*> stack{
		new TMultiGraph("sub_events", ";y-y_{beam};v1"),
		new TMultiGraph("components", ";y-y_{beam};v1")
	};
	std::vector<std::string> graphTitle{
		"v_{1}^{a}",
		"v_{1}^{b}",
		"v_{1}^{c}"
	};
	std::vector<int> graphStyle{ 21, 22, 23 };
	std::vector<int> graphColor{ kRed+1, kGreen+1, kBlue+1 };
	TGraphAsymmErrors* tempGraph;
	int k=0; 
	for( auto flow : flowDiffOnSe )
	{
		flow.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		tempGraph = Qn::DataContainerHelper::ToTGraph(flow);
		tempGraph->SetTitle( graphTitle.at(k).data() );
		tempGraph->SetMarkerStyle( graphStyle.at(k) );
		tempGraph->SetMarkerColor( graphColor.at(k) );
		tempGraph->SetLineColor( graphColor.at(k) );
		tempGraph->SetLineWidth( 3.0 );
		tempGraph->SetMarkerSize( 1.5 );
		stack.at(0)->Add( tempGraph, "P" );
		k++;
	}
	k=0;
	graphTitle = {"v_{1}^{x}","v_{1}^{y}"};
	for( auto flow : flowDiffOnComp )
	{
		flow.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		tempGraph = Qn::DataContainerHelper::ToTGraph(flow);
		tempGraph->SetTitle( graphTitle.at(k).data() );
		tempGraph->SetMarkerStyle( graphStyle.at(k) );
		tempGraph->SetMarkerColor( graphColor.at(k) );
		tempGraph->SetLineColor( graphColor.at(k) );
		tempGraph->SetLineWidth( 3.0 );
		tempGraph->SetMarkerSize( 1.5 );
		stack.at(1)->Add( tempGraph, "P" );
		k++;
	}
	tempGraph = Qn::DataContainerHelper::ToTGraph(integral);
	tempGraph->SetTitle( "refference" );
	tempGraph->SetMarkerStyle( 20 );
	tempGraph->SetMarkerColor( kBlack );
	tempGraph->SetLineColor( kBlack );
	tempGraph->SetLineWidth( 3.0 );
	tempGraph->SetMarkerSize( 1.5 );
	stack.at(0)->Add( tempGraph );
	stack.at(1)->Add( tempGraph );
	TCanvas* canvas = new TCanvas( "Flow_channels", "", 2100, 2000 );
	canvas->Divide(2,2);
	canvas->cd(1);
	stack.at(0)->GetHistogram()->SetMinimum(-0.5);
	stack.at(0)->GetHistogram()->SetMaximum(0.5);
	stack.at(0)->Draw("A");
	gPad->BuildLegend();
	canvas->cd(2);
	stack.at(1)->GetHistogram()->SetMinimum(-0.5);
	stack.at(1)->GetHistogram()->SetMaximum(0.5);
	stack.at(1)->Draw("A");
	gPad->BuildLegend();

	std::vector<Qn::DataContainer<Qn::Stats>> ratioSe;
	std::vector<Qn::DataContainer<Qn::Stats>> ratioComp;
	auto ratio = []( std::vector<Qn::DataContainer<Qn::Stats>> corr ){
		Qn::DataContainer<Qn::Stats> num = corr.at(0);
		Qn::DataContainer<Qn::Stats> den = corr.at(1);
		num = num.Rebin({"Centrality", 2, 20, 30});
		num = num.Projection({"0_Ycm"});
		den = den.Rebin({"Centrality", 2, 20, 30});
		den = den.Projection({"0_Ycm"});
		Qn::DataContainer<Qn::Stats> ratio = num/den;
		return ratio;
	};
	for( auto se : strSe )
	{
		ratioSe.push_back( helper.MadeComputations( {"Integral", se}, ratio, se+"_ratio" ) );
	}
	for( auto comp : strComp )
	{
		ratioComp.push_back( helper.MadeComputations( {"Integral", comp}, ratio, comp+"_ratio" ) );
	}
	// for( auto &ratio : ratioSe )
	// {
	// 	ratio = ratio.Rebin({"Centrality", 2, 20, 30});
	// 	ratio = ratio.Projection({"0_Ycm"});
	// }
	// for( auto &ratio : ratioComp )
	// {
	// 	ratio = ratio.Rebin({"Centrality", 2, 20, 30});
	// 	ratio = ratio.Projection({"0_Ycm"});
	// }
	vector<TMultiGraph*> ratioStack{
		new TMultiGraph("ratio_sub_events", ";y-y_{beam};v1_{ref}/v1_{se}"),
		new TMultiGraph("ratio_components", ";y-y_{beam};v1_{ref}/v1_{comp}")
	};
	k=0; 
	for( auto flow : ratioSe )
	{
		flow.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		tempGraph = Qn::DataContainerHelper::ToTGraph(flow);
		tempGraph->SetMarkerStyle( graphStyle.at(k) );
		tempGraph->SetMarkerColor( graphColor.at(k) );
		tempGraph->SetLineColor( graphColor.at(k) );
		tempGraph->SetLineWidth( 3.0 );
		tempGraph->SetMarkerSize( 1.5 );
		ratioStack.at(0)->Add( tempGraph, "P" );
		k++;
	}
	k=0; 
	for( auto flow : ratioComp )
	{
		flow.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		tempGraph = Qn::DataContainerHelper::ToTGraph(flow);
		tempGraph->SetMarkerStyle( graphStyle.at(k) );
		tempGraph->SetMarkerColor( graphColor.at(k) );
		tempGraph->SetLineColor( graphColor.at(k) );
		tempGraph->SetLineWidth( 3.0 );
		tempGraph->SetMarkerSize( 1.5 );
		ratioStack.at(1)->Add( tempGraph, "P" );
		k++;
	}
	canvas->cd(3);
	ratioStack.at(0)->GetHistogram()->SetMinimum(0.8);
	ratioStack.at(0)->GetHistogram()->SetMaximum(1.2);
	ratioStack.at(0)->Draw("A");
	canvas->cd(4);
	ratioStack.at(1)->GetHistogram()->SetMinimum(0.8);
	ratioStack.at(1)->GetHistogram()->SetMaximum(1.2);
	ratioStack.at(1)->Draw("A");
}