{

  new TCanvas("c1","c1");
  
  for(int i=4;i<25;i++){
    Int_t bn = i/16;
    Int_t ch = i%16;
    cout << bn << " " << ch << endl;
    // tree0->Draw(Form("MaxPH%d[%d]:MaxTT%d[%d]>>h(200,100,300,1500,0.,1500.)",bn,ch,bn,ch),"","colz");
    // tree0->Draw(Form("MaxPH%d[%d]:LED%d[%d]>>h(200,100,300,1500,0.,1500.)",bn,ch,bn,ch),"","colz");
    // tree0->Draw(Form("(QDCl%d[%d]-QDCs%d[%d])/QDCs%d[%d]:QDCl%d[%d]>>(300,0.,300e3,300,-0.4,1)",bn,ch,bn,ch,bn,ch,bn,ch),"","colz");
    tree0->Draw(Form("(QDCl%d[%d]-QDCs%d[%d])/QDCs%d[%d]:MaxPH%d[%d]>>(300,0.,6000,300,-0.4,1)",bn,ch,bn,ch,bn,ch,bn,ch),"","colz");
    TH1D *hh = (TH1D*)gDirectory->Get("h");
    // hh->SetTitle(Form("SEAMINE No.%d",i-3));
    c1->Update();
    cin.ignore();
	
  }

}
