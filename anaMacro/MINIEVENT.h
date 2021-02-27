//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Feb 27 13:56:42 2021 by ROOT version 5.34/38
// from TTree miniDst/miniDst
// found on file: ../test/test_MuDst.root
//////////////////////////////////////////////////////////

#ifndef MINIEVENT_h
#define MINIEVENT_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class MINIEVENT {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           mRunId;
   Int_t           mEventId;
   Short_t         mNTrigs;
   Int_t           mTrigId[64];   //[mNTrigs]
   Float_t         mBField;
   Float_t         mBbcRate;
   Float_t         mZdcRate;
   Float_t         mVpdVz;
   Float_t         mVertexX;
   Float_t         mVertexY;
   Float_t         mVertexZ;
   Float_t         mVertexRanking;
   Int_t           mGRefMult;
   Int_t           mRefMult;
   Int_t           mRefMult2;
   Int_t           mRefMult2East;
   Int_t           mRefMult2West;
   Int_t           mRefMultHalfEast;
   Int_t           mRefMultHalfWest;
   UShort_t        mNBemcMatch;
   UShort_t        mNBtofMatch;
   UShort_t        mOnlineBtofMult;
   UShort_t        mOfflineBtofMult;
   Float_t         mRefMultCorr;
   Float_t         mEvtWeight;
   Short_t         mCentrality;
   UShort_t        mNVpdHitsEast;
   UShort_t        mNVpdHitsWest;
   UShort_t        mNBbcHitsEast;
   UShort_t        mNBbcHitsWest;
   UShort_t        mBbcAdcEast[24];   //[mNBbcHitsEast]
   UShort_t        mBbcAdcWest[24];   //[mNBbcHitsWest]
   UShort_t        mMaxBbcAdcEast;
   UShort_t        mMaxBbcAdcWest;
   UInt_t          mTotalBbcAdcEast;
   UInt_t          mTotalBbcAdcWest;
   Float_t         mZdcSumAdcEast;
   Float_t         mZdcSumAdcWest;
   Float_t         mZdcUnAttenuatedEast;
   Float_t         mZdcUnAttenuatedWest;
   Float_t         mZdcSmdEastHorizontal[8];
   Float_t         mZdcSmdEastVertical[8];
   Float_t         mZdcSmdWestHorizontal[8];
   Float_t         mZdcSmdWestVertical[8];
   UShort_t        mNEpdHits;
   UShort_t        mNEpdHitsEast;
   UShort_t        mNEpdHitsWest;
   Short_t         mSide[800];   //[mNEpdHits]
   Short_t         mPosition[800];   //[mNEpdHits]
   Short_t         mTile[800];   //[mNEpdHits]
   Short_t         mRow[800];   //[mNEpdHits]
   Float_t         mEpdEta[800];   //[mNEpdHits]
   Float_t         mEpdPhi[800];   //[mNEpdHits]
   Short_t         mADC[800];   //[mNEpdHits]
   Short_t         mTAC[800];   //[mNEpdHits]
   Short_t         mTDC[800];   //[mNEpdHits]
   Bool_t          mHasTAC[800];   //[mNEpdHits]
   Float_t         mNMIP[800];   //[mNEpdHits]
   Float_t         mTnMIP[800];   //[mNEpdHits]
   Bool_t          mStatusIsGood[800];   //[mNEpdHits]
   Short_t         mMaxEpdAdcEast;
   Short_t         mMaxEpdAdcWest;
   UInt_t          mTotalEpdAdcEast;
   UInt_t          mTotalEpdAdcWest;

   // List of branches
   TBranch        *b_mRunId;   //!
   TBranch        *b_mEventId;   //!
   TBranch        *b_mNTrigs;   //!
   TBranch        *b_mTrigId;   //!
   TBranch        *b_mBField;   //!
   TBranch        *b_mBbcRate;   //!
   TBranch        *b_mZdcRate;   //!
   TBranch        *b_mVpdVz;   //!
   TBranch        *b_mVertexX;   //!
   TBranch        *b_mVertexY;   //!
   TBranch        *b_mVertexZ;   //!
   TBranch        *b_mVertexRanking;   //!
   TBranch        *b_mGRefMult;   //!
   TBranch        *b_mRefMult;   //!
   TBranch        *b_mRefMult2;   //!
   TBranch        *b_mRefMult2East;   //!
   TBranch        *b_mRefMult2West;   //!
   TBranch        *b_mRefMultHalfEast;   //!
   TBranch        *b_mRefMultHalfWest;   //!
   TBranch        *b_mNBemcMatch;   //!
   TBranch        *b_mNBtofMatch;   //!
   TBranch        *b_mOnlineBtofMult;   //!
   TBranch        *b_mOfflineBtofMult;   //!
   TBranch        *b_mRefMultCorr;   //!
   TBranch        *b_mEvtWeight;   //!
   TBranch        *b_mCentrality;   //!
   TBranch        *b_mNVpdHitsEast;   //!
   TBranch        *b_mNVpdHitsWest;   //!
   TBranch        *b_mNBbcHitsEast;   //!
   TBranch        *b_mNBbcHitsWest;   //!
   TBranch        *b_mBbcAdcEast;   //!
   TBranch        *b_mBbcAdcWest;   //!
   TBranch        *b_mMaxBbcAdcEast;   //!
   TBranch        *b_mMaxBbcAdcWest;   //!
   TBranch        *b_mTotalBbcAdcEast;   //!
   TBranch        *b_mTotalBbcAdcWest;   //!
   TBranch        *b_mZdcSumAdcEast;   //!
   TBranch        *b_mZdcSumAdcWest;   //!
   TBranch        *b_mZdcUnAttenuatedEast;   //!
   TBranch        *b_mZdcUnAttenuatedWest;   //!
   TBranch        *b_mZdcSmdEastHorizontal;   //!
   TBranch        *b_mZdcSmdEastVertical;   //!
   TBranch        *b_mZdcSmdWestHorizontal;   //!
   TBranch        *b_mZdcSmdWestVertical;   //!
   TBranch        *b_mNEpdHits;   //!
   TBranch        *b_mNEpdHitsEast;   //!
   TBranch        *b_mNEpdHitsWest;   //!
   TBranch        *b_mSide;   //!
   TBranch        *b_mPosition;   //!
   TBranch        *b_mTile;   //!
   TBranch        *b_mRow;   //!
   TBranch        *b_mEpdEta;   //!
   TBranch        *b_mEpdPhi;   //!
   TBranch        *b_mADC;   //!
   TBranch        *b_mTAC;   //!
   TBranch        *b_mTDC;   //!
   TBranch        *b_mHasTAC;   //!
   TBranch        *b_mNMIP;   //!
   TBranch        *b_mTnMIP;   //!
   TBranch        *b_mStatusIsGood;   //!
   TBranch        *b_mMaxEpdAdcEast;   //!
   TBranch        *b_mMaxEpdAdcWest;   //!
   TBranch        *b_mTotalEpdAdcEast;   //!
   TBranch        *b_mTotalEpdAdcWest;   //!

   MINIEVENT(TTree *tree=0);
   virtual ~MINIEVENT();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MINIEVENT_cxx
MINIEVENT::MINIEVENT(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../test/test_MuDst.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("../test/test_MuDst.root");
      }
      f->GetObject("miniDst",tree);

   }
   Init(tree);
}

MINIEVENT::~MINIEVENT()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MINIEVENT::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MINIEVENT::LoadTree(Long64_t entry)
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

void MINIEVENT::Init(TTree *tree)
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

   fChain->SetBranchAddress("mRunId", &mRunId, &b_mRunId);
   fChain->SetBranchAddress("mEventId", &mEventId, &b_mEventId);
   fChain->SetBranchAddress("mNTrigs", &mNTrigs, &b_mNTrigs);
   fChain->SetBranchAddress("mTrigId", mTrigId, &b_mTrigId);
   fChain->SetBranchAddress("mBField", &mBField, &b_mBField);
   fChain->SetBranchAddress("mBbcRate", &mBbcRate, &b_mBbcRate);
   fChain->SetBranchAddress("mZdcRate", &mZdcRate, &b_mZdcRate);
   fChain->SetBranchAddress("mVpdVz", &mVpdVz, &b_mVpdVz);
   fChain->SetBranchAddress("mVertexX", &mVertexX, &b_mVertexX);
   fChain->SetBranchAddress("mVertexY", &mVertexY, &b_mVertexY);
   fChain->SetBranchAddress("mVertexZ", &mVertexZ, &b_mVertexZ);
   fChain->SetBranchAddress("mVertexRanking", &mVertexRanking, &b_mVertexRanking);
   fChain->SetBranchAddress("mGRefMult", &mGRefMult, &b_mGRefMult);
   fChain->SetBranchAddress("mRefMult", &mRefMult, &b_mRefMult);
   fChain->SetBranchAddress("mRefMult2", &mRefMult2, &b_mRefMult2);
   fChain->SetBranchAddress("mRefMult2East", &mRefMult2East, &b_mRefMult2East);
   fChain->SetBranchAddress("mRefMult2West", &mRefMult2West, &b_mRefMult2West);
   fChain->SetBranchAddress("mRefMultHalfEast", &mRefMultHalfEast, &b_mRefMultHalfEast);
   fChain->SetBranchAddress("mRefMultHalfWest", &mRefMultHalfWest, &b_mRefMultHalfWest);
   fChain->SetBranchAddress("mNBemcMatch", &mNBemcMatch, &b_mNBemcMatch);
   fChain->SetBranchAddress("mNBtofMatch", &mNBtofMatch, &b_mNBtofMatch);
   fChain->SetBranchAddress("mOnlineBtofMult", &mOnlineBtofMult, &b_mOnlineBtofMult);
   fChain->SetBranchAddress("mOfflineBtofMult", &mOfflineBtofMult, &b_mOfflineBtofMult);
   fChain->SetBranchAddress("mRefMultCorr", &mRefMultCorr, &b_mRefMultCorr);
   fChain->SetBranchAddress("mEvtWeight", &mEvtWeight, &b_mEvtWeight);
   fChain->SetBranchAddress("mCentrality", &mCentrality, &b_mCentrality);
   fChain->SetBranchAddress("mNVpdHitsEast", &mNVpdHitsEast, &b_mNVpdHitsEast);
   fChain->SetBranchAddress("mNVpdHitsWest", &mNVpdHitsWest, &b_mNVpdHitsWest);
   fChain->SetBranchAddress("mNBbcHitsEast", &mNBbcHitsEast, &b_mNBbcHitsEast);
   fChain->SetBranchAddress("mNBbcHitsWest", &mNBbcHitsWest, &b_mNBbcHitsWest);
   fChain->SetBranchAddress("mBbcAdcEast", mBbcAdcEast, &b_mBbcAdcEast);
   fChain->SetBranchAddress("mBbcAdcWest", mBbcAdcWest, &b_mBbcAdcWest);
   fChain->SetBranchAddress("mMaxBbcAdcEast", &mMaxBbcAdcEast, &b_mMaxBbcAdcEast);
   fChain->SetBranchAddress("mMaxBbcAdcWest", &mMaxBbcAdcWest, &b_mMaxBbcAdcWest);
   fChain->SetBranchAddress("mTotalBbcAdcEast", &mTotalBbcAdcEast, &b_mTotalBbcAdcEast);
   fChain->SetBranchAddress("mTotalBbcAdcWest", &mTotalBbcAdcWest, &b_mTotalBbcAdcWest);
   fChain->SetBranchAddress("mZdcSumAdcEast", &mZdcSumAdcEast, &b_mZdcSumAdcEast);
   fChain->SetBranchAddress("mZdcSumAdcWest", &mZdcSumAdcWest, &b_mZdcSumAdcWest);
   fChain->SetBranchAddress("mZdcUnAttenuatedEast", &mZdcUnAttenuatedEast, &b_mZdcUnAttenuatedEast);
   fChain->SetBranchAddress("mZdcUnAttenuatedWest", &mZdcUnAttenuatedWest, &b_mZdcUnAttenuatedWest);
   fChain->SetBranchAddress("mZdcSmdEastHorizontal", mZdcSmdEastHorizontal, &b_mZdcSmdEastHorizontal);
   fChain->SetBranchAddress("mZdcSmdEastVertical", mZdcSmdEastVertical, &b_mZdcSmdEastVertical);
   fChain->SetBranchAddress("mZdcSmdWestHorizontal", mZdcSmdWestHorizontal, &b_mZdcSmdWestHorizontal);
   fChain->SetBranchAddress("mZdcSmdWestVertical", mZdcSmdWestVertical, &b_mZdcSmdWestVertical);
   fChain->SetBranchAddress("mNEpdHits", &mNEpdHits, &b_mNEpdHits);
   fChain->SetBranchAddress("mNEpdHitsEast", &mNEpdHitsEast, &b_mNEpdHitsEast);
   fChain->SetBranchAddress("mNEpdHitsWest", &mNEpdHitsWest, &b_mNEpdHitsWest);
   fChain->SetBranchAddress("mSide", mSide, &b_mSide);
   fChain->SetBranchAddress("mPosition", mPosition, &b_mPosition);
   fChain->SetBranchAddress("mTile", mTile, &b_mTile);
   fChain->SetBranchAddress("mRow", mRow, &b_mRow);
   fChain->SetBranchAddress("mEpdEta", mEpdEta, &b_mEpdEta);
   fChain->SetBranchAddress("mEpdPhi", mEpdPhi, &b_mEpdPhi);
   fChain->SetBranchAddress("mADC", mADC, &b_mADC);
   fChain->SetBranchAddress("mTAC", mTAC, &b_mTAC);
   fChain->SetBranchAddress("mTDC", mTDC, &b_mTDC);
   fChain->SetBranchAddress("mHasTAC", mHasTAC, &b_mHasTAC);
   fChain->SetBranchAddress("mNMIP", mNMIP, &b_mNMIP);
   fChain->SetBranchAddress("mTnMIP", mTnMIP, &b_mTnMIP);
   fChain->SetBranchAddress("mStatusIsGood", mStatusIsGood, &b_mStatusIsGood);
   fChain->SetBranchAddress("mMaxEpdAdcEast", &mMaxEpdAdcEast, &b_mMaxEpdAdcEast);
   fChain->SetBranchAddress("mMaxEpdAdcWest", &mMaxEpdAdcWest, &b_mMaxEpdAdcWest);
   fChain->SetBranchAddress("mTotalEpdAdcEast", &mTotalEpdAdcEast, &b_mTotalEpdAdcEast);
   fChain->SetBranchAddress("mTotalEpdAdcWest", &mTotalEpdAdcWest, &b_mTotalEpdAdcWest);
   Notify();
}

Bool_t MINIEVENT::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MINIEVENT::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MINIEVENT::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MINIEVENT_cxx
