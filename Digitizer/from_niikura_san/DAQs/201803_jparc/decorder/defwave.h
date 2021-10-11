// $Id: defwave.h 2018-02-19 21:32:41 daq Exp $

TGraph *g_wave[16];
TCanvas *c1;

const Int_t ShowBN = 0;
const Int_t RecordLength = 500; // # sample

void DefineGraph()
{
  c1 = new TCanvas("c1","c1");
  /* c1->Divide(4,4); */
  for(int i=0;i<16;i++){
    g_wave[i] = new TGraph();
    g_wave[i]->SetMaximum(17000);
    g_wave[i]->SetMinimum(0);
  }
  g_wave[2]->SetLineColor(2);
  g_wave[10]->SetLineColor(3);
}

void FillGraph()
{
  for(int i=0;i<16;i++){
    for(int j=0;j<RecordLength;j++){
      g_wave[i]->SetPoint(j,j,wave[i][j]);
    }
  }
}

void DrawWave()
{
  /* for(int i=0;i<4;i++){ */
  for(int i=0;i<RecordLength-1;i++){
    g_wave[10]->SetPoint(i,i,g_wave[2]->GetY()[i+1]-g_wave[2]->GetY()[i]+10000);
    if(g_wave[10]->GetY()[i]<9000)
      cout << i << endl;
    if(g_wave[10]->GetY()[i]<8000)
      cout << i << endl;
  }
  g_wave[2]->Draw("al");
  g_wave[10]->Draw("lsame");
  c1->Update();

}
