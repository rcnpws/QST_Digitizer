#ifndef _DIGITIZER_H_
#define _DIGITIZER_H_

#include <CAENDigitizer.h>
#include <CAENComm.h>
#include <CAENVMElib.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "seamine_daq.h"
#include "Functions.h"
#include "WDconfig.h"


// it is needed for consistency inside the CAENDigitizer's functions used to allocate the memory
#define MaxNChannels 16
#define MaxNChannels_V1730 16
#define MaxNChannels_V1724 8

/* several  written by CAEN peaple*/

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


/* some functions to communicate with digitizers */
const char* GetDigitizerErrorMessage(int err);

int ProgramDigitizer_PSD(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PSD_Params_t DPPParams);
//int ProgramDigitizer_PHA(int handle, DigitizerParams_t Params, CAEN_DGTZ_DPP_PHA_Params_t DPPParams);
int ProgramDigitizer_PHA(int handle, DigitizerParams_t Params);
int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo, int board, int isV1730D);

int PrintRegister(int handle,int address);
int PrintAllRegister(int handle);
int PrintAllRegister_PHA(int handle);

int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask) ;
int GetMoreBoardInfo(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo, WaveDumpConfig_t *WDcfg);

int32_t BoardSupportsCalibration(CAEN_DGTZ_BoardInfo_t BoardInfo) ;
void calibrate(int handle, WaveDumpRun_t *WDrun, CAEN_DGTZ_BoardInfo_t BoardInfo) ;
int Monitor_Temperature(int handle, int maxch);

int InitializeDigitizer();
int InitializeDigitizer_PHA();


#endif //_DIGITIZER_H_
