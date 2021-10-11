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

#include <CAENDigitizer.h>
#include <CAENComm.h>
#include <CAENVMElib.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "seamine_daq.h"

#include "sis3800.h"

//#define MANUAL_BUFFER_SETTING   0
// The following define must be set to the actual number of connected boards
#define MAXNB   3
#define MAXNB_V1730   3
#define MAXNB_V1724   0
// NB: the following define MUST specify the ACTUAL max allowed number of board's channels
// it is needed for consistency inside the CAENDigitizer's functions used to allocate the memory
#define MaxNChannels 16
#define MaxNChannels_V1730 16
#define MaxNChannels_V1724 8

#define MAXNBITS 14
//#define MAXNBITS 12

/* include some useful functions from file Functions.h
you can find this file in the src directory */
#include "Functions.h"

static CAEN_DGTZ_IRQMode_t INTERRUPT_MODE = CAEN_DGTZ_IRQ_MODE_ROAK;

/* Error messages */
typedef enum  {
    ERR_NONE= 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_DGZ_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_DGZ_PROGRAM,
    ERR_MALLOC,
    ERR_RESTART,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTFILE_WRITE,
        ERR_OVERTEMP,

    ERR_DUMMY_LAST,
} ERROR_CODES;
static char ErrMsg[ERR_DUMMY_LAST][100] = {
    "No Error",                                         /* ERR_NONE */
    "Configuration File not found",                     /* ERR_CONF_FILE_NOT_FOUND */
    "Can't open the digitizer",                         /* ERR_DGZ_OPEN */
    "Can't read the Board Info",                        /* ERR_BOARD_INFO_READ */
    "Can't run WaveDump for this digitizer",            /* ERR_INVALID_BOARD_TYPE */
    "Can't program the digitizer",                      /* ERR_DGZ_PROGRAM */
    "Can't allocate the memory for the readout buffer", /* ERR_MALLOC */
    "Restarting Error",                                 /* ERR_RESTART */
    "Interrupt Error",                                  /* ERR_INTERRUPT */
    "Readout Error",                                    /* ERR_READOUT */
    "Event Build Error",                                /* ERR_EVENT_BUILD */
    "Can't allocate the memory fro the histograms",     /* ERR_HISTO_MALLOC */
    "Unhandled board type",                             /* ERR_UNHANDLED_BOARD */
    "Output file write error",                          /* ERR_OUTFILE_WRITE */
        "Over Temperature",                                                                     /* ERR_OVERTEMP */

};

const char* GetDigitizerErrorMessage(int err)
{
  static const char* const CAEN_DGTZ_ERR_MSG[34] = {
    "Operation completed successfully",                               // CAEN_DGTZ_Success
    "Communication error",					      // CAEN_DGTZ_CommError
    "Unspecified error",					      // CAEN_DGTZ_GenericError             
    "Invalid parameter",					      // CAEN_DGTZ_InvalidParam             
    "Invalid Link Type",					      // CAEN_DGTZ_InvalidLinkType          
    "Invalid device handle",					      // CAEN_DGTZ_InvalidHandle            
    "Maximum number of devices exceeded",			      // CAEN_DGTZ_MaxDevicesError          
    "The operation is not allowed on this type of board",	      // CAEN_DGTZ_BadBoardType             
    "The interrupt level is not allowed",			      // CAEN_DGTZ_BadInterruptLev          
    "The event number is bad",					      // CAEN_DGTZ_BadEventNumber           
    "Unable to read the registry",				      // CAEN_DGTZ_ReadDeviceRegisterFail   
    "Unable to write into the registry",			      // CAEN_DGTZ_WriteDeviceRegisterFail  
    "The channel number is invalid",				      // CAEN_DGTZ_InvalidChannelNumber     
    "The Channel is busy",					      // CAEN_DGTZ_ChannelBusy              
    "Invalid FPIO Mode",					      // CAEN_DGTZ_FPIOModeInvalid          
    "Wrong acquisition mode",					      // CAEN_DGTZ_WrongAcqMode             
    "This function is not allowed for this module",		      // CAEN_DGTZ_FunctionNotAllowed       
    "Communication Timeout",					      // CAEN_DGTZ_Timeout                  
    "The buffer is invalid",					      // CAEN_DGTZ_InvalidBuffer            
    "The event is not found",					      // CAEN_DGTZ_EventNotFound            
    "The vent is invalid",					      // CAEN_DGTZ_InvalidEvent             
    "Out of memory",						      // CAEN_DGTZ_OutOfMemory              
    "Unable to calibrate the board",				      // CAEN_DGTZ_CalibrationError         
    "Unable to open the digitizer",				      // CAEN_DGTZ_DigitizerNotFound        
    "The Digitizer is already open",				      // CAEN_DGTZ_DigitizerAlreadyOpen     
    "The Digitizer is not ready to operate",			      // CAEN_DGTZ_DigitizerNotReady        
    "The Digitizer has not the IRQ configured",			      // CAEN_DGTZ_InterruptNotConfigured   
    "The digitizer flash memory is corrupted",			      // CAEN_DGTZ_DigitizerMemoryCorrupted 
    "The digitizer dpp firmware is not supported in this lib version",// CAEN_DGTZ_DPPFirmwareNotSupported  
    "Invalid Firmware License",					      // CAEN_DGTZ_InvalidLicense           
    "The digitizer is found in a corrupted status",		      // CAEN_DGTZ_InvalidDigitizerStatus   
    "The given trace is not supported by the digitizer",	      // CAEN_DGTZ_UnsupportedTrace         
    "The given probe is not supported for the given digitizer's trace", // CAEN_DGTZ_InvalidProbe             
  };

  return (err < 1 || err > -34) ? CAEN_DGTZ_ERR_MSG[-err] : NULL;
}

/* ###########################################################################
*  Functions
*  ########################################################################### */

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPPParamsPHA_t DPPParams)
*   \brief   Program the registers of the digitizer with the relevant parameters
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */
int ProgramDigitizer_PSD(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams)
{
    int i, ret = 0;
    ret |= CAEN_DGTZ_Reset(handle);

    if (ret) {
        printf("ERROR: can't reset the digitizer.\n");
        return -1;
    }

    ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
    ret |= CAEN_DGTZ_SetIOLevel(handle, Params.IOlev);
    ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
    //ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);
    ret |= CAEN_DGTZ_SetChannelEnableMask(handle, Params.ChannelMask);
    ret |= CAEN_DGTZ_SetDPPEventAggregation(handle, Params.EventAggr, 0);
    ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    ret |= CAEN_DGTZ_SetDPPParameters(handle, Params.ChannelMask, &DPPParams);
   
    for(i=0; i<MaxNChannels_V1730; i++) {
//printf("processing channel %d\n",i);
        if (Params.ChannelMask & (1<<i)) {
            if((i%2)==0){
                ret |= CAEN_DGTZ_SetRecordLength(handle, Params.RecordLength, i);
            }
if(!ret) printf("no problem in channel loop %d\n", i);
if(ret) printf("problem here with code %d in channel loop %d\n",ret, i);
            ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, 0x8000);
            ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, 80);
            ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity);
        }
    }

    ret |= CAEN_DGTZ_SetDPP_PSD_VirtualProbe(handle, CAEN_DGTZ_DPP_VIRTUALPROBE_SINGLE, CAEN_DGTZ_DPP_PSD_VIRTUALPROBE_Baseline, CAEN_DGTZ_DPP_PSD_DIGITALPROBE1_R6_GateLong, CAEN_DGTZ_DPP_PSD_DIGITALPROBE2_R6_OverThr);

    uint32_t printregister;
    ret |= CAEN_DGTZ_ReadRegister(handle,0x811c,&printregister);
    printf("register 0x811c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x81a0,&printregister);
    printf("register 0x81a0 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x8110,&printregister);
    printf("register 0x8110 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x810c,&printregister);
    printf("register 0x810c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1084,&printregister);
    printf("register 0x1084 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1060,&printregister);
    printf("register 0x1060 setting is %"PRIx32"\n",printregister);

    // Set self trigger configuration
    ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY, 3);
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, 3);
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, Params.ChannelMask);

    ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13f);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13e);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x33c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13d);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x3c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x7c);  //LVDS setting

    ret |= CAEN_DGTZ_WriteRegister(handle,0x8110,0xe0000000);  //TRG-OUT
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8110,0xc0000000);  //TRG-OUT
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x0000);  //LVDS REGISTER
    ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x1111);  //LVDS TRIGGER

    ret |= CAEN_DGTZ_WriteRegister(handle,0x1084,0x5);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1060,0x1ff);

    if (ret) {
        printf("Warning: errors found during the programming of the PSD digitizer.\nSome settings may not be executed\n");
        return ret;
    } else {
        return 0;
    }
}


int ProgramDigitizer_PHA(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams)
{
    int i, ret = 0;
    uint32_t printregister;
    ret |= CAEN_DGTZ_Reset(handle);
    printf("program digitizer PHA\n");

    if (ret) {
        printf("ERROR: can't reset the digitizer.\n");
        return -1;
    }
    ret |= CAEN_DGTZ_ReadRegister(handle,0x8000,&printregister);
    printf("register 0x8000 setting is %"PRIx32"\n",printregister);
    //    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0x010ff910);  // Channel Control Reg (indiv trg, seq readout) ??
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0x010df910);  // Channel Control Reg (indiv trg, seq readout) ??
    ret |= CAEN_DGTZ_ReadRegister(handle,0x8000,&printregister);
    printf("register 0x8000 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0x01000110);  // Channel Control Reg (indiv trg, seq readout) ??
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0x01000114);  // Channel Control Reg (indiv trg, seq readout) ??


    ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
    ret |= CAEN_DGTZ_SetRecordLength(handle, Params.RecordLength);
    ret |= CAEN_DGTZ_SetIOLevel(handle, Params.IOlev);
    ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);
    ret |= CAEN_DGTZ_SetChannelEnableMask(handle, Params.ChannelMask);
    ret |= CAEN_DGTZ_SetDPPEventAggregation(handle, Params.EventAggr, 0);

/*
    //about synchronization
    //ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_TrgOutSinDaisyChain);
    ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    //one to all
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8110, 0x80000000);     // propagate only SW TRG
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8110, (0x80000000 + (1<<Params.RefChannel[i])));     // propagate SW TRG and auto trg to TRGOUT
    //a line just below appear later
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0x40000000);  // accept EXT TRGIN (from trg OR) 
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8100, 0xc);    // software controll 
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8100, 0xc);    // Arm acquisition (Run will be controlled by software)
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8100, 0xe);    // Arm acquisition (Run will start with 1st trigger)
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8100, RUN_START_ON_TRGIN_RISING_EDGE);    // Arm acquisition (Run will start with 1st trigger)
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8170, 0);   // Run Delay due to the transmission of the SW TRG in the TRGIN of the slaves

    //ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_1, CAEN_DGTZ_DPP_VIRTUALPROBE_Trapezoid);
    ////ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_1, CAEN_DGTZ_DPP_VIRTUALPROBE_Delta2);
    ////ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_2, CAEN_DGTZ_DPP_PHA_VIRTUALPROBE_Baseline);
    ////ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_2, CAEN_DGTZ_DPP_VIRTUALPROBE_Input);
    //ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, DIGITAL_TRACE_1, CAEN_DGTZ_DPP_DIGITALPROBE_BLHoldoff);
    ////ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, DIGITAL_TRACE_1, CAEN_DGTZ_DPP_DIGITALPROBE_Peaking);

    //setting for external trigger 
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0x80000001);  //accept SW TRG and desired channel self trigger
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0xc0000001);  //accept EXT and SW TRG and desired channel self trigger
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0xc0000000);  //accept EXT and SW TRG
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0x40000000);  //accept only EXT TRG
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x817c, 0x1);  //disable EXT TRG
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x817c, 0x0);  //enable EXT TRG
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0x80000001);  //trigger validation
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0xc0000000);  //trigger validation
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0x80000001);  //trigger validation
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0xc0000001);  //trigger validation
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0x40000000);
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x811c, 0x13c);  //front panel IO is NIM
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8080, 0x0);
*/
    ret |= CAEN_DGTZ_SetDPPParameters(handle, Params.ChannelMask, &DPPParams);

    for(i=0; i<MaxNChannels_V1730; i++) {
        if (Params.ChannelMask & (1<<i)) {
            ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, 55000);
            ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, 20);
            ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity);
        }
    }

//DAQ setup
//one to all, software trigger to all boards (test mode)
//also check 0x1080, 1180, CAEN_DGTZ_SetChannelSelfTrigger
    ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8110, 0xc0000000);     // propagate SW, EXT, self
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8100, 0xe);    // Arm acquisition (Run will start with 1st trigger)
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8170, 0);   // Run Delay due to the transmission of the SW TRG in the TRGIN of the slaves
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0xc0000000);  //accept SW and EXT TRG
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x817c, 0x1);  //disnable EXT TRG
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0xc0000000);  //trigger validation by SW and EXT
    ret |= CAEN_DGTZ_WriteRegister(handle, 0x811c, 0x13c);  //front panel IO is NIM
////one to all + Ge self, software trigger to all boards (test mode)
//    ret |= CAEN_DGTZ_SetRunSynchronizationMode(handle, CAEN_DGTZ_RUN_SYNC_Disabled);
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8110, 0x80000000);     // propagate only SW TRG
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8100, 0xe);    // Arm acquisition (Run will start with 1st trigger)
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8170, 0);   // Run Delay due to the transmission of the SW TRG in the TRGIN of the slaves
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x810c, 0x80000001);  //accept SW TRG and desired channel self trigger
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x817c, 0x1);  //disable EXT TRG
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x8180, 0x80000001);  //trigger validation
//    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8184, 0x80000002);  //trigger validation
//    ret |= CAEN_DGTZ_WriteRegister(handle, 0x811c, 0x13c);  //front panel IO is NIM
////end of DAQ setup

    // Set self trigger configuration
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY, 0x3);
    ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_DISABLED, 0x3);

    ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x1122);  //LVDS TRIGGER

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8028,0x1);  //dynamic range, 1 = 0.5 Vpp
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1028,0x1);  //dynamic range, 1 = 0.5 Vpp
    //   ret |= CAEN_DGTZ_WriteRegister(handle,0x1128,0x1);  //dynamic range, 1 = 0.5 Vpp
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1228,0x1);  //dynamic range, 1 = 0.5 Vpp

    //ret |= CAEN_DGTZ_ReadRegister(handle,0x8080,&printregister);
    //printf("register 0x8080 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8080,0x21602014);  //+0x1000000
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x8080,&printregister);
    //printf("register 0x8080 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1080,&printregister);
    printf("register 0x1080 setting is %"PRIx32"\n",printregister);
//default value seems to be 00400015
//below two values are decided during Ge test at RCNP on Feb. 5th
//defference is self trigger flag
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x20602014);  //+0x1000000
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x21602014);  //for Ge self
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x00602014);  //+0x1000000
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1080,&printregister);
    printf("register 0x1080 setting is %"PRIx32"\n",printregister);
    //    ret |= CAEN_DGTZ_ReadRegister(handle,0x1180,&printregister);
    //    printf("register 0x1180 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1180,0x20602014);  //+0x1000000
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1180,0x00602014);  //+0x1000000
    //    ret |= CAEN_DGTZ_WriteRegister(handle,0x1180,0x21602014);  //for Ge self
    //    ret |= CAEN_DGTZ_ReadRegister(handle,0x1180,&printregister);
    //    printf("register 0x1180 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1280,&printregister);
    printf("register 0x1280 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1280,0x20602014);  //+0x1000000
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1280,0x21602014);  //+0x1000000
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1280,0x00602014);  //+0x1000000
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1280,&printregister);
    printf("register 0x1280 setting is %"PRIx32"\n",printregister);

    ret |= CAEN_DGTZ_ReadRegister(handle,0x1034,&printregister);
    printf("register 0x1034 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x800c, 0x7);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x800c,&printregister);
    printf("register 0x800c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0xef1c,&printregister);
    printf("register 0xef1c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0xef00,0x8);
    ret |= CAEN_DGTZ_ReadRegister(handle,0xef00,&printregister);
    printf("register 0xef00 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0xef18,0x32);
    ret |= CAEN_DGTZ_ReadRegister(handle,0xef18,&printregister);
    printf("register 0xef18 setting is %"PRIx32"\n",printregister);
    if(ret)printf("error code %d\n",ret);

    //PHA parameters 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x106c,0x40); //trig. threshold
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1058,0x37);  //input rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1054,0x10);  //RC-CR2 smoothing
    ret |= CAEN_DGTZ_ReadRegister(handle,0x105c,&printregister);
    printf("register 0x105c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1064,&printregister);
    printf("register 0x1064 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1064,0x32);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1064,&printregister);
    printf("register 0x1064 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x107c,&printregister);
    printf("register 0x107c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x107c,0xa);  //RC-CR2 smoothing
    ret |= CAEN_DGTZ_ReadRegister(handle,0x107c,&printregister);
    printf("register 0x107c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1038,0x3e8);

    ret |= CAEN_DGTZ_WriteRegister(handle,0x126c,0x40); //trig. threshold
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1258,0x37);  //input rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1254,0x10);  //RC-CR2 smoothing
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1264,&printregister);
    printf("register 0x1264 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1264,0x32);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1264,&printregister);
    printf("register 0x1264 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x127c,&printregister);
    printf("register 0x127c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x127c,0xa);  //RC-CR2 smoothing
    ret |= CAEN_DGTZ_ReadRegister(handle,0x127c,&printregister);
    printf("register 0x127c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1238,0x3e8);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x12c4,&printregister);
    printf("register 0x12c4 setting is %"PRIx32"\n",printregister);

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1258,0x37);  //input rise time
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1254,0x10);  //RC-CR2 smoothing
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x1264,&printregister);
    //printf("register 0x1264 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1264,0x32);
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x1264,&printregister);
    //printf("register 0x1264 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x127c,&printregister);
    //printf("register 0x127c setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x127c,0xa);  //RC-CR2 smoothing
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x127c,&printregister);
    //printf("register 0x127c setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1238,0x3e8);
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x12c4,&printregister);
    //printf("register 0x12c4 setting is %"PRIx32"\n",printregister);

    ret |= CAEN_DGTZ_ReadRegister(handle,0x8120,&printregister);
    printf("register 0x8120 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x10a0,0x55); 
    ret |= CAEN_DGTZ_ReadRegister(handle,0x10a0,&printregister);
    printf("register 0x10a0 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_WriteRegister(handle,0x12a0,0x55); 
    ret |= CAEN_DGTZ_ReadRegister(handle,0x12a0,&printregister);
    printf("register 0x12a0 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1084,&printregister);
    printf("register 0x1084 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1284,&printregister);
    printf("register 0x1284 setting is %"PRIx32"\n",printregister);

    if (ret) {
        printf("Warning: errors found during the programming of the PHA digitizer.\nSome settings may not be executed\n");
        return ret;
    } else {
        return 0;
    }
}

/*! \fn      int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask)
*   \brief   writes 'data' on register at 'address' using 'mask' as bitmask
*
*   \param   handle :   Digitizer handle
*   \param   address:   Address of the Register to write
*   \param   data   :   Data to Write on the Register
*   \param   mask   :   Bitmask to use for data masking
*   \return  0 = Success; negative numbers are error codes
*/
int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask) {
    int32_t ret = CAEN_DGTZ_Success;
    uint32_t d32 = 0xFFFFFFFF;

    ret = CAEN_DGTZ_ReadRegister(handle, address, &d32);
    if(ret != CAEN_DGTZ_Success)
        return ret;

    data &= mask;
    d32 &= ~mask;
    d32 |= data;
    ret = CAEN_DGTZ_WriteRegister(handle, address, d32);
    return ret;
}

int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo, int board)
//int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo)
{
    int i, j, ret = 0;

    /* reset the digitizer */
    ret |= CAEN_DGTZ_Reset(handle);
    if (ret != 0) {
        printf("Error: Unable to reset digitizer.\nPlease reset digitizer manually then restart the program\n");
        printf("error code is %d\n",ret);
        return -1;
    }

printf("programming digitizer with wavedump way\n");
    // Set the waveform test bit for debugging
    if (WDcfg.TestPattern)
        ret |= CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, 1<<3);
    // custom setting for X742 boards
    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
        ret |= CAEN_DGTZ_SetFastTriggerDigitizing(handle,WDcfg.FastTriggerEnabled);
        ret |= CAEN_DGTZ_SetFastTriggerMode(handle,WDcfg.FastTriggerMode);
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)) {
        ret |= CAEN_DGTZ_SetDESMode(handle, WDcfg.DesMode);
    }
    ret |= CAEN_DGTZ_SetRecordLength(handle, WDcfg.RecordLength);
    ret |= CAEN_DGTZ_GetRecordLength(handle, &WDcfg.RecordLength);

    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
        ret |= CAEN_DGTZ_SetDecimationFactor(handle, WDcfg.DecimationFactor);
    }

    ret |= CAEN_DGTZ_SetPostTriggerSize(handle, WDcfg.PostTrigger);
    if(BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
        uint32_t pt;
        ret |= CAEN_DGTZ_GetPostTriggerSize(handle, &pt);
        WDcfg.PostTrigger = pt;
    }
    ret |= CAEN_DGTZ_SetIOLevel(handle, WDcfg.FPIOtype);
    if( WDcfg.InterruptNumEvents > 0) {
        // Interrupt handling
        if( ret |= CAEN_DGTZ_SetInterruptConfig( handle, CAEN_DGTZ_ENABLE,
            VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID,
            (uint16_t)WDcfg.InterruptNumEvents, INTERRUPT_MODE)!= CAEN_DGTZ_Success) {
                printf( "\nError configuring interrupts. Interrupts disabled\n\n");
                WDcfg.InterruptNumEvents = 0;
        }
    }
    ret |= CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg.NumEvents);
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
    ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, WDcfg.ExtTriggerMode);

    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)){
        ret |= CAEN_DGTZ_SetGroupEnableMask(handle, WDcfg.EnableMask);
        for(i=0; i<(WDcfg.Nch/8); i++) {
            if (WDcfg.EnableMask & (1<<i)) {
                if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
                    for(j=0; j<8; j++) {
                        if (WDcfg.DCoffsetGrpCh[i][j] != -1)
                            ret |= CAEN_DGTZ_SetChannelDCOffset(handle,(i*8)+j, WDcfg.DCoffsetGrpCh[i][j]);
                        else
                            ret |= CAEN_DGTZ_SetChannelDCOffset(handle,(i*8)+j, WDcfg.DCoffset[i]);
                    }
                }
                else {
                    ret |= CAEN_DGTZ_SetGroupDCOffset(handle, i, WDcfg.DCoffset[i]);
                    ret |= CAEN_DGTZ_SetGroupSelfTrigger(handle, WDcfg.ChannelTriggerMode[i], (1<<i));
                    ret |= CAEN_DGTZ_SetGroupTriggerThreshold(handle, i, WDcfg.Threshold[i]);
                    ret |= CAEN_DGTZ_SetChannelGroupMask(handle, i, WDcfg.GroupTrgEnableMask[i]);
                }
                ret |= CAEN_DGTZ_SetTriggerPolarity(handle, i, WDcfg.TriggerEdge);
            }
        }
    } else {
//printf("loop in digitizer family except for x740 or x742\n");
        ret |= CAEN_DGTZ_SetChannelEnableMask(handle, WDcfg.EnableMask);
uint32_t mask;
ret |= CAEN_DGTZ_GetChannelEnableMask(handle,&mask);
//printf("channel enable mask is set to %d\n",mask);
        for (i = 0; i < WDcfg.Nch; i++) {
//printf("entering loop to set individual channel setting \n");
            if (WDcfg.EnableMask & (1<<i)) {
                ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, WDcfg.DCoffset[i]);
uint32_t tval;
ret |= CAEN_DGTZ_GetChannelDCOffset(handle, i, &tval);
//printf("setted dc offset for channel %d is %d\n", i, tval);
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX730_FAMILY_CODE &&
                    BoardInfo.FamilyCode != CAEN_DGTZ_XX725_FAMILY_CODE)
                    ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, WDcfg.ChannelTriggerMode[i], (1<<i));
                ret |= CAEN_DGTZ_SetChannelTriggerThreshold(handle, i, WDcfg.Threshold[i]);
ret |= CAEN_DGTZ_GetChannelTriggerThreshold(handle, i, &tval);
//printf("setted trigger threshold for channel %d is %d\n",i , tval);
                ret |= CAEN_DGTZ_SetTriggerPolarity(handle, i, WDcfg.TriggerEdge);
ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, 1);
//ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, CAEN_DGTZ_PulsePolarityNegative);
ret |= CAEN_DGTZ_GetChannelPulsePolarity(handle, i, &tval);
//printf("channel pulse polarity of channel %d is %d\n",i,tval);
            }
        }
        if (BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
            BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) {
            // channel pair settings for x730 boards
            for (i = 0; i < WDcfg.Nch; i += 2) {
                if (WDcfg.EnableMask & (0x3 << i)) {
                    CAEN_DGTZ_TriggerMode_t mode = WDcfg.ChannelTriggerMode[i];
                    uint32_t pair_chmask = 0;

                    // Build mode and relevant channelmask. The behaviour is that,
                    // if the triggermode of one channel of the pair is DISABLED,
                    // this channel doesn't take part to the trigger generation.
                    // Otherwise, if both are different from DISABLED, the one of
                    // the even channel is used.
                    if (WDcfg.ChannelTriggerMode[i] != CAEN_DGTZ_TRGMODE_DISABLED) {
                        if (WDcfg.ChannelTriggerMode[i + 1] == CAEN_DGTZ_TRGMODE_DISABLED)
                            pair_chmask = (0x1 << i);
                        else
                            pair_chmask = (0x3 << i);
                    }
                    else {
                        mode = WDcfg.ChannelTriggerMode[i + 1];
                        pair_chmask = (0x2 << i);
                    }

                    pair_chmask &= WDcfg.EnableMask;
                    ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, mode, pair_chmask);
                }
            }
        }
    }
    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
        for(i=0; i<(WDcfg.Nch/8); i++) {
            ret |= CAEN_DGTZ_SetDRS4SamplingFrequency(handle, WDcfg.DRS4Frequency);
            ret |= CAEN_DGTZ_SetGroupFastTriggerDCOffset(handle,i,WDcfg.FTDCoffset[i]);
            ret |= CAEN_DGTZ_SetGroupFastTriggerThreshold(handle,i,WDcfg.FTThreshold[i]);
        }
    }

    /* execute generic write commands */
    for(i=0; i<WDcfg.GWn; i++)
        ret |= WriteRegisterBitmask(handle, WDcfg.GWaddr[i], WDcfg.GWdata[i], WDcfg.GWmask[i]);

    // Set self trigger configuration
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY, 1);
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, 1);
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, Params.ChannelMask);

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8000,0x50);  //channel polarity is negative
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8000,0x10);  //channel polarity is positive
    ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13f);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13e);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x33c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13d);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x3c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x7c);  //LVDS setting

//    ret |= CAEN_DGTZ_WriteRegister(handle,0x8110,0xe0000000);  //TRG-OUT
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8110,0xc0000000);  //TRG-OUT
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x0000);  //LVDS REGISTER
    ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x1122);  //LVDS TRIGGER

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1084,0x3);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x0);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x32);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x3e8);

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x810c,0xc0000001);  //global trigger is external or first couple self trigger

    /* copy of writing register process in PSD */
    /* should incluede generic write commands above */
    uint32_t printregister;
    ret |= CAEN_DGTZ_ReadRegister(handle,0x8000,&printregister);
    printf("register 0x8000 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x811c,&printregister);
    printf("register 0x811c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x81a0,&printregister);
    printf("register 0x81a0 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x8110,&printregister);
    printf("register 0x8110 setting is %"PRIx32"\n",printregister);
    
    ret |= CAEN_DGTZ_ReadRegister(handle,0x811c,&printregister);
    printf("register 0x811c setting is %"PRIx32"\n",printregister);

    ret |= CAEN_DGTZ_ReadRegister(handle,0x810c,&printregister);
    printf("register 0x810c setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1084,&printregister);
    printf("register 0x1084 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_ReadRegister(handle,0x1060,&printregister);
    //printf("register 0x1060 setting is %"PRIx32"\n",printregister);

    
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1098,&printregister);
    printf("register 0x1098 setting is %"PRIx32"\n",printregister);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1098,0x100);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1028,&printregister);
    printf("register 0x1028 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x1080,&printregister);
    printf("register 0x1080 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0x8000,&printregister);
    printf("register 0x8000 setting is %"PRIx32"\n",printregister);

    ret |= CAEN_DGTZ_ReadRegister(handle,0xef00,&printregister);
    printf("register 0xef00 setting is %"PRIx32"\n",printregister);
    ret |= CAEN_DGTZ_ReadRegister(handle,0xef18,&printregister);
    printf("register 0xef18 setting is %"PRIx32"\n",printregister);

    ////for run synchronization of first board
    ////it is based on individual triggering
    //if(board==2){
    //    ret |= CAEN_DGTZ_ReadRegister(handle, 0x811c, &printregister);
    //    printf("register 0x811c setting is %"PRIx32"\n",printregister);
    //    printregister = printregister & 0xFFF0FFFF | 0x00010000;
    //    ret |= CAEN_DGTZ_WriteRegister(handle, 0x811c, printregister);
    //    ret |= CAEN_DGTZ_ReadRegister(handle, 0x811c, &printregister);
    //    printf("register 0x811c setting is %"PRIx32"\n",printregister);
    //}else if(board==3){
    //    //ret |= CAEN_DGTZ_WriteRegister(handle, ADDR_ACQUISITION_MODE, RUN_START_ON_SIN_LEVEL);
    //    //ret |= CAEN_DGTZ_WriteRegister(handle, ADDR_GLOBAL_TRG_MASK, 0x40000000 + (1<<(int)Params.RefChannel[i]));  //  accept EXT TRGIN or trg from selected channel 
    //    ////ret |= CAEN_DGTZ_ReadRegister(handle[i], ADDR_FRONT_PANEL_IO_SET, &reg);
    //    //ret |= CAEN_DGTZ_WriteRegister(handle, ADDR_TRG_OUT_MASK, 0);   // no trigger propagation to TRGOUT
    //    //ret |= CAEN_DGTZ_WriteRegister(handle, ADDR_RUN_DELAY, 2*(1-i));   // Run Delay decreases with the position (to compensate for run the propagation delay)
    //}


    if (ret)
        printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");

    return 0;
}

int GetMoreBoardInfo(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo, WaveDumpConfig_t *WDcfg)
{
    int ret;
    switch(BoardInfo.FamilyCode) {
        CAEN_DGTZ_DRS4Frequency_t freq;

    case CAEN_DGTZ_XX724_FAMILY_CODE:
    case CAEN_DGTZ_XX781_FAMILY_CODE:
    case CAEN_DGTZ_XX780_FAMILY_CODE:
        WDcfg->Nbit = 14; WDcfg->Ts = 10.0; break;
    case CAEN_DGTZ_XX720_FAMILY_CODE: WDcfg->Nbit = 12; WDcfg->Ts = 4.0;  break;
    case CAEN_DGTZ_XX721_FAMILY_CODE: WDcfg->Nbit =  8; WDcfg->Ts = 2.0;  break;
    case CAEN_DGTZ_XX731_FAMILY_CODE: WDcfg->Nbit =  8; WDcfg->Ts = 2.0;  break;
    case CAEN_DGTZ_XX751_FAMILY_CODE: WDcfg->Nbit = 10; WDcfg->Ts = 1.0;  break;
    case CAEN_DGTZ_XX761_FAMILY_CODE: WDcfg->Nbit = 10; WDcfg->Ts = 0.25;  break;
    case CAEN_DGTZ_XX740_FAMILY_CODE: WDcfg->Nbit = 12; WDcfg->Ts = 16.0; break;
    case CAEN_DGTZ_XX725_FAMILY_CODE: WDcfg->Nbit = 14; WDcfg->Ts = 4.0; break;
    case CAEN_DGTZ_XX730_FAMILY_CODE: WDcfg->Nbit = 14; WDcfg->Ts = 2.0; break;
    case CAEN_DGTZ_XX742_FAMILY_CODE:
        WDcfg->Nbit = 12;
        if ((ret = CAEN_DGTZ_GetDRS4SamplingFrequency(handle, &freq)) != CAEN_DGTZ_Success) return CAEN_DGTZ_CommError;
        switch (freq) {
        case CAEN_DGTZ_DRS4_1GHz:
            WDcfg->Ts = 1.0;
            break;
        case CAEN_DGTZ_DRS4_2_5GHz:
            WDcfg->Ts = (float)0.4;
            break;
        case CAEN_DGTZ_DRS4_5GHz:
            WDcfg->Ts = (float)0.2;
            break;
        }
        switch(BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->MaxGroupNumber = 4;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
        default:
            WDcfg->MaxGroupNumber = 2;
            break;
        }
        break;
    default: return -1;
    }
    if (((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) ||
        (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) ) && WDcfg->DesMode)
        WDcfg->Ts /= 2;

    switch(BoardInfo.FamilyCode) {
    case CAEN_DGTZ_XX724_FAMILY_CODE:
    case CAEN_DGTZ_XX781_FAMILY_CODE:
    case CAEN_DGTZ_XX780_FAMILY_CODE:
    case CAEN_DGTZ_XX720_FAMILY_CODE:
    case CAEN_DGTZ_XX721_FAMILY_CODE:
    case CAEN_DGTZ_XX751_FAMILY_CODE:
    case CAEN_DGTZ_XX761_FAMILY_CODE:
    case CAEN_DGTZ_XX731_FAMILY_CODE:
        switch(BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 8;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR: 
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 4;
            break;
        }
        break;
    case CAEN_DGTZ_XX725_FAMILY_CODE: 
    case CAEN_DGTZ_XX730_FAMILY_CODE: 
        switch(BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 16;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 8;
            break;
        }
        break;
    case CAEN_DGTZ_XX740_FAMILY_CODE:
        switch( BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 64;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 32;
            break;
        }
        break;
    case CAEN_DGTZ_XX742_FAMILY_CODE:
        switch( BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 36;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 16;
            break;
        }
        break;
    default:
        return -1;
    }
    return 0;
}

int32_t BoardSupportsCalibration(CAEN_DGTZ_BoardInfo_t BoardInfo) {
    return
        BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE;
}

void calibrate(int handle, WaveDumpRun_t *WDrun, CAEN_DGTZ_BoardInfo_t BoardInfo) {
    //printf("\n");
    if (BoardSupportsCalibration(BoardInfo)) {
        if (WDrun->AcqRun == 0) {
            int32_t ret = CAEN_DGTZ_Calibrate(handle);
            if (ret == CAEN_DGTZ_Success) {
                printf("ADC Calibration successfully executed.\n");
            }
            else {
                printf("ADC Calibration failed. CAENDigitizer ERR %d\n", ret);
            }
            //printf("\n");
        }
        else {
            printf("Can't run ADC calibration while acquisition is running.\n");
        }
    }
    else {
        printf("ADC Calibration not needed for this board family.\n");
    }
}

int WriteOutputFiles(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, CAEN_DGTZ_EventInfo_t *EventInfo, void *Event, int evtnum_singleread, FILE *f_all, int board_handle, FILE *f_all_test)
{
  CAEN_DGTZ_UINT16_EVENT_t  *Event16 = NULL;
  CAEN_DGTZ_UINT8_EVENT_t   *Event8 = NULL;

  if (WDcfg->Nbit == 8)
    Event8 = (CAEN_DGTZ_UINT8_EVENT_t *)Event;
  else
    Event16 = (CAEN_DGTZ_UINT16_EVENT_t *)Event;

  for (int ch = 0; ch < WDcfg->Nch; ch++) {
    const int Size = (WDcfg->Nbit == 8) ? Event8->ChSize[ch]
                                        : Event16->ChSize[ch];
    if (Size <= 0) {
      continue;
    }

    // Binary file format
    const uint32_t magic_word = 0xffffffff;
    enum { BIN_HEADER_SIZE = 8, };
    const uint32_t BinHeader[BIN_HEADER_SIZE] = {
      EventInfo->TriggerTimeTag,
      EventInfo->BoardId,
      EventInfo->Pattern,
      ch,
      EventInfo->EventCounter,
      6*sizeof(uint32_t) + Size * ((WDcfg->Nbit == 8) ? 1 : 2),
      evtnum_singleread,
      board_handle,
    };

    if( WDcfg->OutFileFlags & OFF_HEADER) {
      // Write the Channel Header
      if(fwrite(&magic_word,sizeof(magic_word),1,f_all)!=1){
	// error writing to file
	fclose(f_all);
	f_all= NULL;
	fprintf(stderr,"error in writing magic word\n");
	return -1;
      }

      if(fwrite(BinHeader, sizeof(*BinHeader), BIN_HEADER_SIZE, f_all) != BIN_HEADER_SIZE) {
	// error writing to file
	fclose(f_all);
	f_all= NULL;
	fprintf(stderr,"error in writing header\n");
	return -1;
      }
    }

    {
      const int ns = (WDcfg->Nbit == 8) ?
		        (int)fwrite(Event8->DataChannel[ch], 1, Size, f_all)
		      : (int)fwrite(Event16->DataChannel[ch] , 1 , Size*2, f_all) / 2;
      if (ns != Size) {
	// error writing to file
	fclose(f_all);
	f_all= NULL;
	fprintf(stderr,"error in writing wave form\n");
	return -1;
      }
    }

    if (WDrun->SingleWrite) {
      fclose(f_all);
      f_all=NULL;
      fclose(WDrun->fout[ch]);
      WDrun->fout[ch]= NULL;
      fclose(WDrun->fout2[ch]);
      WDrun->fout2[ch]= NULL;
      if(ch==0){
	fclose(WDrun->fout3[0]);
	WDrun->fout3[0]= NULL;
      }
    }
  }
  return 0;
}

int WriteOutputFilesPHA(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, uint64_t TimeTag, uint64_t Format, int16_t Extras, uint32_t Extras2, uint32_t channel, uint32_t evtnum, uint32_t board_handle, uint32_t evtnum_singleread, uint16_t Energy, FILE *f_all, FILE *f_all_test)
{
  const uint32_t magic_word = 0xefefefef;

  if( WDcfg->OutFileFlags & OFF_HEADER) {
    if(fwrite(&magic_word,sizeof(magic_word),1,f_all)!=1){
      fprintf(stderr,"error in writing magic word\n");
      fclose(f_all); f_all= NULL; return -1;
    }

    if((fwrite(&TimeTag,           sizeof(TimeTag),           1, f_all) != 1) ||
       (fwrite(&Format,            sizeof(Format),            1, f_all) != 1) ||
       (fwrite(&Extras,            sizeof(Extras),            1, f_all) != 1) ||
       (fwrite(&Extras2,           sizeof(Extras2),           1, f_all) != 1) ||
       (fwrite(&channel,           sizeof(channel),           1, f_all) != 1) ||
       (fwrite(&evtnum,            sizeof(evtnum),            1, f_all) != 1) ||
       (fwrite(&board_handle,      sizeof(board_handle),      1, f_all) != 1) ||
       (fwrite(&evtnum_singleread, sizeof(evtnum_singleread), 1, f_all) != 1) ||
       (fwrite(&Energy,            sizeof(Energy),            1, f_all) != 1) ) {
      fprintf(stderr,"error in writing header\n");
      fclose(f_all); f_all= NULL; return -1;
    }

    fprintf(f_all_test, "TimeTag: %"PRIu64"\n", TimeTag);
    fprintf(f_all_test, "Format: %"PRIu64"\n",  Format);
    fprintf(f_all_test, "Extras: %d\n",  Extras);
    fprintf(f_all_test, "Extras2: %d\n", Extras2);
    fprintf(f_all_test, "channel: %d\n", channel);
    fprintf(f_all_test, "evtnum: %d\n",  evtnum);
    fprintf(f_all_test, "Handle: %d\n",  board_handle);
    fprintf(f_all_test, "evtnum_singleread: %d\n", evtnum_singleread);
    fprintf(f_all_test, "Energy: %d\n",  Energy);
  }

  if (WDrun->SingleWrite) {
    fclose(f_all);
    f_all=NULL;
    fclose(WDrun->fout[channel]);
    WDrun->fout[channel]= NULL;
    fclose(WDrun->fout2[channel]);
    WDrun->fout2[channel]= NULL;
    if(channel==0){
      fclose(WDrun->fout3[0]);
      WDrun->fout3[0]= NULL;
    }
  }
  return 0;
}

int WriteOutputFilesx742(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, CAEN_DGTZ_EventInfo_t *EventInfo, CAEN_DGTZ_X742_EVENT_t *Event)
{
  char fname[100], trname[10], flag = 0;
  for (int gr = 0; gr*8 < WDcfg->Nch; gr++) {
    if (Event->GrPresent[gr]) {
      for(int ch=0; ch<9; ch++) {
	const int Size = Event->DataGroup[gr].ChSize[ch];
	if (Size <= 0) {
	  continue;
	}

	// Check the file format type
	if( WDcfg->OutFileFlags & OFF_BINARY) {
	  // Binary file format
	  if (!WDrun->fout[(gr*9+ch)]) {
	    switch (gr*9 + ch) {
	    case 8:
	      sprintf(fname, "TR_%d_0.dat", gr);
	      sprintf(trname,"TR_%d_0",gr);
	      flag = 1;
	      break;
	    case 17:
	    case 26:
	      sprintf(fname, "TR_0_%d.dat", gr);
	      sprintf(trname,"TR_0_%d",gr);
	      flag = 1;
	      break;
	    case 35:
	      sprintf(fname, "TR_1_%d.dat", gr);
	      sprintf(trname,"TR_1_%d",gr);
	      flag = 1;
	      break;
	    default:
	      sprintf(fname, "wave_%d.dat", (gr*8)+ch);
	      flag = 0;
	      break;
	    }
	    if ((WDrun->fout[(gr*9+ch)] = fopen(fname, "wb")) == NULL) {
	      return -1;
	    }
	  }

	  enum {BIN_HEADER_SIZE = 6,};
	  uint32_t BinHeader[BIN_HEADER_SIZE] = {
	    BIN_HEADER_SIZE * sizeof(uint32_t) + Size * ((WDcfg->Nbit == 8) ? 1 : 4),
	    EventInfo->BoardId,
	    EventInfo->Pattern,
	    ch,
	    EventInfo->EventCounter,
	    EventInfo->TriggerTimeTag,
	  };
	  
	  if( WDcfg->OutFileFlags & OFF_HEADER) {
	    // Write the Channel Header
	    if(fwrite(BinHeader, sizeof(*BinHeader), BIN_HEADER_SIZE, WDrun->fout[(gr*9+ch)])
	       != BIN_HEADER_SIZE) {
	      // error writing to file
	      fclose(WDrun->fout[(gr*9+ch)]);
	      WDrun->fout[(gr*9+ch)]= NULL;
	      return -1;
	    }
	  }
	  
	  {
	    const int ns =
	      (int)fwrite( Event->DataGroup[gr].DataChannel[ch] , 1 , Size*4, WDrun->fout[(gr*9+ch)]) / 4;
	    if (ns != Size) {
	      // error writing to file
	      fclose(WDrun->fout[(gr*9+ch)]);
	      WDrun->fout[(gr*9+ch)]= NULL;
	      return -1;
	    }
	  }
	} else {
	  // Ascii file format
	  if (!WDrun->fout[(gr*9+ch)]) {
	    switch (gr*9 + ch) {
	    case 8:
	      sprintf(fname, "TR_%d_0.dat", gr);
	      sprintf(trname,"TR_%d_0",gr);
	      flag = 1;
	      break;
	    case 17:
	    case 26:
	      sprintf(fname, "TR_0_%d.dat", gr);
	      sprintf(trname,"TR_0_%d",gr);
	      flag = 1;
	      break;
	    case 35:
	      sprintf(fname, "TR_1_%d.dat", gr);
	      sprintf(trname,"TR_1_%d",gr);
	      flag = 1;
	      break;
	    default:
	      sprintf(fname, "wave_%d.dat", (gr*8)+ch);
	      flag = 0;
	      break;
	    }
	    if ((WDrun->fout[(gr*9+ch)] = fopen(fname, "w")) == NULL) {
	      return -1;
	    }
	  }

	  if( WDcfg->OutFileFlags & OFF_HEADER) {
	    // Write the Channel Header
	    fprintf(WDrun->fout[(gr*9+ch)], "Record Length: %d\n", Size);
	    fprintf(WDrun->fout[(gr*9+ch)], "BoardID: %2d\n", EventInfo->BoardId);
	    if (flag)
	      fprintf(WDrun->fout[(gr*9+ch)], "Channel: %s\n",  trname);
	    else
	      fprintf(WDrun->fout[(gr*9+ch)], "Channel: %d\n",  (gr*8)+ ch);
	    fprintf(WDrun->fout[(gr*9+ch)], "Event Number: %d\n", EventInfo->EventCounter);
	    fprintf(WDrun->fout[(gr*9+ch)], "Pattern: 0x%04X\n", EventInfo->Pattern & 0xFFFF);
	    fprintf(WDrun->fout[(gr*9+ch)], "Trigger Time Stamp: %u\n", Event->DataGroup[gr].TriggerTimeTag);
	    fprintf(WDrun->fout[(gr*9+ch)], "DC offset (DAC): 0x%04X\n", WDcfg->DCoffset[ch] & 0xFFFF);
	    fprintf(WDrun->fout[(gr*9+ch)], "Start Index Cell: %d\n", Event->DataGroup[gr].StartIndexCell);
	    flag = 0;
	  }
	  for(int j=0; j<Size; j++) {
	    fprintf(WDrun->fout[(gr*9+ch)], "%f\n", Event->DataGroup[gr].DataChannel[ch][j]);
	  }
	}
	if (WDrun->SingleWrite) {
	  fclose(WDrun->fout[(gr*9+ch)]);
	  WDrun->fout[(gr*9+ch)]= NULL;
	}
      }
    }
  }
  return 0;
}

ERROR_CODES LoadConfigFile(const char* filename, WaveDumpConfig_t* wdcfg)
{
  FILE *f_conf;
  printf("Opening Configuration File: %s\n", filename);
  f_conf = fopen(filename, "r");
  if (f_conf == NULL) {
    return ERR_CONF_FILE_NOT_FOUND;
  }
  ParseConfigFile(f_conf, wdcfg);
  fclose(f_conf);
  return ERR_NONE;
}

const char* GetScalerName(int ch)
{
  /* this array of scaler name can be loaded in future */
  static const char* const SCALER_NAME[32] = {
    "Pla #1",     "Pla #2 U",   "Pla #2 D",   "Pla #2 AND", // ch  0-- 3
    "Beam",       "Pla #3",     "Penetrated", "SM  1|| 2",  // ch  4-- 7
    "SM  3|| 4",  "SM  5|| 6",  "SM  7|| 8",  "SM  9||10",  // ch  8--11
    "SM 11||12",  "SM 13||14",  "SM 15||16",  "SM 17||18",  // ch 12--15
    "SM 19||20",  "SEAMINE 21", "SEAMINE OR", "BaF2 #1",    // ch 16--19
    "BaF2 #2",    "BaF2 #3",    "BaF2 #4",    "BaF2 #5",    // ch 20--23
    "BaF2 #6",    "BaF2 #7",    "Busy",       "Ge #1",      // ch 24--27
    "Ge #2",      "Trigger",    "SEC",        "1k Clock",   // ch 28--31
  };
  return SCALER_NAME[ch];
}

/* ########################################################################### */
/* MAIN                                                                        */
/* ########################################################################### */
int main(int argc, char *argv[])
{
  WaveDumpConfig_t   WDcfg1, WDcfg2;
  WaveDumpRun_t      WDrun;
  //WaveDumpRun_t      WDrun1, WDrun2;
  CAEN_DGTZ_ErrorCode ret;
  ERROR_CODES ErrCode= ERR_NONE;
  char *buffer = NULL;                                    // readout buffer
  char *buffer2 = NULL;                                    // readout buffer
  char *buffer3 = NULL;                                    // readout buffer
  //CAEN_DGTZ_DPP_PSD_Event_t       *PSDEvents[MaxNChannels_V1730];  // events buffer
  //CAEN_DGTZ_DPP_PSD_Waveforms_t   *PSDWaveform=NULL;         // waveforms buffer
  char *EventPtr = NULL;
  char *EventPtr2 = NULL;
  CAEN_DGTZ_DPP_PHA_Event_t       *PHAEvents[MaxNChannels_V1730];  // events buffer
  //CAEN_DGTZ_DPP_PHA_Event_t       *PHAEvents[MaxNChannels_V1724];  // events buffer
  CAEN_DGTZ_DPP_PHA_Waveforms_t   *PHAWaveform=NULL;     // waveforms buffer
  
  //CAEN_DGTZ_DPP_PSD_Params_t PSDParams[MAXNB_V1730];
  int isVMEDevice= 0;
  CAEN_DGTZ_DPP_PHA_Params_t PHAParams[MAXNB_V1730];
  //CAEN_DGTZ_DPP_PHA_Params_t PHAParams[MAXNB_V1724];
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
  //    int TrgCnt_V1724[MAXNB_V1724][MaxNChannels_V1724];
  //    int PurCnt_V1724[MAXNB_V1724][MaxNChannels_V1724];
  uint32_t EventCounter_V1724[MaxNChannels_V1730];
  
  int handle[MAXNB];
  int BHandle;           //bridge handle, added by momiyama on 161211
  
  int i, b, ch, ev;
  int Quit=0;
  int AcqRun = 0;
  uint32_t AllocatedSize, BufferSize;
  int Nb=0, Ne=0;
  int Nb1=0, Ne1=0;
  int Nb2=0, Ne2=0;
  int DoSaveWave[MAXNB][MaxNChannels];
  int MajorNumber;
  int BitMask = 0;
  uint64_t CurrentTime, PrevRateTime, ElapsedTime;
  uint32_t NumEvents_V1730[MaxNChannels_V1730];
  uint32_t NumEvents_V1724[MaxNChannels_V1730];
  //uint32_t NumEvents_V1724[MaxNChannels_V1724];
  uint32_t NumEvents;
  CAEN_DGTZ_BoardInfo_t    BoardInfo1, BoardInfo2, BoardInfo3;
  
  CAEN_DGTZ_EventInfo_t     EventInfo;
  CAEN_DGTZ_UINT16_EVENT_t *Event16=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
  CAEN_DGTZ_UINT8_EVENT_t  *Event8=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */
  CAEN_DGTZ_X742_EVENT_t   *Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
  CAEN_DGTZ_EventInfo_t     EventInfo2;
  CAEN_DGTZ_UINT16_EVENT_t *Event216=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */
  CAEN_DGTZ_UINT8_EVENT_t  *Event28=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */
  CAEN_DGTZ_X742_EVENT_t   *Event2742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
  
  uint32_t printmask_V1730;   //added by momiyama on 161212
  uint32_t printmask_V1724;   //added by momiyama on 161212
  
  FILE *waveformfile1;
  char filename1[20];
  FILE *waveformfile2;
  char filename2[20];
  FILE *f_ini;
  FILE *f_all, *f_all_test;
  char f_all_name[120], f_all_test_name[120];
  FILE *f_scaler;
  char f_scaler_name[120];
  FILE *f_probe1, *f_probe2;
  char probe1_name[120], probe2_name[120];
  uint32_t probe;
  uint16_t DigitalWaveLine16[16000];
  
  int evcnt1, evcnt2;
  int runnumber=-1;
  FILE *frunnumber;
  char frunnumbername[30]="runnumber.txt";

  time_t starttime, timer;
  
  /* *************************************************************************************** */
  /* Open and parse configuration file                                                       */
  /* *************************************************************************************** */
  memset(&WDrun, 0, sizeof(WDrun));
  memset(&WDcfg1, 0, sizeof(WDcfg1));
  memset(&WDcfg2, 0, sizeof(WDcfg2));

  { /* Load Config Files */
    const char* const ConfigFile1 = "/home/daq/work/seamine_daq/WaveDumpConfig1.txt";
    const char* const ConfigFile2 = "/home/daq/work/seamine_daq/WaveDumpConfig2.txt";
    ret  = LoadConfigFile(ConfigFile1,&WDcfg1);
    ret |= LoadConfigFile(ConfigFile2,&WDcfg2);

    if (ret == ERR_CONF_FILE_NOT_FOUND) {
      ErrCode = ret;
      goto QuitProgram;
    }
  }

  memset(DoSaveWave, 0, MAXNB*MaxNChannels*sizeof(int));
  for (i=0; i<MAXNBITS; i++) {
    /* Create a bit mask based on number of bits of the board */
    BitMask |= 1<<i; 
  }

  for(ch=0; ch<MaxNChannels_V1730; ch++){
    // Set all histograms pointers to NULL (we will allocate them later)
    EHisto_V1724[0][ch] = NULL;
  }

  /* *************************************************************************************** */
  /* Set Parameters                                                                          */
  /* *************************************************************************************** */
  memset(&Params, 0, MAXNB*sizeof(DigitizerParams_t));
  memset(&PHAParams, 0, MAXNB_V1730*sizeof(CAEN_DGTZ_DPP_PHA_Params_t));
  for(b=0; b<MAXNB; b++) {
    Params[b].LinkType = CAEN_DGTZ_PCIE_OpticalLink;  // Link Type
    Params[b].VMEBaseAddress = 0;  // For direct CONET connection, VMEBaseAddress must be 0
    Params[b].IOlev = CAEN_DGTZ_IOLevel_NIM;
       
    /****************************\
    *  Acquisition parameters    *
    \****************************/
    Params[b].AcqMode = CAEN_DGTZ_DPP_ACQ_MODE_List; // CAEN_DGTZ_DPP_ACQ_MODE_List, CAEN_DGTZ_DPP_ACQ_MODE_Oscilloscope or CAEN_DGTZ_DPP_ACQ_MODE_Mixed
    Params[b].RecordLength = 100;                    // Num of samples of the waveforms (only for Oscilloscope mode)
    //Params[b].RecordLength = 16000;                // for test of Ge virtual and digital probe
    Params[b].ChannelMask = (b==2) ? 0x5             // Channel enable mask (0x5 == 0b0101) => ch0 and ch2
                                   : 0xffff;
    Params[b].EventAggr = 0;                                  // number of events in one aggregate (0=automatic)
    Params[b].PulsePolarity = CAEN_DGTZ_PulsePolarityPositive; // Pulse Polarity (this parameter can be individual)
  }

  /****************************\
  *      DPP parameters        *
  \****************************/
  for(ch=0; ch<MaxNChannels_V1730; ch++) {
    PHAParams[2].thr[ch] = 100; // Trigger Threshold
    PHAParams[2].k[ch] = 3000;  // Trapezoid Rise Time (N*10ns)
    PHAParams[2].m[ch] = 1000;  // Trapezoid Flat Top  (N*10ns)
    PHAParams[2].M[ch] = 47000; // Decay Time Constant (N*10ns) HACK-FPEP the one expected from fitting algorithm?
    PHAParams[2].ftd[ch] = 800; // Flat top delay (peaking time) (N*10ns) ??
    PHAParams[2].a[ch] = 4;     // Trigger Filter smoothing factor
    PHAParams[2].b[ch] = 10;    // Input Signal Rise time (N*10ns)
    PHAParams[2].trgho[ch] = 1200;  // Trigger Hold Off
    PHAParams[2].nsbl[ch] = 4;  // 3 = bx10 = 64 samples
    PHAParams[2].nspk[ch] = 0;
    PHAParams[2].pkho[ch] = 2000;
    PHAParams[2].blho[ch] = 500;
    PHAParams[2].enf[ch] = 1.0; // Energy Normalization Factor
    PHAParams[2].decimation[ch] = 0;
    PHAParams[2].dgain[ch] = 0;
    PHAParams[2].otrej[ch] = 0;
    PHAParams[2].trgwin[ch] = 0;
    PHAParams[2].twwdt[ch] = 0;
  }

  /* *************************************************************************************** */
  /* Open the digitizer and read board information                                           */
  /* *************************************************************************************** */
  if( CAENVME_Init(cvV2718, 0, 0, &BHandle) != cvSuccess ) {
    fprintf(stderr,"\n\n Error opening the device\n");
    return 0;
  }

  //char* FirmWareRelease;
  //ret = CAENVME_BoardFWRelease(BHandle,&FirmWareRelease);
  //printf("firmware version is %s",FirmWareRelease);
  printf("Bridge handle is %d\n",BHandle);

  /* for SIS3800 */
  uint32_t dummy_val = 0x1;
  const uint32_t ADDR_SIS3800 = 0xaaaaa000;
  uint32_t scalerval, scalertime;
  uint32_t scaler_buff[32];
  uint32_t scaler_overflow[32];
  
  { /* software reset boards */
    const uint32_t ADDR_BOARD_0 = 0x43210000;
    const uint32_t ADDR_BOARD_1 = 0x54320000;
    const uint32_t ADDR_BOARD_2 = 0x32100000;
    ret = CAENVME_WriteCycle(BHandle, ADDR_BOARD_0 | CAEN_DGTZ_SW_RESET_ADD ,&dummy_val,cvA32_U_DATA,cvD32)
        | CAENVME_WriteCycle(BHandle, ADDR_BOARD_1 | CAEN_DGTZ_SW_RESET_ADD ,&dummy_val,cvA32_U_DATA,cvD32)
        | CAENVME_WriteCycle(BHandle, ADDR_BOARD_2 | CAEN_DGTZ_SW_RESET_ADD ,&dummy_val,cvA32_U_DATA,cvD32);
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
    if(b==0){
      ret = CAEN_DGTZ_OpenDigitizer(WDcfg1.LinkType, WDcfg1.LinkNum, WDcfg1.ConetNode, WDcfg1.BaseAddress, &handle[b]);
    }else if(b==1){
      ret = CAEN_DGTZ_OpenDigitizer(WDcfg2.LinkType, WDcfg2.LinkNum, WDcfg2.ConetNode, WDcfg2.BaseAddress, &handle[b]);
    }else if(b==2){
      ret = CAEN_DGTZ_OpenDigitizer(Params[b].LinkType, 0, b+1, Params[b].VMEBaseAddress, &handle[b]);
    }else{
      fprintf(stderr,"invalid board number in opening digitizers\n");
    }
    if (ret) {
      fprintf(stderr,"Can't open digitizer in loop %d\n", b);
      fprintf(stderr,"error code is %d\n", ret);
      goto QuitProgram;    
    }
        
    /* Once we have the handler to the digitizer, we use it to call the other functions */
    //sw reset
    ret |= CAEN_DGTZ_Reset(handle[b]);
    if (ret != 0) {
      fprintf(stderr,
	      "Error: Unable to reset digitizer.\n"
	      "Please reset digitizer manually then restart the program\n"
	      "error code is %d\n",
	      ret);
      goto QuitProgram;
    }

    if(b==0){
      ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo1);
      if (ret) {
	printf("Can't read board info in loop %d\n", b);
	goto QuitProgram;
      }
      printf("Connected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo1.ModelName, b);
      printf("ROC FPGA Release is %s\n", BoardInfo1.ROC_FirmwareRel);
      printf("AMC FPGA Release is %s\n", BoardInfo1.AMC_FirmwareRel);
    }else if(b==1){
      ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo2);
      if (ret) {
	printf("Can't read board info in loop %d\n", b);
	goto QuitProgram;
      }
      printf("Connected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo2.ModelName, b);
      printf("ROC FPGA Release is %s\n", BoardInfo2.ROC_FirmwareRel);
      printf("AMC FPGA Release is %s\n", BoardInfo2.AMC_FirmwareRel);
    }else if(b==2){
      ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo3);
      if (ret) {
	printf("Can't read board info in loop %d\n", b);
	goto QuitProgram;
      }
      printf("Connected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo3.ModelName, b);
      printf("ROC FPGA Release is %s\n", BoardInfo3.ROC_FirmwareRel);
      printf("AMC FPGA Release is %s\n", BoardInfo3.AMC_FirmwareRel);
    }

    // Get Number of Channels, Number of bits, Number of Groups of the board */
    if(b==0){
      ret = GetMoreBoardInfo(handle[b], BoardInfo1, &WDcfg1);
      if (ret) {
	ErrCode = ERR_INVALID_BOARD_TYPE;
	goto QuitProgram;
      }
      // Perform calibration (if needed).
      if (WDcfg1.StartupCalibration)
	calibrate(handle[b], &WDrun, BoardInfo1);
    }else if(b==1){
      ret = GetMoreBoardInfo(handle[b], BoardInfo2, &WDcfg2);
      if (ret) {
	ErrCode = ERR_INVALID_BOARD_TYPE;
	goto QuitProgram;
      }
      // Perform calibration (if needed).
      if (WDcfg2.StartupCalibration)
	calibrate(handle[b], &WDrun, BoardInfo2);
    }else if(b==2){
      // Perform calibration (if needed).
      calibrate(handle[b], &WDrun, BoardInfo3);
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
    if(b==0){
      ret = ProgramDigitizer(handle[b], WDcfg1, BoardInfo1, b);
      if (ret) {
	fprintf(stderr,"Failed to program the first digitizer\n");
	ErrCode = ERR_DGZ_PROGRAM;
	goto QuitProgram;
      }
    }else if(b==1){
      ret = ProgramDigitizer(handle[b], WDcfg2, BoardInfo2, b);
      if (ret) {
	fprintf(stderr,"Failed to program the second digitizer\n");
	ErrCode = ERR_DGZ_PROGRAM;
	goto QuitProgram;
      }
    }else if(b==2){
      ret = ProgramDigitizer_PHA(handle[b],Params[b],PHAParams[b]);
      if (ret) {
	fprintf(stderr,"Failed to program the third digitizer\n");
	ErrCode = ERR_DGZ_PROGRAM;
	goto QuitProgram;
      }
    }
  }

  // Allocate memory for the event data and readout buffer
  if(WDcfg1.Nbit == 8) {
    ret = CAEN_DGTZ_AllocateEvent(handle[0], (void**)&Event8);
  } else {
    if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
      ret = CAEN_DGTZ_AllocateEvent(handle[0], (void**)&Event16);
    } else {
      ret = CAEN_DGTZ_AllocateEvent(handle[0], (void**)&Event742);
    }
  }
  if (ret != CAEN_DGTZ_Success) {
    ErrCode = ERR_MALLOC;
    goto QuitProgram;
  }

  /* WARNING: This malloc must be done after the digitizer programming */
  ret  = CAEN_DGTZ_MallocReadoutBuffer(handle[0], &buffer,&AllocatedSize); 
  ret |= CAEN_DGTZ_MallocReadoutBuffer(handle[1], &buffer2, &AllocatedSize);
  ret |= CAEN_DGTZ_MallocReadoutBuffer(handle[2], &buffer3, &AllocatedSize);
  ret |= CAEN_DGTZ_MallocDPPEvents(handle[2], (void**)PHAEvents, &AllocatedSize); 
  ret |= CAEN_DGTZ_MallocDPPWaveforms(handle[2], (void**)&PHAWaveform, &AllocatedSize); 
  if (ret) {
    fprintf(stderr,"Can't allocate memory buffers\n");
    fprintf(stderr,"ret = %d\n",ret);
    ErrCode = ERR_MALLOC;
    goto QuitProgram;    
  }


  /* runnumber setting */
  frunnumber = fopen(frunnumbername, "r");
  if (frunnumber == NULL) {
    fprintf(stderr, "fail to open runnumber file\n");
    runnumber=0;
    //goto QuitProgram;
  }else if(fscanf(frunnumber,"%d",&runnumber) != 1){
    fprintf(stderr,"Error in reading runnumber\n");
  }
  fclose(frunnumber);
  printf("\nrunnumber: %d\n",runnumber);
  
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

  evcnt1 = 0; evcnt2 = 0;

  PrevRateTime = get_time();
  AcqRun = 0;
  PrintInterface();
  printf("Type a command: ");
  while(!Quit) {
    // Check keyboard
    if(kbhit()) {
      char c = getch();
      if (c == 'q') {
	Quit = 1;
      }
//      if (c == 't')
//	for(b=0; b<MAXNB; b++)
//	  CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a software trigger to each board */

//      if (c == 'h'){
//	for(b=0; b<MAXNB; b++){
//	  if((b==0)||(b==1)){
//	    //for(ch=0; ch<MaxNChannels_V1730; ch++){
//	    //    if(ECnt_V1730[b][ch]!=0){
//	    //        /* Save Histograms to file for each board and channel */
//	    //        SaveHistogram("HistoShort", b, ch, EHistoShort_V1730[b][ch]);  
//	    //        SaveHistogram("HistoLong", b, ch, EHistoLong_V1730[b][ch]);
//	    //    }
//	    //}
//	  }else if(b==2){
//	    for(ch=0; ch<MaxNChannels_V1730; ch++){
//	      if(ECnt_V1724[b][ch]!=0){
//		/* Save Histograms to file for each board and channel */
//		SaveHistogram("Histo", b, ch, EHisto_V1724[b-1][ch]);
//	      }
//	    }
//	  }else{
//	    printf("invalid board number in check keyboard h\n");
//	  }
//	}
//      }
      if (c == 'w'){
	if(AcqRun) {
	  printf("Change to data writing mode is not allowed during the run.\n");
	  continue;
	}
		
	WDrun.ContinuousWrite = 1;

	//for(b=0; b<MAXNB; b++){
	//    for(ch=0; ch<MaxNChannels; ch++){
	//DoSaveWave[b][ch] = 1; /* save waveforms to file for each channel for each board (at next trigger) */

	sprintf(f_all_name, "/data%02d/RCNP1702_%02d/run%04d.dat",
		runnumber%3,runnumber%3,runnumber);

	//	sprintf(f_all_name, "/data00/RCNP1702_00/run%04d.dat",runnumber);
	if ((f_all = fopen(f_all_name, "wb")) == NULL){
	  printf("error in opening wave form f_all\n");
	  return -1;
	}
	sprintf(f_all_test_name, "/data%02d/RCNP1702_%02d/timestamp%04d.txt",		runnumber%3,runnumber%3,runnumber);
	f_all_test = fopen(f_all_test_name, "w");
	if (f_all_test == NULL)
	  return -1;
	//sprintf(f_scaler_name, "/data00/RCNP1702_00/sca%04d.txt",runnumber);

	sprintf(f_scaler_name, "/data%02d/RCNP1702_%02d/sca%04d.txt",
		runnumber%3,runnumber%3,runnumber);
	f_scaler = fopen(f_scaler_name, "w");
	if (f_scaler == NULL)
	  return -1;

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

	printf("open file: %s, %s \n",f_all_name,f_scaler_name);
	
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
	
	for(int k=0; k<32; k++) {
	  scaler_buff[k]=0;
	  scaler_overflow[k]=0;
	}

	for (ch=0; ch<MaxNChannels_V1730; ch++) {
	  ExtendedTT_V1724[0][ch] = 0;
	}

	scalertime=0;

	for(b=0; b<MAXNB; b++) {
	  CAEN_DGTZ_SWStopAcquisition(handle[b]); 
	  CAEN_DGTZ_ClearData(handle[b]);
	}
	
	ret = CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_CLEAR_ALL_COUNTERS,
				 &dummy_val,cvA32_U_DATA,cvD32)
            | CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_RESET_REGISTER_GLOBAL,
				 &dummy_val,cvA32_U_DATA,cvD32)
            | CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_GLOBAL_COUNT_ENABLE,
				 &dummy_val,cvA32_U_DATA,cvD32);
	if (ret) {
	  fprintf(stderr,"Error in initialization of SIS3800.\n");
	  goto QuitProgram;
	}

	{
	  struct timespec const ts = {0, 50000000}; // 50 msec
	  nanosleep(&ts,NULL); // 50 msec sleep
	}
	
	for(b=0; b<MAXNB; b++) {
	  CAEN_DGTZ_SWStartAcquisition(handle[b]);
	}

	
	//consideration about synchronization
	//for TRG OUT -> TRG IN daisy chain of RUN signal, corresponding master and first slave
	//onw to all is the same condition
	CAEN_DGTZ_SendSWtrigger(handle[0]);
	CAEN_DGTZ_SendSWtrigger(handle[1]);
	CAEN_DGTZ_SendSWtrigger(handle[2]);
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
	if(WDrun.ContinuousWrite || WDrun.SingleWrite){
	  fprintf(f_scaler,"\nrun%d started at %s",runnumber-1, ctime(&starttime));
	}
      }
      if (c == 'S')  {
	if(!AcqRun) {
	  printf("Not running!\n");
	  continue;
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

	printf("scaler values at the end of run\n");
	for(i=0; i<32; i++){
	  uint64_t scalerval64 = 0;
	  const char* const scalername = GetScalerName(i);
	  ret = CAENVME_ReadCycle(BHandle, ADDR_SIS3800 | SIS3800_READ_COUNTER + i*4,
				  &scalerval,cvA32_U_DATA,cvD32);

	  if (scalerval < scaler_buff[i]) { // overflow
	    scaler_overflow[i]++;
	  }

	  scalerval64 = ((uint64_t) scaler_overflow[i] << 32) + scalerval;

	  printf(" ch%2d: %-10s : %10"PRIu64" counts\n",i,scalername,scalerval64);
	  if (WDrun.ContinuousWrite || WDrun.SingleWrite){
	    if (i==0) {
	      fprintf(f_scaler,"scaler values at the end of run\n");
	    }
	    fprintf(f_scaler,"ch%2d, %-10s, %10"PRIu64"\n",i,scalername,scalerval64);
	  }
	}
	
	ret = CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_GLOBAL_COUNT_DISABLE,
				 &dummy_val,cvA32_U_DATA,cvD32);

	////TRG OUT -> S IN chain
	//CAEN_DGTZ_WriteRegister(handle[0], 0x8100, 0x0);
	//printf("Acquisition Stopped");
	AcqRun = 0;
	//printf("start reading last events\n");
	/* Read data for the end of run */
	for(b=0; b<MAXNB; b++) {
	  if(b==0){
	    ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
	    if (ret) {
	      ShowAlert();
	      printf("Readout Error : %s\n",GetDigitizerErrorMessage(ret));
	      goto QuitProgram;
	    }
	    if (BufferSize == 0) continue;
	    NumEvents = 0;
	    if (BufferSize != 0) {
	      ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer, BufferSize, &NumEvents);
	      if (ret) { ErrCode = ERR_READOUT; printf("Error on getting event number\n"); goto QuitProgram; }
	    }
	    /* Analyze data */
	    for(i = 0; i < (int)NumEvents; i++) {
	      /* Get one event from the readout buffer */
	      ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer, BufferSize, i, &EventInfo, &EventPtr);
	      if (ret) { ErrCode = ERR_EVENT_BUILD; printf("error on getting event info\n"); goto QuitProgram; }
	      /* decode the event */
	      if (WDcfg1.Nbit == 8)
		ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event8);
	      else
		if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
		  ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event16);
		}
		else {
		  ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event742);
		}
	      if (ret) { ErrCode = ERR_EVENT_BUILD; printf("error on decoding\n"); goto QuitProgram; }
	      /* Write Event data to file */
	      if (WDrun.ContinuousWrite || WDrun.SingleWrite) {
		// Note: use a thread here to allow parallel readout and file writing
		if (BoardInfo1.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
		  ret = WriteOutputFilesx742(&WDcfg1, &WDrun, &EventInfo, Event742);
		} else if (WDcfg1.Nbit == 8) {
		  ret = WriteOutputFiles(&WDcfg1, &WDrun, &EventInfo, Event8, i, f_all, b, f_all_test);
		} else {
		  ret = WriteOutputFiles(&WDcfg1, &WDrun, &EventInfo, Event16, i, f_all, b, f_all_test);
		}
		if (ret) { ErrCode = ERR_OUTFILE_WRITE; printf("error on writing output file\n"); goto QuitProgram; }
		if (WDrun.SingleWrite) { WDrun.SingleWrite = 0; }
	      }else{} /*printf("continuous or single write disabled\n");*/
	    }
	  }else if(b==1){
	    /* Read data from V1730 */
	    ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer2, &BufferSize);
	    if (ret) {
	      ShowAlert();
	      printf("Readout Error ;; %s\n",GetDigitizerErrorMessage(ret));
	      goto QuitProgram;
	    }
	    if (BufferSize == 0) continue;
	    NumEvents = 0;
	    if (BufferSize != 0) {
	      ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer2, BufferSize, &NumEvents);
	      if (ret) { ErrCode = ERR_READOUT; printf("Error on getting event number\n"); goto QuitProgram; }
	    }
	    /* Analyze data */
	    for(i = 0; i < (int)NumEvents; i++) {
	      ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer2, BufferSize, i, &EventInfo2, &EventPtr2);
	      if (ret) { ErrCode = ERR_EVENT_BUILD; printf("error on getting event info\n"); goto QuitProgram; }
	      /* decode the event */
	      if (WDcfg2.Nbit == 8)
		ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr2, (void**)&Event28);
	      else
		if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
		  ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr2, (void**)&Event216);
		}
		else { ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr2, (void**)&Event2742); }
	      if (ret) { ErrCode = ERR_EVENT_BUILD; printf("error on decoding\n"); goto QuitProgram; }
	      /* Write Event data to file */
	      if (WDrun.ContinuousWrite || WDrun.SingleWrite) {
		// Note: use a thread here to allow parallel readout and file writing
		if (BoardInfo2.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
		  ret = WriteOutputFilesx742(&WDcfg2, &WDrun, &EventInfo2, Event2742);
		} else if (WDcfg2.Nbit == 8) {
		  ret = WriteOutputFiles(&WDcfg2, &WDrun, &EventInfo2, Event28, i, f_all, b, f_all_test);
		} else {
		  ret = WriteOutputFiles(&WDcfg2, &WDrun, &EventInfo2, Event216, i, f_all, b, f_all_test);
		}
		if (ret) { ErrCode = ERR_OUTFILE_WRITE; printf("error on writing output file\n"); goto QuitProgram; }
		if (WDrun.SingleWrite) { printf("Single Event saved to output files\n"); WDrun.SingleWrite = 0; }
	      }else{} /*printf("continuous or single write disabled\n");*/
	    }
	  }else if(b==2){
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
		  EHisto_V1724[0][ch][(PHAEvents[ch][ev].Energy)&BitMask]++;
		  ECnt_V1724[0][ch]++;
		} else {  /* PileUp */ PurCnt_V1724[0][ch]++; }
		if (WDrun.ContinuousWrite || WDrun.SingleWrite)
		  WriteOutputFilesPHA(&WDcfg2, &WDrun,
				      PHAEvents[ch][ev].TimeTag, PHAEvents[ch][ev].Format,
				      PHAEvents[ch][ev].Extras,
				      //PHAEvents[ch][ev].Extras2,
				      ExtendedTT_V1724[0][ch], // substitute for Extras2
				      ch, EventCounter_V1724[ch], b, ev,
				      PHAEvents[ch][ev].Energy,
				      f_all, f_all_test);
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
	    printf("invalid board number in analyzing data\n");
	  }
	} // loop on boards
	
	//prepare next start
	ret |= CAEN_DGTZ_WriteRegister(handle[0], 0x8100, 0xe);
	ret |= CAEN_DGTZ_WriteRegister(handle[1], 0x8100, 0xe);
	ret |= CAEN_DGTZ_WriteRegister(handle[2], 0x8100, 0xe);
	
	//printf("just before closing file\n");
	if (WDrun.ContinuousWrite) {
	  fclose(f_all);
	  f_all = NULL;
	  fclose(f_all_test);
	  f_all_test = NULL;
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
    if (WDcfg1.InterruptNumEvents > 0) {
      //if (WDcfg.InterruptNumEvents > 0) {
      //printf("loop for interrupt\n");
      int32_t boardId;
      int VMEHandle = -1;
      int InterruptMask = (1 << VME_INTERRUPT_LEVEL);
      
      BufferSize = 0;
      NumEvents = 0;
      // Interrupt handling
      if (isVMEDevice) {
	ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg1.LinkType, WDcfg1.LinkNum, WDcfg1.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
      } else {
	//printf("entering IRQwait\n");
	ret = CAEN_DGTZ_IRQWait(handle[0], INTERRUPT_TIMEOUT);
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
	    ret = CAEN_DGTZ_RearmInterrupt(handle[0]);
	}
      }
    }
        
    /* Read data from the boards */
    for(b=0; b<MAXNB; b++) {
      if(b==0){
	/* Read data from V1730 */
	ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
	if (ret) {
	  ShowAlert();
	  printf("Readout Error 0; %s\n",GetDigitizerErrorMessage(ret));
	  goto QuitProgram;    
	}
	if (BufferSize == 0) {
	  continue;
	}
	
	NumEvents = 0;
	ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer, BufferSize, &NumEvents);
	if (ret) {
	  printf("error in GetNumEvents with code %d\n",ret);
	  ErrCode = ERR_READOUT;
	  goto QuitProgram;
	}

	Nb1 += BufferSize;
	Ne1 += NumEvents;

	/* Analyze data */
	for(i = 0; i < (int)NumEvents; i++) {
	  /* Get one event from the readout buffer */
	  ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer, BufferSize, i, &EventInfo, &EventPtr);
	  if (ret) {
	    printf("error in getting one event with code %d\n",ret);
	    ErrCode = ERR_EVENT_BUILD;
	    goto QuitProgram;
	  }
	  //printf("board ID for handle %d is %d\n",b,EventInfo.BoardId);
	  /* decode the event */
	  if (WDcfg1.Nbit == 8) {
	    ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event8);
	  } else if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
	    ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr, (void**)&Event16);
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
	      ret = WriteOutputFilesx742(&WDcfg1, &WDrun, &EventInfo, Event742);
	    } else if (WDcfg1.Nbit == 8) {
	      ret = WriteOutputFiles(&WDcfg1, &WDrun, &EventInfo, Event8, i, f_all, b, f_all_test);
	    } else {
	      ret = WriteOutputFiles(&WDcfg1, &WDrun, &EventInfo, Event16, i, f_all, b, f_all_test);
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
      }else if(b==1){
	/* Read data from V1730 */
	ret = CAEN_DGTZ_ReadData(handle[b], CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer2, &BufferSize);
	if (ret) {
	  ShowAlert();
	  printf("Readout Error 1; %s\n",GetDigitizerErrorMessage(ret));
	  goto QuitProgram;
	}
	if (BufferSize == 0)
	  continue;
	NumEvents = 0;
	if (BufferSize != 0) {
	  ret = CAEN_DGTZ_GetNumEvents(handle[b], buffer2, BufferSize, &NumEvents);
	  if (ret) {
	    printf("error in GetNumEvents with code %d\n",ret);
	    ErrCode = ERR_READOUT;
	    goto QuitProgram;
	  }
	}
	Nb2 += BufferSize;
	Ne2 += NumEvents;
	
	/* Analyze data */
	for(i = 0; i < (int)NumEvents; i++) {
	  /* Get one event from the readout buffer */
	  ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer2, BufferSize, i, &EventInfo2, &EventPtr2);
	  //ret = CAEN_DGTZ_GetEventInfo(handle[b], buffer2, BufferSize, i, &EventInfo, &EventPtr);
	  if (ret) {
	    printf("error in getting one event with code %d\n",ret);
	    ErrCode = ERR_EVENT_BUILD;
	    goto QuitProgram;
	  }
	  //printf("board ID for handle %d is %d\n",b,EventInfo2.BoardId);
	  /* decode the event */
	  if (WDcfg2.Nbit == 8)
	    ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr2, (void**)&Event28);
	  else
	    if (BoardInfo1.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
	      ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr2, (void**)&Event216);
	    } else {
	      ret = CAEN_DGTZ_DecodeEvent(handle[b], EventPtr2, (void**)&Event2742);
	    }
	  if (ret) {
	    ErrCode = ERR_EVENT_BUILD;
	    goto QuitProgram;
	  }
	  /* Write Event data to file */
	  if (WDrun.ContinuousWrite || WDrun.SingleWrite) {
	    //printf("writing procedure\n");
	    // Note: use a thread here to allow parallel readout and file writing
	    if (BoardInfo2.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
	      ret = WriteOutputFilesx742(&WDcfg2, &WDrun, &EventInfo2, Event2742);
	    }
	    else if (WDcfg2.Nbit == 8) {
	      ret = WriteOutputFiles(&WDcfg2, &WDrun, &EventInfo2, Event28, i, f_all, b, f_all_test);
	    }
	    else {
	      ret = WriteOutputFiles(&WDcfg2, &WDrun, &EventInfo2, Event216, i, f_all, b, f_all_test);
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
	}
      }else if(b==2){
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
	      EHisto_V1724[0][ch][(PHAEvents[ch][ev].Energy)&BitMask]++;
	      ECnt_V1724[0][ch]++;
	    } else {  /* PileUp */
	      PurCnt_V1724[0][ch]++;
	    }
	    if (WDrun.ContinuousWrite || WDrun.SingleWrite)
	      WriteOutputFilesPHA(&WDcfg2, &WDrun,
				  PHAEvents[ch][ev].TimeTag, PHAEvents[ch][ev].Format,
				  PHAEvents[ch][ev].Extras,
				  //PHAEvents[ch][ev].Extras2,
				  ExtendedTT_V1724[0][ch], // substitute for Extras2
				  ch, EventCounter_V1724[ch], b, ev,
				  PHAEvents[ch][ev].Energy,
				  f_all, f_all_test);
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
	printf("invalid board number in analyzing data\n");
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
      }else{
	printf("Run%4d [no save mode]\n",runnumber);
      }

      printf("Started at %s",ctime(&starttime));
      printf("Time elapsed = %1d:%02d:%02d\n",
      	     elapsedSec/3600,(elapsedSec/60)%60,elapsedSec%60);

      for(b=0; b<MAXNB; b++) {
	printf("Board %d: ",b);
	if(b==0){
	  if (Nb1 == 0) {
	    if (ret == CAEN_DGTZ_Timeout) {
	      printf("Timeout...\n");
	    } else {
	      printf("No data...\n");
	    }
	  } else {
	    printf("Reading at %.2f MiB/s (Trg Rate: %.2f Hz)\n",
		   Nb1 / 1048.576 / ElapsedTime, Ne1 * 1000. / ElapsedTime);
	  }
	  Nb1 = 0;
	  Ne1 = 0;
	  PrevRateTime = CurrentTime;
	} else if (b==1) {
	  if (Nb2 == 0) {
	    if (ret == CAEN_DGTZ_Timeout) {
	      printf ("Timeout...\n");
	    } else {
	      printf("No data...\n");
	    }
	  } else {
	    printf("Reading at %.2f MiB/s (Trg Rate: %.2f Hz)\n",
		   Nb2 / 1048.576 / ElapsedTime, Ne2 * 1000. / ElapsedTime);
	  }
	  Nb2 = 0;
	  Ne2 = 0;
	  PrevRateTime = CurrentTime;
	} else if(b==2) {

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
	  
	  printf("scaler rate monitor\n");
	  ret = CAENVME_ReadCycle(BHandle, ADDR_SIS3800 | SIS3800_READ_COUNTER + 31*4,
				  &scalerval,cvA32_U_DATA,cvD32);  	    
	  scalertime = scalerval-scaler_buff[31]; // read clock value first to calculate scalertime
	  
	  for(i=0; i<32; i++){
	    ret = CAENVME_ReadCycle(BHandle, ADDR_SIS3800 | SIS3800_READ_COUNTER + i*4,
				    &scalerval,cvA32_U_DATA,cvD32);
	    {
	      uint64_t scalerval64 = 0;
	      const double scalerrate = (i==31) ? 1000.
		                                : (scalerval-scaler_buff[i])*1000./scalertime;
	      const char* const scalername = GetScalerName(i);
	      const char* const color_seq =
		  scalerrate < 1e+1 ? "\x1b[49m"  // default
		: scalerrate < 1e+2 ? "\x1b[47m"  // gray
		: scalerrate < 1e+3 ? "\x1b[44m"  // blue
		: scalerrate < 1e+4 ? "\x1b[42m"  // green
		: scalerrate < 1e+5 ? "\x1b[43m"  // yellow
		: scalerrate < 1e+6 ? "\x1b[45m"  // magenta
		:                     "\x1b[41m"; // red

	      if (scalerval < scaler_buff[i]) { // overflow
		scaler_overflow[i]++;
	      }

	      scalerval64 = ((uint64_t) scaler_overflow[i] << 32) + scalerval;
	      
	      printf(" %s \x1b[49mch%2d %-10s : %9.1f Hz : %10"PRIu64" counts\n",
		     color_seq,i,scalername,scalerrate,scalerval64);
	      if (WDrun.ContinuousWrite || WDrun.SingleWrite){
		fprintf(f_scaler,"ch%2d, %-10s, %10"PRIu64"\n",
			i,scalername,scalerval64);
	      }
	    }
	    scaler_buff[i] = scalerval;
	  }
	}else{
	  fprintf(stderr,"invalid board number in counting\n");
	}
      }
      Nb = 0;
      PrevRateTime = CurrentTime;
    }
  } // End of readout loop

QuitProgram:
  //printf("in the quit sequence\n");
  /* stop the acquisition, close the device and free the buffers */
  for(b=0; b<MAXNB; b++) {
    //printf("board %d, software stop\n",b);
    CAEN_DGTZ_SWStopAcquisition(handle[b]);
  }

  for (ch=0; ch<MaxNChannels_V1730; ch++) {
    //printf("free EHisto\n");
    free(EHisto_V1724[0][ch]);
  }

  //printf("free wavedump events\n");
  if(Event8) {
    CAEN_DGTZ_FreeEvent(handle[0], (void**)&Event8);
  }
  if(Event16) {
    CAEN_DGTZ_FreeEvent(handle[0], (void**)&Event16);
  }

  //printf("free DPP waveform\n");
  CAEN_DGTZ_FreeDPPWaveforms(handle[2], PHAWaveform);
  //printf("free readout buffer\n");
  CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  CAEN_DGTZ_FreeReadoutBuffer(&buffer2);
  CAEN_DGTZ_FreeReadoutBuffer(&buffer3);

  for(b=0; b<MAXNB; b++){
    //printf("close digitizer\n");
    CAEN_DGTZ_CloseDigitizer(handle[b]);
  }
  return ret;
}


