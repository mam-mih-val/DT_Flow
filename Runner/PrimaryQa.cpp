#include "../Painter/SetStyle.h"
void PrimaryQa(std::string name="output_2.root")
{
	std::unique_ptr<TFile> file{TFile::Open( name.data() )};
	std::unique_ptr<TTree> tree{ (TTree*) file->Get("tree")};
	auto* fw1Sp = new Qn::DataContainer<Qn::QVector>;
	auto* fw2Sp = new Qn::DataContainer<Qn::QVector>;
	auto* fw3Sp = new Qn::DataContainer<Qn::QVector>;
	auto* rs1Sp = new Qn::DataContainer<Qn::QVector>;
	auto* rs2Sp = new Qn::DataContainer<Qn::QVector>;
	auto* full 	= new Qn::DataContainer<Qn::QVector>;
	tree->SetBranchAddress( "Fw1Sp", &fw1Sp );
	tree->SetBranchAddress( "Fw2Sp", &fw2Sp );
	tree->SetBranchAddress( "Fw3Sp", &fw3Sp );
	tree->SetBranchAddress( "Rs1Sp", &rs1Sp );
	tree->SetBranchAddress( "Rs2Sp", &rs2Sp );
	tree->SetBranchAddress( "Full",  &full 	);
	float centrality;
	tree->SetBranchAddress( "Centrality", &centrality );
	std::vector<TH2F*> weghtVsCent{
		new TH2F( "Fw1Sp", "Fw1;centrality[%];Sum of weights [ADC]", 100, 0., 100., 100, 0., 5000. ),
		new TH2F( "Fw2Sp", "Fw2;centrality[%];Sum of weights [ADC]", 100, 0., 100., 100, 0., 5000. ),
		new TH2F( "Fw3Sp", "Fw3;centrality[%];Sum of weights [ADC]", 100, 0., 100., 100, 0., 5000. ),
		new TH2F( "Rs1Sp", "Rs1;centrality[%];Sum of weights [ADC]", 100, 0., 100., 100, 0., 5000. ),
		new TH2F( "Rs2Sp", "Rs2;centrality[%];Sum of weights [ADC]", 100, 0., 100., 100, 0., 5000. ),
		new TH2F( "Full",  "Full;centrality[%];Sum of weights [ADC]",100, 0., 100., 100, 0., 5000. )
	};
	uint nEvents = tree->GetEntries();
	for(uint i=0; i<nEvents; i++)
	{
		tree->GetEntry(i);
		weghtVsCent.at(0)->Fill( centrality, fw1Sp->At(0).sumweights() );
		weghtVsCent.at(1)->Fill( centrality, fw2Sp->At(0).sumweights() );
		weghtVsCent.at(2)->Fill( centrality, fw3Sp->At(0).sumweights() );
		weghtVsCent.at(3)->Fill( centrality, rs1Sp->At(0).sumweights() );
		weghtVsCent.at(4)->Fill( centrality, rs2Sp->At(0).sumweights() );
		weghtVsCent.at(5)->Fill( centrality, full->At(0).sumweights() );
	}
	SetStyle();
	auto canvas = new TCanvas( "Qn_Qa", "", 3150, 2000 );
	canvas->Divide(3,2);
	for( int i=0; i<weghtVsCent.size(); i++ )
	{
		canvas->cd(i+1)->SetLogz();
		weghtVsCent.at(i)->Draw("colz");
	}
	canvas->Print("CheckingSumW.C");
}