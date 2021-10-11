void FitGaus( double constant=100.,double mean=500.,double sigma=1.0,double bg0=100,double bg1=0.0){
  int runnumber=1096;

  TFile * f=new TFile(Form("./root/run%04d.root",runnumber));
  TTree* tree1=(TTree*)f->Get("tree2");
  TTree* tree2=(TTree*)f->Get("tree3");

  TCanvas* c=new TCanvas("c","",0,0,1000,500);
  c->Divide(2,1);
  c->cd(1);
  TH1F*  h1=new TH1F(Form("h1"),"Ge1",40,mean-20,mean+20); 
  tree1->Draw("GeE>>h1");
  TF1* f1=new TF1("f1","gaus(0)+[3]+[4]*x",mean-20,mean+20);
  f1->SetParameters(constant,mean,sigma,bg0,bg1);
  h1->Fit("f1","r");
  
  c->cd(2);
  TH1F*  h2=new TH1F(Form("h2"),"Ge2",40,mean-20,mean+20);
  tree2->Draw("GeE>>h2");
  TF1* f2=new TF1("f2","gaus(0)+[3]+[4]*x",mean-20,mean+20);
  f2->SetParameters(constant,mean,sigma,bg0,bg1);
  h2->Fit("f2","r");

  cout<< mean << "\t" << f1->GetParameter(1)<<"\t" << f2->GetParameter(1)<<endl; 
}
