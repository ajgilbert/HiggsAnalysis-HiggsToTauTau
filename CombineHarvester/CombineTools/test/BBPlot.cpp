#include "CombineTools/interface/Plotting.h"
#include "CombineTools/interface/Plotting_Style.h"


struct Info {
  float r;
  float t_cpu;
  double t_cpu_minim;
  bool fit_status;
};

std::vector<Info> FillVec(TTree *t) {
  Info info;
  std::vector<Info> res;
  t->SetBranchAddress("r", &info.r);
  t->SetBranchAddress("t_cpu", &info.t_cpu);
  t->SetBranchAddress("t_cpu_minim", &info.t_cpu_minim);
  t->SetBranchAddress("fit_status", &info.fit_status);
  for (unsigned i = 0; i < t->GetEntries(); ++i) {
    t->GetEntry(i);
    res.push_back(info);
  }
  return res;
}

void plotRDiff(std::vector<Info> & i2, std::vector<Info> & i3, TString plotName, TString xaxis, double range) {
  TH1F h_r_diff("h_r_diff", "h_r_diff", 20, -range, range);
  for (unsigned i = 0; i < i2.size(); ++i) {
    if (i3[i].fit_status && i2[i].fit_status) h_r_diff.Fill(i3[i].r - i2[i].r);
  }

  TCanvas * canv = new TCanvas(plotName, plotName);
  canv->cd();
  std::vector<TPad*> pads = OnePad();
  std::vector<TH1*> h = CreateAxisHists(1, &h_r_diff);
  h[0]->GetXaxis()->SetTitle(xaxis);
  h[0]->GetYaxis()->SetTitle("Number of fits");
  h[0]->Draw();
  h_r_diff.SetLineColor(2);
  h_r_diff.SetLineWidth(3);
  h_r_diff.Draw("SAME");
  FixTopRange(pads[0], GetPadYMax(pads[0]), 0.0);
  FixOverlay();

  canv->Print(".pdf");
}

template <typename Function>
void plotTCPU(std::vector<Info> &i1, std::vector<Info> &i2,
              std::vector<Info> &i3, TString plotName, TString xaxis, Function func) {
  TH1F h_r_diff1("h_t_cpu1", "h_t_cpu1", 50, 0, 4);
  TH1F h_r_diff2("h_t_cpu2", "h_t_cpu2", 50, 0, 4);
  TH1F h_r_diff3("h_t_cpu3", "h_t_cpu3", 50, 0, 4);
  for (unsigned i = 0; i < i1.size(); ++i) {
    h_r_diff1.Fill(func(i1[i]));
    h_r_diff2.Fill(func(i2[i]));
    h_r_diff3.Fill(func(i3[i]));
  }

  TCanvas * canv = new TCanvas(plotName, plotName);
  canv->cd();
  std::vector<TPad*> pads = OnePad();
  std::vector<TH1*> h = CreateAxisHists(1, &h_r_diff1);
  h[0]->GetXaxis()->SetTitle(xaxis);
  h[0]->GetYaxis()->SetTitle("Number of fits");
  h[0]->Draw();
  h_r_diff1.SetLineColor(2);
  h_r_diff1.SetLineWidth(3);
  h_r_diff1.Draw("SAME");
  h_r_diff2.SetLineColor(3);
  h_r_diff2.SetLineWidth(3);
  h_r_diff2.Draw("SAME");
  h_r_diff3.SetLineColor(4);
  h_r_diff3.SetLineWidth(3);
  h_r_diff3.Draw("SAME");

  TLegend *leg = PositionedLegend(0.3, 0.35, 3, 0.02);
  leg->AddEntry(&h_r_diff1, TString::Format("#splitline{Morphing}{Mean = %.3f}", h_r_diff1.GetMean()), "L");
  leg->AddEntry(&h_r_diff2, TString::Format("#splitline{Bins-Fit}{Mean = %.3f}", h_r_diff2.GetMean()), "L");
  leg->AddEntry(&h_r_diff3, TString::Format("#splitline{Bins-Solved}{Mean = %.3f}", h_r_diff3.GetMean()), "L");
  leg->Draw();

  FixTopRange(pads[0], GetPadYMax(pads[0]), 0.0);
  FixOverlay();

  canv->Print(".pdf");
}

int main() {
  ModTDRStyle();

  TFile f1("output/bbb-test/higgsCombine_full_old_bbb.MultiDimFit.mH125.1.123456.root");
  TTree *t1 = (TTree*)f1.Get("limit");
  std::vector<Info> i1 = FillVec(t1);

  TFile f2("output/bbb-test/higgsCombine_bbb_test.MultiDimFit.mH125.1.123456.root");
  TTree *t2 = (TTree*)f2.Get("limit");
  std::vector<Info> i2 = FillVec(t2);


  TFile f3("output/bbb-test/higgsCombine_bbb_enabled.MultiDimFit.mH125.1.123456.root");
  TTree *t3 = (TTree*)f3.Get("limit");
  std::vector<Info> i3 = FillVec(t3);

  plotRDiff(i2, i3, "r_diff", "r_{bbb-solved} - r_{bbb-fit}", 0.05);
  plotRDiff(i1, i3, "r_diff_m", "r_{bbb-solved} - r_{bbb-morphed}", 0.2);
  plotTCPU(i1, i2, i3, "t_cpu", "CPU time (mins)", [](Info & i) { return i.t_cpu; });
  plotTCPU(i1, i2, i3, "t_cpu_minim", "Minimization time (mins)", [](Info & i) { return i.t_cpu_minim; });

  return 0;
}