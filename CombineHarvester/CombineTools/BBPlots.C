#include "CombineTools/interface/Plotting.h"
#include "CombineTools/interface/Plotting_Style.h"


struct Info {
  float r;
};

std::vector<Info> FillVec(TTree *t) {
  Info info;
  std::vector<Info> res;
  t->SetBranchAddress("r", &info.r);
  for (unsigned i = 0; i < t->GetEntries(); ++i) {
    t->GetEntry();
    res.push_back(info);
  }
  return res;
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
  TTree *t3 = (TTree*)f1.Get("limit");
  std::vector<Info> i3 = FillVec(t3);

  TH1F h_r_diff("h_r_diff", "h_r_diff", 100, -1, 1);
  for (unsigned i = 0; i < i1.size(); ++i) {
    h_r_diff.Fill(i3[i].r - i2[i].r);
  }

  TCanvas * canv = new TCanvas("r_diff", "r_diff");
  canv->cd();
  std::vector<TPad*> pads = OnePad();
  std::vector<TH1*> h = CreateAxisHists(1, &h_r_diff);
  h[0]->Draw();
  h_r_diff.Draw("SAME");
  canv->Print("r_diff.pdf");

  return 0;
}