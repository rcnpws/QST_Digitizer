void CheckGe(int runnumber){
  TFile * f=new TFile(Form("./root/run%04d.root",runnumber));
  TTree* tree2=(TTree*)f->Get("tree2");
  TTree* tree3=(TTree*)f->Get("tree3");

  TCanvas* c=new TCanvas("c","beam tuning",0,0,800,500);
  c->Divide(1,2);
  TH1F* hge1 = new TH1F("hge1","ge 1",1000,1,1001);
  TH1F* hge2 = new TH1F("hge2","ge 2",1000,1,1001);

  c->cd(1);  
  tree2->Draw("GeE>>hge1","","");

  c->cd(2);  
  tree3->Draw("GeE>>hge2","","");

}
