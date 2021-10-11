// 2017.8.2 T. Saito
// DAQ with CAEN digitizers for RAL muon experiment
// SIS3800 and  LUPO TS functoins
//

#include <CAENComm.h>
#include <CAENVMElib.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>

#include "sis3800.h"
#include "vlupo.h"

const char* GetScalerName(int ch)
{
  /* this array of scaler name can be loaded in future */
  static const char* const SCALER_NAME[32] = {
    "Pla #1",     "Pla #2",   "Pla coin",   "Cherenkov", // ch  0-- 3
    "",  "",  "", "",  // ch  4-- 7
    "50Hz",  "40Hz",  "",  "",  // ch  8--11
    "",  "",  "",  "",  // ch 12--15
    "50Hz+pla",  "", "", "",    // ch 16--19
    "",    "",    "",    "",    // ch 20--23
    "Ge 1",    "Ge 2",    "",       "",      // ch 24--27
    "Accepted",      "Trigger",    "Busy",        "1k Clock",   // ch 28--31
  };
  return SCALER_NAME[ch];
}

int InitializeScaler(int BHandle, Scaler_t * pScaler){
  uint32_t dummy_val = 0x01;
  int i;
  int ret;

  //initialize scaler value of previous run 
  for(i=0;i<SIS3800_NCH;i++){
    pScaler->val[i]=0;
    pScaler->previous[i]=0;
    pScaler->overflow[i]=0;
    pScaler->rate[i]=0;
  }
  pScaler->scalertime=0;  

  // initialize SIS3800 module
  ret = CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_CLEAR_ALL_COUNTERS,
			   &dummy_val,cvA32_U_DATA,cvD32)
    | CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_RESET_REGISTER_GLOBAL,
			       &dummy_val,cvA32_U_DATA,cvD32)
    | CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_GLOBAL_COUNT_ENABLE,
			 &dummy_val,cvA32_U_DATA,cvD32);

  struct timespec const ts = {0, 50000000}; // 50 msec
  nanosleep(&ts,NULL); // 50 msec sleep
  	
  return ret;
}

int ScalerRead(int BHandle, Scaler_t * pScaler){
  int ret=0;
  int i;
  ret = CAENVME_ReadCycle(BHandle, ADDR_SIS3800 | SIS3800_READ_COUNTER + (SIS3800_NCH - 1)*4,
			  &pScaler->val[SIS3800_NCH - 1],cvA32_U_DATA,cvD32);  	    
  pScaler->scalertime = pScaler->val[SIS3800_NCH-1]-pScaler->previous[SIS3800_NCH -1 ]; // read clock value first to calculate scalertime

  for(i=0;i<SIS3800_NCH;i++){
    ret = CAENVME_ReadCycle(BHandle, ADDR_SIS3800 | SIS3800_READ_COUNTER + i*4,
			    &pScaler->val[i],cvA32_U_DATA,cvD32);
    
    pScaler->rate[i] = (i==31) ? 1000.
      : (pScaler->val[i]-pScaler->previous[i])*1000./pScaler->scalertime;

   if (pScaler->val[i] < pScaler->previous[i]) { // overflow
      pScaler->overflow[i]++;
    }
   pScaler->previous[i] = pScaler->val[i];
   
  }
  
  return ret;
}

int ScalerDuringRun(int BHandle, Scaler_t * pScaler,int isWrite, FILE* f_scaler){
  uint64_t scalerval64 = 0;
  int ret=0;
  int i;

  ret = ScalerRead(BHandle, pScaler);
  
  printf("scaler rate monitor\n");
  for(i=0; i<32; i++){
  
    const char* const scalername = GetScalerName(i);
    const char* const color_seq =
      pScaler->rate[i] < 1e+1 ? "\x1b[49m"  // default
      : pScaler->rate[i] < 1e+2 ? "\x1b[47m"  // gray
      : pScaler->rate[i] < 1e+3 ? "\x1b[44m"  // blue
      : pScaler->rate[i] < 1e+4 ? "\x1b[42m"  // green
      : pScaler->rate[i]< 1e+5 ? "\x1b[43m"  // yellow
      : pScaler->rate[i] < 1e+6 ? "\x1b[45m"  // magenta
      :                     "\x1b[41m"; // red
        
    scalerval64 = ((uint64_t) pScaler->overflow[i] << 32) + pScaler->val[i];
    
    printf(" %s \x1b[49mch%2d %-10s : %9.1f Hz : %10"PRIu64" counts\n",
	   color_seq,i,scalername,pScaler->rate[i],scalerval64);
    if (isWrite){
      fprintf(f_scaler,"ch%2d, %-10s, %10"PRIu64"\n",
	      i,scalername,scalerval64);
    }
  }

  return ret;
}


int ScalerEndOfRun(int BHandle, Scaler_t * pScaler,int isWrite, FILE* f_scaler){

  uint64_t scalerval64 = 0;
  int i;
  int ret=0;
  uint32_t dummy_val=0x01;

  ret = ScalerRead(BHandle, pScaler);
  
  printf("scaler values at the end of run\n");
  for(i=0; i<32; i++){
    const char* const scalername = GetScalerName(i);
    scalerval64 = ((uint64_t) pScaler->overflow[i] << 32) + pScaler->val[i];
    printf(" ch%2d: %-10s : %10"PRIu64" counts\n",i,scalername,scalerval64);
    if (isWrite){
      if (i==0) {
	fprintf(f_scaler,"scaler values at the end of run\n");
      }
      fprintf(f_scaler,"ch%2d, %-10s, %10"PRIu64"\n",i,scalername,scalerval64);
    }
  }
	
  ret = CAENVME_WriteCycle(BHandle, ADDR_SIS3800 | SIS3800_GLOBAL_COUNT_DISABLE,
			   &dummy_val,cvA32_U_DATA,cvD32);

  
  return ret;
}
// lupo ts

int InitializeLUPOTS(int BHandle){
  int ret=0;
  uint16_t dummy_val16=0x00;
  ret |= CAENVME_WriteCycle(BHandle, ADDR_LUPOTS | VLUPO_TS_INTDISABLE, &dummy_val16, cvA32_U_DATA,cvD16); //disable interrupt
  ret |= CAENVME_WriteCycle(BHandle, ADDR_LUPOTS | VLUPO_TS_CLKMODE, &dummy_val16, cvA32_U_DATA,cvD16); //clk internal
  ret |= CAENVME_ReadCycle(BHandle, ADDR_LUPOTS | VLUPO_TS_CLEARFIFO, &dummy_val16,  cvA32_U_DATA,cvD16); // clear fifo

  return ret;
}
int ReadLUPOTS(int BHandle, int isWrite, FILE* f_scaler){
  int ret=0;
  uint32_t timestamp_upper, timestamp_lower;

  uint16_t dummy_val16=0x03; // 0x03 is for trigger output
  ret |= CAENVME_WriteCycle(BHandle, ADDR_LUPOTS |VLUPO_TS_TRGCOUNTER, &dummy_val16, cvA32_U_DATA,cvD16); //for trigger output	  
  ret |= CAENVME_ReadCycle(BHandle, ADDR_LUPOTS | VLUPO_TS_DATA32L, &timestamp_lower,cvA32_U_DATA,cvD32); //lower
  ret |= CAENVME_ReadCycle(BHandle, ADDR_LUPOTS | VLUPO_TS_DATA32H, &timestamp_upper,cvA32_U_DATA,cvD32); //uppper
  ret |= CAENVME_ReadCycle(BHandle, ADDR_LUPOTS | VLUPO_TS_CLEAR, &dummy_val16,cvA32_U_DATA,cvD16); // clear current data
  
  printf("test : lupo ts is == %u + %u \n",timestamp_upper,timestamp_lower);

  if(isWrite){
    fprintf(f_scaler, "LUPOT TS, upper, %u \nLUPOT TS, lower, %u \n",timestamp_upper,timestamp_lower);
  }
  return ret;
}
