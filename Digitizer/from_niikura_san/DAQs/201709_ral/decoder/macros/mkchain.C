{

  // TChain *ch0 = new TChain("tree2");
  // ch0->Add("./root/run10*.root");
  // TChain *ch1 = new TChain("tree2");
  // ch1->Add("./root/run1008.root");
  // ch1->Add("./root/run1012.root");
  
  TChain *ch08 = new TChain("tree2");
  ch08->Add("./root/run100*.root");
  ch08->Add("./root/run1010.root");
  ch08->Add("./root/run1011.root");
  ch08->Add("./root/run1012.root");
  ch08->Add("./root/run1013.root");
  ch08->Add("./root/run1014.root");
  ch08->Add("./root/run1015.root");

  TChain *ch10 = new TChain("tree2");
  ch10->Add("./root/run1016.root");
  ch10->Add("./root/run1017.root");
  ch10->Add("./root/run1018.root");
  ch10->Add("./root/run1019.root");
  ch10->Add("./root/run102*.root");
  ch10->Add("./root/run1030.root");

  TChain *ch05 = new TChain("tree2");
  ch05->Add("./root/run1057.root");
  ch05->Add("./root/run1058.root");
  ch05->Add("./root/run1059.root");
  ch05->Add("./root/run106*.root");
  ch05->Add("./root/run1071.root");
  ch05->Add("./root/run1072.root");
  ch05->Add("./root/run1073.root");
  ch05->Add("./root/run1074.root");
  ch05->Add("./root/run1075.root");
  ch05->Add("./root/run1076.root");

  gStyle->SetHistMinimumZero(1);
  ch08->Draw("GeE>>h0(460,170,400)","","");
  ch10->Draw("GeE>>h1(460,170,400)","","samehist");
  ch05->Draw("GeE>>h2(460,170,400)","","samehist");

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  h0->GetYaxis()->SetNdivisions(505);
  h0->GetXaxis()->SetTitle("Gamma-ray energy (keV)");
  h0->GetYaxis()->SetTitle("Count / 0.5 keV");
  h0->GetXaxis()->CenterTitle();
  h0->GetYaxis()->CenterTitle();
  h0->GetXaxis()->SetTitleFont(62);
  h0->GetYaxis()->SetTitleFont(62);
  h0->GetXaxis()->SetTitleSize(0.04);
  h0->GetYaxis()->SetTitleSize(0.04);
  h0->GetXaxis()->SetTitleOffset(1.1);;
  h0->GetYaxis()->SetTitleOffset(1.3);;


  h0->SetLineColor(632);
  h1->SetLineColor(600);
  h2->SetLineColor(418);
  h0->SetLineWidth(2);
  h1->SetLineWidth(2);
  h2->SetLineWidth(2);

  TLegend *ll = new TLegend(0.7,0.7,0.99,0.99);
  ll->AddEntry(h0,"Pd-108 (9h)");
  ll->AddEntry(h1,"Pd-110 (10h)");
  ll->AddEntry(h2,"Pd-105 (17h)");
  ll->Draw();
  

}
