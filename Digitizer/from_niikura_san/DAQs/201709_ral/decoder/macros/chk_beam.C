{


  Int_t Run[5]      = { 323,  333,  334,  335,  336};
  Double_t scale[5] = {3048, 4185, 3070, 3283, 3057};
  Int_t color[5]    = {kRed, kBlue, kGreen+2, kMagenta, kBlack};
  TTree *tt;
  TH1D *hh[5];

  for(int i=0;i<5;i++){
    TFile *ff = TFile::Open(Form("root/run%04d.root",Run[i]));
    tt = (TTree*)ff->Get("tree0");
    tt->Draw("QDCl[0]>>h(130,-10,120)","QDCl[2]>3","hist");
    hh[i] = (TH1D*)ff->Get("h");
    hh[i]->Scale(1000./scale[i]);
    hh[i]->SetLineColor(color[i]);
  }

  hh[0]->Draw("HIST");
  hh[1]->Draw("HISTSAME");
  hh[2]->Draw("HISTSAME");
  hh[3]->Draw("HISTSAME");
  hh[4]->Draw("HISTSAME");

  
}
