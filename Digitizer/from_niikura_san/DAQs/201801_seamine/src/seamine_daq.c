/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/
// 2017.7.27 T. Saito  
//     Ge test at Hongo for RAL ISIS muon experiment
// 2017.9.13 T.Saito
//     RAL ISIS Pd experiment


#include <CAENDigitizer.h>
#include <CAENComm.h>
#include <CAENVMElib.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "seamine_daq.h"
#include "Digitizer.h"
#include "IOconfig.h"
#include "sis3800.h"
#include "vlupo.h"


//#define MANUAL_BUFFER_SETTING   0
// The following define must be set to the actual number of connected boards
#define MAXNB   4 // @
#define MAXNB_V1724   0

// the board number with PHA
#define NB_PHA   MAXNB-1 // last board is pha
#define NB_V1730D   1   
#define NB_WF 0
// NB: the following define MUST specify the ACTUAL max allowed number of board's channels

#define MAXNBITS 14
//#define MAXNBITS 12

/* include some useful functions from file Functions.h
you can find this file in the src directory */
#include "Functions.h"

static CAEN_DGTZ_IRQMode_t INTERRUPT_MODE = CAEN_DGTZ_IRQ_MODE_ROAK;

int GetRunnumber(char* frunnumbername){
  int runnumber;
  FILE *frunnumber;
  
  frunnumber = fopen(frunnumbername, "r");
  if (frunnumber == NULL) {
    fprintf(stderr, "fail to open runnumber file\n");
    runnumber=0;
    return -1;
  }else if(fscanf(frunnumber,"%d",&runnumber) != 1){
    fprintf(stderr,"Error in reading runnumber\n");
    return -1;
  }
  fclose(frunnumber);
  printf("\nrunnumber: %d\n",runnumber);

  return runnumber;
}

int UpdateRunnumber(int runnumber,char* frunnumbername){
  FILE *frunnumber;
  frunnumber = fopen(frunnumbername, "w");
  if (frunnumber == NULL ) {
    printf("fail to open runnumber file\n");
    //runnumber=0;
    //goto QuitProgram;
  }else{
    runnumber = runnumber+1;
    fprintf(frunnumber, "%d\n", runnumber);
    printf(" \n\nwriteing mode was set, run number: %d\n\n",runnumber-1);
  }
  fclose(frunnumber);

  return runnumber;
}

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{
  //*****************************//
  //        input files          //
  //*****************************//
  char* ConfigFile[MAXNB];
  ConfigFile[0] = "./WaveDumpConfig1.txt";
  ConfigFile[1] = "./WaveDumpConfig2.txt";
  ConfigFile[2] = "./WaveDumpConfig3.txt";
  ConfigFile[3] = "./WaveDumpConfigPHA.txt";


  //const char datafile_path[100]="./data/";
  char datafile_path[100];
  const int changeRunTime=3600; //in sec. should be 3600 for 1 hour.

  char frunnumbername[30]="./runnumber.txt";

  //*******************************//

  
  WaveDumpConfig_t   WDcfg[MAXNB];
  
  WaveDumpRun_t      WDrun;
  CAEN_DGTZ_ErrorCode ret;
  ERROR_CODES ErrCode= ERR_NONE;
  char *buffer0 = NULL;                                    // readout buffer
  char *buffer1 = NULL;                                    // readout buffer
  char *buffer2 = NULL;                                    // readout buffer
  char *buffer3 = NULL;                                    // readout buffer

  char *EventPtr = NULL;
  char *EventPtr2 = NULL;
  CAEN_DGTZ_DPP_PHA_Event_t       *PHAEvents[MaxNChannels_V1730];  // events buffer
  //CAEN_DGTZ_DPP_PHA_Event_t       *PHAEvents[MaxNChannels_V1724];  // events buffer
  CAEN_DGTZ_DPP_PHA_Waveforms_t   *PHAWaveform=NULL;     // waveforms buffer
  
  int isVMEDevice= 0;

  DigitizerParams_t Params[MAXNB];
  HighVoltageParams_t HVParams[HV_MAXCHANNELS];
  
  //uint64_t PrevTime_V1730[MAXNB_V1730][MaxNChannels_V1730];
  uint64_t PrevTime_V1724[1][MaxNChannels_V1730];
  //    uint64_t PrevTime_V1724[MAXNB_V1724][MaxNChannels_V1724];
  //uint64_t ExtendedTT_V1730[MAXNB_V1730][MaxNChannels_V1730];
  uint64_t ExtendedTT_V1724[1][MaxNChannels_V1730];
  //    uint64_t ExtendedTT_V1724[MAXNB_V1724][MaxNChannels_V1724];
  //uint32_t *EHistoShort_V1730[MAXNB_V1730][MaxNChannels_V1730];   // Energy Histograms for short gate charge integration
  //uint32_t *EHistoLong_V1730[MAXNB_V1730][MaxNChannels_V1730];    // Energy Histograms for long gate charge integration
  //float *EHistoRatio_V1730[MAXNB_V1730][MaxNChannels_V1730];      // Energy Histograms for ratio Long/Short
  uint32_t *EHisto_V1724[1][MaxNChannels_V1730]; // Energy Histograms 
  //    uint32_t *EHisto_V1724[MAXNB_V1724][MaxNChannels_V1724]; // Energy Histograms 
  //int ECnt_V1730[MAXNB_V1730][MaxNChannels_V1730];                // Number-of-Entries Counter for Energy Histograms short and long gate
  int ECnt_V1724[1][MaxNChannels_V1730];                // Number-of-Entries Counter for Energy Histograms short and long gate
  //    int ECnt_V1724[MAXNB_V1724][MaxNChannels_V1724];                // Number-of-Entries Counter for Energy Histograms short and long gate
  //int TrgCnt_V1730[MAXNB_V1730][MaxNChannels_V1730];
  int TrgCnt_V1724[1][MaxNChannels_V1730];
  int PurCnt_V1724[1][MaxNChannels_V1730];

  uint32_t EventCounter_V1724[MaxNChannels_V1730];
  
  int handle[MAXNB];
  int BHandle;           //bridge handle, added by momiyama on 161211
  
  int i, b, ch, ev;
  int Quit=0;
  int AcqRun = 0;
  uint32_t AllocatedSize[MAXNB];
  uint32_t BufferSize;

  int Nb=0, Ne=0;
  int Nbite[MAXNB], Nevent[MAXNB];
  for(i=0;i<MAXNB;i++){
    Nbite[i]=0; Nevent[i]=0;
  }
  
  int DoSaveWave[MAXNB][MaxNChannels];
  int MajorNumber;
  int BitMask = 0;
  uint64_t CurrentTime, PrevRateTime, ElapsedTime;
  uint32_t NumEvents_V1730[MaxNChannels_V1730];
  uint32_t NumEvents_V1724[MaxNChannels_V1730];
  //uint32_t NumEvents_V1724[MaxNChannels_V1724];
  uint32_t NumEvents;
  CAEN_DGTZ_BoardInfo_t    BoardInfo1, BoardInfo2;
  
  CAEN_DGTZ_EventInfo_t     EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *Event16[MAXNB];  /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
  for(b=0;b<MAXNB;b++){
    Event16[b]=NULL;
  }
  CAEN_DGTZ_UINT8_EVENT_t  *Event8=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */
  CAEN_DGTZ_X742_EVENT_t   *Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
  CAEN_DGTZ_EventInfo_t     EventInfo2;
  CAEN_DGTZ_UINT16_EVENT_t *Event216=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
  CAEN_DGTZ_UINT8_EVENT_t  *Event28=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */
  CAEN_DGTZ_X742_EVENT_t   *Event2742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
  
  uint32_t printmask_V1730;   //added by momiyama on 161212
  uint32_t printmask_V1724;   //added by momiyama on 161212

  
  char f_all_name[120];
  //char f_all_test_name[120];
  char f_scaler_name[120];
  
  FILE *f_all;
  //FILE *f_all_test;
  FILE *f_scaler;
  
  FILE *f_probe1, *f_probe2;
  char probe1_name[120], probe2_name[120];
  uint32_t probe;
  uint16_t DigitalWaveLine16[16000];
  
  int evcnt1, evcnt2;
  int runnumber=-1;
  
  time_t starttime=0;
  time_t timer=0;
  int currentRunTime;
  
  uint32_t dummy_val = 0x1;

  /* for SIS3800 */
  Scaler_t Scaler;
  
  /* runnumber setting */
  runnumber = GetRunnumber(frunnumbername);
  if(runnumber<0){
    goto QuitProgram;
  }

  /* *********************************************************************** */
  /* Open and parse configuration file                                                       */
  /* *********************************************************************** */
  memset(&WDrun, 0, sizeof(WDrun));
  for(b=0;b<MAXNB;b++){
    memset(&WDcfg[b], 0, sizeof(WDcfg[b]));
  }
  
  { /* Load Config Files */
    ret=0;
    for(b=0;b<MAXNB;b++){
      ret |= LoadConfigFile(ConfigFile[b],&WDcfg[b]);
      if(b==NB_V1730D){
	WDcfg[b].Nch=8;
	WDcfg[b].EnableMask=0x00ff;
      }
    }
    if(ret){
      printf("error in loading wavedumpconfig file\n");
      goto QuitProgram;
    }
    if (ret == ERR_CONF_FILE_NOT_FOUND) {
      ErrCode = ret;
      goto QuitProgram;
    }
  }

  /////debug.... 
  time(&starttime);
  //printf("current time is %s",ctime(&starttime));
  
  memset(DoSaveWave, 0, MAXNB*MaxNChannels*sizeof(int));
  for (i=0; i<MAXNBITS; i++) {
    /* Create a bit mask based on number of bits of the board */
    BitMask |= 1<<i; 
  }

  for(ch=0; ch<MaxNChannels_V1730; ch++){
    // Set all histograms pointers to NULL (we will allocate them later)
    EHisto_V1724[0][ch] = NULL;
  }

  /* ********************************************************************** */
  /* Set Parameters                                                                          */
  /* *********************************************************************** */
  memset(&Params, 0, MAXNB*sizeof(DigitizerParams_t));

    //@@@@@@@@@@ this Params[] seems to be used for only PHA board (wf board is using WDcfg files. must be chacked later)
  Params[NB_PHA].LinkType = CAEN_DGTZ_PCIE_OpticalLink;  // Link Type
  Params[NB_PHA].VMEBaseAddress = 0;  // For direct CONET connection, VMEBaseAddress must be 0
  Params[NB_PHA].IOlev = CAEN_DGTZ_IOLevel_NIM;
       
  /****************************\
   *  Acquisition parameters    *
  \****************************/
  Params[NB_PHA].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_List; // CAEN_DGTZ_DPP_ACQ_MODE_List, CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope or CAEN_DGTZ_DPP_ACQ_MODE_Mixed
  Params[NB_PHA].RecordLength = 100;                    // Num of samples of the waveforms (only for Oscilloscope mode)
  //Params[NB_PHA].RecordLength = 16000;                // for test of Ge virtual and digital probe

  //Params[NB_PHA].ChannelMask = (b==2) ? 0x5  : 0xffff;           // Channel enable mask (0x5 == 0b0101) => ch0 and ch2
  //Params[NB_PHA].ChannelMask = 0x5;             // Channel enable mask (0x5 == 0b0101) => ch0 and ch2 
  Params[NB_PHA].ChannelMask = 0x15;             // Channel enable mask (0x5 == 0b0101) => ch0 and ch2 and ch4
                                   
  Params[NB_PHA].EventAggr = 0;                                  // number of events in one aggregate (0=automatic)
  Params[NB_PHA].PulsePolarity = CAEN_DGTZ_PulsePolarityPositive; // Pulse Polarity (this parameter can be individual)
    
  /* *********************************************************************** */
  /* Open the digitizer and read board information                                           */
  /* *********************************************************************** */
  if( CAENVME_Init(cvV2718, 0, 0, &BHandle) != cvSuccess ) {
    fprintf(stderr,"\n\n Error opening the device\n");
    return 0;
  }

  //char* FirmWareRelease;
  //ret = CAENVME_BoardFWRelease(BHandle,&FirmWareRelease);
  //printf("firmware version is %s",FirmWareRelease);
  printf("Bridge handle is %d\n",BHandle);

  { /* software reset boards */
    const uint32_t ADDR_BOARD_0 = 0x11110000;
    const uint32_t ADDR_BOARD_1 = 0x22220000; // @
    //    ret = CAENVME_WriteCycle(BHandle, ADDR_BOARD_0 | CAEN_DGTZ_SW_RESET_ADD ,&dummy_val,cvA32_U_DATA,cvD32)
    //   | CAENVME_WriteCycle(BHandle, ADDR_BOARD_1 | CAEN_DGTZ_SW_RESET_ADD ,&dummy_val,cvA32_U_DATA,cvD32);
    ret = CAENVME_WriteCycle(BHandle, ADDR_BOARD_1 | CAEN_DGTZ_SW_RESET_ADD ,&dummy_val,cvA32_U_DATA,cvD32);
    if(ret != 0) {
      fprintf(stderr,"Error in resetting board.\n");
      goto QuitProgram;
    }

    {
      struct timespec const ts = {0, 200000000}; // 200 msec
      nanosleep(&ts,NULL); // at lease 100 msec sleep after software reset is recommended in the manual
    }
  }

  for(b=0; b<MAXNB; b++) {
    /* IMPORTANT: The following function identifies the different boards with a system which may change
       for different connection methods (USB, Conet, ecc). Refer to CAENDigitizer user manual for more info.
       Some examples below */
        
    /* The following is for b boards connected via b USB direct links
       in this case you must set Params[b].LinkType = CAEN_DGTZ_USB and Params[b].VMEBaseAddress = 0 */
    //        ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, b, 0, Params[b].VMEBaseAddress, &handle[b]);

    /* The following is for b boards connected via 1 opticalLink in dasy chain
       in this case you must set Params[b].LinkType = CAEN_DGTZ_PCI_OpticalLink and Params[b].VMEBaseAddress = 0 */
    //ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, b, Params[b].VMEBaseAddress, &handle[b]);

    /* The following is for b boards connected to A2818 (or A3818) via opticalLink (or USB with A1718)
       in this case the boards are accessed throught VME bus, and you must specify the VME address of each board:
       Params[b].LinkType = CAEN_DGTZ_PCI_OpticalLink (CAEN_DGTZ_PCIE_OpticalLink for A3818 or CAEN_DGTZ_USB for A1718)
       Params[0].VMEBaseAddress = <0xXXXXXXXX> (address of first board) 
       Params[1].VMEBaseAddress = <0xYYYYYYYY> (address of second board) 
       etc */
    if(b==NB_PHA){
      ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, b+1, Params[b].VMEBaseAddress, &handle[b]);
      printf("opening digitizer as handle %d\n", b);
    }else{
      ret = CAEN_DGTZ_OpenDigitizer(WDcfg[b].LinkType, WDcfg[b].LinkNum, WDcfg[b].ConetNode, WDcfg[b].BaseAddress, &handle[b]);
      printf("opening digitizer as handle %d\n", b);    
    }
    if (ret) {
      fprintf(stderr,"Can't open digitizer in loop %d\n", b);
      fprintf(stderr,"error code is %d\n", ret);
      goto QuitProgram;    
    }

    /* Once we have the handler to the digitizer, we use it to call the other functions */
    /*
    //sw reset // excluded by saito because we did sw reset using VME bus above
    ret |= CAEN_DGTZ_Reset(handle[b]); 
    if (ret != 0) {
      fprintf(stderr,
	      "Error: Unable to reset digitizer %d.\n"
	      "Please reset digitizer manually then restart the program\n"
	      "error code is %d\n",
	      handle[b],ret);
      goto QuitProgram;
    }
    */
         
    if(b==NB_PHA){
      ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo2);
      if (ret) {
	printf("Can't read board info in loop %d\n", b);
	goto QuitProgram;
      }
      printf("Connected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo2.ModelName, b);
      printf("ROC FPGA Release is %s\n", BoardInfo2.ROC_FirmwareRel);
      printf("AMC FPGA Release is %s\n", BoardInfo2.AMC_FirmwareRel);
    }else{
      ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo1);
      if (ret) {
	printf("Can't read board info in loop %d\n", b);
	goto QuitProgram;
      }
      printf("Connected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo1.ModelName, b);
      printf("ROC FPGA Release is %s\n", BoardInfo1.ROC_FirmwareRel);
      printf("AMC FPGA Release is %s\n", BoardInfo1.AMC_FirmwareRel);
    }
    // Get Number of Channels, Number of bits, Number of Groups of the board */

    if(b==NB_PHA){
      // Perform calibration (if needed).
      calibrate(handle[b], &WDrun, BoardInfo2);
    }else{
      ret = GetMoreBoardInfo(handle[b], BoardInfo1, &WDcfg[b]);
      if (ret) {
	ErrCode = ERR_INVALID_BOARD_TYPE;
	goto QuitProgram;
      }
      // Perform calibration (if needed).
      //if (WDcfg[b].StartupCalibration)
      calibrate(handle[b], &WDrun, BoardInfo1);
    }
     
  }

  //printf("NBits are %d and %d",WDcfg1.Nbit,WDcfg2.Nbit);

  //    ret = CAENComm_Info(BoardInfo.CommHandle,CAENComm_VMELIB_handle,&BHandle);
  //    printf("error code is %d\n",ret);
  //    printf("BoardInfo.CommHandle is %d\n",BoardInfo.CommHandle);
  //    printf("BHandle is %d\n",BHandle);
  //    CAENVME_SystemReset(BHandle);

  /* *************************************************************************************** */
  /* Program the digitizer (see function ProgramDigitizer)                                   */
  /* *************************************************************************************** */
  for(b=0; b<MAXNB; b++) {
     
    if(b==NB_PHA){
      ret = ProgramDigitizer_PHA(handle[b],Params[b]); 
      if (ret) {
	fprintf(stderr,"Failed to program the second digitizer\n");
	ErrCode = ERR_DGZ_PROGRAM;
	goto QuitProgram;
      }
    }else{
      ret = ProgramDigitizer(handle[b], WDcfg[b], BoardInfo1, b, 0);
      if (ret) {
	fprintf(stderr,"Failed to program the first digitizer\n");
	ErrCode = ERR_DGZ_PROGRAM;
	goto QuitProgram;
      }
    }
  }

  for(b=0;b<MAXNB;b++){
    // Allocate memory for the event data and readout buffer
    printf("b == %d  Nbit  = %d   ",b, WDcfg[b].Nbit);
    if(WDcfg[b].Nbit == 8) {
      ret = CAEN_DGTZ_AllocateEvent(handle[b], (void**)&Event8);
    } else if (BoardInfo1.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
      ret = CAEN_DGTZ_AllocateEvent(handle[b], (void**)&Event742);
    } else {
      ret = CAEN_DGTZ_AllocateEvent(handle[b], (void**)&Event16[b]);  
    }
    
    if (ret != CAEN_DGTZ_Success) {
      ErrCode = ERR_MALLOC;
      goto QuitProgram;
    }
  }

  /* WARNING: This malloc must be done after the digitizer programming */
  //@ changed by saito
  //ret  = CAEN_DGTZ_MallocReadoutBuffer(handle[NB_WF], &buffer,&AllocatedSize[b]); // changed 180116
  ret  = CAEN_DGTZ_MallocReadoutBuffer(handle[0], &buffer0,&AllocatedSize[0]); // changed 180116
  ret |= CAEN_DGTZ_MallocReadoutBuffer(handle[1], &buffer1,&AllocatedSize[1]); // changed 180116
  ret |= CAEN_DGTZ_MallocReadoutBuffer(handle[2], &buffer2,&AllocatedSize[2]); // changed 180116
  
  ret |= CAEN_DGTZ_MallocReadoutBuffer(handle[3], &buffer3,&AllocatedSize[3]); // changed 180116
  ret |= CAEN_DGTZ_MallocDPPEvents(handle[NB_PHA], (void**)PHAEvents, &AllocatedSize[b]); 
  ret |= CAEN_DGTZ_MallocDPPWaveforms(handle[NB_PHA], (void**)&PHAWaveform, &AllocatedSize[b]); 
  
  if (ret) {
    fprintf(stderr,"Can't allocate memory buffers\n");
    fprintf(stderr,"ret = %d\n",ret);
    ErrCode = ERR_MALLOC;
    goto QuitProgram;    
  }


  /* *************************************************************************************** */
  /* Readout Loop                                                                            */
  /* *************************************************************************************** */

  /* initialization */
  for (ch=0; ch<MaxNChannels_V1730; ch++) {
    EHisto_V1724[0][ch] = (uint32_t *)malloc((1 << MAXNBITS) * sizeof(uint32_t));
    memset(EHisto_V1724[0][ch], 0, (1 << MAXNBITS) * sizeof(uint32_t));
    TrgCnt_V1724[0][ch] = 0;
    ECnt_V1724[0][ch] = 0;
    PrevTime_V1724[0][ch] = 0;
    ExtendedTT_V1724[0][ch] = 0;
    PurCnt_V1724[0][ch] = 0;
    NumEvents_V1724[ch] = 0;       //added by momiyama on 161211
    EventCounter_V1724[ch] = 0;       //added by momiyama on 161211
  }


  //@@@@@ debug
  /*
  ret |= CAEN_DGTZ_WriteRegister(handle[0],0x8100,0x00);  //aqcuisition mode
  uint32_t printregister;
  ret |= CAEN_DGTZ_ReadRegister(handle[0],0x8100,&printregister);
  printf("register 0x8100 setting is %"PRIx32"\n",printregister);
  */
  
  evcnt1 = 0; evcnt2 = 0;

  PrevRateTime = get_time();
  AcqRun = 0;
  PrintInterface();
  printf("Type a command: ");
  // while(Quit) {
  while(1) {
    // Check keyboard
    if(kbhit()) {
      char c = getch();
      if (c == 'q') {
	if(AcqRun) {
	  printf("running! \n");
	  continue;
	}       
	//Quit = 1;
	goto QuitProgram;
      }
      if(c == 'm'){
	if(AcqRun) {
	  printf("running! \n");
	  //continue; debug
	}
	printf("tempureture monitor ... \n");
	for(b=0;b<MAXNB;b++){
	  if(b==NB_V1730D){
	    ret = Monitor_Temperature(handle[b],8);
	  }else{
	    ret = Monitor_Temperature(handle[b],MaxNChannels_V1730);
	  }
	  if(ret){
	    printf("problem occered during reading tempureture board %d \n",b);
	    goto QuitProgram;
	  }
	  continue;
	}
      }
      if (c == 'w'){
	if(AcqRun) {
	  printf("Change to data writing mode is not allowed during the run.\n");
	  continue;
	}
	
	WDrun.ContinuousWrite = 1;
	
	//for(b=0; b<MAXNB; b++){
	//    for(ch=0; ch<MaxNChannels; ch++){
	//DoSaveWave[b][ch] = 1; /* save waveforms to file for each channel for each board (at next trigger) */
		
	/* set output filename */
	sprintf(datafile_path,"/data0%d/RCNP1801_0%d",runnumber%3,runnumber%3); //change HDD to write
	//sprintf(datafile_path,"/data02/RAL1709_02"); //using one HDD
	
	sprintf(f_all_name, "%s/run%04d.dat",datafile_path,runnumber);
	//sprintf(f_all_test_name, "%s/timestamp%04d.dat",datafile_path,runnumber);
	sprintf(f_scaler_name, "%s/sca%04d.txt",datafile_path,runnumber);

	if ((f_all = fopen(f_all_name, "wb")) == NULL){
	  printf("error in opening wave form f_all %s\n",f_all_name);
	  return -1;
	}

	
	//f_all_test = fopen(f_all_test_name, "w");
	//if (f_all_test == NULL){
	//  printf("error in opening wave form f_all_test %s\n",f_all_test_name);
	//  return -1;
	//}

	f_scaler = fopen(f_scaler_name, "w");
	if (f_scaler == NULL){
	  printf("error in opening wave form f_scalert %s\n",f_scaler_name);
	  return -1;
	}
	runnumber = UpdateRunnumber(runnumber,frunnumbername);

	//printf("open file: %s, %s \n",f_all_name,f_scaler_name);
	
	c = 's'; // start data taking immediately
      }
//      if (c == 'r')  {
//	for(b=0; b<MAXNB; b++) {
//	  CAEN_DGTZ_SWStopAcquisition(handle[b]); 
//	  printf("Restarted\n");
//	  CAEN_DGTZ_ClearData(handle[b]);
//	  CAEN_DGTZ_SWStartAcquisition(handle[b]);
//	}
//      }
      if (c == 's')  {
	if(AcqRun) {
	  printf("Already running!\n");
	  continue;
	}
	
	if(f_all == NULL) {
	  WDrun.ContinuousWrite = 0;
	}
	
	for (ch=0; ch<MaxNChannels_V1730; ch++) {
	  ExtendedTT_V1724[0][ch] = 0;
	}

	for(b=0; b<MAXNB; b++) {
	  CAEN_DGTZ_SWStopAcquisition(handle[b]); 
	  CAEN_DGTZ_ClearData(handle[b]);
	}

	// SIS3800
	ret = InitializeScaler(BHandle,&Scaler);
	if (ret) {
	  fprintf(stderr,"Error in initialization of SIS3800.\n");
	  goto QuitProgram;
	}
		
	// LUPO TS
	ret = InitializeLUPOTS(BHandle);
	
	if (ret) {
	  fprintf(stderr,"Error in initialization of LUPO TS.\n");
	  goto QuitProgram;
	}
	
	for(b=0; b<MAXNB; b++) {
	  CAEN_DGTZ_SWStartAcquisition(handle[b]);
	}
	
	//consideration about synchronization
	//for TRG OUT -> TRG IN daisy chain of RUN signal, corresponding master and first slave
	//onw to all is the same condition
	for(b=0; b<MAXNB; b++) {
	  CAEN_DGTZ_SendSWtrigger(handle[b]);
	}

	//CAEN_DGTZ_SWStartAcquisition(handle[2]);
	//CAEN_DGTZ_WriteRegister(handle[0], 0x817c, 0);  //for daisy chain to activate EXT IN on first board
	
	////SEAMINE mode TRG OUT -> S IN chain, corresponding first and second slave
	//CAEN_DGTZ_SendSWtrigger(handle[0]);
	//CAEN_DGTZ_WriteRegister(handle[0], 0x817c, 0); // Enable TRGIN of the first board
	////if (StartMode == START_SW_CONTROLLED) {
	//    CAEN_DGTZ_WriteRegister(handle[0], 0x8100, 0x4);
	////} else {
	////    CAEN_DGTZ_WriteRegister(handle[0], 0x8100, 0x5);
	////    printf("Run starts/stops on the S-IN high/low level\n");
	////}
	AcqRun = 1;

	time(&starttime);

	currentRunTime=0;
	
	if(WDrun.ContinuousWrite || WDrun.SingleWrite){
	  fprintf(f_scaler,"\nrun%d started at %s",runnumber-1, ctime(&starttime));
	}

      }
      if (c == 'S')  {
	if(!AcqRun) {
	  printf("Not running!\n");
	  continue;
	}

	for(b=0; b<MAXNB; b++) {
	  CAEN_DGTZ_SWStopAcquisition(handle[b]); 
	  CAEN_DGTZ_ClearData(handle[b]);
	}
	//consideration about run synchronization
	//TRG OUT -> TRG IN daisy chain
	//one to all is the same condition
	int cmdret = system(CLEARSCR);
	time(&timer);
	const int elapsedSec = difftime(timer,starttime);
	if(WDrun.ContinuousWrite || WDrun.SingleWrite){
	  printf("\n\nRun%d stopped at %s",runnumber-1, ctime(&timer));
	  fprintf(f_scaler,"\nrun%d stopped at %s",runnumber-1, ctime(&timer));
	}else{
	  printf("\n\nRun%d stopped at %s",runnumber, ctime(&timer));
	}
	printf("Duration = %1d:%02d:%02d\n\n",
	       elapsedSec/3600,(elapsedSec/60)%60,elapsedSec%60);

	for (b = 0; b < MAXNB; b++) {
	  //CAEN_DGTZ_SWStopAcquisition(handle[b]); 
	  CAEN_DGTZ_WriteRegister(handle[b], 0x8100, 0);
	  //printf("Acquisition Stopped for Board %d\n", b);
	  //caleraddress = 0xaaaaa02c;  //global counter disable
	  //ret = CAENVME_WriteCycle(BHandle,scaleraddress,&dummy_val,cvA32_U_DATA,cvD32);
	  //ret = CAENComm_Write32(BHandle,0xaaaaa000,0x100);
	  //for(ch=0; ch<MaxNChannels; ch++){
	  //  printf("ch %d counter = %d\n",ch,ChCnt[b][ch]);
	  //}
	}

	// sis3800
	ret = ScalerEndOfRun(BHandle, &Scaler,WDrun.ContinuousWrite || WDrun.SingleWrite,f_scaler );
	if (ret) {
	  fprintf(stderr,"Error in ireadout of SIS3800.\n");
	  goto QuitProgram;
	}
	
	////TRG OUT -> S IN chain
	//CAEN_DGTZ_WriteRegister(handle[0], 0x8100, 0x0);
	//printf("Acquisition Stopped");
	AcqRun = 0;
	//printf("start reading last events\n");
	/* Read data for the end of run */
	for(b=0; b<MAXNB; b++) {
	  
	  if(b==NB_PHA){
	    /* Read data from V1724 */
	    ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer3, &BufferSize);
	    if (ret) {
	      ShowAlert();
	      printf("Readout Error 2; %s\n",GetDigitizerErrorMessage(ret));
	      goto QuitProgram;
	    }
	    if (BufferSize == 0){ continue; }
	    //ret = DataConsistencyCheck((uint32_t *)buffer, BufferSize/4);
	    ret |= CAEN_DGTZ_GetDPPEvents(handle[b], buffer3, BufferSize, (void**)PHAEvents, NumEvents_V1724);
	    if (ret) { printf("Data Error: %d\n", ret); goto QuitProgram; }
	    /* Analyze data */
	    for (ch = 0; ch < MaxNChannels_V1724; ch++) {
	      ret = CAEN_DGTZ_GetChannelEnableMask(handle[b], &printmask_V1724);
	      if (!(printmask_V1724 & (1<<ch))) {continue; }
	      /* Update Histograms */
	      for (ev = 0; ev < NumEvents_V1724[ch]; ev++) {
		TrgCnt_V1724[0][ch]++;
		/* Time Tag */
		if (PHAEvents[ch][ev].TimeTag < PrevTime_V1724[0][ch])
		  ExtendedTT_V1724[0][ch]++;
		PrevTime_V1724[0][ch] = PHAEvents[ch][ev].TimeTag;
		/* Energy */
		if (PHAEvents[ch][ev].Energy > 0) {
		  // Fill the histograms
		  //EHisto_V1724[0][ch][(PHAEvents[ch][ev].Energy)&BitMask]++;
		  ECnt_V1724[0][ch]++;
		} else {  /* PileUp */ PurCnt_V1724[0][ch]++; }
		if (WDrun.ContinuousWrite || WDrun.SingleWrite)
		  WriteOutputFilesPHA(&WDcfg[NB_PHA], &WDrun,
				      PHAEvents[ch][ev].TimeTag, PHAEvents[ch][ev].Format,
				      PHAEvents[ch][ev].Extras,
				      //PHAEvents[ch][ev].Extras2,
				      ExtendedTT_V1724[0][ch], // substitute for Extras2
				      ch, EventCounter_V1724[ch], b, ev,
				      PHAEvents[ch][ev].Energy,
				      f_all);
		EventCounter_V1724[ch]++;
		/* Get Waveforms (continuously) */
		if ((Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) && WDrun.ContinuousWrite) {
		  int size;
		  int16_t *WaveLine;
		  uint8_t *DigitalWaveLine;
		  uint16_t *DigitalWaveLine16;
		  CAEN_DGTZ_DecodeDPPWaveforms(handle[b], &PHAEvents[ch][ev], PHAWaveform);
		  
		  // Use waveform data here...
		  size = (int)(PHAWaveform->Ns); // Number of samples
		  WaveLine = PHAWaveform->Trace1; // First trace (VIRTUALPROBE1 set with CAEN_DGTZ_SetDPP_PSD_VirtualProbe)
		  //SaveWaveform(b, ch, 1, size, WaveLine);
		} // loop to save waves        
	      } // loop on events
	    } // loop on channels
	  }else{
	    if(b==0) ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer0, &BufferSize);
	    if(b==1) ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer1, &BufferSize);
	    if(b==2) ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer2, &BufferSize);
			    	 
	    
	    if (ret) {
	      ShowAlert();
	      printf("Readout Error : %s\n",GetDigitizerErrorMessage(ret));
	      goto QuitProgram;
	    }
	    if (BufferSize == 0) continue;
	    NumEvents = 0;
	    if (BufferSize != 0) {
	      if(b==0)ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer0, BufferSize, &NumEvents);
	      if(b==1)ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer1, BufferSize, &NumEvents);
	      if(b==2)ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer2, BufferSize, &NumEvents);
	      
	      if (ret) { ErrCode = ERR_READOUT; printf("Error on getting event number\n"); goto QuitProgram; }
	    }
	    /* Analyze data */
	    for(i = 0; i < (int)NumEvents; i++) {
	      /* Get one event from the readout buffer */
	      if(b==0) ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer0, BufferSize, i, &EventInfo, &EventPtr);
	      if(b==1) ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer1, BufferSize, i, &EventInfo, &EventPtr);
	      if(b==2) ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer2, BufferSize, i, &EventInfo, &EventPtr);
	      
	      if (ret) { ErrCode = ERR_EVENT_BUILD; printf("error on getting event info\n"); goto QuitProgram; }
	      /* decode the event */
	      if (WDcfg[b].Nbit == 8)
		ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event8);
	      else
		if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
		  ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event16[b]);
		}
		else {
		  ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event742);
		}
	      if (ret) { ErrCode = ERR_EVENT_BUILD; printf("error on decoding\n"); goto QuitProgram; }
	      /* Write Event data to file */
	      if (WDrun.ContinuousWrite || WDrun.SingleWrite) {
		// Note: use a thread here to allow parallel readout and file writing
		if (BoardInfo1.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
		  ret = WriteOutputFilesx742(&WDcfg[b], &WDrun, &EventInfo, Event742);
		} else if (WDcfg[b].Nbit == 8) {
		  ret = WriteOutputFiles(&WDcfg[b], &WDrun, &EventInfo, Event8, i, f_all, b);
		} else {
		  ret = WriteOutputFiles(&WDcfg[b], &WDrun, &EventInfo, Event16[b], i, f_all, b);
		}
		if (ret) { ErrCode = ERR_OUTFILE_WRITE; printf("error on writing output file\n"); goto QuitProgram; }
		if (WDrun.SingleWrite) { WDrun.SingleWrite = 0; }
	      }else{} /*printf("continuous or single write disabled\n");*/
	    }
	    
	  }
	} // loop on boards
	
	//prepare next start
     	for(b=0; b<MAXNB; b++) {
	  ret |= CAEN_DGTZ_WriteRegister(handle[b], 0x8100, 0xe);
	}
	
	if (WDrun.ContinuousWrite) {
	  fclose(f_all);
	  f_all = NULL;
	  //fclose(f_all_test);
	  //f_all_test = NULL;
	  fclose(f_scaler);
	  f_scaler = NULL;
	}
	//if(WDrun.ContinuousWrite) fclose(f_probe1);
	//if(WDrun.ContinuousWrite) fclose(f_probe2);
      }
    }
    if (!AcqRun) {
      Sleep(10);
      continue;
    }
    

    /********************* interrupt *********************/
    // interrupt is made by V1730
    if (WDcfg[b].InterruptNumEvents > 0) {
      /* @@@@@@@@@@@@@@debug

      //if (WDcfg.InterruptNumEvents > 0) {
      //printf("loop for interrupt\n");
      int32_t boardId;
      int VMEHandle = -1;
      int InterruptMask = (1 << VME_INTERRUPT_LEVEL);
      
      BufferSize = 0;
      NumEvents = 0;
      // Interrupt handling
      if (isVMEDevice) {
	ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg[b].LinkType, WDcfg[b].LinkNum, WDcfg[b].ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
      } else {
	//printf("entering IRQwait\n");
	ret = CAEN_DGTZ_IRQWait(handle[b], INTERRUPT_TIMEOUT);
      }
      if (ret == CAEN_DGTZ_Timeout){  // No active interrupt requests
	//printf("bail out to Timeout\n");
	goto InterruptTimeout;
      }
      //printf("accepted interrupt\n");
      if (ret != CAEN_DGTZ_Success)  {
	ErrCode = ERR_INTERRUPT;
	//printf("interrupt error with code %d\n",ret);
	goto QuitProgram;
      }
      // Interrupt Ack
      if (isVMEDevice) {
	ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
	if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
	  goto InterruptTimeout;
	} else {
	  if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
	    ret = CAEN_DGTZ_RearmInterrupt(handle[b]);
	}
      }
      */
    }
        
    /* Read data from the boards */
    for(b=0; b<MAXNB; b++) {

      if(b==NB_PHA){
	//printf("PHA loop start\n");
	/* Read data from V1724 */
	ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer3, &BufferSize);
	if (ret) {
	  ShowAlert();
	  printf("Readout Error 22; %s\n",GetDigitizerErrorMessage(ret));
	  goto QuitProgram;
	}
	if (BufferSize == 0){continue;}
	
	Nb += BufferSize;
	ret |= CAEN_DGTZ_GetDPPEvents(handle[b], buffer3, BufferSize, (void**)PHAEvents, NumEvents_V1724);
	if (ret) {
	  fprintf(stderr,"Data Error: %d\n", ret);
	  goto QuitProgram;
	}

	/* Analyze data */
	for (ch = 0; ch < MaxNChannels_V1730; ch++) {
	  //printf("loop in analyzing data in V1724\n");
	  ret = CAEN_DGTZ_GetChannelEnableMask(handle[b], &printmask_V1724);
	  //printf("gotten channel enable mask is %"PRIx32"\n",printmask_V1724);
	  if (!(printmask_V1724 & (1<<ch))){
	    //printf("bail out from channel mask in PHA for ch %d\n", ch);
	    continue;
	  }

	  /* Update Histograms */
	  for (ev = 0; ev < NumEvents_V1724[ch]; ev++) {
	    TrgCnt_V1724[0][ch]++;
	    /* Time Tag */
	    if (PHAEvents[ch][ev].TimeTag < PrevTime_V1724[0][ch])
	      ExtendedTT_V1724[0][ch]++;
	    PrevTime_V1724[0][ch] = PHAEvents[ch][ev].TimeTag;
	    /* Energy */
	    if (PHAEvents[ch][ev].Energy > 0) {
	      // Fill the histograms
	      //EHisto_V1724[0][ch][(PHAEvents[ch][ev].Energy)&BitMask]++;
	      ECnt_V1724[0][ch]++;
	    } else {  /* PileUp */
	      PurCnt_V1724[0][ch]++;
	    }
	    if (WDrun.ContinuousWrite || WDrun.SingleWrite)
	      WriteOutputFilesPHA(&WDcfg[NB_PHA], &WDrun,
				  PHAEvents[ch][ev].TimeTag, PHAEvents[ch][ev].Format,
				  PHAEvents[ch][ev].Extras,
				  //PHAEvents[ch][ev].Extras2,
				  ExtendedTT_V1724[0][ch], // substitute for Extras2
				  ch, EventCounter_V1724[ch], b, ev,
				  PHAEvents[ch][ev].Energy,
				  f_all);
	    EventCounter_V1724[ch]++;
	    /* Get Waveforms (continuously) */
	    if ((Params[b].AcqMode != CAEN_DGTZ_DPP_ACQ_MODE_List) && WDrun.ContinuousWrite) {
	      int size;
	      int16_t *WaveLine;
	      uint8_t *DigitalWaveLine;
	      CAEN_DGTZ_DecodeDPPWaveforms(handle[b], &PHAEvents[ch][ev], PHAWaveform);
	      
	      // Use waveform data here...
	      size = (int)(PHAWaveform->Ns); // Number of samples
	      WaveLine = PHAWaveform->Trace1; // First trace (VIRTUALPROBE1 set with CAEN_DGTZ_SetDPP_PSD_VirtualProbe)
	      WaveLine = PHAWaveform->Trace2;

	      /* here should be code for saving waves */

	    } // loop to save waves        
	  } // loop on events
	} // loop on channels
	//printf("PHA loop end\n");
      }else{
	
	/* Read data from V1730 */
	
	if(b==0) ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer0, &BufferSize);
	if(b==1) ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer1, &BufferSize);
	if(b==2) ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer2, &BufferSize);
	
	if (ret) {
	  ShowAlert();
	  printf("Readout Error 0; %d %s\n",ret,GetDigitizerErrorMessage(ret));
	  printf("b == %d,  \n",b);
	  goto QuitProgram;    
	}
	if (BufferSize == 0) {
	  continue;
	}
	
	NumEvents = 0;
	if(b==0) ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer0, BufferSize, &NumEvents);
	if(b==1) ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer1, BufferSize, &NumEvents);
	if(b==2) ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer2, BufferSize, &NumEvents);

	
	if (ret) {
	  printf("error in GetNumEvents with code %d\n",ret);
	  ErrCode = ERR_READOUT;
	  goto QuitProgram;
	}

	Nbite[b] += BufferSize;
	Nevent[b] += NumEvents;

	/* Analyze data */
	for(i = 0; i < (int)NumEvents; i++) {
	  /* Get one event from the readout buffer */
	  if(b==0) ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer0, BufferSize, i, &EventInfo, &EventPtr);
	  if(b==1) ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer1, BufferSize, i, &EventInfo, &EventPtr);
	  if(b==2) ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer2, BufferSize, i, &EventInfo, &EventPtr);

	  
	  if (ret) {
	    printf("error in getting one event with code %d\n",ret);
	    ErrCode = ERR_EVENT_BUILD;
	    goto QuitProgram;
	  }
	  //printf("board ID for handle %d is %d\n",b,EventInfo.BoardId);
	  /* decode the event */
	  if (WDcfg[b].Nbit == 8) {
	    ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event8);
	  } else if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
	    ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event16[b]);
	  } else {
	    ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event742);
	  }
	  if (ret) {
	    ErrCode = ERR_EVENT_BUILD;
	    goto QuitProgram;
	  }
	  /* Write Event data to file */
	  if (WDrun.ContinuousWrite || WDrun.SingleWrite) {
	    //printf("writing procedure\n");
	    // Note: use a thread here to allow parallel readout and file writing
	    if (BoardInfo1.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
	      ret = WriteOutputFilesx742(&WDcfg[b], &WDrun, &EventInfo, Event742);
	    } else if (WDcfg[b].Nbit == 8) {
	      ret = WriteOutputFiles(&WDcfg[b], &WDrun, &EventInfo, Event8, i, f_all, b);
	    } else {
	      ret = WriteOutputFiles(&WDcfg[b], &WDrun, &EventInfo, Event16[b], i, f_all, b);
	    }
	    if (ret) {
	      ErrCode = ERR_OUTFILE_WRITE;
	      goto QuitProgram;
	    }
	    if (WDrun.SingleWrite) {
	      printf("Single Event saved to output files\n");
	      WDrun.SingleWrite = 0;
	    }
	  }
	} // loop on events
	
      }
    } // loop on boards

InterruptTimeout:
    /* Calculate throughput and trigger rate (every second) */

    CurrentTime = get_time();
    ElapsedTime = CurrentTime - PrevRateTime; /* milliseconds */
    if (ElapsedTime > 1000) {
      int cmdret = system(CLEARSCR);
      time(&timer);
      const int elapsedSec = difftime(timer,starttime);
      if(WDrun.ContinuousWrite || WDrun.SingleWrite){
	printf("\x1b[7mRun%4d [data writing]\x1b[0m\n",runnumber-1);
	printf("datafile is in %s\n",datafile_path);
      }else{
	printf("Run%4d [no save mode]\n",runnumber);
      }
      printf("Started at %s",ctime(&starttime));
      printf("Time elapsed = %1d:%02d:%02d\n",
      	     elapsedSec/3600,(elapsedSec/60)%60,elapsedSec%60);
      
      /*change run automaticaly*/
      if(elapsedSec - currentRunTime >= changeRunTime){
	printf("change run to next\n");
	currentRunTime=elapsedSec;
	if (WDrun.ContinuousWrite) {
	  fclose(f_all);
	  f_all = NULL;
	  //fclose(f_all_test);
	  //f_all_test = NULL;
	  fclose(f_scaler);
	  f_scaler = NULL;

	  sprintf(datafile_path,"/data0%d/RCNP1801_0%d",runnumber%3,runnumber%3); //change HDD to write

	  //sprintf(datafile_path,"/data02/RAL1709_02"); //using one HDD
	  
	  sprintf(f_all_name, "%s/run%04d.dat",datafile_path,runnumber);
	  //sprintf(f_all_test_name, "%s/timestamp%04d.dat",datafile_path,runnumber);
	  sprintf(f_scaler_name, "%s/sca%04d.txt",datafile_path,runnumber);
	  
	  if ((f_all = fopen(f_all_name, "wb")) == NULL){
	    printf("error in opening wave form f_all %s\n",f_all_name);
	    return -1;
	  }
	  //f_all_test = fopen(f_all_test_name, "w");
	  //if (f_all_test == NULL){
	  //  printf("error in opening wave form f_all_test %s\n",f_all_test_name);
	  //    return -1;
	  //}

	  f_scaler = fopen(f_scaler_name, "w");
	  if (f_scaler == NULL){
	    printf("error in opening wave form f_scalert %s\n",f_scaler_name);
	    return -1;
	  }
	  runnumber = UpdateRunnumber(runnumber,frunnumbername);


	}
      }
      
      for(b=0; b<MAXNB; b++) {
	printf("Board %d: ",b);
       
	if(b==NB_PHA) {
	  
	  printf("Reading at %5.1f KiB/s\n",
		 Nb / 1.024 / ElapsedTime);
	  
	  for(i=0; i<3; i += 2) {
	    //	    for(i=0; i<MaxNChannels_V1730; i++) {
	    if (TrgCnt_V1724[0][i]>0) {
	      printf("  ch%2d: TrgRate = %.2f kHz, PileUpRate = %.2f%%\n", i,
		     (float)TrgCnt_V1724[0][i]/ElapsedTime, 
		     100.*PurCnt_V1724[0][i]/TrgCnt_V1724[0][i]);
	    } else {
	      printf("  ch%2d: No Data\n", i);
	    }
	    TrgCnt_V1724[0][i]=0;
	    PurCnt_V1724[0][i]=0;
	  }
	  //scaler
	  ret = ScalerDuringRun(BHandle, &Scaler, WDrun.ContinuousWrite || WDrun.SingleWrite, f_scaler);
	  if (ret) {
	    fprintf(stderr,"Error in readout of SIS3800.\n");
	    goto QuitProgram;
	  }  
	  // lupo
	  ReadLUPOTS(BHandle,WDrun.ContinuousWrite || WDrun.SingleWrite, f_scaler);
	  if (ret) {
	    fprintf(stderr,"Error in readout of LUPO TS.\n");
	    goto QuitProgram;
	  }
	  
	}else{
	  if (Nbite[b] == 0) {
	    if (ret == CAEN_DGTZ_Timeout) {
	      printf("Timeout...\n");
	    } else {
	      printf("No data...\n");
	    }
	  } else {
	    printf("Reading at %.2f MiB/s (Trg Rate: %.2f Hz)\n",
		   Nbite[b] / 1048.576 / ElapsedTime, Nevent[b] * 1000. / ElapsedTime);
	  }
	  Nbite[b] = 0;
	  Nevent[b] = 0;
	  PrevRateTime = CurrentTime;
	}
	
      }
      Nb = 0;
      PrevRateTime = CurrentTime;
    }
  } // End of readout loop
  
QuitProgram:
  
  printf("quiting daq...");

  //printf("in the quit sequence\n");
  
  // stop the acquisition, close the device and free the buffers 
  for(b=0; b<MAXNB; b++) {
    //printf("board %d, software stop\n",b);
    CAEN_DGTZ_SWStopAcquisition(handle[b]);
  }


  for (ch=0; ch<MaxNChannels_V1730; ch++) {
    //printf("free EHisto\n");
    if(EHisto_V1724[0][ch]){
      free(EHisto_V1724[0][ch]);
      EHisto_V1724[0][ch]=NULL;
    }
  }

  //printf("free wavedump events\n");
  if(Event8) {
    CAEN_DGTZ_FreeEvent(handle[NB_WF], (void**)&Event8);
  }
      
  if(Event16) {
    CAEN_DGTZ_FreeEvent(handle[NB_WF], (void**)&Event16);
  }

  if(buffer0)  CAEN_DGTZ_FreeReadoutBuffer(&buffer0);
  if(buffer1)  CAEN_DGTZ_FreeReadoutBuffer(&buffer1);
  if(buffer2)  CAEN_DGTZ_FreeReadoutBuffer(&buffer2);
  if(buffer3)  CAEN_DGTZ_FreeReadoutBuffer(&buffer3);

  //printf("free DPP waveform\n");
  CAEN_DGTZ_FreeDPPWaveforms(handle[NB_PHA], PHAWaveform); 

  //printf("free readout buffer\n");
   
  for(b=0; b<MAXNB; b++){
    //printf("close digitizer\n");
    CAEN_DGTZ_CloseDigitizer(handle[b]);
  }

  printf(" .... done quiting program \n");
  
  
  return ret;
}

