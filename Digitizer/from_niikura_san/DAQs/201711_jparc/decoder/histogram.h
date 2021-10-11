//
// histogram.h
//      written by M. Niikura
// $Id: histogram.h 2017-02-19 21:26:38 niikura Exp $

// --
Int_t h_update = 1000;

// --
TCanvas *c1;
TH1D *h_plastic1;
TH1D *h_plastic2u;
TH1D *h_plastic2d;
TH1D *h_plastic3;

void DefineHist()
{
  h_plastic1  = new TH1D("h_plastic1", "Pulse Height of Plastic 1", 16000,0.,16000.);
  h_plastic2u = new TH1D("h_plastic2u","Pulse Height of Plastic 2u",16000,0.,16000.);
  h_plastic2d = new TH1D("h_plastic2d","Pulse Height of Plastic 2d",16000,0.,16000.);
  h_plastic3  = new TH1D("h_plastic3", "Pulse Height of Plastic 3", 16000,0.,16000.);
}

void FillHist()
{
  h_plastic1  ->Fill(MaxPH[0]);
  h_plastic2u ->Fill(MaxPH[1]);
  h_plastic2d ->Fill(MaxPH[2]);
  h_plastic3  ->Fill(MaxPH[3]);
}

void DrawHist()
{
  c1->cd(1); h_plastic1  ->Draw();
  c1->cd(2); h_plastic2u ->Draw();
  c1->cd(3); h_plastic2d ->Draw();
  c1->cd(4); h_plastic3  ->Draw();

  c1->Update();
}
