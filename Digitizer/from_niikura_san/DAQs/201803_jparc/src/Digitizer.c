// 2017.7.31 T. Saito
// DAQ with CAEN digitizers for RAL muon experiment
// function written by CAEN is corrected in this file

#include <CAENDigitizer.h>
#include <CAENComm.h>
#include <CAENVMElib.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "Digitizer.h"

static CAEN_DGTZ_IRQMode_t INTERRUPT_MODE = CAEN_DGTZ_IRQ_MODE_ROAK;

/* ###########################################################################
*  Functions
*  ########################################################################### */

/* --------------------------------------------------------------------------------------------------------- */
/*! \fn      int ProgramDigitizer(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPPParamsPHA_t DPPParams)
*   \brief   Program the registers of the digitizer with the relevant parameters
*   \return  0=success; -1=error */
/* --------------------------------------------------------------------------------------------------------- */

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

/*int ProgramDigitizer_PSD(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams)
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
*/

//int ProgramDigitizer_PHA(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams)
int ProgramDigitizer_PHA(int handle, DigitizerParams_t Params)
{
    int i, ret = 0;
    uint32_t printregister;
    ret |= CAEN_DGTZ_Reset(handle);
    printf("program digitizer PHA\n");

    if (ret) {
        printf("ERROR: can't reset the digitizer.\n");
        return -1;
    }

    
    ret |= CAEN_DGTZ_SetDPPAcquisitionMode(handle, Params.AcqMode, CAEN_DGTZ_DPP_SAVE_PARAM_EnergyAndTime);

    if(ret)printf("error code %d\n",ret);

    //for PHA parameter check
    ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_1, CAEN_DGTZ_DPP_VIRTUALPROBE_Trapezoid);
    ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_2, CAEN_DGTZ_DPP_VIRTUALPROBE_Input);
    
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

    ///ret |= CAEN_DGTZ_SetDPP_VirtualProbe(handle, ANALOG_TRACE_1, CAEN_DGTZ_DPP_VIRTUALPROBE_Trapezoid);
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

    //*default way to parameter set. Now we set each parameters directry below
    //ret |= CAEN_DGTZ_SetDPPParameters(handle, Params.ChannelMask, &DPPParams);

    for(i=0; i<MaxNChannels_V1730; i++) {
        if (Params.ChannelMask & (1<<i)) {
	  //ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, 5000);
	    ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, 65000);
            ret |= CAEN_DGTZ_SetDPPPreTriggerSize(handle, i, 20);
            ret |= CAEN_DGTZ_SetChannelPulsePolarity(handle, i, Params.PulsePolarity);
        }
    }
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0x010df910);  // Channel Control Reg (indiv trg, seq readout) /// this is old setting 20180227
    //ret |= OverwriteRegister(handle, 0x8000, 0x010df910,0xecf3f807);  // Channel Control Reg (indiv trg, seq readout) 
    //ret |= OverwriteRegister(handle, 0x8000, 0x00012802,0xecf3f807);  // Channel Control Reg (indiv trg, seq readout) //debug!!!!!
    //ret |= OverwriteRegister(handle, 0x8020, 0xff,0x00003fff);  // record length
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0xc0110);//default for experiment 20180227
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x8000, 0xf2912); // for PHA debug

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
    
    // for LVDS out apparrently stupid. see later 2017.9.14 saito 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x0001);  //LVDS TRIGGER
    ret |= CAEN_DGTZ_WriteRegister(handle,0x8084,0x5);  // set logic width 40 ns for all ch
    ret |= CAEN_DGTZ_WriteRegister(handle,0x10a0,0x55); //alg. control 2 for trigger generaton 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x12a0,0x55);

    
    ret |= CAEN_DGTZ_WriteRegister(handle,0xef00,0x8);  //readout ctrl
    ret |= CAEN_DGTZ_WriteRegister(handle,0xef18,0x32); // interrupt evtnum
    
    //PHA parameters
    
    //ch 0
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x20612f0f);  //for negative  
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1028,0x0);  //dynamic range,1=0.5Vpp
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x20102f0a);  //for positive 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x10c4,0x4ff); //fine gain
   
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1038,0x3e8); //pre trigger 
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x105c,0xaa); // trap. rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1060,0x3d); // trap. flat top
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1064,0x15); // peaking time 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1068,0x2e2); // decay time
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1068,0x0380); // decay time 
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x106c,0x20); //trig. threshold 0x40
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1058,0x64);  //input rise time 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1054,0x64);  //RC-CR2 smoothing

    ret |= CAEN_DGTZ_WriteRegister(handle,0x1074,0x12);  //trig. hold off
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1078,0x1f);  //peak hold off
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x107c,0xa);  //baseline hold off

    //ch 2
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1228,0x0);  //dynamic range,0=2Vpp
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1280,0x20612f0f);  //for negative
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1280,0x20102f0a);  //for positive
    ret |= CAEN_DGTZ_WriteRegister(handle,0x12c4,0x4ff); //fine gain
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1238,0x3e8); //pre trigger
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x125c,0xaa); // trap. rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1260,0x3d); // trap. flat top
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1264,0x15); // peaking time 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1268,0x2e2); // decay time 

    ret |= CAEN_DGTZ_WriteRegister(handle,0x126c,0x20); //trig. threshold 0x40
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1258,0x64);  //input rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1254,0x64);  //RC-CR2 smoothing
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1274,0x12);  //trig. hold off
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1278,0x1f);  //peak hold off
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x127c,0xa);  //baseline hold off
     
    /*
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1228,0x0);  //dynamic range,0=2Vpp
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1280,0x20101514);  //for positive
    ret |= CAEN_DGTZ_WriteRegister(handle,0x12c4,0x7fff); //fine gain
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1238,0x3e8); //pre trigger
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x125c,500); // trap. rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1260,500); // trap. flat top
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1264,500); // peaking time 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1268,12500); // decay time 

    ret |= CAEN_DGTZ_WriteRegister(handle,0x1258,100); //input rise time
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x126c,0x180); //trig. threshold 0x180
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1254,0x10); //RC-CR2 smoothing
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1274,0x12);  //trig. hold off
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1278,0x1f);  //peak hold off
    ret |= CAEN_DGTZ_WriteRegister(handle,0x127c,0xa);  //baseline hold off
    */ 
    
    //ch 3 = 1
    /*
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1428,0x0);  //dynamic range,1=0.5Vpp
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1480,0x20112f0f);  //for negative 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x14c4,0x7fff); //fine gain
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1438,0x3e8); //pre trigger 
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x145c,0xfa); // trap. rise time
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1460,0x7d); // trap. flat top
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1464,0x25); // peaking time 
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1468,0x0380); // decay time 
    
    ret |= CAEN_DGTZ_WriteRegister(handle,0x146c,0x40); //trig. threshold
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1458,0x64);  //input rise time 0x37
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1454,0x10);  //RC-CR2 smoothing

    ret |= CAEN_DGTZ_WriteRegister(handle,0x1474,0x12);  //trig. hold off
    ret |= CAEN_DGTZ_WriteRegister(handle,0x1478,0x1f);  //peak hold off
    ret |= CAEN_DGTZ_WriteRegister(handle,0x147c,0xa);  //baseline hold off

    ret |= CAEN_DGTZ_WriteRegister(handle,0x14a0,0x55);
    */
    
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1284,0xb); 
    //ret |= CAEN_DGTZ_WriteRegister(handle, 0x800c, 0x7);
    
    ret |= PrintAllRegister_PHA(handle);
    
    if (ret) {
        printf("Warning: errors found during the programming of the PHA digitizer.\nSome settings may not be executed\n");
        return ret;
    } else {
        return 0;
    }
}

int PrintAllRegister(int handle){
  int ret = 0;
  int printregister;
  printf("--debug-- \n printing all the registers of WF board with handle %d \n",handle);

  ret|=PrintRegister(handle,0x1080);
  ret|=PrintRegister(handle,0x1180);
  ret|=PrintRegister(handle,0x1280);
  ret|=PrintRegister(handle,0x1380);
  ret|=PrintRegister(handle,0x1480);

  ret|=PrintRegister(handle,0x8080);
  
  ret|=PrintRegister(handle,0x8000);
  ret|=PrintRegister(handle,0x800c);
  ret|=PrintRegister(handle,0x8020);
  ret|=PrintRegister(handle,0x8028);
    
  ret|=PrintRegister(handle,0x8100);
  ret|=PrintRegister(handle,0x8104);

  ret|=PrintRegister(handle,0x810c);
  ret|=PrintRegister(handle,0x8110);
  ret|=PrintRegister(handle,0x8114);
  ret|=PrintRegister(handle,0x8118);
  ret|=PrintRegister(handle,0x811c);

  ret|=PrintRegister(handle,0x8120);
  ret|=PrintRegister(handle,0x8124);
  ret|=PrintRegister(handle,0x812c);

  ret|=PrintRegister(handle,0x8138);
  ret|=PrintRegister(handle,0x8140);
  ret|=PrintRegister(handle,0x8144);
  ret|=PrintRegister(handle,0x814c);
  ret|=PrintRegister(handle,0x8168);
  ret|=PrintRegister(handle,0x816c);
  ret|=PrintRegister(handle,0x8170);
  ret|=PrintRegister(handle,0x8178);
  
  ret|=PrintRegister(handle,0x81a0);

  ret|=PrintRegister(handle,0xef00);
  ret|=PrintRegister(handle,0xef04);
  ret|=PrintRegister(handle,0xef08);
  ret|=PrintRegister(handle,0xef0c);
  ret|=PrintRegister(handle,0xef10);
  ret|=PrintRegister(handle,0xef14);
  ret|=PrintRegister(handle,0xef18);
  ret|=PrintRegister(handle,0xef1c);
  
  printf("--debug--\n");
  return ret;
}

int PrintAllRegister_PHA(int handle){
  int ret = 0;
  int printregister;
  printf("--debug-- \n printing all the registers of PHA board with handle %d \n",handle);

  ret|=PrintRegister(handle,0x1020);  ret|=PrintRegister(handle,0x1220);
  ret|=PrintRegister(handle,0x1028);  ret|=PrintRegister(handle,0x1228);
  ret|=PrintRegister(handle,0x1034);  ret|=PrintRegister(handle,0x1234);
  ret|=PrintRegister(handle,0x1038);  ret|=PrintRegister(handle,0x1238);

  ret|=PrintRegister(handle,0x1040);  ret|=PrintRegister(handle,0x1240);
  ret|=PrintRegister(handle,0x1054);  ret|=PrintRegister(handle,0x1254);
  ret|=PrintRegister(handle,0x1058);  ret|=PrintRegister(handle,0x1258);
  ret|=PrintRegister(handle,0x105c);  ret|=PrintRegister(handle,0x125c);

  ret|=PrintRegister(handle,0x1060);  ret|=PrintRegister(handle,0x1260);
  ret|=PrintRegister(handle,0x1064);  ret|=PrintRegister(handle,0x1264);
  ret|=PrintRegister(handle,0x1068);  ret|=PrintRegister(handle,0x1268);
  ret|=PrintRegister(handle,0x106c);  ret|=PrintRegister(handle,0x126c);
  ret|=PrintRegister(handle,0x1070);  ret|=PrintRegister(handle,0x1270);
  ret|=PrintRegister(handle,0x1074);  ret|=PrintRegister(handle,0x1274);
  ret|=PrintRegister(handle,0x1078);  ret|=PrintRegister(handle,0x1278);
  ret|=PrintRegister(handle,0x107c);  ret|=PrintRegister(handle,0x127c);

  ret|=PrintRegister(handle,0x1080);  ret|=PrintRegister(handle,0x1280);
  ret|=PrintRegister(handle,0x1084);  ret|=PrintRegister(handle,0x1284);
  ret|=PrintRegister(handle,0x1088);  ret|=PrintRegister(handle,0x1288);
  ret|=PrintRegister(handle,0x108c);  ret|=PrintRegister(handle,0x128c);

  ret|=PrintRegister(handle,0x1098);  ret|=PrintRegister(handle,0x1298);
  ret|=PrintRegister(handle,0x10a0);  ret|=PrintRegister(handle,0x12a0);
  ret|=PrintRegister(handle,0x10a8);  ret|=PrintRegister(handle,0x12a8);
  
  ret|=PrintRegister(handle,0x10c4);  ret|=PrintRegister(handle,0x12c4);
  ret|=PrintRegister(handle,0x10d4);  ret|=PrintRegister(handle,0x12d4);

  ret|=PrintRegister(handle,0x8000);
  ret|=PrintRegister(handle,0x800c);
    
  ret|=PrintRegister(handle,0x8100);
  ret|=PrintRegister(handle,0x8104);

  ret|=PrintRegister(handle,0x810c);
  ret|=PrintRegister(handle,0x8110);
  ret|=PrintRegister(handle,0x8118);
  ret|=PrintRegister(handle,0x811c);

  ret|=PrintRegister(handle,0x8120);
  ret|=PrintRegister(handle,0x8124);

  ret|=PrintRegister(handle,0x8138);
  ret|=PrintRegister(handle,0x8140);
  ret|=PrintRegister(handle,0x8144);
  ret|=PrintRegister(handle,0x814c);
  ret|=PrintRegister(handle,0x8158);
  ret|=PrintRegister(handle,0x8168);
  ret|=PrintRegister(handle,0x8170);
  ret|=PrintRegister(handle,0x8178);
  ret|=PrintRegister(handle,0x817c);
  ret|=PrintRegister(handle,0x8180);  ret|=PrintRegister(handle,0x8184);
  ret|=PrintRegister(handle,0x81a0);

  ret|=PrintRegister(handle,0xef00);
  ret|=PrintRegister(handle,0xef04);
  ret|=PrintRegister(handle,0xef08);
  ret|=PrintRegister(handle,0xef0c);
  ret|=PrintRegister(handle,0xef10);
  ret|=PrintRegister(handle,0xef14);
  ret|=PrintRegister(handle,0xef18);
  ret|=PrintRegister(handle,0xef1c);
  
  printf("--debug--\n");
  return ret;
}

int PrintRegister(int handle, int address){
  int ret=0;
  int printregister;
  
  ret |= CAEN_DGTZ_ReadRegister(handle,address,&printregister);
  printf("register 0x%x setting is 0x%"PRIx32" = %d \n",address,printregister,printregister);
  
  return ret;
}
int OverwriteRegister(int handle, int address, int val, int mask){
  int ret=0;
  int oldregister=0;
  int newregister=0;
  ret |= CAEN_DGTZ_ReadRegister(handle,address,&oldregister);
  newregister= (val & mask) | (oldregister & ~mask);
  ret |= CAEN_DGTZ_WriteRegister(handle,address,newregister);
  return ret;
}

int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo, int board, int isV1730D){
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
		//printf("channel pulse polarity of channel %d is %d\n",i,tval)
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
    if(ret) printf("error here D %d\n",i);

    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
      for(i=0; i<(WDcfg.Nch/8); i++) {
	ret |= CAEN_DGTZ_SetDRS4SamplingFrequency(handle, WDcfg.DRS4Frequency);
	ret |= CAEN_DGTZ_SetGroupFastTriggerDCOffset(handle,i,WDcfg.FTDCoffset[i]);
	ret |= CAEN_DGTZ_SetGroupFastTriggerThreshold(handle,i,WDcfg.FTThreshold[i]);
      }
    }

    
    /* execute generic write commands */
    for(i=0; i<WDcfg.GWn; i++){
      ret |= WriteRegisterBitmask(handle, WDcfg.GWaddr[i], WDcfg.GWdata[i], WDcfg.GWmask[i]);
    }
    
    // Set self trigger configuration
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY, 1);
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_EXTOUT_ONLY, 1);
    //ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT, Params.ChannelMask);
    
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8000,0x50);  //channel polarity is negative
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8000,0x10);  //channel polarity is positive
    
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13f);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13e);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x33c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13d);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x3c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x7c);  //LVDS setting

    //    ret |= CAEN_DGTZ_WriteRegister(handle,0x8110,0xe0000000);  //TRG-OUT
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8110,0xc0000000);  //TRG-OUT
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x0000);  //LVDS REGISTER

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x1122);  //LVDS TRIGGER
    ret |= CAEN_DGTZ_WriteRegister(handle,0x811c,0x13c);  //LVDS setting
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x81a0,0x2311);  //LVDS TRIGGER <- set in wdcfg
    ret |= CAEN_DGTZ_WriteRegister(handle,0x8070,125);  //LVDS TRIGGER width
    
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8000,0x52);  //set overlaped triggered data tiking available
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x8100,0x0);  // aqcuisition status

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1084,0x3);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x0);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x32);
    //ret |= CAEN_DGTZ_WriteRegister(handle,0x1080,0x3e8);

    //ret |= CAEN_DGTZ_WriteRegister(handle,0x810c,0xc0000001);  //global trigger is external or first couple self trigger

    /* copy of writing register process in PSD */
    /* should incluede generic write commands above */
   
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
    //ret |=PrintRegister(handle, 0x1028);    
    
    ret |=PrintAllRegister(handle);
    if (ret)
        printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");

    return 0;
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
  printf("calibration ... \n");
  int success=0;
  while(success==0){
    if (BoardSupportsCalibration(BoardInfo)) {
      if (WDrun->AcqRun == 0) {
	int32_t ret = CAEN_DGTZ_Calibrate(handle);
	if (ret == CAEN_DGTZ_Success) {
	  printf("ADC Calibration successfully executed.\n");
	  success=1;
	}
	else {
	  printf("ADC Calibration failed. CAENDigitizer ERR %d\n", ret);

	  //reset 
	  ret |= CAEN_DGTZ_Reset(handle); 
	  if(ret != 0) {
	    printf("Error ro reset board. CAENDigitizer ERR %d\n", ret);
	  }
	}
	//printf("\n");
      }
      else {
	printf("Can't run ADC calibration while acquisition is running.\n");
	success=-1;
      }
    }
    else {
      printf("ADC Calibration not needed for this board family.\n");
      success=1;
    }
  }
}
int Monitor_Temperature(int handle, int maxch){
  int ret=0;
  int ch;
  int printregister;
  printf("tempureture of handle %d \n",handle);
  for(ch=0;ch<maxch;ch++){
    ret |= CAEN_DGTZ_ReadRegister(handle, 0x10a8+0x0100*ch, &printregister);
    printf("CH %d = %d \n",ch,printregister);
  }
  return ret;
}
int SetPHAanalysis(int handle){
  int ret=0;
  //ret |= OverwriteRegister(handle, 0x8020, 0xfff,0x00003fff);  // record length
  //ret |= OverwriteRegister(handle, 0x8000, 0x00013802,0xecf3f807);  // Channel Control Reg (indiv trg, seq readout)
  //ret |= OverwriteRegister(handle, 0x8000, 0x00013802,0xecf2f807);  // Channel Control Reg (indiv trg, seq readout)
  
  sleep(1);
  return 0;
}
int StopPHAanalysis(int handle){
  int ret=0;
  ret |= OverwriteRegister(handle, 0x8000, 0x010df910,0xecf3f807);  // Channel Control Reg (indiv trg, seq readout)   
  return 0;
}
