// utils.h
// written by M. Niikura
// $Id: utils.h 2018-02-20 19:50:29 daq Exp $

using namespace std;

void show_help(){
  cout << endl;
  cout << "Usage : ./viewwave [option] <run_number>" << endl;
  cout << "        -s: Show histogram mode for WaveDomp (no save)" << endl;
  cout << "        -S: Show histogram mode for PHA (no save)" << endl;
  cout << "        -o: oscilloscope event-by-event mode (no save)" << endl;
  cout << "        -O: oscilloscope continuous-update mode (no save)" << endl;
  cout << "        -d: debug mode (many messages)" << endl;
  cout << endl;
}

void show_error(char message[]){
  cout << "\x1b[31mERROR : " << message << "\x1b[m" << endl;
}
  
