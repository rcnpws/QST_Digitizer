void Btuning_jparc(int runnumber){
  TFile * f=new TFile(Form("./root/run%04d.root",runnumber));
  TTree* tree=(TTree*)f->Get("tree0");

  TCanvas* c=new TCanvas("c","beam tuning",0,0,800,500);
  c->Divide(2,1);
  TH1F* hplaf_mu = new TH1F("hplaf_mu","plaf muon",2000,1,1001);
  TH1F* hplaf_e = new TH1F("hplaf_e","plaf e",2000,1,1001);
  TH1F* hplab_mu = new TH1F("hplab_mu","plab muon",2000,1,1001);
  TH1F* hplab_e = new TH1F("hplab_e","plab e",2000,1,1001);

  hplaf_e ->SetLineColor(2);
  hplab_e ->SetLineColor(2);

  c->cd(1);  
  tree->Draw("QDCs[0]>>hplaf_mu","","");
  tree->Draw("QDCe[0]>>hplaf_e","","same");
  c->cd(2);  
  tree->Draw("QDCs[1]>>hplab_mu","","");
  tree->Draw("QDCe[1]>>hplab_e","","same");

  cout<<"pla F muon : "<<hplaf_mu->GetMean()<<endl;
  cout<<"pla B muon : "<<hplab_mu->GetMean()<<endl;
  cout<<"pla F e    : "<<hplaf_e->GetMean()<<endl;
  cout<<"pla B e    : "<<hplab_e->GetMean()<<endl;
  
}
