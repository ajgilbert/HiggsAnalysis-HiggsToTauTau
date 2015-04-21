 #include "TDirectory.h"
#include "HiggsAnalysis/HiggsToTauTau/interface/PlotLimits.h"

/// This is the core plotting routine that can also be used within
/// root macros. It is therefore not element of the PlotLimits class.
void plottingGoodnessOfFit(TCanvas& canv, TH1F* exp, TGraph* obs, std::string& xaxis, std::string& yaxis, std::string& masslabel, int mass, double min, double max, int lowerBin, int upperBin, bool log, double p_value);

void
PlotLimits::plotGoodnessOfFit(TCanvas& canv, const char* directory)
{
  // set up styles
  SetStyle();

  for(unsigned int imass=0; imass<bins_.size(); ++imass){
    // buffer mass value
    float mass = bins_[imass];

    // fill histogram from TTree for observed
    TString fullpath = TString::Format("%s/%d/higgsCombineTest.GoodnessOfFit.mH%d.root", directory, (int)mass, (int)mass);
    std::cout << "open file: " << fullpath << std::endl;
    TFile* file_ = TFile::Open(fullpath); if(!file_){ std::cout << "--> TFile is corrupt: skipping masspoint." << std::endl; continue; }
    TTree* limit = (TTree*) file_->Get("limit"); if(!limit){ std::cout << "--> TTree is corrupt: skipping masspoint." << std::endl; continue; }
    double chi2;
    int digits_obs = pow(10, int(log10(limit->GetMaximum("limit"))));
    double lower_obs = std::max(0., double((int(limit->GetMinimum("limit")/digits_obs)-1)*digits_obs));
    double upper_obs = (int(limit->GetMaximum("limit")/digits_obs)+1)*digits_obs;

    TGraph* obs = new TGraph();
    limit->SetBranchAddress("limit", &chi2);  
    int nevent = limit->GetEntries();
    for(int i=0; i<nevent; ++i){
      limit->GetEvent(i);
      obs->SetPoint(0, chi2, 1.);
    }
    file_->Close();

    // fill histogram from TTree for expected
    fullpath = TString::Format("%s/%d/batch_collected_goodness_of_fit.root", directory, (int)mass);
    std::cout << "open file: " << fullpath << std::endl;
    file_ = TFile::Open(fullpath); if(!file_){ std::cout << "--> TFile is corrupt: skipping masspoint." << std::endl; continue; }
    limit = (TTree*) file_->Get("limit"); if(!limit){ std::cout << "--> TTree is corrupt: skipping masspoint." << std::endl; continue; }
    double nentries = limit->GetEntries(); 
    limit->Draw("limit>>exp");
    TH1F* exp = (TH1F*)gDirectory->Get("exp"); exp->SetTitle("");
    int nq = 2; double xq[2] = {0.05, 0.95}; double yq[2]; exp->GetQuantiles(nq, yq, xq);
    int digits_exp = pow(10, int(log10(yq[1]))); 
    double lower_exp = std::max(0., yq[0]); 
    double upper_exp = (int(yq[1]/digits_exp)+1.5)*digits_exp;
    //double p_value_exp = exp->Integral(exp->GetXaxis()->FindBin(chi2), exp->GetXaxis()->GetNbins()-1)/exp->Integral();
    //std::cout << p_value_exp << std::endl;
    //std::cout << exp->Integral() << " " << exp->Integral(exp->GetXaxis()->FindBin(chi2), exp->GetXaxis()->GetNbins()-1) << " " << p_value << " " << exp->Integral(exp->GetXaxis()->FindBin(chi2), exp->GetXaxis()->FindBin(std::max(upper_exp, upper_obs)*11/10))/exp->Integral() << " " << exp->Integral(exp->GetXaxis()->FindBin(std::max(upper_exp, upper_obs)*11/10), exp->GetXaxis()->GetNbins()-1)/exp->Integral() << " " << exp->Integral(exp->GetXaxis()->FindBin(800), exp->GetXaxis()->GetNbins()-1)/exp->Integral() <<std::endl;
    
    TH1F* exp2 = new TH1F("", "", 100, std::min(lower_exp, lower_obs)*9/10, std::max(upper_exp, upper_obs)*11/10);
    for(int i=0; i<exp2->GetXaxis()->GetNbins(); i++){
      exp2->SetBinContent(i, 0);
    }
    double content;
    limit->SetBranchAddress("limit",&content);
    for(int i=0; i<nentries; i++){
      limit->GetEntry(i);
      exp2->AddBinContent(exp2->GetXaxis()->FindBin(content));
    }
    //exp2->SaveAs("test.root");

    // do the plotting
    exp2->Scale(1/nentries);
    double p_value_exp2 = exp2->Integral(exp2->GetXaxis()->FindBin(chi2), exp2->GetXaxis()->GetNbins()-1);
    //std::cout << exp2->Integral(exp2->GetXaxis()->FindBin(chi2), exp2->GetXaxis()->GetNbins()-1) << " " << exp2->Integral()  <<  std::endl;
    std::string masslabel = mssm_ ? std::string("m_{#phi}") : std::string("m_{H}");
    plottingGoodnessOfFit(canv, exp2, obs, xaxis_, yaxis_, masslabel, mass, min_, max_, 0, 100, log_, p_value_exp2);
    //exp->GetXaxis()->SetRangeUser(std::min(lower_exp, lower_obs), std::max(upper_exp, upper_obs));
    //exp->GetXaxis()->SetRange(exp->GetXaxis()->FindBin(std::min(lower_exp, lower_obs)), exp->GetXaxis()->FindBin(std::max(upper_exp, upper_obs)));

    // add the CMS Preliminary stamp
    CMSPrelim(dataset_.c_str(), "", 0.160, 0.835);
    if(png_){
      canv.Print(TString::Format("%s-%s-%d.png", output_.c_str(), label_.c_str(), (int)mass));
    }
    if(pdf_){
      canv.Print(TString::Format("%s-%s-%d.pdf", output_.c_str(), label_.c_str(), (int)mass));
      canv.Print(TString::Format("%s-%s-%d.eps", output_.c_str(), label_.c_str(), (int)mass));
    }
    if(root_){
      TFile* output = new TFile("goodness-of-fit.root", "update");
      if(!output->cd(output_.c_str())){
	output->mkdir(output_.c_str());
	output->cd(output_.c_str());
      }
      if(exp){ exp->Write(TString::Format("expected_%d", (int)mass)); }
      if(obs){ obs->Write(TString::Format("observed_%d", (int)mass)); }
      output->Close();
    }
  }
}
