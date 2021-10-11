{
  gStyle->SetHistMinimumZero();
  const int runN=2;
  int run[runN]={1143,1133};
  
  double cge1_0;
  double cge1_1;
  double cge2_0;
  double cge2_1;
  
  TCanvas* c1=new TCanvas("c1","",0,0,800,500);
  TCanvas* c2=new TCanvas("c2","",0,0,800,500);
  
  TLegend* lg=new TLegend(0.65,0.65,0.85,0.85,"","NDC");
  
  TFile* f[runN];
  TTree* tree1[runN];
  TTree* tree2[runN];
  TH1F*  h1[runN];
  TH1F*  h2[runN];
  
  for(int i=0;i<runN;i++){
    f[i]=new TFile(Form("./root/run%04d.root",run[i]));
    tree1[i]=(TTree*)f[i]->Get("tree2");
    h1[i]=new TH1F(Form("h1%d",i),"Ge1",4000,0,2000);
    tree2[i]=(TTree*)f[i]->Get("tree3");
    h2[i]=new TH1F(Form("h2%d",i),"Ge2",4000,0,2000);

    h1[i]->SetLineColor(i+1);
    h2[i]->SetLineColor(i+1);
    
    lg->AddEntry(h1[i],Form("run%d",run[i]));


    if(run[i]>1087){
      //for Zr run;
      
      //cge1_0=-1.1383;
      cge1_0=0.0;
      cge1_1=1.0096;
      //cge2_0=-1.0152;
      cge2_0=1;
      cge2_1=1.0114;

    }else{
      cge1_0=0;
      cge1_1=1.0;
      cge2_0=1;
      cge2_1=1.0;
    }
    
    if(i==0){
      c1->cd();
      tree1[i]->Draw(Form("%f + GeE*%f>>h1%d",cge1_0,cge1_1,i));
      c2->cd();
      tree2[i]->Draw(Form("%f + GeE*%f>>h2%d",cge2_0,cge2_1,i));
    }else{
      c1->cd();
      tree1[i]->Draw(Form("%f + GeE*%f>>h1%d",cge1_0,cge1_1,i),"","same");
      c2->cd();
      tree2[i]->Draw(Form("%f + GeE*%f>>h2%d",cge2_0,cge2_1,i),"","same");
    }
  }
  c1->cd();
  lg->Draw("same");  
  
  
}
