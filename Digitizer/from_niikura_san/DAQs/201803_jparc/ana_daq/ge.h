// ge.h
// $Id: ge.h 2018-02-21 11:37:03 daq Exp $

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TArrow.h"
#include "TH1.h"
#include "TText.h"

class Ge
{
 private:
  TTree *tree;
  Int_t geid;

  // for analysis
 private:
  ULong64_t     TimeStamp;
  Int_t         TT1;
  ULong64_t     TTprev;
 public:
  ULong64_t	TT;
  Int_t		Eraw;
  Double_t	Ecal;
  Int_t		ievent;

  void FillTree();
  void SaveTree(){tree->Write("",TObject::kOverwrite);};

 public:
  Ge(TFile*, Int_t);
  ~Ge();

  // for Histogram mode
 public:
  TH1D *hraw;
  TH1D *hcal;
 public:
  void DefineHist();
  void FillHist();
  
};

// constructor
Ge::Ge(TFile *f, Int_t id)
{
  TT1=0;
  TTprev=0;
  TimeStamp=0;
  
  geid=id;
  f->cd();
  tree = new TTree(Form("ge%d",id),Form("Ge%d Tree",id));
  tree->Branch("TT",&TT,"TT/l");
  tree->Branch("TimeStamp",&TimeStamp,"TimeStamp/l");
  tree->Branch("Eraw",&Eraw,"Eraw/I");
  tree->Branch("Ecal",&Ecal,"Ecal/D");
  tree->Branch("ievent",&ievent,"ievent/I");
}

// destructor
Ge::~Ge()
{
}

// ============ ANALYSIS ===========
void Ge::FillTree(){
  // -- timestamp
  if(TTprev>TT) TT1++;
  TimeStamp = (TT1*TMath::Power(2,31)+TT)*2;
  TTprev=TT;

  tree->Fill();
}

// ============ FOR HISTOGRAM MODE ===========
void Ge::DefineHist()
{
  hraw = new TH1D(Form("hraw%d",geid),Form("Gamma (raw) For Ge%d",geid),4000,0,4000);
  hcal = new TH1D(Form("hcal%d",geid),Form("Gamma (cal) For Ge%d",geid),3000,0,1500);
}

void Ge::FillHist()
{
  hraw->Fill(Eraw);
  hcal->Fill(Ecal);
}

