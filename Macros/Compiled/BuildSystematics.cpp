#pragma once
#include "Stats.h"
#include "DataContainer.h"
#include "DataContainerHelper.h"
#include "TH1F.h"
#include "TFile.h"
#include "CorrelationHelper.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "THStack.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TPad.h"
#include "SetStyle.h"
#include "TRatioPlot.h"

void BuildSystematics( std::string fileName="../../Output/Bs.root", std::string canvasName="jenya" )
{
	CorrelationHelper helper;
	helper.SetFile(fileName);
	// Q-vector correlations names
	vector<std::string> corrX = { "Fw1_Fw2_XX", "Fw1_Fw3_XX", "Fw2_Fw3_XX" }; 
	vector<std::string> corrY = { "Fw1_Fw2_YY", "Fw1_Fw3_YY", "Fw2_Fw3_YY" };
	// Resolution names
	std::vector<std::string> resNamesX = {"resolution_Fw1_XX", "resolution_Fw2_XX", "resolution_Fw3_XX"};
	std::vector<std::string> resNamesY = {"resolution_Fw1_YY", "resolution_Fw2_YY", "resolution_Fw3_YY"};
	// Computing resolution
	helper.BuildResolution3Se( corrX, resNamesX );
	helper.BuildResolution3Se( corrY, resNamesY );
	// <u,Q> correlations names
	vector<std::string> unX = { "ProtonMdc_Fw1_XX", "ProtonMdc_Fw2_XX", "ProtonMdc_Fw3_XX" };
	vector<std::string> unY = { "ProtonMdc_Fw1_YY", "ProtonMdc_Fw2_YY", "ProtonMdc_Fw3_YY" };
	// Flow names
	std::vector<std::string> flowNamesX = {"flow_Fw1_XX", "flow_Fw2_XX", "flow_Fw3_XX"};
	std::vector<std::string> flowNamesY = {"flow_Fw1_YY", "flow_Fw2_YY", "flow_Fw3_YY"};
	// Computing flow
	helper.BuildFlow3Se(unX, resNamesX, flowNamesX);
	helper.BuildFlow3Se(unY, resNamesY, flowNamesY);
	// list of flow containers to merge to get averaged on sub-events and components
	std::vector<std::string> listIntegrated;
	std::vector<std::string> flowComp = { "flow_XX", "flow_YY" };
	std::vector<std::string> flowSe = { "flow_Fw1", "flow_Fw2", "flow_Fw3" };
	for(uint i=0; i<flowNamesX.size(); i++)
	{
		listIntegrated.push_back(flowNamesX.at(i));
		listIntegrated.push_back(flowNamesY.at(i));
		std::vector<std::string> se={flowNamesX.at(i), flowNamesY.at(i)};
		helper.Merge( se, flowSe.at(i) );
	}
	// averaging on sub-events & components
	helper.Merge(listIntegrated, "flow_refference");
	helper.Merge( flowNamesX, flowComp.at(0) );
	helper.Merge( flowNamesY, flowComp.at(1) );
	
	// lambda function for projection and recomputing errors using bootstrapping
	auto rebinProj = [](std::vector<Qn::DataContainer<Qn::Stats>> container){
		container.at(0) = container.at(0).Rebin({"Centrality", 2, 20, 30});
		container.at(0) = container.at(0).Projection({"0_Ycm"});
		container.at(0).SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		return container.at(0);
	};
	auto ErrProp = [](std::vector<Qn::DataContainer<Qn::Stats>> container){
		container.at(0).SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
		return container.at(0);
	};
	for( auto &flow : flowComp )
	{
		helper.MakeComputations({flow}, rebinProj, flow+"_1d");
		flow+="_1d";
	}
	for( auto &flow : flowSe )
	{
		helper.MakeComputations({flow}, rebinProj, flow+"_1d");
		flow+="_1d";
	}
	helper.MakeComputations({"flow_refference"}, rebinProj, "flow_refference_1d");
	
	auto setDrawSettings = [](TH1F* histo, std::string title, int color, int style){
		histo->SetTitle(title.data());
		histo->SetLineColor(color);
		histo->SetMarkerColor(color);
		histo->SetMarkerStyle(style);
		histo->SetMarkerSize( 1.5 );
		histo->SetLineWidth( 3.0 );
	};
	std::vector<std::string> ratioComp;
	for( auto flow : flowComp )
	{
		helper.BuildRatio({"flow_refference_1d", flow}, "ratio_"+flow);
		ratioComp.push_back("ratio_"+flow);
	}
	std::vector<std::string> ratioSe;
	for( auto flow : flowSe )
	{
		helper.BuildRatio({"flow_refference_1d", flow}, "ratio_"+flow);
		ratioSe.push_back("ratio_"+flow);
	}
	TH1F* histoFlowRefference = helper.GetTh1f("flow_refference_1d");
	vector<TH1F*> histoFlowComp = helper.GetVectorTh1f(flowComp);
	vector<TH1F*> histoRatioComp = helper.GetVectorTh1f(ratioComp);
	vector<TH1F*> histoFlowSe = helper.GetVectorTh1f(flowSe);
	vector<TH1F*> histoRatioSe = helper.GetVectorTh1f(ratioSe);

	
	std::vector<int> histoStyle{ 21, 22, 23 };
	std::vector<int> histoColor{ kRed+1, kGreen+1, kBlue+1 };
	std::vector<THStack*> stack {
		new THStack( "Components", ";y-y_{beam};v_{1}" ),
		new THStack( "Sub-Events", ";y-y_{beam};v_{1}" ),
		new THStack( "Components_ratio", ";y-y_{beam}" ),
		new THStack( "Sub-Events_ratio", ";y-y_{beam}" )
	};
	std::vector<std::string> histoTitle = {"v_{1}^{x}", "v_{1}^{y}"};
	for( uint i=0; i<histoFlowComp.size(); i++ )
	{
		setDrawSettings(histoFlowComp.at(i), histoTitle.at(i), histoColor.at(i), histoStyle.at(i) );
		setDrawSettings(histoRatioComp.at(i), "ratio_"+histoTitle.at(i), histoColor.at(i), histoStyle.at(i) );
		stack.at(0)->Add(histoFlowComp.at(i));
		stack.at(2)->Add( histoRatioComp.at(i) );
	}
	histoTitle = {"v_{1}^{a}", "v_{1}^{b}", "v_{1}^{c}"};
	
	for( uint i=0; i<histoFlowSe.size(); i++ )
	{
		setDrawSettings(histoFlowSe.at(i), histoTitle.at(i), histoColor.at(i), histoStyle.at(i) );
		setDrawSettings(histoRatioSe.at(i), "ratio_"+histoTitle.at(i), histoColor.at(i), histoStyle.at(i) );
		stack.at(1)->Add(histoFlowSe.at(i));
		stack.at(3)->Add(histoRatioSe.at(i));
	}
	setDrawSettings(histoFlowRefference, "Refference", kBlack, 20 );
	stack.at(0)->Add(histoFlowRefference);
	stack.at(1)->Add(histoFlowRefference);
	SetStyle();
	std::vector<TCanvas*> canvas;
	canvas.push_back(new TCanvas("Components", "", 2100, 1000));
	canvas.at(0)->Divide(2,1);
	canvas.at(0)->cd(1);
	stack.at(0)->SetMinimum(-0.5);
	stack.at(0)->SetMaximum(0.5);
	stack.at(0)->Draw("NOSTACK");
	gPad->BuildLegend();
	canvas.at(0)->cd(2);
	stack.at(2)->SetMinimum(0.9);
	stack.at(2)->SetMaximum(1.1);
	stack.at(2)->Draw("NOSTACK");
	auto name = canvasName+"_1.png";
	canvas.at(0)->Print(name.data());
	canvas.push_back( new TCanvas("Sub-Events", "", 2100, 1000));
	canvas.at(1)->Divide(2,1);
	canvas.at(1)->cd(1);
	stack.at(1)->SetMinimum(-0.5);
	stack.at(1)->SetMaximum(0.5);
	stack.at(1)->Draw("NOSTACK");
	gPad->BuildLegend();
	canvas.at(1)->cd(2);
	stack.at(3)->SetMinimum(0.9);
	stack.at(3)->SetMaximum(1.1);
	stack.at(3)->Draw("NOSTACK");
	name = canvasName+"_2.png";
	canvas.at(1)->Print(name.data());
	helper.SaveToFile(canvasName+".root");
}