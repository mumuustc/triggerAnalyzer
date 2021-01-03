const Int_t mNMax = 2000;
const Int_t mNBbcTiles = 24;
const Int_t mNZdcStrips = 8;
struct StEvtData
{
	// event information
	Int_t    mRunId;
	Int_t    mEventId;
	Short_t  mNTrigs;
	Int_t    mTrigId[64];
	Float_t  mBField;
	Float_t  mBbcRate;
	Float_t  mZdcRate;
	Float_t  mVpdVz;
	Float_t  mVertexX;
    Float_t  mVertexY;
    Float_t  mVertexZ;
	Float_t  mVertexRanking;

	// refMult and centrality information
	Int_t    mGRefMult;
	Int_t    mRefMult;
	Int_t    mRefMult2;
	Int_t    mRefMult2East;
	Int_t    mRefMult2West;
	Int_t    mRefMultHalfEast;
	Int_t    mRefMultHalfWest;
	UShort_t mNBemcMatch;
	UShort_t mNBtofMatch;
	UShort_t mOfflineBtofMult;
	Float_t  mRefMultCorr;
	Float_t  mEvtWeight;
	Short_t  mCentrality;

	UShort_t mNVpdHitsEast;
	UShort_t mNVpdHitsWest;

	// BBC ADC information
	UShort_t mNBbcHitsEast;
	UShort_t mNBbcHitsWest;
	UShort_t mBbcAdcEast[mNBbcTiles];
	UShort_t mBbcAdcWest[mNBbcTiles];
	UShort_t mMaxBbcAdcEast;
	UShort_t mMaxBbcAdcWest;
	UInt_t   mTotalBbcAdcEast;
	UInt_t   mTotalBbcAdcWest;

	// ZDC ADC information
	Float_t  mZdcSumAdcEast;
	Float_t  mZdcSumAdcWest;
	Float_t  mZdcSmdEastHorizontal[mNZdcStrips];
	Float_t  mZdcSmdEastVertical[mNZdcStrips];
	Float_t  mZdcSmdWestHorizontal[mNZdcStrips];
	Float_t  mZdcSmdWestVertical[mNZdcStrips];

	// EPD information
	UShort_t mNEpdHits;
	UShort_t mNEpdHitsEast;
	UShort_t mNEpdHitsWest;
	Short_t  mSide[mNMax];
	Short_t  mPosition[mNMax];
	Short_t  mTile[mNMax];
	Short_t  mADC[mNMax];
	Short_t  mTAC[mNMax];
	Short_t  mTDC[mNMax];
	Bool_t   mHasTAC[mNMax];
	Float_t  mNMIP[mNMax];
	Float_t  mTnMIP[mNMax];
	Bool_t   mStatusIsGood[mNMax];
	Short_t  mMaxEpdAdcEast;
	Short_t  mMaxEpdAdcWest;
	UInt_t   mTotalEpdAdcEast;
	UInt_t   mTotalEpdAdcWest;

	//track information
	UShort_t mNTrks;
	Short_t  mTrkId[mNMax];
	Bool_t   mTPCeTrkFlag[mNMax];
	Short_t  mBEMCTraitsIndex[mNMax];
	Short_t  mCharge[mNMax];
	Float_t  mPt[mNMax];
	Float_t  mEta[mNMax];
	Float_t  mPhi[mNMax];
	Float_t  mgPt[mNMax];
	Float_t  mgEta[mNMax];
	Float_t  mgPhi[mNMax];
	Float_t  mgOriginX[mNMax];
	Float_t  mgOriginY[mNMax];
	Float_t  mgOriginZ[mNMax];
	Short_t  mNHitsFit[mNMax];
	Short_t  mNHitsMax[mNMax];
	Short_t  mNHitsDedx[mNMax];
	Float_t  mDedx[mNMax];
	Float_t  mNSigmaE[mNMax];
	Float_t  mDca[mNMax];
	Bool_t   mIsHFTTrk[mNMax];
	Bool_t   mHasHFT4Layers[mNMax];
	Short_t  mTOFMatchFlag[mNMax];
	Float_t  mTOFLocalY[mNMax];
	Float_t  mBeta2TOF[mNMax];

	//BEMC pidTrait information
	UShort_t mNBEMCTrks;
	Short_t  mBEMCTrkIndex[mNMax];
	Short_t  mBEMCAdc0[mNMax];
	Float_t  mBEMCE0[mNMax];
	Float_t  mBEMCE[mNMax];
	Float_t  mBEMCZDist[mNMax];
	Float_t  mBEMCPhiDist[mNMax];
	Short_t  mBEMCnEta[mNMax];
	Short_t  mBEMCnPhi[mNMax];
	Float_t  mNSigmaPi[mNMax];
	Float_t  mNSigmaK[mNMax];
	Float_t  mNSigmaP[mNMax];
};
