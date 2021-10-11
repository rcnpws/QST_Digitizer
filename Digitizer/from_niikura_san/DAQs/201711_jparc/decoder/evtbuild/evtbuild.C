// ======================================================
// -- Analysis.C
//  main program for simple analysis
//  normally user do not need to modify this file
//
//                 written by M. Niikura
// 
// $Id: evtbuild.C 2018-03-19 15:35:43 daq Exp $

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

// -- User headers
#define BN0_cxx
#define Ge0_cxx
#define Ge1_cxx

#include "BN0.h"
#include "Ge0.h"
#include "Ge1.h"


using namespace std;

int main(int argc, char** argv)
{

  if(argc>2 || argc==1){
    cout << Form("Usage: ./evtbuild <RUN_NUMBER>\n");
    exit(0);
  }
  
  Int_t RunNumber = atoi(argv[argc-1]);
  TString inputfilename = Form("./root/run%04d.root",RunNumber);
  
  // TString inputfilename_gamma = Form("./root/run%04d_wave.root",RunNumber);
  // TString inputfilename_plastic = Form("./root/psa%04d.root",RunNumber);

  TChain *chain0 = new TChain("tree0");
  TChain *chain2 = new TChain("tree2");
  TChain *chain3 = new TChain("tree3");

  chain0->Add(inputfilename.Data());
  chain2->Add(inputfilename.Data());  
  chain3->Add(inputfilename.Data());

  TFile *outputfile = TFile::Open(Form("./root/sorted%04d.root",RunNumber),"recreate");
  BN0 fData0(chain0);
  Ge0 fData2(chain2);
  Ge1 fData3(chain3);

  // ULong64_t TSlong;
  // ULong64_t PreTS=0;
  // ULong64_t extratime=0;

  Int_t TSoffset=0;
  if(RunNumber==1001) TSoffset=-30;
  if(RunNumber==1089) TSoffset=-300;
  // if(RunNumber==3069) TSoffset=18200+370;
  // if(RunNumber==3070) TSoffset=20780+370;
  // if(RunNumber==3071) TSoffset=19370+485;
  // if(RunNumber==3072) TSoffset=18100+510;
  // if(RunNumber==3073) TSoffset=19360+490;
  // if(RunNumber==3074) TSoffset=19690+375;
  // if(RunNumber==3075) TSoffset=18100+445;
  // if(RunNumber==3076) TSoffset=18080+470;
  // if(RunNumber==3077) TSoffset=64830+480;

  TH1D *h1 = new TH1D("h1","h1",2000,0,50000);
  // TH1D *h2 = new TH1D("h2","h2",200,-10000,10000);
  // TH1D *TSdiff0 = new TH1D("TSdiff0","time1-time0",1000,-1e4,1e4);
  // TH1D *TSdiff1 = new TH1D("TSdiff1","time1-time0",1000,-1000,1000);
  // TH1D *TSdiff2 = new TH1D("TSdiff2","time2-time0",1000,-1000,1000);

  TH1D *gamma1p = new TH1D("gamma1p","gamma1",4000,0.,2000);
  TH1D *gamma1b = new TH1D("gamma1b","gamma1",4000,0.,2000);
  // TH1D *gamma2 = new TH1D("gamma2","gamma2",8000,0.,2000);
  // TH1D *gamma3 = new TH1D("gamma3","gamma3",8000,0.,2000);
  // TH2D *gg = new TH2D("gg","gg",5000,0.,5000,5000,0.,5000);

  // TTree *tree = new TTree("tree","eventbuild tree");
  // Double_t tpl;
  // Double_t g1,g2;
  // Int_t tsd1,tsd2;
  // Bool_t flag1,flag2;
  // Int_t goodevt;
  
  // tree->Branch("tpl",&tpl,"tpl/D");
  // tree->Branch("g1",&g1,"g1/D");
  // tree->Branch("g2",&g2,"g2/D");
  // tree->Branch("tsd1",&tsd1,"tsd1/I");
  // tree->Branch("tsd2",&tsd2,"tsd2/I");
  // tree->Branch("flag1",&flag1,"flag1/O");
  // tree->Branch("flag2",&flag2,"flag2/O");
  // tree->Branch("goodevt",&goodevt,"goodevt/I");
  
  ULong64_t start_evt1=0;
  // Long64_t start_evt2=0;

  ULong64_t imax = fData0.GetNumberOfEntry();
  ULong64_t maxj1 = fData2.GetNumberOfEntry();
  ULong64_t maxj2 = fData3.GetNumberOfEntry();

  cout << "imax  = " << imax << endl;
  cout << "maxj1 = " << maxj1 << endl;
  cout << "maxj2 = " << maxj2 << endl;

  ULong64_t preTS=0;
  
  for(ULong64_t i=0;i<imax;i++){
    
    if(i%10000==0)
      cout << i << endl;

    Long64_t time0 = fData0.GetTimeStamp(i)/1000.*8+TSoffset;
    
    // cout << Form("%lld %lld %lld %lld\n",i,time0, preTS, time0-preTS);
    if(i!=0 && time0-preTS<19500){
      // cout << "**" << endl;
      continue;
    }
    preTS=time0;

    ULong64_t j1=start_evt1;
    // ULong64_t j2=start_evt2;

    // cout << endl;
    // if(i==880) break;
    
    while(1){
      // === Ge0
      Long64_t time1 = fData2.GetTimeStamp(j1)/1000*2;
      j1++;
      if(j1>maxj1) break;

      if(time1<time0){
    	start_evt1=j1;
    	continue;
      } else if(time1-time0>40000.){
    	break;
      } else {
	// cout << Form("%lld",time1-time0) << endl;
	h1->Fill(time1-time0);
	if((time1-time0)>200 && (time1-time0)<500){
	  // cout << "**" << endl;
	  // cout << fData2.GetEnergy(j1-1) << endl;
	  // cout << "**" << endl;

	  gamma1p->Fill(fData2.GetEnergy(j1-1));
	}else if((time1-time0)>2500){
	  gamma1b->Fill(fData2.GetEnergy(j1-1));
	}	  
      }
    }

    // while(1){
    //   // === Ge2
    //   time2 = fData3.GetTimeStamp(j2)*2;
    //   j2++;
    //   if(j2>maxj2) break;
      
    //   // cout << Form("%lld %lld %lld\n",time0, time2, time2-time0);
    //   // cout << " j = " << j << endl;
    //   // cout << " startevt = " << start_evt << endl;

    //   if(time2-TSlong<-100000){
    // 	start_evt2=j2;
    // 	continue;
    //   }
    //   else if(time2-TSlong>100000.){
    // 	break;
    //   }

    //   tsd2=time2-TSlong;
    //   g2=fData3.GetEnergy(j2-1);
      
    //   TSdiff2->Fill(time2-TSlong);
    //   // TSdiff2->Fill(time2-(TSlong+fData0.GetTiming(i)));
    //   // if(time2-TSlong>tmax && time2-TSlong<tmin){
    //   // 	h_gamma2->Fill(fData3.GetEnergy(j2-1));
    //   // 	h_gamma->Fill(fData3.GetEnergy(j2-1));
    //   // }

    // }
      
    // // Long64_t time3 = fData3.GetTimeStamp(i)*2;
    // // cout << Form("          %10lld %10lld %10lld\n",
    // // 		 time1-time0,time2-time0,time3-time0);

    // if(tpl>-1000 && g1>0 && tsd1-tpl>0 && tsd1-tpl<500){
    //   flag1=true;
    //   gamma1->Fill(g1);
    //   gamma3->Fill(g1);
    // }
    // if(tpl>-1000 && g2>0 && tsd2-tpl>0 && tsd2-tpl<400){
    //   flag2=true;
    //   gamma2->Fill(g2);
    //   gamma3->Fill(g2);
    // }
    
    // if(flag1 && flag2){
    //   goodevt=3;
    // }else if(flag1){
    //   goodevt=1;
    // }else if(flag2){
    //   goodevt=2;
    // }else{
    //   goodevt=0;
    // }

    // if(goodevt==3){
    //   gg->Fill(g1,g2);
    //   gg->Fill(g2,g1);
    // }
    
    // tree->Fill();
  }

  outputfile->Write();
  outputfile->Close();
  
}

ULong64_t BN0::GetTimeStamp(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return TimeStamp;
}

ULong64_t BN0::GetNumberOfEntry()
{
  return fChain->GetEntries();
}

Double_t BN0::GetTiming(ULong64_t)
{
  // fChain->GetEntry(jevent);
  // if(aa[0]>1000 && TMath::Sqrt(aa[1]*aa[2])>2000 &&
  //    TMath::Abs(0.5*(tt[1]+tt[2])-tt[0])<1 &&
  //    TMath::Abs(pp[0]-1.061)<0.2 &&
  //    TMath::Abs(pp[1]-0.994)<0.2 &&
  //    TMath::Abs(pp[2]-0.992)<0.2){
  //   return tt[1]+tt[2];
  // }else{
  //   return -99999;
  // }
  return 0;
}

ULong64_t Ge0::GetTimeStamp(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return TimeStamp;
}

ULong64_t Ge0::GetNumberOfEntry()
{
  return fChain->GetEntries();
}

Double_t Ge0::GetEnergy(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return GeE;
}

ULong64_t Ge1::GetNumberOfEntry()
{
  return fChain->GetEntries();
}

Double_t Ge1::GetEnergy(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return GeE;
}

ULong64_t Ge1::GetTimeStamp(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return TimeStamp;
}

