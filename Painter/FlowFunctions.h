Qn::DataContainer<Qn::Stats> *BuildResolution(TFile *file, bool bs=false);
Qn::DataContainer<Qn::Stats> *BuildFlow(TFile *file, bool bs=false);
Qn::DataContainer<Qn::Stats> *ObservableFlow(TFile *file, bool bs=false);
Qn::DataContainer<Qn::Stats> *ComputeFlow(Qn::DataContainer<Qn::Stats> *uq, Qn::DataContainer<Qn::Stats> *res, bool bs=false);
std::vector<TGraphAsymmErrors *> ToTGraphAsymmErrors(Qn::DataContainer<Qn::Stats> *container);

Qn::DataContainer<Qn::Stats> *BuildResolution(TFile *file, bool bs)
{
	Qn::DataContainer<Qn::Stats> *q1q2[2], *q2q3[2], *q1q3[2];
	std::vector<string> combination{"XX", "YY"};
	for (int i = 0; i < combination.size(); i++)
	{
		std::string name = "Fw1_Fw2_" + combination.at(i);
		file->GetObject(name.c_str(), q1q2[i]);
		// if(!bs)
		 	// q1q2[i]->CallOnElement([](Qn::Stats &stat){ stat.SetStatus(Qn::Stats::Status::POINTAVERAGE); });
		name = "Fw2_Fw3_" + combination.at(i);
		file->GetObject(name.c_str(), q2q3[i]);
		// if(!bs)
		 	// q2q3[i]->CallOnElement([](Qn::Stats &stat){ stat.SetStatus(Qn::Stats::Status::POINTAVERAGE); });
		name = "Fw1_Fw3_" + combination.at(i);
		file->GetObject(name.c_str(), q1q3[i]);
		// if(!bs)
		 	// q1q3[i]->CallOnElement([](Qn::Stats &stat){ stat.SetStatus(Qn::Stats::Status::POINTAVERAGE); });
	}
	
	auto res = new Qn::DataContainer<Qn::Stats>[6];
	int k = 0;

	for (int i = 0; i < 2; i++)
	{
		// for(int j=0; j<q1q2[i]->size(); j++)
			// q2q3[i]->At(j).SetStatus(Qn::Stats::Status::POINTAVERAGE);
		res[i] = (*q1q2[i]) * (*q1q3[i]) / (*q2q3[i] * 2.0);
		res[i] = Sqrt(res[k + i]);
	}
	k = 2;
	for (int i = 0; i < 2; i++)
	{
		res[2 + i] = (*q1q2[i]) * (*q1q3[i]) / (*q2q3[i] * 2.0);
		res[2 + i] = Sqrt(res[k + i]);
	}
	k = 4;
	for (int i = 0; i < 2; i++)
	{
		res[4 + i] = (*q1q2[i]) * (*q1q3[i]) / (*q2q3[i] * 2.0);
		res[4 + i] = Sqrt(res[k + i]);
	}
	if( bs )
	{
		for(int i=0; i<6; i++)
		{
			res[i].SetSetting(Qn::Stats::Settings::CORRELATEDERRORS);
		}
	}
	return res;
}

Qn::DataContainer<Qn::Stats> *ObservableFlow(TFile *file, bool bs)
{
	Qn::DataContainer<Qn::Stats> *uq[6];
	std::vector<string> combination = {"XX", "YY"};
	int k = 0;
	for (int i = 0; i < combination.size(); i++)
	{
		std::string name = "ProtonMdc_Fw1_" + combination.at(i);
		file->GetObject(name.c_str(), uq[i + k]);
	}
	k = 2;
	for (int i = 0; i < combination.size(); i++)
	{
		std::string name = "ProtonMdc_Fw2_" + combination.at(i);
		file->GetObject(name.c_str(), uq[i + k]);
	}
	k = 4;
	for (int i = 0; i < combination.size(); i++)
	{
		std::string name = "ProtonMdc_Fw3_" + combination.at(i);
		file->GetObject(name.c_str(), uq[i + k]);
	}
	auto uq1 = new Qn::DataContainer<Qn::Stats>[6];
	for (int i = 0; i < 6; i++)
	{
		uq1[i] = *uq[i];
		if(bs)
			uq1[i].SetSetting(Qn::Stats::Settings::CORRELATEDERRORS);
	}
	return uq1;
}

Qn::DataContainer<Qn::Stats> *ComputeFlow(Qn::DataContainer<Qn::Stats> *uq, Qn::DataContainer<Qn::Stats> *res, bool bs)
{
	auto flow = new Qn::DataContainer<Qn::Stats>[6];
	for (int i = 0; i < 6; i++)
	{
		flow[i] = uq[i] / res[i];
		if (bs)
			flow[i].SetSetting(Qn::Stats::Settings::CORRELATEDERRORS);
	}
	return flow;
}

vector<TGraphAsymmErrors *> ToTGraphAsymmErrors(Qn::DataContainer<Qn::Stats> *container)
{
	vector<TGraphAsymmErrors *> graph;
	for (int i = 0; i < 6; i++)
	{
		graph.push_back(Qn::DataContainerHelper::ToTGraph(container[i]));
	}
	return graph;
}

vector<TH1F *> ToTH1F(Qn::DataContainer<Qn::Stats> *container)
{
	vector<TH1F *> histo;
	for(int i=0; i<6; i++)
	{
		std::vector<Qn::Axis> axis = container[i].GetAxes();
		if( axis.size() > 1 )
		{
			cout << "Error: Only 1-dimension Data Containers can be converted to TH1" << endl;
			cout << axis.size() << " axis found" << endl;
			cout << i << " position in given array of containers" << endl;
			return histo;
		}
		std::string name = "DataContainer"+to_string(i); 
		histo.push_back( new TH1F(name.data(), "", container[i].size(), axis.back().GetLowerBinEdge(0), axis.back().GetUpperBinEdge( container[i].size()-1 ) ) );
		for(int j=0; j<container[i].size(); j++)
		{
			Qn::Stats bin = container[i].At(j);
			histo.back()->SetBinContent(j+1, bin.Mean());
			histo.back()->SetBinError(j+1, bin.Error());
		}
	}
	return histo;
}

Qn::DataContainer<Qn::Stats> *BuildFlow(TFile *file, bool bs)
{
	auto res = new Qn::DataContainer<Qn::Stats>[6];
	res = BuildResolution(file);
	auto uq = new Qn::DataContainer<Qn::Stats>[6];
	uq = ObservableFlow(file);
	auto flow = new Qn::DataContainer<Qn::Stats>[6];
	flow = ComputeFlow(uq, res, bs);
	return flow;
}