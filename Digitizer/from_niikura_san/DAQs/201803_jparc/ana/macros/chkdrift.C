{
  TFile *f1 = TFile::Open("root/run1050.root");
  TFile *f2 = TFile::Open("root/run1051.root");
  TFile *f3 = TFile::Open("root/run1052.root");

  f1->cd();
  wave->Draw("2*t2:TimeStamp/1e9/60>>h0(1000,0.,200,150,0.,300)","","colz");
  TH1D *h0 = (TH1D*)gDirectory->Get("h0");
  f2->cd();
  wave->Draw("2*t2:TimeStamp/1e9/60+60>>h1(1000,0.,200,150,0.,300)","","colz");
  TH1D *h1 = (TH1D*)gDirectory->Get("h1");
  f3->cd();
  wave->Draw("2*t2:TimeStamp/1e9/60+120>>h2(1000,0.,200,150,0.,300)","","colz");
  TH1D *h2 = (TH1D*)gDirectory->Get("h2");

  h0->Draw("colz");
  h1->Draw("colzsame");
  h2->Draw("colzsame");

}
