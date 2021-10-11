// wave.C for oscilloscope mode only
// $Id: wave.C 2018-02-19 19:51:48 daq Exp $


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
#include "TRandom.h"

// -- General headers
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <unistd.h>

// -- User headers
#include "wave.h"
#include "utils.h"
#include "defwave.h"

using namespace std;

int main(int argc, char *argv[])
{
  if(argc<2){
    show_help();
    return 1;
  }

  Int_t RunNumber=0;
  Int_t option;

  Bool_t debug=false;
  Int_t oscillo_flag = 0;

  while((option=getopt(argc,argv,"wsoOd"))!=-1){
    switch(option){
    case 'o':
      oscillo_flag=1;
      break;
    case 'O':
      oscillo_flag=2;
      break;
    case 'd':
      debug=true;
      break;
    }
  }

  cout << endl;

  if(optind==argc-1){
    RunNumber = atoi(argv[optind]);
    if(RunNumber==0){
      show_error(Form("<run_number> should be integer!"));
      show_help();
      return 1;
    }
  }else if(optind>argc-1){
    show_error(Form("No <run_number> given!"));
    show_help();
    return 1;
  }else if(optind<argc-1){
    show_error(Form("This program can analyze run-by-run."));
    show_error(Form("Provide only ONE <run_number> !"));
    show_help();
    return 1;
  }

  cout << "========== START WaveForm Viewer ==========" << endl;
  cout << endl;
  cout << Form("---> Analysis for RUN # %04d",RunNumber) << endl;
  
  if(argc>2){
    cout << "---> Optoins : " << endl;
  }
  if(oscillo_flag==1)
    cout << "       event-by-event oscilloscope mode ON (No save mode)" << endl;
  if(oscillo_flag==2)
    cout << "       continuous oscilloscope mode ON (No save mode)" << endl;
  if(debug)
    cout << "       debug mode ON" << endl;

  cout << endl;


  TApplication app( "app", &argc, argv );
  DefineGraph();

  // ===================================================================================================
  // --- OPEN FILES

  char *inputfilename;
  if(RunNumber%3==0){
    inputfilename = Form("/data00/RCNP1801_00/run%04d.dat",RunNumber);
  }else if(RunNumber%3==1){
    inputfilename = Form("/data01/RCNP1801_01/run%04d.dat",RunNumber);
  }else{
    inputfilename = Form("/data02/RCNP1801_02/run%04d.dat",RunNumber);
  }

  std::ifstream *fin = new std::ifstream(inputfilename, ios::in | ios::binary);
  cout << "---> Input File  : " << inputfilename << endl;
  if(fin->fail()){
    show_error(Form("File open failed!! --- %s",inputfilename));
    return 1;
  }

  uint32_t dd;

  Int_t header[8];
  // event number for Ge
  Int_t iGeEvent1 = 0;
  Int_t iGeEvent2 = 0;
  // check bn and ch
  // Int_t PrevBN = 0;
  // Int_t PrevCH = 0;
  
  // Int_t ch	= -99; // Channel Number
  // Int_t bn	= -99; // Board Number

  fin->read( (char *)&dd, sizeof(uint32_t) );

  while(1){

    // =============== GE ==================
    if(dd==magic_number_ge){
      ULong64_t	TimeTag;
      ULong64_t	Format;
      uint16_t	Extras;
      uint32_t	Extras2;
      uint32_t	channel;
      uint32_t	evtnum;
      uint32_t	board_handle;
      uint32_t	evtnum_singleread;
      uint16_t	energy;
      fin->read( (char *)&TimeTag, sizeof(TimeTag) );
      fin->read( (char *)&Format, sizeof(Format) );
      fin->read( (char *)&Extras, sizeof(Extras) );
      fin->read( (char *)&Extras2, sizeof(Extras2) );
      fin->read( (char *)&channel, sizeof(channel) );
      fin->read( (char *)&evtnum, sizeof(evtnum) );
      fin->read( (char *)&board_handle, sizeof(board_handle) );
      fin->read( (char *)&evtnum_singleread, sizeof(evtnum_singleread) );     
      fin->read( (char *)&energy, sizeof(energy) );

      if(debug){
	if(channel==0){
	  iGeEvent1++;
	  cout << "GE1: " << iGeEvent1 << endl;
	}
	if(channel==2){
	  iGeEvent2++;
	  cout << "GE2: " << iGeEvent2 << endl;
	}
      }
      fin->read( (char *)&dd, sizeof(uint32_t) );
      continue;

    // =============== WAVE ==================
    }else if(dd==magic_number_wave){
      // cout << "Wave" << endl;
      for (int i=0;i<8;i++){
	fin->read( (char *)&dd, sizeof(int) );
	if(i==0) dd=dd&0x7fffffff; // TimeStamp has 31 bit
	header[i]=dd;
      }
      if(debug)
	cout << Form("Header Info: TS(0) = %10d, Event(4) = %6d,   BN(7) = %1d, CH(3) = %2d, MultiEvent(6) = %1d\n",header[0],header[4],header[7],header[3],header[6]);

      Int_t isample=0;
      while(1){
	fin->read( (char *)&dd, sizeof(int) );
	if(dd==magic_number_wave || dd==magic_number_ge) break;
	
	Int_t d1=( dd&0x0000ffff );
	Int_t d2=( dd&0xffff0000 ) >> 16;
	wave[header[3]][isample*2] = d1;
	wave[header[3]][isample*2+1] = d2;
	isample++;
	if(fin->eof()) break;
      }
      
    // =============== OTHER?? ==================
    }else{
      show_error(Form("?????"));
      exit(1);
    }

    // =============== END OF BOARD ==================
    if((header[7]==0 && LastCH[0]==header[3]) ||
       (header[7]==1 && LastCH[1]==header[3]) ||
       (header[7]==2 && LastCH[2]==header[3])){
      if(debug)
	cout << "=== End of Board ===" << endl;

      if(oscillo_flag && ShowBN==header[7]){
	FillGraph();
	DrawWave();
	if(oscillo_flag==1)
	  cin.ignore();
      }
    }

    if(fin->eof()) break;
  }    

}
