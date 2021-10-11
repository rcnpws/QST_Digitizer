void chkge(Int_t run){
  TFile *f = TFile::Open(Form("root/sorted%04d.root",run));
  TH1D *h1 = (TH1D*)f->Get("h1");
  TH1D *h2 = (TH1D*)f->Get("h1");

  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  c1->Divide(1,2);

  c1->cd(1);
  gPad->SetLogy(1);
  h1->Rebin(2);
  h1->GetXaxis()->SetRangeUser(0,5);
  h1->Draw();

  c1->cd(2);
  gPad->SetLogy(1);
  h2->Rebin(2);
  h2->GetXaxis()->SetRangeUser(0,5);
  h2->Draw();
  
  
  

  
  
}
