#include "TMath.h"

//event cuts
const Double_t mVzCut = 30.;
const Double_t mVzDiffCut = 3.;
const Double_t mVrCut = 2.;
const Double_t mNMipTh = 0.3;
const Int_t    mEpdAdcTh = 30;

const Int_t    mEpdInnerOuterBoundary = 5;  // row <= 5 - inner; row > 5 outer

const Int_t    mNOuterEpdHitsEastTh = 15;
const Int_t    mNOuterEpdHitsWestTh = 15;
