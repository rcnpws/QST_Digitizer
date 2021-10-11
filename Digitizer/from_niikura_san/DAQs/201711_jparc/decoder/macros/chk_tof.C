{

  new TCanvas("c1","c1");

  Double_t tof_offset[32];
  
  for(int i=0;i<32;i++){
    Int_t bn = i/16;
    Int_t ch = i%16;
    TF1 *fn = new TF1("fn","gaus",-10,0);
    cout << bn << " " << ch << endl;
    // tree0->Draw(Form("MaxPH%d[%d]:MaxTT%d[%d]>>h(200,100,300,1500,0.,1500.)",bn,ch,bn,ch),"","colz");
    tree0->Draw(Form("LED%d[%d]-Tbaf>>h(200,-100,100)",bn,ch),"Tbaf>29 && Tbaf<38","colz");
    TH1D *hh = (TH1D*)gDirectory->Get("h");
    hh->Fit("fn","r");
    tof_offset[i]=fn->GetParameter(1);
    cout << tof_offset[i] << endl;
    c1->Update();
    cin.ignore();
  }

  for(int i=0;i<32;i++){
    cout << tof_offset[i] << ", ";
  }
  
}
