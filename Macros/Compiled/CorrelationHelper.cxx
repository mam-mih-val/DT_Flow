#include "CorrelationHelper.h"

Qn::DataContainer<Qn::Stats>& CorrelationHelper::GetDataContainer(std::string name){ 
	Qn::DataContainer<Qn::Stats> empty;
	Qn::DataContainer<Qn::Stats>* ptr{nullptr};
	if( fHeap.count(name) !=0 ) 
		return fHeap.at(name); 
	else
	{
		if(!fFile)
			return empty;
		fFile->GetObject(name.data(), ptr);
		if(!ptr)
			return empty;
		fHeap.insert( make_pair(name, *ptr) );
		return *ptr;
	}
}

Qn::DataContainer<Qn::Stats> CorrelationHelper::MakeComputations(
	std::vector<std::string> varNames,
	std::function<Qn::DataContainer<Qn::Stats>(std::vector<Qn::DataContainer<Qn::Stats>>)> lambda,
	std::string resultName
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

Qn::DataContainer<Qn::Stats> CorrelationHelper::Merge(std::vector<std::string> inputName, std::string outName)
{
	TList* array = new TList;
	Qn::DataContainer<Qn::Stats> out{GetDataContainer(inputName.at(0))};
	inputName.erase( inputName.begin() );
	std::vector<Qn::DataContainer<Qn::Stats>> forMerge;
	for( auto name : inputName )
	{
		array->AddLast( (Qn::DataContainer<Qn::Stats>*) &(this->GetDataContainer(name)) );
	}
	out.Merge( (TList*) array );
	fHeap.insert(make_pair(outName, out));
	return out;
}

std::vector<Qn::DataContainer<Qn::Stats>> CorrelationHelper::BuildResolution3Se(
	std::vector<std::string> corrNames,
	std::vector<std::string> resNames
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
	for( int i=0; i<3; i++ )
	{
		for( auto corr : corrNames )
			std::cout << corr << "\t";
		std::cout<< std::endl;
		result.push_back( MakeComputations(corrNames, R, resNames.at(i)) );
		corrNames = { corrNames.at(2), corrNames.at(0), corrNames.at(1) };
	}
	return result;
}

Qn::DataContainer<Qn::Stats> CorrelationHelper::BuildFullEvtResolution(
	std::string corrName,
	std::string resultName
)
{
	auto R = [](std::vector<Qn::DataContainer<Qn::Stats>> corr){
		Qn::DataContainer<Qn::Stats> result;
		result = ResFullEvent(corr.at(0));
		return result;
	};
	Qn::DataContainer<Qn::Stats> result;
	result = MakeComputations({corrName}, R, resultName);
	return result;
}

double CorrelationHelper::GetResolutionRs( double meanCosine )
{
	auto Rchi = [](double chi){
		using namespace TMath;
		double chi2 = chi*chi;
		double I0 = BesselI0(chi2*0.5);
		double I1 = BesselI1(chi2*0.5);
		double pi = Pi();
		double res = Sqrt( pi )*0.5*chi*Exp( -chi2/2.0 )*( I0+I1 );
		return res;
	};
	auto Dichtomy = []( 
		double a, 
		double b, 
		std::function<double(double)> f
	)
	{
		double D =  pow( 10, -6 );
		while( fabs(a-b) > D )
		{
			double c = 	0.5*(a+b);
			double fa = f(a);
			double fb = f(b);
			if( fa * fb > 0 )
			{
				std::cout << "there is no solution on this range" << endl;
				std::cout << "f(" << a << ")=" << fa << std::endl;
				std::cout << "f(" << b << ")=" << fb << std::endl;
				break;
			}
			double fc = f(c);
			if( fa * fc < 0.0 )
				b = c;
			if( fb * fc < 0.0 )
				a = c;
		}
		return 0.5*(a+b);
	};
	double min=0.0;
	double max=3.0;
	double chi0 = Dichtomy( min, max, [Rchi, meanCosine](double chi){ return Rchi(chi) - meanCosine; } );
	return Rchi( sqrt(2)*chi0 );
}

TH1F* CorrelationHelper::BuildResolutionFullEvent(
	std::string corrName,
	std::string resName
)
{
	Qn::DataContainer<Qn::Stats> container = GetDataContainer(corrName);
	container = Sqrt( container*2.0 );
	container.SetSetting( Qn::Stats::Settings::CORRELATEDERRORS );
	std::vector<Qn::Axis> axes = container.GetAxes();
	int  	nbins = axes.at(0).size();
	float  	xmin = axes.at(0).GetLowerBinEdge(0);
	float  	xmax = axes.at(0).GetUpperBinEdge(nbins-1);
	fRsResolution.push_back( new TH1F(resName.data(), "", nbins, xmin, xmax ) );
	for(int i=0; i<container.size(); i++)
	{
		Qn::Stats bin = container.At(i);
		float mean = bin.Mean();
		float error = bin.Error();
		float res = GetResolutionRs( mean );
		fRsResolution.back()->SetBinContent(i+1, res);
		fRsResolution.back()->SetBinError(i+1, error);
	}
	fHistoHeap.insert( make_pair(resName, fRsResolution.back()) );
	return fRsResolution.back();
}

Qn::DataContainer<Qn::Stats> CorrelationHelper::BuildFlowFullEvent(
	std::string uName,
	std::string resName,
	std::string resultName
)
{
	auto flow = [](std::vector<Qn::DataContainer<Qn::Stats>> corr){
		auto result = corr.at(0)/corr.at(1) * 2;
		return result;
	};
	auto result = MakeComputations( {uName, resName}, flow, resultName );
	return result;
}

std::vector<Qn::DataContainer<Qn::Stats>> CorrelationHelper::BuildFlow3Se(
	std::vector<std::string> unNames,
	std::vector<std::string> resNames,
	std::vector<std::string> flowNames
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
	for(int i=0; i<unNames.size(); i++)
	{
		std::vector<std::string> varNames = { unNames.at(i), resNames.at(i) };
		result.push_back( MakeComputations(varNames, flow, flowNames.at(i)) );
	}
	return result;
}

TH1F* CorrelationHelper::GetTh1f(std::string name)
{
	Qn::DataContainer<Qn::Stats> container = GetDataContainer(name);
	std::vector<Qn::Axis> axes = container.GetAxes();
	if( axes.size() > 1 )
	{
		std::cout << "Error: There are more than 1 axis" << std::endl;
		return nullptr;
	}
	int  	nbins = axes.at(0).size();
	float  	xmin = axes.at(0).GetLowerBinEdge(0);
	float  	xmax = axes.at(0).GetUpperBinEdge(nbins-1);
	std::string histoName = name+"_histo";
	// std::cout << "xmin=" << xmin << " xmax=" << xmax << " nbins=" << nbins << endl;
	auto histo = new TH1F(histoName.data(), axes.at(0).Name().data(), nbins, xmin, xmax);
	int entries = 0;
	for( int i=0; i<container.size(); i++ )
	{
		Qn::Stats bin = container.At(i);
		entries+=bin.GetProfile().Entries();
		float mean = bin.Mean();
		float error = bin.Error();
		// std::cout << "bin: " << i << " mean:" << mean << " error:" << error << std::endl;
		histo->SetBinContent(i+1, mean);
		histo->SetBinError(i+1, error);
		// std:: cout << histo->GetBinContent( i+1 ) << std::endl;
	}
	histo->SetEntries(entries);
	axes.clear();
	return histo;
}

std::vector<TH1F*> CorrelationHelper::GetVectorTh1f( std::vector<std::string> names )
{
	std::vector<TH1F*> vecHisto;
	for( auto name : names )
		vecHisto.push_back( GetTh1f(name) );
	return vecHisto;
}

Qn::DataContainer<Qn::Stats> CorrelationHelper::BuildRatio(
		std::vector<std::string> corr,
		std::string resultName
)
{
	auto ratio = []( std::vector<Qn::DataContainer<Qn::Stats>> corr ){
		Qn::DataContainer<Qn::Stats> result_;
		if( corr.size() != 2 )
		{
			std::cout << "Arguments amount error" << std::endl;
			return result_;
		}
		result_ = corr.at(0)/corr.at(1);
		// result_.SetSetting(Qn::Stats::Settings::CORRELATEDERRORS);
		return result_;
	};
	return MakeComputations(corr, ratio, resultName);
}