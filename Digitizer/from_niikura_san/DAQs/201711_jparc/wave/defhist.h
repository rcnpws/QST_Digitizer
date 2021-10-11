//
// $Id: defhist.h 2018-01-29 22:26:29 niikura Exp $

TCanvas *c2;

TH1D *h1 = new TH1D("h1","h1",1000,0.,1000);

void DefineHist()
{
  c1 = new TCanvas("c1","c1");
  c1->Divide(2,2);

}
