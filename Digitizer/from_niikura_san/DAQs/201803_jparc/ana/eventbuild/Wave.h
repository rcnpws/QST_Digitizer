//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar 19 15:04:57 2018 by ROOT version 6.10/08
// from TTree wave/WaveForm Tree
// found on file: root/run0326.root
//////////////////////////////////////////////////////////

#ifndef Wave_h
#define Wave_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class Wave {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           TS;
   Int_t           ievent;
   ULong64_t       TimeStamp;
   Double_t        baseline[4];
   Int_t           t0;
   Int_t           t1;
   Int_t           t2;
   Int_t           t3;
   Double_t        QDC;
   Double_t        QDCb1;
   Double_t        QDCb2;

   // List of branches
   TBranch        *b_TS;   //!
   TBranch        *b_ievent;   //!
   TBranch        *b_TimeStamp;   //!
   TBranch        *b_baseline;   //!
   TBranch        *b_t0;   //!
   TBranch        *b_t1;   //!
   TBranch        *b_t2;   //!
   TBranch        *b_t3;   //!
   TBranch        *b_QDC;   //!
   TBranch        *b_QDCb1;   //!
   TBranch        *b_QDCb2;   //!

   Wave(TTree *tree=0);
   virtual ~Wave();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   /* virtual void     Loop(); */
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   virtual ULong64_t GetTimeStamp(ULong64_t);
   virtual ULong64_t GetNumberOfEntry();

};

#endif

#ifdef Wave_cxx
Wave::Wave(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root/run0326.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root/run0326.root");
      }
      f->GetObject("wave",tree);

   }
   Init(tree);
}

Wave::~Wave()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Wave::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Wave::LoadTree(Long64_t entry)
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

void Wave::Init(TTree *tree)
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

   fChain->SetBranchAddress("TS", &TS, &b_TS);
   fChain->SetBranchAddress("ievent", &ievent, &b_ievent);
   fChain->SetBranchAddress("TimeStamp", &TimeStamp, &b_TimeStamp);
   fChain->SetBranchAddress("baseline", baseline, &b_baseline);
   fChain->SetBranchAddress("t0", &t0, &b_t0);
   fChain->SetBranchAddress("t1", &t1, &b_t1);
   fChain->SetBranchAddress("t2", &t2, &b_t2);
   fChain->SetBranchAddress("t3", &t3, &b_t3);
   fChain->SetBranchAddress("QDC", &QDC, &b_QDC);
   fChain->SetBranchAddress("QDCb1", &QDCb1, &b_QDCb1);
   fChain->SetBranchAddress("QDCb2", &QDCb2, &b_QDCb2);
   Notify();
}

Bool_t Wave::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Wave::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Wave::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Wave_cxx