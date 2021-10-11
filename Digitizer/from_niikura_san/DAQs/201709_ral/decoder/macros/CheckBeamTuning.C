void CheckBeamTuning(){
  TChain *tree1 =new TChain("tree0","nominal");
  TChain *tree2 =new TChain("tree0","defocused");

  //string title="34 MeV without C";
  //tree1->Add("./root/run1044.root");
  //tree2->Add("./root/run1053.root");

  //string title="34 MeV with C";
  //tree1->Add("./root/run1047.root");
  //tree2->Add("./root/run1048.root");

  // string title="33 MeV without C";
  //tree1->Add("./root/run1043.root");
  //tree2->Add("./root/run1052.root");

  //string title="33 MeV with C";
  //tree1->Add("./root/run1051.root");
  //tree2->Add("./root/run1050.root");

  //string title="35 MeV without C";
  //tree1->Add("./root/run1045.root");
  //tree2->Add("./root/run1054.root");

  string title="35 MeV with C";
  tree1->Add("./root/run1046.root");
  tree2->Add("./root/run1049.root");
  
  TCanvas* c=new TCanvas("c","c",0,0,700,500);
  TH1F *h1=new TH1F("h1",title.c_str(),440,-20,200);
  TH1F *h2=new TH1F("h2",title.c_str(),440,-20,200);

  tree2->Draw("QDCl[0]>>h2","QDCl[2]>5");
  tree1->Draw("QDCl[0]>>h1","QDCl[2]>5","same");
  
  h2->SetLineColor(2);
}
