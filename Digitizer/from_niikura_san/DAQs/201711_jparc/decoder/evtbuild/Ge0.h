//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Apr 10 18:38:02 2017 by ROOT version 6.06/08
// from TTree tree2/DPP-PHA for ch0
// found on file: root/run3068_wave.root
//////////////////////////////////////////////////////////

#ifndef Ge0_h
#define Ge0_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class Ge0 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   ULong64_t       TimeTag;
   ULong64_t       Format;
   UShort_t        Extras;
   UInt_t          Extras2;
   UInt_t          channel;
   UInt_t          evtnum;
   UInt_t          board_handle;
   UInt_t          evtnum_singleread;
   Int_t           rawGe;
   Double_t        GeE;
   Int_t           GeCFD;
   ULong64_t       TimeStamp;
   /* Double_t        calGe; */

   // List of branches
   TBranch        *b_TimeTag;   //!
   TBranch        *b_Format;   //!
   TBranch        *b_Extras;   //!
   TBranch        *b_Extras2;   //!
   TBranch        *b_channel;   //!
   TBranch        *b_evtnum;   //!
   TBranch        *b_board_handle;   //!
   TBranch        *b_evtnum_singleread;   //!
   TBranch        *b_rawGe;   //!
   TBranch        *b_GeE;   //!
   TBranch        *b_GeCFD;   //!
   TBranch        *b_TimeStamp;   //!
   /* TBranch        *b_calGe;   //! */

   Ge0(TTree *tree=0);
   virtual ~Ge0();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   /* virtual void     Loop(); */
   virtual ULong64_t GetTimeStamp(ULong64_t jevent);
   virtual ULong64_t  GetNumberOfEntry();
   virtual Double_t GetEnergy(ULong64_t jevent);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif
#ifdef Ge0_cxx
Ge0::Ge0(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root/run3068_wave.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root/run3068_wave.root");
      }
      f->GetObject("tree2",tree);

   }
   Init(tree);
}

Ge0::~Ge0()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Ge0::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Ge0::LoadTree(Long64_t entry)
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

void Ge0::Init(TTree *tree)
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

   fChain->SetBranchAddress("TimeTag", &TimeTag, &b_TimeTag);
   fChain->SetBranchAddress("Format", &Format, &b_Format);
   fChain->SetBranchAddress("Extras", &Extras, &b_Extras);
   fChain->SetBranchAddress("Extras2", &Extras2, &b_Extras2);
   fChain->SetBranchAddress("channel", &channel, &b_channel);
   fChain->SetBranchAddress("evtnum", &evtnum, &b_evtnum);
   fChain->SetBranchAddress("board_handle", &board_handle, &b_board_handle);
   fChain->SetBranchAddress("evtnum_singleread", &evtnum_singleread, &b_evtnum_singleread);
   fChain->SetBranchAddress("rawGe", &rawGe, &b_rawGe);
   fChain->SetBranchAddress("GeE", &GeE, &b_GeE);
   fChain->SetBranchAddress("GeCFD", &GeCFD, &b_GeCFD);
   fChain->SetBranchAddress("TimeStamp", &TimeStamp, &b_TimeStamp);
   /* fChain->SetBranchAddress("calGe", &calGe, &b_calGe); */
   Notify();
}

Bool_t Ge0::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Ge0::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Ge0::Cut(Long64_t)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Ge0_cxx
