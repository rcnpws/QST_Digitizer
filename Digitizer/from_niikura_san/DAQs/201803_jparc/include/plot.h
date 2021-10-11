#ifndef _PLOT_H_
#define _PLOT_H_

int PlotWaveform(FILE* gnuplot, CAEN_DGTZ_UINT16_EVENT_t *Event16, WaveDumpConfig_t *WDcfg, int plotCh);
int PlotWaveformAllch(FILE* gnuplot, CAEN_DGTZ_UINT16_EVENT_t *Event16, WaveDumpConfig_t *WDcfg);
int PlotPHA(FILE* gnuplot, CAEN_DGTZ_DPP_PHA_Waveforms_t  *PHAWaveform);
int ClosePlot(FILE* gnuplot);
#endif // _PLOT_H
