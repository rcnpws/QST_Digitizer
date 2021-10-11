// utils.h
// written by M. Niikura
// $Id: utils.h 2017-02-05 19:41:11 niikura Exp $

using namespace std;

Bool_t debug;

int event_status=1000;

void show_help(){
  cout << endl;
  cout << "Usage : ./mktree [option] <run_number>" << endl;
  cout << "        -w: waveform saving mode" << endl;
  cout << "        -s: show histograms mode" << endl;
  cout << "        -o: oscilloscope event-by-event mode (no save)" << endl;
  cout << "        -O: oscilloscope continuous-update mode (no save)" << endl;
  cout << "        -d: debug mode (many messages)" << endl;
  cout << endl;
}

void show_error(char message[]){
  cout << "\x1b[31mERROR : " << message << "\x1b[m" << endl;
}
  
