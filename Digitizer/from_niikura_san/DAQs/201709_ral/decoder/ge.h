
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
Double_t	GeE;		//calibrated val.
Int_t		GeCFD;

void DefineTreeGe()
{
  tree2 = new TTree("tree2","DPP-PHA for ch0");
  tree3 = new TTree("tree3","DPP-PHA for ch2");
  tree4 = new TTree("tree4","DPP-PHA for ch4");

  tree2->Branch("TimeTag",&TimeTag,Form("TimeTag/l"));
  tree2->Branch("Format",&Format,Form("Format/l"));
  tree2->Branch("Extras",&Extras,Form("Extras/s"));
  tree2->Branch("Extras2",&Extras2,Form("Extras2/i"));
  tree2->Branch("channel",&channel,Form("channel/i"));
  tree2->Branch("evtnum",&evtnum,Form("evtnum/i"));
  tree2->Branch("board_handle",&board_handle,Form("board_handle/i"));
  tree2->Branch("evtnum_singleread",&evtnum_singleread,Form("evtnum_singleread/i"));
  tree2->Branch("rawGe",&rawGe,Form("rawGe/I"));
  tree2->Branch("GeE",&GeE,Form("GeE/D"));
  tree2->Branch("GeCFD",&GeCFD,Form("GeCFD/I"));
  
  tree3->Branch("TimeTag",&TimeTag,Form("TimeTag/l"));
  tree3->Branch("Format",&Format,Form("Format/l"));
  tree3->Branch("Extras",&Extras,Form("Extras/s"));
  tree3->Branch("Extras2",&Extras2,Form("Extras2/i"));
  tree3->Branch("channel",&channel,Form("channel/i"));
  tree3->Branch("evtnum",&evtnum,Form("evtnum/i"));
  tree3->Branch("board_handle",&board_handle,Form("board_handle/i"));
  tree3->Branch("evtnum_singleread",&evtnum_singleread,Form("evtnum_singleread/i"));
  tree3->Branch("rawGe",&rawGe,Form("rawGe/I"));
  tree3->Branch("GeE",&GeE,Form("GeE/D"));
  tree3->Branch("GeCFD",&GeCFD,Form("GeCFD/I"));
  
  tree4->Branch("TimeTag",&TimeTag,Form("TimeTag/l"));
  tree4->Branch("Format",&Format,Form("Format/l"));
  tree4->Branch("Extras",&Extras,Form("Extras/s"));
  tree4->Branch("Extras2",&Extras2,Form("Extras2/i"));
  tree4->Branch("channel",&channel,Form("channel/i"));
  tree4->Branch("evtnum",&evtnum,Form("evtnum/i"));
  tree4->Branch("board_handle",&board_handle,Form("board_handle/i"));
  tree4->Branch("evtnum_singleread",&evtnum_singleread,Form("evtnum_singleread/i"));
  tree4->Branch("rawGe",&rawGe,Form("rawGe/I"));
  tree4->Branch("GeE",&GeE,Form("GeE/D"));
  tree4->Branch("GeCFD",&GeCFD,Form("GeCFD/I"));
  
}

void InitGe()
{

  TimeTag = 0;
  Format = 0;
  Extras = 0;
  Extras2 = 0;
  channel = 0;
  evtnum = 0;
  board_handle = 0;
  evtnum_singleread = 0;
  energy = 0;

  rawGe = 0;
  GeE = 0;		//calibrated val.
  GeCFD = 0;
  
  
}
  
