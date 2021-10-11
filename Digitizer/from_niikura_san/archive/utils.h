// utils.h
// written by M. Niikura
// $Id: utils.h 2018-08-22 11:50:49 niikura Exp $

using namespace std;

void show_help(){
  cout << endl;
  cout << "Usage : ./analysis [option] <run_number>" << endl;
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
  
