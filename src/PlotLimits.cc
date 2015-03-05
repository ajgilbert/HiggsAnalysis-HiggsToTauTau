#include "HiggsAnalysis/HiggsToTauTau/interface/PlotLimits.h"
TList* contourFromTH2(TH2 *h2in, double threshold, int minPoints=20, bool require_minPoints=true);
struct STestFunctor{
  STestFunctor() { sum = 0; }
  void operator() (TObject *aObj) {sum +=1;}
  int sum;
} stestfunctorpl; 


PlotLimits::PlotLimits(const char* output, const edm::ParameterSet& cfg) : 
  output_(output),
  dataset_(cfg.getParameter<std::string>("dataset")),
  xaxis_(cfg.getParameter<std::string>("xaxis")),
  yaxis_(cfg.getParameter<std::string>("yaxis")),
  min_  (cfg.existsAs<double>("min" ) ? cfg.getParameter<double>("min" ) :    0.),
  max_  (cfg.existsAs<double>("max" ) ? cfg.getParameter<double>("max" ) :   -1.),
  log_  (cfg.existsAs<int   >("log" ) ? cfg.getParameter<int   >("log" ) :     0),
  png_  (cfg.existsAs<bool  >("png" ) ? cfg.getParameter<bool  >("png" ) : false),
  pdf_  (cfg.existsAs<bool  >("pdf" ) ? cfg.getParameter<bool  >("pdf" ) : false),
  txt_  (cfg.existsAs<bool  >("txt" ) ? cfg.getParameter<bool  >("txt" ) : false),
  root_ (cfg.existsAs<bool  >("root") ? cfg.getParameter<bool  >("root") : false),
  parabolic_ (cfg.existsAs<bool  >("parabolic") ? cfg.getParameter<bool  >("parabolic") : false),
  mssm_ (cfg.existsAs<bool  >("mssm") ? cfg.getParameter<bool  >("mssm") : false),
  mssm_nolog_ (cfg.existsAs<bool  >("mssm_nolog") ? cfg.getParameter<bool  >("mssm_nolog") : false)
{
  // mass-points
  bins_=cfg.getParameter<std::vector<double> >("masspoints");
  for(unsigned int i=0; i<bins_.size(); ++i){ valid_.push_back(true);}

  // minima as function of mass (if available as such)
  if(cfg.existsAs<std::vector<edm::ParameterSet> >("xmins")){
    std::vector<edm::ParameterSet> mins = cfg.getParameter<std::vector<edm::ParameterSet> >("xmins");
    for(std::vector<edm::ParameterSet>::const_iterator m=mins.begin(); m!=mins.end(); ++m){
      double key = m->getParameter<double>("mass");
      double val = m->getParameter<double>("min");
      xmins_[key] = val;
    }
  }
  if(cfg.existsAs<std::vector<edm::ParameterSet> >("ymins")){
    std::vector<edm::ParameterSet> mins = cfg.getParameter<std::vector<edm::ParameterSet> >("ymins");
    for(std::vector<edm::ParameterSet>::const_iterator m=mins.begin(); m!=mins.end(); ++m){
      double key = m->getParameter<double>("mass");
      double val = m->getParameter<double>("min");
      ymins_[key] = val;
    }
  }
  // maxima as function of mass (if available as such)
  if(cfg.existsAs<std::vector<edm::ParameterSet> >("xmaxs")){
    std::vector<edm::ParameterSet> maxs = cfg.getParameter<std::vector<edm::ParameterSet> >("xmaxs");
    for(std::vector<edm::ParameterSet>::const_iterator m=maxs.begin(); m!=maxs.end(); ++m){
      double key = m->getParameter<double>("mass");
      double val = m->getParameter<double>("max");
      xmaxs_[key] = val;
    }
  }
  if(cfg.existsAs<std::vector<edm::ParameterSet> >("ymaxs")){
    std::vector<edm::ParameterSet> maxs = cfg.getParameter<std::vector<edm::ParameterSet> >("ymaxs");
    for(std::vector<edm::ParameterSet>::const_iterator m=maxs.begin(); m!=maxs.end(); ++m){
      double key = m->getParameter<double>("mass");
      double val = m->getParameter<double>("max");
      ymaxs_[key] = val;
    }
  }
  // common configs
  theory_ = cfg.existsAs<std::string>("theory") ? cfg.getParameter<std::string>("theory") : std::string();
  label_ = cfg.existsAs<std::string>("outputLabel") ? cfg.getParameter<std::string>("outputLabel") : std::string();
  verbosity_ = cfg.existsAs<unsigned int>("verbosity") ? cfg.getParameter<unsigned int>("verbosity") : 0;
  // specifics to plot signal strength
  signal_ = cfg.existsAs<std::string>("signal") ? cfg.getParameter<std::string>("signal") : std::string();
  // specifics to plot scan-2d
  model_ = cfg.existsAs<std::string>("model") ? cfg.getParameter<std::string>("model") : std::string();
  temp_  = cfg.existsAs<bool>("temp") ? cfg.getParameter<bool>("temp") : false;
  smooth_= cfg.existsAs<bool>("smooth") ? cfg.getParameter<bool>("smooth") : false;
  // specifics to plot signal strength
  bestfit_ = cfg.existsAs<bool>("bestfit") ? cfg.getParameter<bool>("bestfit") : false;
  drawsm_ = cfg.existsAs<bool>("drawSM") ? cfg.getParameter<bool>("drawSM") : false;
  // specifics to plot xsec limits
  injectedMass_ = cfg.existsAs<std::string>("injectedMass") ? cfg.getParameter<std::string>("injectedMass") : std::string();
  extra_    = cfg.existsAs<std::string>("extra") ? cfg.getParameter<std::string>("extra") : std::string();
  injected_ = cfg.existsAs<bool>("injected") ? cfg.getParameter<bool>("injected") : false;
  BG_Higgs_ = cfg.existsAs<bool>("higgsBG") ? cfg.getParameter<bool>("higgsBG") : false;
  // specifics to plot MSSM mA-tanb limits
  arXiv_1211_6956_ =cfg.existsAs<bool>("arXiv_1211_6956" ) ? cfg.getParameter<bool>("arXiv_1211_6956" ) : false;
  arXiv_1204_2760_ =cfg.existsAs<bool>("arXiv_1204_2760" ) ? cfg.getParameter<bool>("arXiv_1204_2760" ) : false;
  arXiv_1302_2892_ =cfg.existsAs<bool>("arXiv_1302_2892" ) ? cfg.getParameter<bool>("arXiv_1302_2892" ) : false;
  arXiv_1205_5736_ =cfg.existsAs<bool>("arXiv_1205_5736" ) ? cfg.getParameter<bool>("arXiv_1205_5736" ) : false;
  HIG_12_052_      =cfg.existsAs<bool>("HIG_12_052"      ) ? cfg.getParameter<bool>("HIG_12_052"      ) : false;
  higgs125_ =cfg.existsAs<bool>("higgs125" ) ? cfg.getParameter<bool>("higgs125" ) : false;
  linearFit_=cfg.existsAs<bool>("linearFit") ? cfg.getParameter<bool>("linearFit") : false;
  transparent_=cfg.existsAs<bool>("transparent") ? cfg.getParameter<bool>("transparent") : false;
  expectedOnly_=cfg.existsAs<bool>("expectedOnly") ? cfg.getParameter<bool>("expectedOnly") : false;
  MSSMvsSM_=cfg.existsAs<bool  >("MSSMvsSM") ? cfg.getParameter<bool  >("MSSMvsSM") : false;
  Brazilian_=cfg.existsAs<bool  >("Brazilian") ? cfg.getParameter<bool  >("Brazilian") : false;
}


  TGraph*
PlotLimits::fillCentral(const char* directory, TGraph* plot, const char* filename, const char* low_tanb /*=""*/)
{
  std::vector<double> central;
  // fill pre-defined values from previous results
  if (std::string(filename).find("HIG")!=std::string::npos){
    bool initial = masses_.empty();
    for(unsigned int imass=0; imass<bins_.size(); ++imass){
      if(std::string(filename)==std::string("HIG-11-020-obs")){
	prepareHIG_11_020(central, "observed", false, bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-11-020-exp")){
	prepareHIG_11_020(central, "expected", false, bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-11-029-obs")){
	prepareHIG_11_029(central, "observed", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-11-029-exp")){
	prepareHIG_11_029(central, "expected", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-018-obs")){
	prepareHIG_12_018(central, "observed", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-018-exp")){
	prepareHIG_12_018(central, "expected", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-032-obs")){
	prepareHIG_12_032(central, "observed", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-032-exp")){
	prepareHIG_12_032(central, "expected", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-043-obs")){
	prepareHIG_12_043(central, "observed", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-043-exp")){
	prepareHIG_12_043(central, "expected", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-050-obs")){
	prepareHIG_12_050(central, "observed", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-12-050-exp")){
	prepareHIG_12_050(central, "expected", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-13-004-obs")){
	prepareHIG_13_004(central, "observed", bins_[imass], initial);
      }
      else if(std::string(filename)==std::string("HIG-13-004-exp")){
	prepareHIG_13_004(central, "expected", bins_[imass], initial);
      }
    }
  }
  else{
    if(std::string(filename)==std::string("MEDIAN") || std::string(filename)==std::string("MEAN")){
      prepareByToy(directory, central, filename);
    }
    else if(std::string(filename).find("obs.Asymptotic") != std::string::npos){
      prepareByValue(directory, central, "higgsCombine-obs.Asymptotic.mH$MASS", -1.);
    }
    else if(std::string(filename).find("exp.Asymptotic") != std::string::npos){
      prepareByValue(directory, central, "higgsCombine-exp.Asymptotic.mH$MASS", 0.5);
    }
    else if(std::string(filename).find("MaxLikelihood") != std::string::npos){
      prepareByValue(directory, central, filename, -1.);
    }
    else if(std::string(filename).find("NLL") != std::string::npos){
      prepareByFitOutput(directory, central, "out/mlfit", "tree_fit_sb", "nll_min");
    }
    else{
      if(std::string(low_tanb)==std::string("low")){
	prepareByFile(directory, central, filename, "low");
      }
      else{ 
	prepareByFile(directory, central, filename);
      }
    }
  }
  bool first_low=true;
  for(unsigned int imass=0, ipoint=0; imass<bins_.size(); ++imass){
    if(valid_[imass] && std::string(filename).find("HIG")==std::string::npos){
      if(mssm_){
	// fill the upper limit contour for MSSM. The lowest value in the MSSM tanb scan is 0.5. If
	// central value is 0.5 for the upper limit this mean that there is now upper bound found. 
	// For such cases it is necessary to consider the first and last point before and after the 
	// uppler limit exclusion starts to be able to close the contour for plotting. 
	if(!first_low){
	  if((central[imass]==0.5 && central[imass-1]>0.5) || central[imass]!=0.5) {
	    plot->SetPoint(ipoint++, bins_[imass], central[imass]);
	    //std::cout<< "lastlow" << central[imass] << " " << bins_[imass] << std::endl;
	  }
	}
	if(first_low){
	  if((central[imass]==0.5 && central[imass+1]>0.5) || central[imass]!=0.5) {
	    plot->SetPoint(ipoint++, bins_[imass], central[imass]); first_low=false;
	    //std::cout<< "firstlow" << central[imass] << " "  << central[imass+1] << " "<< bins_[imass] << std::endl;
	  }
	}
      }
      else {
	plot->SetPoint(ipoint++, bins_[imass], central[imass]);
      }
      if(verbosity_>1){ std::cout << "INFO: central [" << bins_[imass] << "] = " << central[imass] << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl; }
    }
    else{
      if(valid_[imass]){
	for(unsigned int jmass=0; jmass<masses_.size(); ++jmass){
	  if(masses_[jmass]==bins_[imass]){
	    plot->SetPoint(ipoint++, masses_[jmass], central[jmass]);  
	    if(verbosity_>1){ 
	      std::cout << "INFO: central [" << masses_[jmass] << "] = " << central[jmass] << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl; 
	      std::cout << plot->GetN() << " " << plot->GetX()[imass] << " " << plot->GetY()[imass] << std::endl;
	    }
	    break;
	  }
	}
      }
    }
  }
  return plot;
}
  TGraphAsymmErrors*
PlotLimits::fillBand(const char* directory, TGraphAsymmErrors* plot, const char* method, bool innerBand, const char* low_tanb /*=""*/)
{
  std::vector<double> upper, lower, expected;

  if (std::string(method).find("HIG")!=std::string::npos){
    bool initial = masses_.empty();
    for(unsigned int imass=0; imass<bins_.size(); ++imass){
      if(std::string(method) == std::string("HIG-11-020")){
	bool xsec = false;
	prepareHIG_11_020(expected, "expected"                       , xsec, bins_[imass], initial);
	prepareHIG_11_020(upper   , innerBand ? "+1sigma" : "+2sigma", xsec, bins_[imass], initial);
	prepareHIG_11_020(lower   , innerBand ? "-1sigma" : "-2sigma", xsec, bins_[imass], initial);
      }
      else if(std::string(method) == std::string("HIG-11-029")){
	prepareHIG_11_029(expected, "expected"                       , bins_[imass], initial);
	prepareHIG_11_029(upper   , innerBand ? "+1sigma" : "+2sigma", bins_[imass], initial);
	prepareHIG_11_029(lower   , innerBand ? "-1sigma" : "-2sigma", bins_[imass], initial);
      }
      else if(std::string(method) == std::string("HIG-12-018")){
	prepareHIG_12_018(expected, "expected"                       , bins_[imass], initial);
	prepareHIG_12_018(upper   , innerBand ? "+1sigma" : "+2sigma", bins_[imass], initial);
	prepareHIG_12_018(lower   , innerBand ? "-1sigma" : "-2sigma", bins_[imass], initial);
      }
      else if(std::string(method) == std::string("HIG-12-032")){
	prepareHIG_12_032(expected, "expected"                       , bins_[imass], initial);
	prepareHIG_12_032(upper   , innerBand ? "+1sigma" : "+2sigma", bins_[imass], initial);
	prepareHIG_12_032(lower   , innerBand ? "-1sigma" : "-2sigma", bins_[imass], initial);
      }
      else if(std::string(method) == std::string("HIG-12-043")){
	prepareHIG_12_043(expected, "expected"                       , bins_[imass], initial);
	prepareHIG_12_043(upper   , innerBand ? "+1sigma" : "+2sigma", bins_[imass], initial);
	prepareHIG_12_043(lower   , innerBand ? "-1sigma" : "-2sigma", bins_[imass], initial);
      }
      else if(std::string(method) == std::string("HIG-12-050")){
	prepareHIG_12_050(expected, "expected"                       , bins_[imass], initial);
	prepareHIG_12_050(upper   , innerBand ? "+1sigma" : "+2sigma", bins_[imass], initial);
	prepareHIG_12_050(lower   , innerBand ? "-1sigma" : "-2sigma", bins_[imass], initial);
      }
      else if(std::string(method) == std::string("HIG-13-004")){
	prepareHIG_13_004(expected, "expected"                       , bins_[imass], initial);
	prepareHIG_13_004(upper   , innerBand ? "+1sigma" : "+2sigma", bins_[imass], initial);
	prepareHIG_13_004(lower   , innerBand ? "-1sigma" : "-2sigma", bins_[imass], initial);
      }
    }
  }
  else{
    if(std::string(method) == std::string("TOYBASED")){
      prepareByToy(directory, expected, "MEDIAN");
      prepareByToy(directory, upper   , innerBand ? "+1SIGMA" : "+2SIGMA");
      prepareByToy(directory, lower   , innerBand ? "-1SIGMA" : "-2SIGMA");
    }
    else if(std::string(method) == std::string("CLs")){
      if(std::string(low_tanb)==std::string("low")){
	prepareCLs(directory, expected, ".quant0.500", "low");
	prepareCLs(directory, upper   , innerBand ? ".quant0.840" : ".quant0.975", "low");
	prepareCLs(directory, lower   , innerBand ? ".quant0.160" : ".quant0.027", "low");
      }
      else{
	prepareCLs(directory, expected, ".quant0.500");
	prepareCLs(directory, upper   , innerBand ? ".quant0.840" : ".quant0.975");
	prepareCLs(directory, lower   , innerBand ? ".quant0.160" : ".quant0.027");
      }
    }
    else if(std::string(method).find("Asymptotic") != std::string::npos){
      prepareByValue(directory, expected, method, 0.50);
      prepareByValue(directory, upper   , method, innerBand ? 0.84 : 0.975);
      prepareByValue(directory, lower   , method, innerBand ? 0.16 : 0.025);
    }
    else if(std::string(method).find("MaxLikelihood") != std::string::npos){
      prepareByValue(directory, expected, method, -1);
      prepareByValue(directory, upper, method, 0.84);
      prepareByValue(directory, lower, method, 0.16);
    }
    else{
      std::cout << "ERROR: chose wrong method to fill uncertainty band: " << method << std::endl;
      exit(1);
    }
  }
  for(unsigned int imass=0, ipoint=0; imass<bins_.size(); ++imass){
    if(valid_[imass] && std::string(method).find("HIG")==std::string::npos){
      plot->SetPoint(ipoint, bins_[imass], expected[imass]);
      plot->SetPointEYhigh(ipoint, upper[imass] - expected[imass]);
      plot->SetPointEYlow (ipoint, expected[imass] - lower[imass]);
      ++ipoint; // only add valid mass points to the TGraph
      if(verbosity_>1){
	std::cout << "INFO: Calculating " << (innerBand ? "inner" : "outer") << " uncertainty band" << std::endl;
	std::cout << "INFO: upper    [" << bins_[imass] << "] = " << " (" << upper[imass] << ") " << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl;
	std::cout << "INFO: expected [" << bins_[imass] << "] = " << expected[imass]          << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl;
	std::cout << "INFO: lower    [" << bins_[imass] << "] = " << " (" << lower[imass] << ") " << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl;
      }
    }
    else{
      if(valid_[imass]){
	for(unsigned int jmass=0; jmass<masses_.size(); ++jmass){
	  if(masses_[jmass]==bins_[imass]){
	    plot->SetPoint(ipoint, masses_[jmass], expected[jmass]);
	    plot->SetPointEYhigh(ipoint, upper[jmass] - expected[jmass]);
	    plot->SetPointEYlow (ipoint, expected[jmass] - lower[jmass]);
	    ++ipoint; // only add valid mass points to the TGraph
	    if(verbosity_>1){
	      std::cout << "INFO: Calculating " << (innerBand ? "inner" : "outer") << " uncertainty band" << std::endl;
	      std::cout << "INFO: upper    [" << masses_[jmass] << "] = " << " (" << upper[jmass] << ") " << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl;
	      std::cout << "INFO: expected [" << masses_[jmass] << "] = " << expected[jmass]          << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl;
	      std::cout << "INFO: lower    [" << masses_[jmass] << "] = " << " (" << lower[jmass] << ") " << "[" << (valid_[imass] ? "OK]" : "FAILED]") << std::endl;
	    }
	    break;
	  }
	}
      }
    }
  }
  return plot;
}

  float
PlotLimits::maximum(TGraph* graph)
{
  float maximum = -1;
  if(max_<0){
    for(int ibin=0; ibin<graph->GetN(); ++ibin){
      if(maximum<0 || graph->GetY()[ibin]>maximum){
	maximum=graph->GetY()[ibin];
      }
    }
    maximum*=2.5;
  }
  else{
    maximum=max_;
  }
  std::cout << "M A X I M U M   I S : " << maximum << std::endl;
  return maximum;
}

  float
PlotLimits::minimum(TGraph* graph)
{
  float minimum = 99999.;
  if(min_<0){
    for(int ibin=0; ibin<graph->GetN(); ++ibin){
      if(graph->GetY()[ibin]<minimum){
	minimum=graph->GetY()[ibin];
      }
    }
    minimum/=2.5;
  }
  else{
    minimum=min_;
  }
  std::cout << "M I N I M U M   I S : " << minimum << std::endl;
  return minimum;
}

  TGraphAsymmErrors* 
PlotLimits::higgsConstraint(TH2D* plane_expected, double mass, double deltaM, const char* model, const char* type)
{
  TGraphAsymmErrors* graph = new TGraphAsymmErrors();

  for(int imass=0, ipoint=0; imass<plane_expected->GetNbinsX(); ++imass){
    //for(int i=300, ipoint=0; i<3101;i=1+100){
    std::string line;
    bool filled = false;
    float tanb_save=-99.0, tanb, mh, mA, mH, upperTanb=-1., lowerTanb=-1.;
    double x_save=plane_expected->GetXaxis()->GetBinUpEdge(imass);

    //double x_save=(int)i;
    ifstream higgs (TString::Format("HiggsAnalysis/HiggsToTauTau/data/Higgs125/%s/higgs_%d.dat", model, (int)x_save)); 
    //std::cout << TString::Format("HiggsAnalysis/HiggsToTauTau/data/Higgs125/%s/higgs_%d.dat", model, (int)x_save) << std::endl;
    if(higgs.is_open()){
      while(higgs.good()){
	getline(higgs,line);
	sscanf(line.c_str(),"%f %f %f %f", &tanb, &mh, &mA, &mH);
	if (TString::Format(model)=="lowmH") {
	  if(fabs(mh-mass)<deltaM && tanb!=tanb_save){
	    if(!filled){
	      graph->SetPoint(ipoint, x_save, tanb); 
	      graph->SetPointEYlow(ipoint, 0.);
	      tanb_save=tanb;
	      ipoint++; filled = true;
	      lowerTanb=tanb;
	    }
	    upperTanb=tanb;
	  }
	}
	else{
	  if (TString::Format(type)=="h") {
	    if(fabs(mh-mass)<deltaM && tanb!=tanb_save){
	      if(!filled){
		graph->SetPoint(ipoint, x_save, tanb); 
		graph->SetPointEYlow(ipoint, 0.);
		tanb_save=tanb;
		ipoint++; filled = true;
		lowerTanb=tanb;
	      }
	      upperTanb=tanb;
	    }
	  }
	  else if(TString::Format(type)=="H") {
	    if(fabs(mH-mass)<deltaM && tanb!=tanb_save){
	      if(!filled){
		graph->SetPoint(ipoint, x_save, tanb); 
		graph->SetPointEYlow(ipoint, 0.);
		tanb_save=tanb;
		ipoint++; filled = true;
		lowerTanb=tanb;
	      }
	      upperTanb=tanb;
	    }
	  }
	}
      }
      if(upperTanb>0){
	graph->SetPointEYhigh(ipoint-1, upperTanb-lowerTanb);
      }
    }
    higgs.close();
  } 

  return graph;
}

  std::vector<std::vector<TGraph*>>
PlotLimits::higgsConstraintLowTb(TH2D* plane_expected, double hmass, double hdeltaM, double Hmass, double HdeltaM, const char* model)
{
  TH2D* plane_hconstraint= new TH2D("hconstraint","hconstraint",21,145,355,86,0.95,9.55);
  TH2D* plane_Hconstraint= new TH2D("Hconstraint","Hconstraint",21,145,355,86,0.95,9.55);
  for(int idx=1; idx<plane_hconstraint->GetNbinsX()+1; idx++){
    for(int idy=1; idy<plane_hconstraint->GetNbinsY()+1; idy++){
      plane_hconstraint->SetBinContent(idx, idy, 1.1);
      plane_Hconstraint->SetBinContent(idx, idy, 1.1);
    }
  }

  for(int imass=0 ; imass<plane_expected->GetNbinsX(); ++imass){
    std::string line;
    float tanb_save=-99.0, tanb, mh, mA, mH;
    double x_save=plane_expected->GetXaxis()->GetBinUpEdge(imass);

    TGraph *graph_hconstraint = new TGraph();
    TGraph *graph_Hconstraint = new TGraph();

    //double x_save=(int)i;
    ifstream higgs (TString::Format("HiggsAnalysis/HiggsToTauTau/data/Higgs125/%s/higgs_%d.dat", model, (int)x_save)); 
    //std::cout << TString::Format("HiggsAnalysis/HiggsToTauTau/data/Higgs125/%s/higgs_%d.dat", model, (int)x_save) << std::endl;
    if(higgs.is_open()){
      int k=0;
      int l=0;
      while(higgs.good()){
	getline(higgs,line);
	sscanf(line.c_str(),"%f %f %f %f", &tanb, &mh, &mA, &mH);
	if(fabs(mh-hmass)<hdeltaM&&tanb!=tanb_save){
	  plane_hconstraint->SetBinContent(plane_hconstraint->GetXaxis()->FindBin(x_save),plane_hconstraint->GetYaxis()->FindBin(tanb), (fabs(mh-hmass)-hdeltaM+1));
	  graph_hconstraint->SetPoint(k,tanb,(fabs(mh-hmass)-hdeltaM+1));
	  k++;
	}
	else if(tanb!=tanb_save){
	  graph_hconstraint->SetPoint(k,tanb,(fabs(mh-hmass)-hdeltaM+1));
	  k++;
	}
	if(fabs(mH-Hmass)<HdeltaM&&tanb!=tanb_save){
	  plane_Hconstraint->SetBinContent(plane_hconstraint->GetXaxis()->FindBin(x_save),plane_hconstraint->GetYaxis()->FindBin(tanb), (fabs(mH-Hmass)-HdeltaM+1));
	  graph_Hconstraint->SetPoint(l,tanb,(fabs(mH-Hmass)-HdeltaM+1));
	  l++;
	}
	else if(tanb!=tanb_save){
	  graph_Hconstraint->SetPoint(l,tanb,(fabs(mH-Hmass)-HdeltaM+1));
	  l++;
	}

      }

      higgs.close();
    }
    for(int idy=1; idy<plane_hconstraint->GetNbinsY()+1; idy++){
      plane_hconstraint->SetBinContent(plane_hconstraint->GetXaxis()->FindBin(x_save), idy, graph_hconstraint->Eval(plane_hconstraint->GetYaxis()->GetBinCenter(idy)));
      plane_Hconstraint->SetBinContent(plane_Hconstraint->GetXaxis()->FindBin(x_save), idy, graph_Hconstraint->Eval(plane_Hconstraint->GetYaxis()->GetBinCenter(idy)));
    }

  }
  for(int idy=0; idy<plane_hconstraint->GetNbinsY()+1; idy++){
    TGraph* graph_hconstraint_tanb = new TGraph();
    TGraph* graph_Hconstraint_tanb = new TGraph();
    for(int imass=0; imass<plane_expected->GetNbinsX(); ++imass){
      // buffer mass value
      float mass = plane_expected->GetXaxis()->GetBinUpEdge(imass);
      graph_hconstraint_tanb   ->SetPoint(imass, mass, plane_hconstraint->GetBinContent(plane_hconstraint->GetXaxis()->FindBin(mass), idy));
      graph_Hconstraint_tanb   ->SetPoint(imass, mass, plane_Hconstraint->GetBinContent(plane_Hconstraint->GetXaxis()->FindBin(mass), idy));
    }
    for(int idx=0; idx<plane_hconstraint->GetNbinsX()+1; idx++)
    {
      plane_hconstraint->SetBinContent(idx, idy, graph_hconstraint_tanb->Eval(plane_hconstraint->GetXaxis()->GetBinCenter(idx)));
      plane_Hconstraint->SetBinContent(idx, idy, graph_Hconstraint_tanb->Eval(plane_Hconstraint->GetXaxis()->GetBinCenter(idx)));
    }
  }
  std::vector<TGraph*> gr_hconstraint;
  std::vector<TGraph*> gr_Hconstraint;

  int  n_hc;
  TIter iterhc((TList *)contourFromTH2(plane_hconstraint, 1.0, 20, false));
  STestFunctor hc = std::for_each( iterhc.Begin(), TIter::End(), STestFunctor() );
  n_hc=hc.sum; 
  int  n_Hc;
  TIter iterHc((TList *)contourFromTH2(plane_Hconstraint, 1.0, 20, false));
  STestFunctor Hc = std::for_each( iterHc.Begin(), TIter::End(), STestFunctor() );
  n_Hc=Hc.sum; 

  for(int i=0; i<n_hc; i++) {gr_hconstraint.push_back(   (TGraph *)((TList *)contourFromTH2(plane_hconstraint,    1.0, 20, false))->At(i));}
  for(int i=0; i<n_Hc; i++) {gr_Hconstraint.push_back(   (TGraph *)((TList *)contourFromTH2(plane_Hconstraint,    1.0, 20, false))->At(i));}
  //std::cout<< gr_minus2sigma.size() << " " << gr_minus1sigma.size() << " " << gr_expected.size() << " " << gr_plus1sigma.size() << " " << gr_plus2sigma.size() << " " << gr_observed.size() << std::endl;

  std::vector<std::vector<TGraph*>> hcontour_bands;
  hcontour_bands.push_back(gr_hconstraint);
  hcontour_bands.push_back(gr_Hconstraint); 
  return hcontour_bands;
}
