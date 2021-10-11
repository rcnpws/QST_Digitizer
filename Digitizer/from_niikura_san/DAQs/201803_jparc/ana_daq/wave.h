// wave.h
// $Id: wave.h 2018-02-21 17:42:00 daq Exp $

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TArrow.h"
#include "TH1.h"
#include "TText.h"
#include "TMath.h"

class WaveForm
{
 private:
  TTree *tree;

  // for analysis
 private:
  Double_t baseline[4] = {};
  Int_t ch2det[16] = { 0, 1, 2, 3,  4, 5, 6, 7,
		       8, 9,10,11, 12,13,14,15};
 public:
  void PSA();
  void FillTree(){tree->Fill();};
  void SaveTree(){tree->Write("",TObject::kOverwrite);};
  void InitVal();
  
  Int_t wave[16][2000];
  Int_t TS;
  Int_t ievent;
 private:
  Int_t TS1,TSprev;
  ULong64_t TimeStamp;
  Int_t t0, t1;
  Double_t QDC;
  Int_t gate[2] = {40,120};

  // for Oscilloscope mode
 private:
  TGraph *gra[10];
  Int_t col[10]={kRed, kBlack, kBlue, kGreen, kBlack, kBlack, kBlack, kBlack, kBlack, kBlack};
  TCanvas *c1;
 public:
  void DefineWave();
  void DrawWave();

  // for Histgram mode
 private:
  TH1D *h[10];
 public:
  void DefineHist();
  void FillHist();
  void DrawHist();
  
  // constuctors
 public:
  WaveForm(TFile*);
  ~WaveForm();

  
};

// constructor
WaveForm::WaveForm(TFile *f)
{
  TS1=0;
  TSprev=0;
  TimeStamp=0;
  
  f->cd();
  tree = new TTree("wave","WaveForm Tree");
  tree->Branch("TS",&TS,"TS/I");
  tree->Branch("ievent",&ievent,"ievent/I");
  tree->Branch("TimeStamp",&TimeStamp,"TimeStamp/l");
  tree->Branch("baseline",baseline,"baseline[4]/D");
  tree->Branch("t0",&t0,"t0/I");
  tree->Branch("t1",&t1,"t1/I");
  tree->Branch("QDC",&QDC,"QDC/D");
  
}

// destructor
WaveForm::~WaveForm()
{
}
// ========== PSA ===========

void WaveForm::InitVal()
{
  // -- get baseline
  for(int i=0;i<16;i++){
    for(int j=0;j<2000;j++){
      wave[i][j]=0;
    }
  }
  
}
void WaveForm::PSA()
{
  // -- get baseline
  for(int i=0;i<4;i++){
    baseline[i]=0;
    for(int j=0;j<30;j++){
      baseline[i]+=(double)wave[ch2det[i]][j]/30.;
    }
  }
  // -- find 25Hz timing
  t0=0;t1=0;
  for(int j=0;j<300;j++){
    if(baseline[ch2det[2]]-wave[ch2det[2]][j]>100){
      t0=j;
      break;
    }
  }
  for(int j=0;j<300;j++){
    if(baseline[ch2det[3]]-wave[ch2det[3]][j]>100){
      t1=j;
      break;
    }
  }
  // -- qdc
  QDC=0;
  if(t0>0){
    for(int j=t0+gate[0];j<t0+gate[1];j++){
      QDC+=baseline[ch2det[0]]-wave[ch2det[0]][j];
    }
  }
  if(QDC==0) QDC=-99;

  // -- timestamp
  if(TSprev>TS) TS1++;
  TimeStamp = (TS1*TMath::Power(2,31)+TS)*8 + t0*2;
  TSprev=TS;
  
}

// ========== FOR HISTOGRAM MODE ==============
void WaveForm::DefineHist()
{
  c1 = new TCanvas("c1","c1");
  h[0] = new TH1D("h","Beam intensity (per pulse)",1000,0,1000e3);
  
}

void WaveForm::FillHist()
{
  h[0]->Fill(QDC);
}

void WaveForm::DrawHist()
{
  h[0]->Draw();
  h[0]->GetXaxis()->SetRange(10,1000);
  Double_t mean = h[0]->GetMean();
  Double_t emean = h[0]->GetMeanError();
  h[0]->GetXaxis()->UnZoom();
  Double_t ypos = h[0]->GetMaximum();
  TText *tt = new TText();
  tt->SetTextAlign(12);
  tt->DrawText(500e3, ypos*0.9, Form("peak mean:"));
  tt->DrawText(500e3, ypos*0.8, Form("%.1f +- %.1f",mean,emean));
  c1->Update();
}

// ========== FOR OSCILLOSCOPE MODE ===========

void WaveForm::DefineWave()
{
  c1 = new TCanvas("c1","c1");
  c1->Divide(1,2);
  for(int i=0;i<10;i++){
    gra[i] = new TGraph();
    gra[i]->SetLineColor(col[i]);
    gra[i]->SetMaximum(17000);
    gra[i]->SetMinimum(0);
  }

}

void WaveForm::DrawWave()
{
  c1->cd(1);
  for(int i=0;i<4;i++){
    for(int j=0;j<300;j++){
      gra[i]->SetPoint(j,j,wave[ch2det[i]][j]);
    }
    if(i==0) gra[i]->Draw("al");
    else     gra[i]->Draw("lsame");
  }
  c1->cd(2);
  Int_t jj=0;
  for(int j=t0;j<t0+200;j++){
    gra[4]->SetPoint(jj,jj,baseline[ch2det[0]]-wave[ch2det[0]][j]+1000);
    jj++;
  }
  gra[4]->Draw("al");
  TArrow *ar = new TArrow();
  ar->DrawArrow(gate[0],10000,gate[1],10000,0.01,"<|>");
  TText *tt = new TText();
  tt->SetTextAlign(22);
  tt->DrawText((gate[0]+gate[1])/2, 11000, "gate");
  c1->Update();
}