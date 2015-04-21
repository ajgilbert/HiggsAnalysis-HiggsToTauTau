#include "TH1F.h"
#include "TGraph.h"
#include "TArrow.h"
#include "TGraph.h"
#include "TString.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include <iostream>

void
plottingGoodnessOfFit(TCanvas& canv, TH1F* exp, TGraph* obs, std::string& xaxis, std::string& yaxis, std::string& masslabel, int mass, double min, double max, int lowerBin, int upperBin, bool log=false, double p_value=0.5)
{
  // set up styles
  canv.cd();
  canv.SetGridx(1);
  canv.SetGridy(1);
  if(log){ 
    canv.SetLogy(1); 
  }

  exp->SetXTitle(xaxis.c_str());
  exp->GetXaxis()->SetLabelFont(62);
  exp->GetXaxis()->SetTitleColor(1);
  exp->GetXaxis()->SetTitleOffset(1.05);
  exp->GetXaxis()->SetRange(lowerBin+1, upperBin);
  exp->SetYTitle(yaxis.c_str());
  exp->GetYaxis()->SetLabelFont(62);
  exp->GetYaxis()->SetTitleSize(0.05);
  exp->GetYaxis()->SetTitleOffset(1.4);
  exp->GetYaxis()->SetLabelSize(0.04);
  exp->SetMinimum(min);
  if(max>0){
    exp->SetMaximum(max);
  }
  else{
    exp->SetMaximum(1.3*exp->GetMaximum());
  }
  exp->SetLineWidth(3.); 
  exp->SetLineColor(kBlack); 
  exp->Draw();

  TArrow* arr = new TArrow(obs->GetX()[0], 0.001, obs->GetX()[0], exp->GetMaximum()/8, 0.02, "<|");
  arr->SetLineColor(kBlue);
  arr->SetFillColor(kBlue);
  arr->SetFillStyle(1001);
  arr->SetLineWidth(6.);
  arr->SetLineStyle(1.);
  arr->SetAngle(60);
  arr->Draw("<|same");

  TLegend* leg = new TLegend(0.50, 0.81, 0.92, 0.90);
  leg->SetBorderSize( 0 );
  leg->SetFillStyle ( 0 );
  leg->SetFillColor (kWhite);
  leg->AddEntry(exp, "expected (from toys)", "L");
  leg->Draw("same");
  
  TString label = TString::Format("%s = %d GeV", masslabel.c_str(), mass);
  TPaveText* textlabel = new TPaveText(0.18, 0.81, 0.50, 0.90, "NDC");
  textlabel->SetBorderSize(   0 );
  textlabel->SetFillStyle (   0 );
  textlabel->SetTextAlign (  12 );
  textlabel->SetTextSize  (0.04 );
  textlabel->SetTextColor (   1 );
  textlabel->SetTextFont  (  62 );
  textlabel->AddText(label);
  textlabel->Draw();

  TPaveText* pvalue = new TPaveText(0.18, 0.75, 0.50, 0.80, "NDC");
  pvalue->SetBorderSize(   0 );
  pvalue->SetFillStyle (   0 );
  pvalue->SetTextAlign (  12 );
  pvalue->SetTextSize  (0.04 );
  pvalue->SetTextColor (   1 );
  pvalue->SetTextFont  (  62 );
  pvalue->AddText(TString::Format( "p-value = %0.3f", p_value ));
  pvalue->Draw();
  
  canv.RedrawAxis();
  return;
}

