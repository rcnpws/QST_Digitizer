#ifndef _IOCONFIG_H_
#define _IOCONFIG_H_

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

int WriteOutputFiles(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, CAEN_DGTZ_EventInfo_t *EventInfo, void *Event, int evtnum_singleread, FILE *f_all, int board_handle);

int WriteOutputFilesPHA(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, uint64_t TimeTag, uint64_t Format, int16_t Extras, uint32_t Extras2, uint32_t channel, uint32_t evtnum, uint32_t board_handle, uint32_t evtnum_singleread, uint16_t Energy, FILE *f_all);

int WriteOutputFilesx742(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, CAEN_DGTZ_EventInfo_t *EventInfo, CAEN_DGTZ_X742_EVENT_t *Event);

ERROR_CODES LoadConfigFile(const char* filename, WaveDumpConfig_t* wdcfg);
#endif // _IOCONFIG_H_
