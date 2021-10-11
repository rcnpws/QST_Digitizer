const int magic_number=0xffffffff;
const int magic_number_ge=0xefefefef;

int CheckBinary(  int RunNumber){
  

  char *inputfilename;
  if(RunNumber%3==0){
    inputfilename = Form("/data00/RCNP1801_00/run%04d.dat",RunNumber);
  }else if(RunNumber%3==1){
    inputfilename = Form("/data01/RCNP1801_01/run%04d.dat",RunNumber);
  }else{
    inputfilename = Form("/data02/RCNP1801_02/run%04d.dat",RunNumber);
  }

  std::ifstream *fin = new std::ifstream(inputfilename, ios::in | ios::binary);
  cout << "---> Input File  : " << inputfilename << endl;
  if(fin->fail()){
    printf(Form("File open failed!! --- %s",inputfilename));
    return 1;
  }

  TFile *fout;
  char *outputfilename = Form("../root/run%04d.root",RunNumber);
  cout << "---> Output File : " << outputfilename << endl;
  fout = TFile::Open(outputfilename,"recreate");
  TTree* tree=new TTree("tree","Ge degug");
  
  ULong64_t	TimeTag;
  ULong64_t	Format;
  uint16_t	Extras;
  uint32_t	Extras2;
  uint32_t	channel;
  uint32_t	evtnum;
  uint32_t	board_handle;
  uint32_t	evtnum_singleread;
  uint16_t	energy;
  
  Int_t		rawGe;

  tree->Branch("rawGe",&rawGe,"rawGe/I");
  tree->Branch("channel",&channel,"channel/I");
  
  uint32_t dd;
  fin->read( (char *)&dd, sizeof(int) );
  
  int neve=0;
  while(1){
    if(fin->eof())break;
    //    if(neve>1000)break;

    if(dd==magic_number_ge){
      fin->read( (char *)&TimeTag, sizeof(TimeTag) );
      fin->read( (char *)&Format, sizeof(Format) );
      fin->read( (char *)&Extras, sizeof(Extras) );
      fin->read( (char *)&Extras2, sizeof(Extras2) );
      fin->read( (char *)&channel, sizeof(channel) );
      fin->read( (char *)&evtnum, sizeof(evtnum) );
      fin->read( (char *)&board_handle, sizeof(board_handle) );
      fin->read( (char *)&evtnum_singleread, sizeof(evtnum_singleread) );     
      fin->read( (char *)&energy, sizeof(energy) );
      rawGe=(Int_t)energy;

      //cout<<channel<<" "<<rawGe<<" "<< TimeTag<<endl;

      tree->Fill();
      neve++;
    }    
    fin->read( (char *)&dd, sizeof(int) );
  }
  tree->Write();
  fout->Close();
  return 0;
}
