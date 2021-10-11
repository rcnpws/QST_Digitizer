// wave.C for oscilloscope mode only
// $Id: eventbuild.C 2018-03-19 16:55:58 daq Exp $


// -- ROOT headers
#include "TApplication.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH2.h"
#include "TString.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TRandom3.h"

// -- General headers
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

#define Wave_cxx
#define Ge1_cxx
#define Ge2_cxx

// -- User Headers
#include "Wave.h"
#include "Ge1.h"
#include "Ge2.h"

using namespace std;

int main(int argc, char *argv[])
{

  if(argc!=2){
    cout << "Usage: ./eventbuild <RUNNUMBER>" << endl;
    return 1;
  }

  // === Open files
  Int_t RunNumber = atoi(argv[1]);
  TString inputfilename = Form("./root/run%04d.root",RunNumber);
  TString outputfilename = Form("./root/sorted%04d.root",RunNumber);
  TFile *outputfile = TFile::Open(outputfilename,"recreate");
  cout << endl;
  cout << Form("======== Start analysis RUN #%04d ========",RunNumber) << endl;
  cout << Form("---> Open input file  : %s",inputfilename.Data()) << endl;
  cout << Form("---> Open output file : %s",outputfilename.Data()) << endl;

  TChain *ch0    = new TChain("wave");
  TChain *ch1 = new TChain("ge1");
  TChain *ch2 = new TChain("ge2");

  ch0->Add(inputfilename.Data());
  ch1->Add(inputfilename.Data());  
  ch2->Add(inputfilename.Data());
  
  Wave fData0(ch0);
  Ge1  fData1(ch1);
  Ge2  fData2(ch2);
  
  // === Define Histograms

  TH1D *h1 = new TH1D("h1","h1",4200,0,42);
  TH1D *h2 = new TH1D("h2","h2",4200,0,42);
  // TH1D *gamma1p = new TH1D("gamma1p","gamma1",4000,0.,2000);
  // TH1D *gamma1b = new TH1D("gamma1b","gamma1",4000,0.,2000);

  ULong64_t imax = fData0.GetNumberOfEntry();
  ULong64_t maxj1 = fData1.GetNumberOfEntry();
  ULong64_t maxj2 = fData2.GetNumberOfEntry();

  cout << "imax  = " << imax << endl;
  cout << "maxj1 = " << maxj1 << endl;
  cout << "maxj2 = " << maxj2 << endl;
  
  ULong64_t i1=0;
  ULong64_t i2=0;
  
  for(ULong64_t i=0;i<imax;i++){
  // for(ULong64_t i=0;i<1;i++){

    if(i%1000==0)
      cout << i << endl;
    
    Double_t time0 = fData0.GetTimeStamp(i)/1.e6;
    Double_t post_time;
    if(i<imax-1)
      post_time = fData0.GetTimeStamp(i+1)/1.e6;
    else
      post_time = time0+40.0;
    
    // cout << Form("--->  %f %f %f",time0,post_time,post_time-time0) << endl;

    while(1){
      Double_t time1 = fData1.GetTimeStamp(i1)/1.e6;
      i1++;
      if(i1>maxj1) break;
      
      if(time1<time0){
	continue;
      }else if(time1<post_time){
	Double_t Tdiff = time1-time0;
	h1->Fill(Tdiff);
      }else{
	i1--;
	break;
      }
    }
    
    while(1){
      Double_t time2 = fData2.GetTimeStamp(i2)/1.e6;
      i2++;
      if(i2>maxj2) break;
      
      if(time2<time0){
	continue;
      }else if(time2<post_time){
	Double_t Tdiff = time2-time0;
	h2->Fill(Tdiff);
      }else{
	i2--;
	break;
      }
    }
  }

  outputfile->Write();
  outputfile->Close();
  
}



ULong64_t Wave::GetTimeStamp(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  // return TimeStamp-0.1e6;
  return TimeStamp;
}

ULong64_t Wave::GetNumberOfEntry()
{
  return fChain->GetEntries();
}

ULong64_t Ge1::GetNumberOfEntry()
{
  return fChain->GetEntries();
}

Double_t Ge1::GetEnergy(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return Ecal;
}

ULong64_t Ge1::GetTimeStamp(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return TimeStamp;
}

ULong64_t Ge2::GetTimeStamp(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return TimeStamp;
}

ULong64_t Ge2::GetNumberOfEntry()
{
  return fChain->GetEntries();
}

Double_t Ge2::GetEnergy(ULong64_t jevent)
{
  fChain->GetEntry(jevent);
  return Ecal;
}

