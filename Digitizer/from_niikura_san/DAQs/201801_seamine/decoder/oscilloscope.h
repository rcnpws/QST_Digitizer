//
// oscilloscope.h
//     written by M. Niikura
// $Id: oscilloscope.h 2017-09-19 22:10:20 niikura Exp $

//
TGraph *g_wave[5];
Int_t linecolor[5] = {kRed, kBlue, kBlack, kGreen+2, kYellow+2};
Double_t gain[5] = {1, 1, 1, 0.2, 0.2};

void DefineGraph()
{
  for(int i=0;i<5;i++){
    g_wave[i] = new TGraph();
    g_wave[i]->SetLineColor(linecolor[i]);
    g_wave[i]->SetMaximum(17000);
    g_wave[i]->SetMinimum(0);
  }
}

void FillGraph()
{
  for(int i=0;i<5;i++){
    for(int j=0;j<1000;j++){
      g_wave[i]->SetPoint(j,j*2,wave[i][j]*gain[i]);
      /* cout << i << " " << j << " " << wave[i][j] << endl; */
    }
  }
}

void DrawWave()
{
  /* c1->cd(1); */
  /* g_wave[0]->SetTitle("Plastics"); */
  g_wave[ 0]->Draw("al");
  g_wave[ 1]->Draw("lsame");
  
  /* c1->cd(2); */
  /* g_wave[2]->SetTitle("40,50Hz & Cherenkov"); */
  g_wave[ 2]->Draw("lsame");
  g_wave[ 3]->Draw("lsame");
  g_wave[ 4]->Draw("lsame");


  c1->Update();
}
