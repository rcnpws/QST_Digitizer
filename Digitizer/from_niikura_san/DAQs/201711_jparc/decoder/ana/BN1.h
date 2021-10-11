//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 24 11:29:27 2017 by ROOT version 6.06/08
// from TTree tree1/Tree for Board #1
// found on file: ./root/run2085.root
//////////////////////////////////////////////////////////

#ifndef BN1_h
#define BN1_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class BN1 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           TS1;
   Int_t           nsample1[16];
   Double_t        baseline1[16];
   Double_t        MaxPH1[16];
   Int_t           MaxTT1[16];
   Double_t        LED1[16];
   Double_t        QDCs1[16];
   Double_t        QDCl1[16];
   ULong64_t       TimeStamp;

   // List of branches
   TBranch        *b_TS1;   //!
   TBranch        *b_nsample;   //!
   TBranch        *b_baseline;   //!
   TBranch        *b_MaxPH;   //!
   TBranch        *b_MaxTT;   //!
   TBranch        *b_LED;   //!
   TBranch        *b_QDCs;   //!
   TBranch        *b_QDCl;   //!
   TBranch        *b_TimeStamp;

   BN1(TTree *tree=0);
   virtual ~BN1();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   /* virtual void     Loop(); */
   virtual ULong64_t GetTimeStamp(Long64_t jevent);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef BN1_cxx
BN1::BN1(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("./root/run2085.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("./root/run2085.root");
      }
      f->GetObject("tree1",tree);

   }
   Init(tree);
}

BN1::~BN1()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t BN1::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t BN1::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void BN1::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("TS1", &TS1, &b_TS1);
   fChain->SetBranchAddress("nsample1", nsample1, &b_nsample);
   fChain->SetBranchAddress("baseline1", baseline1, &b_baseline);
   fChain->SetBranchAddress("MaxPH1", MaxPH1, &b_MaxPH);
   fChain->SetBranchAddress("MaxTT1", MaxTT1, &b_MaxTT);
   fChain->SetBranchAddress("LED1", LED1, &b_LED);
   fChain->SetBranchAddress("QDCs1", QDCs1, &b_QDCs);
   fChain->SetBranchAddress("QDCl1", QDCl1, &b_QDCl);
   fChain->SetBranchAddress("TimeStamp", &TimeStamp, &b_TimeStamp);
   Notify();
}

Bool_t BN1::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void BN1::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t BN1::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef BN1_cxx