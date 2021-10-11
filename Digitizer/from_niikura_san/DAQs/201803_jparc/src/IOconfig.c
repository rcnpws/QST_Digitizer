// 2017.8.2 T. Saito
// DAQ with CAEN digitizers for RAL muon experiment
// function written by CAEN about file io is corrected this

#include <CAENDigitizer.h>
#include <CAENComm.h>
#include <CAENVMElib.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "Digitizer.h"

int WriteOutputFiles(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, CAEN_DGTZ_EventInfo_t *EventInfo, void *Event, int evtnum_singleread, FILE *f_all, int board_handle)
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


int WriteOutputFilesPHA(WaveDumpConfig_t *WDcfg, WaveDumpRun_t *WDrun, uint64_t TimeTag, uint64_t Format, int16_t Extras, uint32_t Extras2, uint32_t channel, uint32_t evtnum, uint32_t board_handle, uint32_t evtnum_singleread, uint16_t Energy, FILE *f_all)
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
    /*
    fprintf(f_all_test, "TimeTag: %"PRIu64"\n", TimeTag);
    fprintf(f_all_test, "Format: %"PRIu64"\n",  Format);
    fprintf(f_all_test, "Extras: %d\n",  Extras);
    fprintf(f_all_test, "Extras2: %d\n", Extras2);
    fprintf(f_all_test, "channel: %d\n", channel);
    fprintf(f_all_test, "evtnum: %d\n",  evtnum);
    fprintf(f_all_test, "Handle: %d\n",  board_handle);
    fprintf(f_all_test, "evtnum_singleread: %d\n", evtnum_singleread);
    fprintf(f_all_test, "Energy: %d\n",  Energy);
    */
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
