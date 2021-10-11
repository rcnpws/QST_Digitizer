//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 24 11:29:23 2017 by ROOT version 6.06/08
// from TTree tree0/Tree for Board #0
// found on file: ./root/run2085.root
//////////////////////////////////////////////////////////

#ifndef BN0_h
#define BN0_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class BN0 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           TS0;
   Int_t           nsample0[16];
   Double_t        baseline0[16];
   Double_t        MaxPH0[16];
   Int_t           MaxTT0[16];
   Double_t        LED0[16];
   Double_t        QDCs0[16];
   Double_t        QDCl0[16];

   // List of branches
   TBranch        *b_TS0;   //!
   TBranch        *b_nsample;   //!
   TBranch        *b_baseline;   //!
   TBranch        *b_MaxPH;   //!
   TBranch        *b_MaxTT;   //!
   TBranch        *b_LED;   //!
   TBranch        *b_QDCs;   //!
   TBranch        *b_QDCl;   //!

   BN0(TTree *tree=0);
   virtual ~BN0();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   /* virtual ULong64_t GetTimeStamp(Long64_t); */
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef BN0_cxx
BN0::BN0(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("./root/run2085.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("./root/run2085.root");
      }
      f->GetObject("tree0",tree);

   }
   Init(tree);
}

BN0::~BN0()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t BN0::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t BN0::LoadTree(Long64_t entry)
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

void BN0::Init(TTree *tree)
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

   fChain->SetBranchAddress("TS0", &TS0, &b_TS0);
   fChain->SetBranchAddress("nsample0", nsample0, &b_nsample);
   fChain->SetBranchAddress("baseline0", baseline0, &b_baseline);
   fChain->SetBranchAddress("MaxPH0", MaxPH0, &b_MaxPH);
   fChain->SetBranchAddress("MaxTT0", MaxTT0, &b_MaxTT);
   fChain->SetBranchAddress("LED0", LED0, &b_LED);
   fChain->SetBranchAddress("QDCs0", QDCs0, &b_QDCs);
   fChain->SetBranchAddress("QDCl0", QDCl0, &b_QDCl);
   Notify();
}

Bool_t BN0::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void BN0::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t BN0::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef BN0_cxx
