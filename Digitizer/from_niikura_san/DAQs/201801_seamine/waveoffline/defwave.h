// $Id: defwave.h 2018-03-15 22:33:28 daq Exp $

TGraph *g_wave[16];
TCanvas *c1;

const Int_t ShowBN = 0;
const Int_t RecordLength = 300; // # sample

void DefineGraph()
{
  c1 = new TCanvas("c1","c1");
  //c1->Divide(4,4);
  for(int i=0;i<16;i++){
    g_wave[i] = new TGraph();
    g_wave[i]->SetMaximum(17000);
    g_wave[i]->SetMinimum(0);
    //g_wave[i]->SetMaximum(17000);
    //g_wave[i]->SetMinimum(8000);
  }    
}

void FillGraph()
{
  for(int i=0;i<16;i++){
    for(int j=0;j<300;j++){
      g_wave[i]->SetPoint(j,j,wave[i][j]);
      
    }
  }
}

void DrawWave()
{
  /*
  for(int i=0;i<16;i++){
    c1->cd(i+1);
    g_wave[i]->Draw("al");
  }
  */
  g_wave[11]->Draw("al");
  //g_wave[11]->GetXaxis()->ZoomOut(0.5);
  c1->Update();
}
