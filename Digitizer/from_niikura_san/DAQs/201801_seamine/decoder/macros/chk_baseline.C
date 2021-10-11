{

  new TCanvas("c1","c1");
  // c1->Divide(4,8);

  Double_t fixed_baseline[32] = {  15500.0, 15500.0, 15500.0, 15500.0,
				 15625.1, 15686.3, 15797.1, 15730.7,
				 15640.1, 15801.0, 15613.8, 15755.4,
				 15759.3, 15877.1, 15678.5, 15934.8,

				 15698.0, 15700.6, 15755.7, 15667.8,
				 15820.2, 15747.9, 15760.2, 15749.2,
				 15867.1, 15728.1, 15785.2, 15829.6,
				 15902.2, 15901.8, 15867.1, 15855.3};
  Double_t base[32];

  for(int i=0;i<32;i++){
    Int_t bn = i/16;
    Int_t ch = i%16;
    cout << bn << " " << ch << endl;
    tree0->Draw(Form("baseline%d[%d]>>h(100,-6,4)",bn,ch),"","");
    TH1D *hh = (TH1D*)gDirectory->Get("h");
    base[i] = fixed_baseline[i] - hh->GetMean();
    cout << hh->GetMean() << endl;
    if(i%8==0) cout << endl;
    c1->Update();
    cin.ignore();
	
  }

  // cout << Form("Double_t fixed_baseline[32] = {  ");
  // for(int i=0;i<32;i++){
  //   cout << Form("%7.1f, ",base[i]);
  // }
  // cout << Form("};\n");
}
