#include <CAENDigitizer.h>
#include <CAENComm.h>
#include <CAENVMElib.h>
#include "Digitizer.h"

#include <stdio.h>
#include <stdlib.h>

int PlotWaveform(FILE* gnuplot, CAEN_DGTZ_UINT16_EVENT_t *Event16, WaveDumpConfig_t *WDcfg, int plotCh){
  
  const int samplemax= WDcfg->RecordLength;
  int data[samplemax];
  int sample[samplemax];
  int i=0;
  
  for(i=0;i<samplemax;i++){
    sample[i]=i;
    //printf("%d %d \n",i ,(int)Event16->DataChannel[plotCh][i]);
    data[i]=(int)Event16->DataChannel[plotCh][i];
  }

  printf(" plotting... \n");

  fprintf(gnuplot, "set xrange [0:%d]\n",samplemax);
  fprintf(gnuplot, "set yrange [0:17000]\n");
  
  fprintf(gnuplot,"plot '-' with lines linetype %d title \" ch %d \" \n",plotCh+1,plotCh);
  for(i=0;i<samplemax ;i++){
    fprintf(gnuplot,"%d\t%d\n",sample[i],data[i]);
  }
  fprintf(gnuplot,"e\n");
  fflush(gnuplot);
  
  return 0;
}
int ClosePlot(FILE* gnuplot){
  fflush(gnuplot);
  fprintf(gnuplot,"quit \n");
  pclose(gnuplot);

  return 0;
}

int PlotWaveformAllch(FILE* gnuplot, CAEN_DGTZ_UINT16_EVENT_t *Event16, WaveDumpConfig_t *WDcfg){
  fprintf(gnuplot,"set multiplot\n");
  
  PlotWaveform(gnuplot, Event16, WDcfg, 0);
  PlotWaveform(gnuplot, Event16, WDcfg, 1);
  PlotWaveform(gnuplot, Event16, WDcfg, 2);
  PlotWaveform(gnuplot, Event16, WDcfg, 3);
      
  fprintf(gnuplot, "set nomultiplot\n");
  return 0;
}

int PlotPHA(FILE* gnuplot, CAEN_DGTZ_DPP_PHA_Waveforms_t   *PHAWaveform){
  const int samplemax= (int)(PHAWaveform->Ns);
  int sample[samplemax];
  int i=0;
  uint16_t *Trace1;
  uint16_t  *Trace2;
  uint8_t *DTrace1;
  uint8_t *DTrace2;

  Trace1=PHAWaveform->Trace1;
  Trace2=PHAWaveform->Trace2;
  DTrace1=PHAWaveform->DTrace1;
  DTrace2=PHAWaveform->DTrace2;
  
  for(i=0;i<samplemax;i++){
    sample[i]=i;
  }

  //printf(" plotting PHA... \n");
  //for(i=0;i<size;i++) printf("%d ",(int)data[i]);

  fprintf(gnuplot,"set multiplot\n");
  
  fprintf(gnuplot, "set xrange [0:%d]\n",samplemax);
  fprintf(gnuplot, "set yrange [0:40000]\n");

 
  fprintf(gnuplot,"plot '-' with lines linetype 1 title \" PHA 0\" \n");
  for(i=0;i<samplemax ;i++){
    fprintf(gnuplot,"%d\t%d\n",sample[i],Trace1[i]); 
  }
  fprintf(gnuplot,"e\n");

  fprintf(gnuplot,"plot '-' with lines linetype 2 title \" PHA 1\" \n");
  for(i=0;i<samplemax ;i++){
    fprintf(gnuplot,"%d\t%d\n",sample[i],Trace2[i]); 
  }
  fprintf(gnuplot,"e\n");

    fprintf(gnuplot,"plot '-' with lines linetype 3 title \" PHA 2 \" \n");
  for(i=0;i<samplemax ;i++){
    fprintf(gnuplot,"%d\t%d\n",sample[i],DTrace1[i]*1000 + 3000); 
  }
  fprintf(gnuplot,"e\n");

    fprintf(gnuplot,"plot '-' with lines linetype 4 title \" PHA 3 \" \n");
  for(i=0;i<samplemax ;i++){
    fprintf(gnuplot,"%d\t%d\n",sample[i],DTrace2[i]*1000 + 1000); 
  }
  fprintf(gnuplot,"e\n");
  
  fflush(gnuplot);
  
  return 0;
}
