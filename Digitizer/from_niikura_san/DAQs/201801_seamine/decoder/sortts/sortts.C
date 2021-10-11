// ======================================================
// -- Analysis.C
//  main program for simple analysis
//  normally user do not need to modify this file
//
//                 written by M. Niikura
// 
// $Id: sortts.C 2017-09-20 20:51:28 niikura Exp $

// -- ROOT headers
#include <TTree.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TF1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCutG.h>
#include <TString.h>
#include <TDatime.h>
#include <TMath.h>

// -- General headers
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

// // -- User headers
// #define BN0_cxx
// #define BN1_cxx
// #define Ge0_cxx
// #define Ge2_cxx

// #include "BN0.h"
// #include "BN1.h"
// #include "Ge0.h"
// #include "Ge2.h"

// #include "../detector.h"

// // ---
// Int_t debug;

using namespace std;

int main(int argc, char** argv)
{

  if(argc>2 || argc==1){
    cout << Form("Usage: ./sortts <RUN_NUMBER>\n");
    exit(0);
  }
  
  Int_t RunNumber = atoi(argv[argc-1]);
  // TString inputfilename = Form("./root/run%04d.root",RunNumber);
  TFile *inputfile = TFile::Open(Form("./root/run%04d.root",RunNumber),"update");

  Int_t PreTS;
  ULong64_t PreTimeTag;
  ULong64_t extratime;
  Long64_t nentries;
    
  Int_t TS0;
  ULong64_t TimeTag;
  ULong64_t TimeStamp;

  // =========
  TTree   *t0 = (TTree*)inputfile->Get("tree0");
  TBranch *branch0 = t0->Branch("TimeStamp",&TimeStamp);
  t0->SetBranchAddress("TS0",&TS0);
  nentries = t0->GetEntries();

  PreTS=0;
  extratime=0;
  
  for(Long64_t i=0;i<nentries;i++){
    t0->GetEntry(i);
    if(PreTS>TS0){
      extratime++;
    }
    TimeStamp = TS0 + extratime*TMath::Power(2,31);
    PreTS = TS0;
    branch0->Fill();
  }
  // t0->Print();
  t0->Write();

  // // =========
  // TTree   *t1 = (TTree*)inputfile->Get("tree1");
  // TBranch *branch1 = t1->Branch("TimeStamp",&TimeStamp);
  // t1->SetBranchAddress("TS1",&TS1);
  // nentries = t1->GetEntries();

  // PreTS=0;
  // extratime=0;
  
  // for(Long64_t i=0;i<nentries;i++){
  //   t1->GetEntry(i);
  //   if(PreTS>TS1){
  //     extratime++;
  //   }
  //   TimeStamp = TS1 + extratime*TMath::Power(2,31);
  //   PreTS = TS1;
  //   branch1->Fill();
  // }
  
  // // t1->Print();
  // t1->Write();
  
  // =========
  TTree   *t2 = (TTree*)inputfile->Get("tree2");
  TBranch *branch2 = t2->Branch("TimeStamp",&TimeStamp);
  t2->SetBranchAddress("TimeTag",&TimeTag);
  nentries = t2->GetEntries();

  PreTimeTag=0;
  extratime=0;
  
  for(Long64_t i=0;i<nentries;i++){
    t2->GetEntry(i);
    if(PreTimeTag>TimeTag){
      extratime++;
    }
    TimeStamp = TimeTag + extratime*TMath::Power(2,31);
    PreTimeTag = TimeTag;
    // cout << TimeStamp << " " << TimeTag << endl;
    branch2->Fill();
  }
  
  // t2->Print();
  t2->Write();
  
  // =========
  TTree   *t3 = (TTree*)inputfile->Get("tree3");
  TBranch *branch3 = t3->Branch("TimeStamp",&TimeStamp);
  t3->SetBranchAddress("TimeTag",&TimeTag);
  nentries = t3->GetEntries();

  PreTimeTag=0;
  extratime=0;
  
  for(Long64_t i=0;i<nentries;i++){
    t3->GetEntry(i);
    if(PreTimeTag>TimeTag){
      extratime++;
    }
    TimeStamp = TimeTag + extratime*TMath::Power(2,31);
    PreTimeTag = TimeTag;
    // cout << TimeStamp << " " << TimeTag << endl;
    branch3->Fill();
  }
  
  // t3->Print();
  t3->Write();
  
  
  delete inputfile;
}
