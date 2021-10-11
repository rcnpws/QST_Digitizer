{
  TFile *f0 = TFile::Open("root/run0009.root");
  tree2->Draw("GeE>>h1(4000,0,2000)","","hist");
  
  TFile *f1 = TFile::Open("root/run1112.root");
  tree2->Draw("GeE*1.010>>h2(4000,0,2000)","","histsame");
  h2->SetLineColor(2);
  h2->Scale(1.4);
  
}
