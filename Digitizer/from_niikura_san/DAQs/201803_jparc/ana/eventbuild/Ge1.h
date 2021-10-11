//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar 19 15:05:04 2018 by ROOT version 6.10/08
// from TTree ge1/Ge1 Tree
// found on file: root/run0326.root
//////////////////////////////////////////////////////////

#ifndef Ge1_h
#define Ge1_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class Ge1 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   ULong64_t       TT;
   ULong64_t       TimeStamp;
   Int_t           Eraw;
   Double_t        Ecal;
   Int_t           ievent;

   // List of branches
   TBranch        *b_TT;   //!
   TBranch        *b_TimeStamp;   //!
   TBranch        *b_Eraw;   //!
   TBranch        *b_Ecal;   //!
   TBranch        *b_ievent;   //!

   Ge1(TTree *tree=0);
   virtual ~Ge1();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   /* virtual void     Loop(); */
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   virtual ULong64_t GetTimeStamp(ULong64_t);
   virtual ULong64_t GetNumberOfEntry();
   virtual Double_t  GetEnergy(ULong64_t);
};

#endif

#ifdef Ge1_cxx
Ge1::Ge1(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root/run0326.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root/run0326.root");
      }
      f->GetObject("ge1",tree);

   }
   Init(tree);
}

Ge1::~Ge1()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Ge1::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Ge1::LoadTree(Long64_t entry)
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

void Ge1::Init(TTree *tree)
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

   fChain->SetBranchAddress("TT", &TT, &b_TT);
   fChain->SetBranchAddress("TimeStamp", &TimeStamp, &b_TimeStamp);
   fChain->SetBranchAddress("Eraw", &Eraw, &b_Eraw);
   fChain->SetBranchAddress("Ecal", &Ecal, &b_Ecal);
   fChain->SetBranchAddress("ievent", &ievent, &b_ievent);
   Notify();
}

Bool_t Ge1::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Ge1::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Ge1::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Ge1_cxx
