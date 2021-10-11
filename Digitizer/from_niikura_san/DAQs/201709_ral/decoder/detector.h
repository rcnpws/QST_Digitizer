// detector.h
// written by M. Niikura
// $Id: detector.h 2017-09-19 22:17:27 niikura Exp $

const uint32_t magic_number_wave=0xffffffff;
const uint32_t magic_number_ge  =0xefefefef;

// detector_type
//   0: non
//   1: Plastics
//   2: NIM Logic

Int_t detector_type[32] = {1, 1, 1, 1,   1, 1, 1, 1,
			   1, 1, 1, 1,   1, 1, 1, 1,
			   1, 1, 1, 1,   1, 1, 1, 1,
			   1, 1, 1, 1,   1, 1, 1, 1};


/* Double_t fixed_baseline[32] = {0}; */
Double_t fixed_baseline[32] = {16384, 16384, 16384, 16384,
			       16384, 16384, 16384, 16384,
			       16384, 16384, 16384, 16384,
			       16384, 16384, 16384, 16384,
			       
			       16384, 16384, 16384, 16384,
			       16384, 16384, 16384, 16384,
			       16384, 16384, 16384, 16384,
			       16384, 16384, 16384, 16384};

/* Double_t fixed_baseline[32] = { 15500.0, 15500.0, 15500.0, 15500.0, */
/* 				15625.1, 15687.0, 15797.3, 15704.3, */
/* 				15640.9, 15802.2, 15614.0, 15755.8, */
/* 				15759.4, 15877.8, 15679.2, 15935.5, */

/* 				15698.4, 15701.0, 15756.2, 15668.9, */
/* 				15820.2, 15748.6, 15760.2, 15749.2, */
/* 				15867.2, 15729.9, 15786.1, 15831.0, */
/* 				15903.4, 15902.9, 15867.4, 15857.1}; */

/* Double_t fixed_baseline[32] = {15711.9, 15774.8, 15637.8, 15765.4, */
/* 			       15612.8, 15677.9, 15791.0, 15795.3, */
/* 			       15637.4, 15801.4, 15614.2, 15754.0, */
/* 			       15759.3, 15872.1, 15676.3, 15933.8, */
/* 			       15699.4, 15702.9, 15758.2, 15671.1, */
/* 			       15817.4, 15735.7, 15748.3, 15739.9, */
/* 			       15862.6, 15732.0, 15786.0, 15835.4, */
/* 			       15918.3, 0.0, 0.0, 0.0}; */

Int_t threshold[32] = {  0,   0,   0,   0,   100, 100, 100, 100,
		       100, 100, 100, 100,   100, 100, 100, 100,
		       100, 100, 100, 100,   100, 100, 100, 100,
		       100, 100, 100, 100,   100, 100, 100, 100};
/* Int_t threshold[32] = {200, 200, 250, 250,   300, 300, 300, 300, */
/* 		       200, 200, 200, 600,  1500, 200, 200, 400, */
/* 		        10, 200, 200, 200,   300, 600, 600, 600, */
/* 		       600, 600, 600, 600,   300, 30000, 30000, 30000};		        */

Double_t tof_offset[32] = { 0.0,  0.0,  0.0,  0.0,
			    -3.7535, -4.17201, -4.55516, -4.31454,
			    -5.40852, -4.85723, -4.42515, -5.14277,
			    -3.70965, -4.31982, -4.71415, -3.89942,
			    -5.2251, -4.89552, -4.90586, -4.68153,
			    -4.0383, -5.37289, -4.21662, -3.93389,
			    -4.83729, 0.0, 0.0, 0.0,
			    0.0, 0.0, 0.0, 0.0};

// ========= DEFINE Trees =========
TTree *tree0;
TTree *tree1;
TTree *tree2;
TTree *tree3;
TTree *tree4;
TTree *tree5;

// --- valuables for board info
Int_t TS0, TS1;

// --- common for waveform analysis
Int_t wave[32][2000];
Int_t ss[32][2000];

Int_t nevent0 = 0;
Int_t nevent1 = 0;

Int_t	 nsample[32];
Double_t baseline[32];
Double_t MaxPH[32];
Int_t	 MaxTT[32];

Double_t LED[32];
Double_t QDCs[32];
Double_t QDCl[32];

// valuables for analyzed tree
Int_t TSdiff;
Int_t mBaF2;
Double_t Tbaf;
Double_t TOF[32];
Double_t En[32];

void DefineTree(Bool_t waveform_flag){
  tree0 = new TTree("tree0","Tree for Board #0");
  tree1 = new TTree("tree1","Tree for Board #1");
  
  if(waveform_flag){
    tree0->Branch("wave0",&wave[ 0][0],"wave[16][1000]/I");
    tree1->Branch("wave1",&wave[16][0],"wave[16][1000]/I");
    tree0->Branch("ss0",&ss[ 0][0],"ss[16][1000]/I");
    tree1->Branch("ss1",&ss[16][0],"ss[16][1000]/I");
  }
  tree0->Branch("TS0",&TS0,"TS0/I");
  tree1->Branch("TS1",&TS1,"TS1/I");
  /* tree0->Branch("nevent0",&nevent0,"nevent0/I"); */
  /* tree1->Branch("nevent1",&nevent1,"nevent1/I"); */
  tree0->Branch("nsample0",&nsample[ 0],"nsample[16]/I");
  tree1->Branch("nsample1",&nsample[16],"nsample[16]/I");
  tree0->Branch("baseline0",&baseline[ 0],"baseline[16]/D");
  tree1->Branch("baseline1",&baseline[16],"baseline[16]/D");
  tree0->Branch("MaxPH0",&MaxPH[ 0],"MaxPH[16]/D");
  tree1->Branch("MaxPH1",&MaxPH[16],"MaxPH[16]/D");
  tree0->Branch("MaxTT0",&MaxTT[ 0],"MaxTT[16]/I");
  tree1->Branch("MaxTT1",&MaxTT[16],"MaxTT[16]/I");
  tree0->Branch("LED0",&LED[ 0],"LED[16]/D");
  tree1->Branch("LED1",&LED[16],"LED[16]/D");
  tree0->Branch("QDCs0",&QDCs[ 0],"QDCs[16]/D");
  tree1->Branch("QDCs1",&QDCs[16],"QDCs[16]/D");
  tree0->Branch("QDCl0",&QDCl[ 0],"QDCl[16]/D");
  tree1->Branch("QDCl1",&QDCl[16],"QDCl[16]/D");

  tree5 = new TTree("tree5","Analyzed Tree");
  tree5->Branch("TSdiff",&TSdiff,"TSdiff/I");
  tree5->Branch("mBaF2",&mBaF2,"mBaF2/I");
  tree5->Branch("Tbaf",&Tbaf,"Tbaf/D");
  tree5->Branch("TOF",TOF,"TOF[32]/D");
  tree5->Branch("En",En,"En[32]/D");
  
}  
  
void Init(){
  TS0=0;
  TS1=0;
  memset(wave, 0, sizeof(wave));
  for(int i=0;i<32;i++){
    for(int j=0;j<1000;j++){
      ss[i][j]=j;
    }
  }
  
  memset(nsample, 0, sizeof(nsample));
  memset(baseline, 0, sizeof(baseline));
  memset(MaxPH, 0, sizeof(MaxPH));
  memset(MaxTT, 0, sizeof(MaxTT));
  memset(LED, 0, sizeof(LED));
  memset(QDCs, 0, sizeof(QDCs));
  memset(QDCl, 0, sizeof(QDCl));
  mBaF2=0;
  Tbaf=0.0;
  memset(TOF, 0, sizeof(TOF));
  memset(En, 0, sizeof(En));
}

// --- DECODER

int common_decoder(std::ifstream *fin, Int_t chid){

  Int_t isample=0;
  uint32_t dd;
  
  while(1){

    
    fin->read( (char *)&dd, sizeof(int) );

    if(dd==magic_number_wave || dd==magic_number_ge) break;

    Int_t d1=( dd&0x0000ffff );
    Int_t d2=( dd&0xffff0000 ) >> 16;

    /* wave[chid][isample*2] = ((double)d1); */
    /* wave[chid][isample*2+1] = ((double)d2); */
    wave[chid][isample*2] = -((double)d1-fixed_baseline[chid]);
    wave[chid][isample*2+1] = -((double)d2-fixed_baseline[chid]);

    // --- find baseline
    if(isample<5){
      baseline[chid]+=(wave[chid][isample*2]+wave[chid][isample*2+1])/10.0;
    }else{
      // --- find max pulse hight and its timing
      if(wave[chid][isample*2]>MaxPH[chid]){
	MaxPH[chid]=wave[chid][isample*2];
	MaxTT[chid]=isample*2;
      }
      if(wave[chid][isample*2+1]>MaxPH[chid]){
	MaxPH[chid]=wave[chid][isample*2+1];
	MaxTT[chid]=isample*2+1;
      }
    }

    isample++;
      
    if(fin->eof()) break;
  }

  nsample[chid] = isample*2;

  return dd;
  
}

void ana_seamine(Int_t chid)
{

  if(MaxPH[chid]>threshold[chid]){
    Bool_t flag_led=true;
    QDCs[chid]=0.0;
    QDCl[chid]=0.0;

    for(int i=MaxTT[chid]-15;i<MaxTT[chid]+300;i++){
      if(wave[chid][i]<MaxPH[chid]*0.5 && wave[chid][i+1]>MaxPH[chid]*0.5 && flag_led){
	LED[chid] = i + ((double)MaxPH[chid]*0.5-(double)wave[chid][i])/((double)wave[chid][i+1]-(double)wave[chid][i]);
	flag_led=false;
      }
      if(i<MaxTT[chid]+20)
	QDCs[chid]+=wave[chid][i];
      QDCl[chid]+=wave[chid][i];
    }
  }else{
    LED[chid]=-100.;
  }
}

void ana_plastic(Int_t chid)
{
  QDCl[chid]=0;
  for(int i=0;i<300;i++){
    QDCl[chid]+=(wave[chid][i]-baseline[chid])/1000.;
  }


}

void ana_torpedo(Int_t chid)
{
  if(MaxPH[chid]>threshold[chid]){
    for(int i=0;i<MaxTT[chid];i++){
      if(wave[chid][i]<600 && wave[chid][i+1]>600){
	LED[chid] = i + (600-(double)wave[chid][i])/((double)wave[chid][i+1]-(double)wave[chid][i]);
	/* LED[chid] = i + (600-wave[chid][i])/(wave[chid][i+1]-wave[chid][i]); */
	break;
      }
    }
  }else{
    LED[chid]=-100.;
  }
  
}

void ana_common()
{

  // --- check TS difference
  TSdiff = TS0-TS1;

  // --- get BaF2 timing

  mBaF2=0;
  for(int i=25;i<=31;i++){
    if(LED[i]>0){
      Tbaf=LED[i];
      mBaF2++;
      /* cout << Form("%d, %f, %f, %d\n",i,LED[i],Tbaf,mBaF2); */
    }
  }
  // --- now we use BaF2_multihit=1 event only
  if(mBaF2!=1){
    Tbaf=-100;
  }
  // --- calculate TOF
  if(Tbaf>29 && Tbaf<38){
    for(int i=0;i<32;i++){
      if(i>=4 && i<=24){
	TOF[i] = 2*((LED[i]-Tbaf) - tof_offset[i]) + 1.333; // ns
	Double_t beta = 400./TOF[i]/300.;
	En[i] = (1/sqrt(1-beta*beta)-1)*938.0;
      }else{
	TOF[i]=-1000.;
      }
    }
  }else{
    for(int i=0;i<32;i++){
      TOF[i]=-800.;
    }
  }
    
  
  
}
/* void ana_seamine(Int_t chid) */
/* { */
/*   // --- GET LED */
/*   Double_t threshold = MaxPH[chid]*0.1; */
/*   for(int i=MaxTT[chid]-10;i<MaxTT[chid];i++){ */
/*     if(wave[chid][i]<threshold && wave[chid][i+1]>threshold){ */
/*   }   */
/* } */
