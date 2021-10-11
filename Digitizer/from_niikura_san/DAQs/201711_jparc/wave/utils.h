// utils.h
// written by M. Niikura
// $Id: utils.h 2018-01-27 16:43:02 niikura Exp $

using namespace std;

void show_help(){
  cout << endl;
  cout << "Usage : ./viewwave [option] <run_number>" << endl;
  cout << "        -s: show histograms mode (not available)" << endl;
  cout << "        -o: oscilloscope event-by-event mode (no save)" << endl;
  cout << "        -O: oscilloscope continuous-update mode (no save)" << endl;
  cout << "        -d: debug mode (many messages)" << endl;
  cout << endl;
}

void show_error(char message[]){
  cout << "\x1b[31mERROR : " << message << "\x1b[m" << endl;
}
  
