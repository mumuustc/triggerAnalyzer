#include "headers.h"
#include "StMiniTreeMaker.h"

ClassImp(StMiniTreeMaker)

//_____________________________________________________________________________
StMiniTreeMaker::StMiniTreeMaker(const Char_t *name) : StMaker(name), mFillTree(1), mFillTrkInfo(1), mFillHisto(1), mPrintConfig(1), mPrintMemory(0), mPrintCpu(0), fOutFile(0), mOutFileName(""), mEvtTree(0), mDefaultVtx(1), mMaxVtxR(1.e4), mMaxVtxZ(1.e4), mMaxVzDiff(1.e4), mMinTrkPt(0.2), mMaxTrkEta(1.), mMinNHitsFit(15), mMinNHitsFitRatio(0.52), mMinNHitsDedx(10), mMaxDca(10.), mMaxnSigmaE(3), mMaxBeta2TOF(0.03)
{
	// default constructor

	// run19 AuAu 14.5 GeV st_physics 
	mTriggerIDs.clear();
	//mTriggerIDs.push_back(650000);     // minbias
	//mTriggerIDs.push_back(650002);     // minbias-hltgood
	//mTriggerIDs.push_back(650004);     // minbias-allvtx 
	//mTriggerIDs.push_back(650005);     // mb_epdcomponent 
	//mTriggerIDs.push_back(650006);     // mb_vpdcomponent
	//mTriggerIDs.push_back(650007);     // mb_zdccomponent 
}
//_____________________________________________________________________________
StMiniTreeMaker::~StMiniTreeMaker()
{
	// default destructor
}
//_____________________________________________________________________________
Int_t StMiniTreeMaker::Init()
{
	////refMultCorr = CentralityMaker::instance()->getgRefMultCorr_VpdMBnoVtx()
	//refMultCorr = new StRefMultCorr("grefmult_VpdMBnoVtx");

	mEpdGeom = new StEpdGeom();

	mEmcPosition = new StEmcPosition();
	for(Int_t i=0;i<4;i++){
		if(i==1) continue;
		mEmcGeom[i] = StEmcGeom::getEmcGeom(detname[i].Data());
	}

	if(!mOutFileName.Length()){
		LOG_ERROR << "StMiniTreeMaker:: no output file specified for tree and histograms." << endm;
		return kStERR;
	}
	fOutFile = new TFile(mOutFileName.Data(),"recreate");
	LOG_INFO << "StMiniTreeMaker:: create the output file to store the tree and histograms: " << mOutFileName.Data() << endm;

	if(mFillTree)    bookTree();
	if(mFillHisto)   bookHistos();

	return kStOK;
}
//_____________________________________________________________________________
Int_t StMiniTreeMaker::Finish()
{
	if(fOutFile){
		fOutFile->cd();
		fOutFile->Write();
		fOutFile->Close();
		LOG_INFO << "StMiniTreeMaker::Finish() -> write out tree in " << mOutFileName.Data() << endm;
	}

	if(mPrintConfig) printConfig();

	return kStOK;
}
//_____________________________________________________________________________
Int_t StMiniTreeMaker::Make()
{
	memset(&mEvtData, 0, sizeof(mEvtData)); // all the variables in mEvtData structure are preset to be 0

	StTimer timer;
	if(mPrintMemory) StMemoryInfo::instance()->snapshot();
	if(mPrintCpu)    timer.start();

	mMuDstMaker = (StMuDstMaker *)GetMaker("MuDst");
	mPicoDstMaker = (StPicoDstMaker *)GetMaker("picoDst");

	if(mPicoDstMaker){
		if(Debug()) LOG_INFO<<"Use Pico file as input"<<endm;
		mPicoDst = mPicoDstMaker->picoDst();
		if(!mPicoDst){
			LOG_WARN<<"No PicoDst !"<<endm;
			return kStOK;
		}
	}
	else if(mMuDstMaker){
		if(Debug()) LOG_INFO<<"Use MuDst file as input"<<endm;
		mMuDst = mMuDstMaker->muDst();
		if(!mMuDst){
			LOG_WARN<<"No MuDst !"<<endm;
			return kStOK;
		}
	}
	else{
		LOG_WARN<<"No StPicoDstMaker or StMuDstMaker!"<<endm;
		return kStOK;
	}

	if(mPicoDstMaker){
		if(!processPicoEvent())  return kStOK;
	}
	else{
		if(!processMuDstEvent()) return kStOK;
	}

	if(mFillTree) mEvtTree->Fill();

	if(mPrintMemory){
		StMemoryInfo::instance()->snapshot();
		StMemoryInfo::instance()->print();
	}

	if(mPrintCpu){
		timer.stop();
		LOG_INFO << "CPU time for StMiniTreeMaker::Make(): " 
			<< timer.elapsedTime() << "sec " << endm;
	}

	return kStOK;
}
//_____________________________________________________________________________
Bool_t StMiniTreeMaker::processPicoEvent()
{
	if(mFillHisto) hEvent->Fill(0.5);

	StPicoEvent *mPicoEvt = mPicoDst->event();
	if(!mPicoEvt){
		LOG_WARN<<"No event level information !"<<endm;
		return kFALSE;
	}

	Bool_t validTrigger = kFALSE;

	Int_t nTrigs = 0;
	if(mTriggerIDs.size() == 0){
		validTrigger = kTRUE;

		UIntVec triggerIds = mPicoEvt->triggerIds();
		for(size_t i=0; i<triggerIds.size(); i++){
			mEvtData.mTrigId[nTrigs] = triggerIds[i];
			nTrigs++;
		}
	}
	else{
		for(size_t i=0; i<mTriggerIDs.size(); i++){
			if( mPicoEvt->isTrigger(mTriggerIDs[i]) ){
				validTrigger = kTRUE;

				mEvtData.mTrigId[nTrigs] = mTriggerIDs[i];
				nTrigs++;
			}
		}
	}
	mEvtData.mNTrigs = nTrigs;

	if(!validTrigger){
		LOG_WARN<<"No valid interested triggers !"<<endm;
		return kFALSE;
	}

	if(mFillHisto){
		if(validTrigger) hEvent->Fill(2.5);
	}

	mEvtData.mRunId    = mPicoEvt->runId();
	mEvtData.mEventId  = mPicoEvt->eventId();
	mEvtData.mBField   = mPicoEvt->bField();
	mEvtData.mBbcRate  = mPicoEvt->BBCx(); // BBC coincidence rate represents luminosity
	mEvtData.mZdcRate  = mPicoEvt->ZDCx(); // ZDC coincidence rate represents luminosity
	mEvtData.mVpdVz    = mPicoEvt->vzVpd();

	TVector3 vtxPos    = mPicoEvt->primaryVertex();
	mEvtData.mVertexX  = vtxPos.x();
	mEvtData.mVertexY  = vtxPos.y();
	mEvtData.mVertexZ  = vtxPos.z();
	mEvtData.mVertexRanking  = mPicoEvt->ranking();

	if(Debug()){
		LOG_INFO<<"RunId: "<<mEvtData.mRunId<<endm;
		LOG_INFO<<"EventId: "<<mEvtData.mEventId<<endm;
		LOG_INFO<<"mZDCX: "<<mEvtData.mZdcRate<<endm;
		LOG_INFO<<"VPD Vz: "<<mEvtData.mVpdVz<<" \tTPC Vz: "<<mEvtData.mVertexZ<<endm;
	}

	if(TMath::Abs(vtxPos.x())<1.e-5 && TMath::Abs(vtxPos.y())<1.e-5 && TMath::Abs(vtxPos.z())<1.e-5) return kFALSE;
	if(mFillHisto) hEvent->Fill(4.5);
	if(sqrt(vtxPos.x()*vtxPos.x()+vtxPos.y()*vtxPos.y())>=mMaxVtxR) return kFALSE;
	if(mFillHisto) hEvent->Fill(5.5);
	if(TMath::Abs(vtxPos.z())>=mMaxVtxZ) return kFALSE;
	if(mFillHisto) hEvent->Fill(6.5);
	if(TMath::Abs(mEvtData.mVertexZ - mEvtData.mVpdVz)>=mMaxVzDiff) return kFALSE;
	if(mFillHisto) hEvent->Fill(7.5);

	// check refmult definition in https://www.star.bnl.gov/webdata/dox/html/StPicoUtilities_8h_source.html
	// https://www.star.bnl.gov/webdata/dox/html/StMuEvent_8cxx_source.html
	mEvtData.mGRefMult        = mPicoEvt->grefMult();        // gRefMult (|eta|<0.5)
	mEvtData.mRefMult         = mPicoEvt->refMult();         // RefMult (|eta|<0.5) 
	mEvtData.mRefMult2        = mPicoEvt->refMult2();        // RefMult2 (0.5<|eta|<1)
	mEvtData.mRefMult2East    = mPicoEvt->refMult2East();    // RefMult2 (-1<eta<-0.5)
	mEvtData.mRefMult2West    = mPicoEvt->refMult2West();    // RefMult2 (0.5<eta<1)
	mEvtData.mRefMultHalfEast = mPicoEvt->refMultHalfEast(); // RefMult (-1<eta<0)
	mEvtData.mRefMultHalfWest = mPicoEvt->refMultHalfWest(); // RefMult (0<eta<1)
	mEvtData.mNBemcMatch      = mPicoEvt->nBEMCMatch();      // to check pileup effect
	mEvtData.mNBtofMatch      = mPicoEvt->nBTOFMatch();      // to check pileup effect
	mEvtData.mOfflineBtofMult = mPicoEvt->btofTrayMultiplicity();

	if(Debug()){
		LOG_INFO<<"Online TofMult: "<<mEvtData.mOnlineBtofMult<<" \tOffline TofMult: "<<mEvtData.mOfflineBtofMult<<endm;
	}

	// VPD hits
	mEvtData.mNVpdHitsEast = mPicoEvt->nVpdHitsEast();
	mEvtData.mNVpdHitsWest = mPicoEvt->nVpdHitsWest();

	Int_t ie = 0, iw = 0;

	// BBC information
	// Note, all variables are preset to be 0 in memset(&mEvtData, 0, sizeof(mEvtData));
	for(Int_t i=0; i<mNBbcTiles; i++){
		if(mPicoEvt->bbcAdcEast(i) > 0){
			mEvtData.mBbcAdcEast[ie]   = mPicoEvt->bbcAdcEast(i);
			mEvtData.mTotalBbcAdcEast += mPicoEvt->bbcAdcEast(i);

			if(mEvtData.mMaxBbcAdcEast < mPicoEvt->bbcAdcEast(i)){
				mEvtData.mMaxBbcAdcEast = mPicoEvt->bbcAdcEast(i);
			}

			ie++;
		}

		if(mPicoEvt->bbcAdcWest(i) > 0){
			mEvtData.mBbcAdcWest[iw]   = mPicoEvt->bbcAdcWest(i);
			mEvtData.mTotalBbcAdcWest += mPicoEvt->bbcAdcWest(i);

			if(mEvtData.mMaxBbcAdcWest < mPicoEvt->bbcAdcWest(i)){
				mEvtData.mMaxBbcAdcWest = mPicoEvt->bbcAdcWest(i);
			}

			iw++;
		}
	}
	mEvtData.mNBbcHitsEast = ie;
	mEvtData.mNBbcHitsWest = iw;

	mEvtData.mZdcSumAdcEast = mPicoEvt->ZdcSumAdcEast();
	mEvtData.mZdcSumAdcWest = mPicoEvt->ZdcSumAdcWest();
	//mEvtData.mZdcUnAttenuatedEast = mPicoEvt->zdcUnAttenuatedEast();
	//mEvtData.mZdcUnAttenuatedWest = mPicoEvt->zdcUnAttenuatedWest();

	for(Int_t i=0; i<mNZdcStrips; i++){
		mEvtData.mZdcSmdEastHorizontal[i] = mPicoEvt->ZdcSmdEastHorizontal(i);
		mEvtData.mZdcSmdEastVertical[i]   = mPicoEvt->ZdcSmdEastVertical(i);
		mEvtData.mZdcSmdWestHorizontal[i] = mPicoEvt->ZdcSmdWestHorizontal(i);
		mEvtData.mZdcSmdWestVertical[i]   = mPicoEvt->ZdcSmdWestVertical(i);
	}

	//refMultCorr->init(mEvtData.mRunId);
	//refMultCorr->initEvent(mEvtData.mRefMult, mEvtData.mVertexZ, mEvtData.mZdcRate);
	//mEvtData.mRefMultCorr   = refMultCorr->getRefMultCorr(); 
	//mEvtData.mEvtWeight     = refMultCorr->getWeight();
	//mEvtData.mCentrality    = refMultCorr->getCentralityBin16();
	//if(Debug()) LOG_INFO<<"refMult: "<<mEvtData.mRefMult<<" \t refMultCorr: "<<mEvtData.mRefMultCorr<<" \t mCentrality: "<<mEvtData.mCentrality<<endm;

	//if(mFillHisto){
	//	hRefMultVsRefMultCorr->Fill(mEvtData.mRefMultCorr, mEvtData.mRefMult);
	//	hCentrality->Fill(mEvtData.mCentrality);
	//}

	// EPD information
	// check details in https://www.star.bnl.gov/webdata/dox/html/classStPicoEpdHit.html
	// Note, all variables are preset to be 0 in memset(&mEvtData, 0, sizeof(mEvtData));
	
	ie = 0; iw = 0;

	mEvtData.mNEpdHits = mPicoDst->numberOfEpdHits();
	for(UInt_t i=0; i<mPicoDst->numberOfEpdHits(); i++){
		StPicoEpdHit *epdHit = mPicoDst->epdHit(i);
		if(!epdHit) continue;

		mEvtData.mSide[i]     = epdHit->side();
		mEvtData.mPosition[i] = epdHit->position();
		mEvtData.mTile[i]     = epdHit->tile();
		mEvtData.mRow[i]      = epdHit->row();
		mEvtData.mADC[i]      = epdHit->adc();
		mEvtData.mTAC[i]      = epdHit->tac();
		mEvtData.mTDC[i]      = epdHit->tdc();
		mEvtData.mHasTAC[i]   = epdHit->hasTac();
		mEvtData.mNMIP[i]     = epdHit->nMIP();
		mEvtData.mTnMIP[i]    = epdHit->TnMIP(2, 0.3); // you can re-evaluate this variable offline, see this function in https://www.star.bnl.gov/webdata/dox/html/StPicoEpdHit_8h_source.html. NOTE, this function requires STAR library >= SL20d
		mEvtData.mStatusIsGood[i] = epdHit->isGood();

		TVector3 StraightLine = mEpdGeom->TileCenter(epdHit->id()) - vtxPos;
		//TVector3 StraightLine = mEpdGeom->TileCenter(epdHit->position(), epdHit->tile(), epdHit->side()) - vtxPos;
		mEvtData.mEpdEta[i] = StraightLine.Eta();
		mEvtData.mEpdPhi[i] = StraightLine.Phi();

		if(epdHit->side()<0){
			mEvtData.mTotalEpdAdcEast += epdHit->adc();
			if(mEvtData.mMaxEpdAdcEast < epdHit->adc()){
				mEvtData.mMaxEpdAdcEast = epdHit->adc();
			}

			ie++;
		}

		if(epdHit->side()>0){
			mEvtData.mTotalEpdAdcWest += epdHit->adc();
			if(mEvtData.mMaxEpdAdcWest < epdHit->adc()){
				mEvtData.mMaxEpdAdcWest = epdHit->adc();
			}

			iw++;
		}
	}
	mEvtData.mNEpdHitsEast = ie;
	mEvtData.mNEpdHitsWest = iw;

	if(mFillHisto){
		hVPDVzVsTPCVz->Fill(mEvtData.mVertexZ, mEvtData.mVpdVz);
		hVzDiff->Fill(mEvtData.mVertexZ - mEvtData.mVpdVz);
		hGRefMultVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mGRefMult);
		hNEpdHitsVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mNEpdHits);
		hNBtofMatchVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mNBtofMatch);
		hOfflineTofMultVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mOfflineBtofMult);
		hOnlineTofMultVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mOnlineBtofMult);
		hOnlineTofMultVsOfflineTofMult->Fill(mEvtData.mOfflineBtofMult, mEvtData.mOnlineBtofMult);
		hNEpdHitsVsOfflineBtofMult->Fill(mEvtData.mOfflineBtofMult, mEvtData.mNEpdHits);
		hNEpdHitsVsOnlineBtofMult->Fill(mEvtData.mOnlineBtofMult, mEvtData.mNEpdHits);
		hTotalBbcAdcVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mTotalBbcAdcEast + mEvtData.mTotalBbcAdcWest);
		hTotalEpdAdcVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mTotalEpdAdcEast + mEvtData.mTotalEpdAdcWest);
		hBbcAdcWestVsBbcAdcEast->Fill(mEvtData.mTotalBbcAdcEast, mEvtData.mTotalBbcAdcWest);
		hEpdAdcWestVsEpdAdcEast->Fill(mEvtData.mTotalEpdAdcEast, mEvtData.mTotalEpdAdcWest);
		hBbcAdcEastVsEpdAdcEast->Fill(mEvtData.mTotalEpdAdcEast, mEvtData.mTotalBbcAdcEast);
		hBbcAdcWestVsEpdAdcWest->Fill(mEvtData.mTotalEpdAdcWest, mEvtData.mTotalBbcAdcWest);
	}

	if(!mFillTrkInfo)  return kTRUE;

	Int_t nNodes = mPicoDst->numberOfTracks();
	if(Debug()){
		LOG_INFO<<"# of global tracks in picoDst: "<<nNodes<<endm;
	}

	UShort_t nTrks = 0;
	UShort_t nBEMCTrks = 0;
	for(Int_t i=0; i<nNodes; i++){
		StPicoTrack *pTrack = mPicoDst->track(i);
		if(!pTrack) continue;

		if(!isValidTrack(pTrack, vtxPos)) continue;

		mEvtData.mTrkId[nTrks]            = i;  
		mEvtData.mTPCeTrkFlag[nTrks]      = kFALSE;
		mEvtData.mBEMCTraitsIndex[nTrks]  = -999;

		mEvtData.mCharge[nTrks]           = pTrack->charge();

		TVector3 pMom    = pTrack->pMom();
		TVector3 gMom    = pTrack->gMom();
		TVector3 origin  = pTrack->origin();

		mEvtData.mPt[nTrks]        = pMom.Perp();
		mEvtData.mEta[nTrks]       = pMom.PseudoRapidity();
		mEvtData.mPhi[nTrks]       = pMom.Phi();
		mEvtData.mgPt[nTrks]       = gMom.Perp();
		mEvtData.mgEta[nTrks]      = gMom.PseudoRapidity();
		mEvtData.mgPhi[nTrks]      = gMom.Phi();
		mEvtData.mgOriginX[nTrks]  = origin.x();
		mEvtData.mgOriginY[nTrks]  = origin.y();
		mEvtData.mgOriginZ[nTrks]  = origin.z();

		mEvtData.mNHitsFit[nTrks]  = pTrack->nHitsFit();
		mEvtData.mNHitsMax[nTrks]  = pTrack->nHitsMax();
		mEvtData.mNHitsDedx[nTrks] = pTrack->nHitsDedx();
		mEvtData.mDedx[nTrks]      = pTrack->dEdx(); 
		mEvtData.mNSigmaE[nTrks]   = pTrack->nSigmaElectron();
		mEvtData.mDca[nTrks]       = pTrack->gDCA(vtxPos).Mag();
		//mEvtData.mIsHFTTrk[nTrks]      = pTrack->isHFTTrack();
		//mEvtData.mHasHFT4Layers[nTrks] = pTrack->hasHft4Layers();

		if(mFillHisto){
			hChargePt->Fill(mEvtData.mPt[nTrks]*mEvtData.mCharge[nTrks]);
			hdEdxVsP->Fill(pMom.Mag(), mEvtData.mDedx[nTrks]);
			hnSigEVsP->Fill(pMom.Mag(), mEvtData.mNSigmaE[nTrks]);
		}

		Int_t bTofPidTraitsIndex          = pTrack->bTofPidTraitsIndex();
		mEvtData.mTOFMatchFlag[nTrks]     = -1; 
		mEvtData.mTOFLocalY[nTrks]        = -999;
		mEvtData.mBeta2TOF[nTrks]         = -999;
		if(bTofPidTraitsIndex>=0){
			StPicoBTofPidTraits *btofPidTraits = mPicoDst->btofPidTraits(bTofPidTraitsIndex);
			mEvtData.mTOFMatchFlag[nTrks] = btofPidTraits->btofMatchFlag(); 
			mEvtData.mTOFLocalY[nTrks]    = btofPidTraits->btofYLocal();
			mEvtData.mBeta2TOF[nTrks]     = btofPidTraits->btofBeta();

			if(mFillHisto){
				hBetaVsP->Fill(pMom.Mag(), 1./mEvtData.mBeta2TOF[nTrks]);
				if(mEvtData.mBeta2TOF[nTrks]>0 && TMath::Abs(1.-1./mEvtData.mBeta2TOF[nTrks])<=0.025){
					hnSigEVsP_wBetaCut->Fill(pMom.Mag(), mEvtData.mNSigmaE[nTrks]);
				}
			}
		}

		if(
				TMath::Abs(mEvtData.mNSigmaE[nTrks])<=mMaxnSigmaE
				&& mEvtData.mBeta2TOF[nTrks]>0.
				&& TMath::Abs(1.-1./mEvtData.mBeta2TOF[nTrks])<=mMaxBeta2TOF
		  ){
			mEvtData.mTPCeTrkFlag[nTrks] = kTRUE;
		}

		Int_t bemcPidTraitsIndex              = pTrack->bemcPidTraitsIndex();
		if(
				bemcPidTraitsIndex>=0
				//&& mEvtData.mPt[nTrks] > 1.5
				//&& TMath::Abs(mEvtData.mNSigmaE[nTrks])<=mMaxnSigmaE
		  ){
			StPicoBEmcPidTraits *bemcPidTraits = mPicoDst->bemcPidTraits(bemcPidTraitsIndex);
			mEvtData.mBEMCTraitsIndex[nTrks]   = nBEMCTrks;
			mEvtData.mBEMCTrkIndex[nBEMCTrks]  = nTrks;
			mEvtData.mBEMCAdc0[nBEMCTrks]      = bemcPidTraits->bemcAdc0();
			mEvtData.mBEMCE0[nBEMCTrks]        = bemcPidTraits->bemcE0();
			mEvtData.mBEMCE[nBEMCTrks]         = bemcPidTraits->bemcE();
			mEvtData.mBEMCZDist[nBEMCTrks]	   = bemcPidTraits->bemcZDist();
			mEvtData.mBEMCPhiDist[nBEMCTrks]   = bemcPidTraits->bemcPhiDist();
			mEvtData.mBEMCnEta[nBEMCTrks]      = bemcPidTraits->bemcSmdNEta();
			mEvtData.mBEMCnPhi[nBEMCTrks]      = bemcPidTraits->bemcSmdNPhi();
			mEvtData.mNSigmaPi[nBEMCTrks]      = pTrack->nSigmaPion();
			mEvtData.mNSigmaK[nBEMCTrks]       = pTrack->nSigmaKaon();
			mEvtData.mNSigmaP[nBEMCTrks]       = pTrack->nSigmaProton();

			if(Debug()){
				LOG_INFO<<"BEMC associated trkId:"<<pTrack->id()<<endm;
				LOG_INFO<<"BEMC associated trkPt: "<<pTrack->pMom().Perp()<<endm;
				LOG_INFO<<"BEMC associated trkEta: "<<pTrack->pMom().PseudoRapidity()<<endm;
				LOG_INFO<<"BEMC associated trkPhi: "<<pTrack->pMom().Phi()<<endm;
				LOG_INFO<<"BEMC associated trkNHitsFit: "<<pTrack->nHitsFit()<<endm;
				LOG_INFO<<"BEMC associated trkNHitsMax: "<<pTrack->nHitsMax()<<endm;
				LOG_INFO<<"BEMC associated trkNHitsFration: "<<pTrack->nHitsFit()*1./pTrack->nHitsMax()<<endm;
				LOG_INFO<<"BEMC associated trkNHitsDedx: "<<pTrack->nHitsDedx()<<endm;
				LOG_INFO<<"BEMC associated trkDca: "<<pTrack->gDCA(vtxPos).Mag()<<endm;
				LOG_INFO<<"BEMC Adc0: "<<mEvtData.mBEMCAdc0[nBEMCTrks]<<endm;
				LOG_INFO<<"BEMC E0: "<<mEvtData.mBEMCE0[nBEMCTrks]<<endm;
				LOG_INFO<<"BEMC E: "<<mEvtData.mBEMCE[nBEMCTrks]<<endm;
				LOG_INFO<<"BEMC ZDist: "<<mEvtData.mBEMCZDist[nBEMCTrks]<<endm;
				LOG_INFO<<"BEMC PhiDist: "<<mEvtData.mBEMCPhiDist[nBEMCTrks]<<endm;
				LOG_INFO<<"BEMC nEta: "<<(Int_t)mEvtData.mBEMCnEta[nBEMCTrks]<<endm;
				LOG_INFO<<"BEMC nPhi: "<<(Int_t)mEvtData.mBEMCnPhi[nBEMCTrks]<<endm;
			}

			nBEMCTrks++;
		}

		if(
				mEvtData.mTPCeTrkFlag[nTrks] 
				|| mEvtData.mBEMCTraitsIndex[nTrks]>=0
		  ){
			nTrks++;
		}
	}

	//if(nTrks==0 ) return kFALSE;

	mEvtData.mNTrks       = nTrks;
	mEvtData.mNBEMCTrks   = nBEMCTrks;
	if(Debug()){
		LOG_INFO<<"# of stored tracks stored: "<<mEvtData.mNTrks<<endm;
		LOG_INFO<<"# of EMC matched Tracks stored: "<<mEvtData.mNBEMCTrks<<endm;
	}

	return kTRUE;
}
//_____________________________________________________________________________
Bool_t StMiniTreeMaker::processMuDstEvent()
{
	if(mFillHisto) hEvent->Fill(0.5);

	StMuEvent *mMuEvent = mMuDst->event();
	if(!mMuEvent){
		LOG_WARN<<"No event level information !"<<endm;
		return kFALSE;
	}

	Bool_t validTrigger = kFALSE;

	Int_t nTrigs = 0;
	if(mTriggerIDs.size() == 0){
		validTrigger = kTRUE;

		UIntVec triggerIds = mMuEvent->triggerIdCollection().nominal().triggerIds();
		for(size_t i=0; i<triggerIds.size(); i++){
			mEvtData.mTrigId[nTrigs] = triggerIds[i];
			nTrigs++;
		}
	}
	else{
		for(size_t i=0; i<mTriggerIDs.size(); i++){
			if(mMuEvent->triggerIdCollection().nominal().isTrigger(mTriggerIDs[i])){
				validTrigger = kTRUE;

				mEvtData.mTrigId[nTrigs] = mTriggerIDs[i];
				nTrigs++;
			}
		}
	}
	mEvtData.mNTrigs = nTrigs;

	if(!validTrigger){
		LOG_WARN<<"No valid interested triggers !"<<endm;
		return kFALSE;
	}

	if(mFillHisto){
		if(validTrigger) hEvent->Fill(2.5);
	}

	//select the right vertex using VPD
	Float_t vpdVz = -999; 
	StBTofHeader *mBTofHeader = mMuDst->btofHeader();
	if(mBTofHeader) vpdVz = mBTofHeader->vpdVz();
	for(UInt_t i=0; !mDefaultVtx && i<mMuDst->numberOfPrimaryVertices(); i++){
		StMuPrimaryVertex *vtx = mMuDst->primaryVertex(i);
		if(!vtx) continue;
		Float_t vz = vtx->position().z();
		if(fabs(vpdVz)<200 && fabs(vpdVz-vz)<3.){
			mMuDst->setVertexIndex(i); 
			break;
		}
	}

	mEvtData.mRunId          = mMuEvent->runId();
	mEvtData.mEventId        = mMuEvent->eventId();
	mEvtData.mBField         = mMuEvent->runInfo().magneticField();
	mEvtData.mBbcRate        = mMuEvent->runInfo().bbcCoincidenceRate(); // BBC coincidence rate represents luminosity
	mEvtData.mZdcRate        = mMuEvent->runInfo().zdcCoincidenceRate(); // ZDC coincidence rate represents luminosity
	mEvtData.mVpdVz          = vpdVz;

	StThreeVectorF vtxPos    = mMuEvent->primaryVertexPosition();
	mEvtData.mVertexX        = vtxPos.x();
	mEvtData.mVertexY        = vtxPos.y();
	mEvtData.mVertexZ        = vtxPos.z();

	StMuPrimaryVertex *vtx = mMuDst->primaryVertex();
	if(vtx){
		mEvtData.mVertexRanking = vtx->ranking();
		mEvtData.mNBemcMatch    = vtx->nBEMCMatch();  // to check pileup effect
		mEvtData.mNBtofMatch    = vtx->nBTOFMatch();  // to check pileup effect
	}
	else{
		mEvtData.mVertexRanking = -999;
	}

	if(Debug()){
		LOG_INFO<<"RunId: "<<mEvtData.mRunId<<endm;
		LOG_INFO<<"EventId: "<<mEvtData.mEventId<<endm;
		LOG_INFO<<"mZDCX: "<<mEvtData.mZdcRate<<endm;
		LOG_INFO<<"VPD Vz: "<<mEvtData.mVpdVz<<" \tTPC Vz: "<<mEvtData.mVertexZ<<endm;
	}

	if(TMath::Abs(vtxPos.x())<1.e-5 && TMath::Abs(vtxPos.y())<1.e-5 && TMath::Abs(vtxPos.z())<1.e-5) return kFALSE;
	if(mFillHisto) hEvent->Fill(4.5);
	if(sqrt(vtxPos.x()*vtxPos.x()+vtxPos.y()*vtxPos.y())>=mMaxVtxR) return kFALSE;
	if(mFillHisto) hEvent->Fill(5.5);
	if(TMath::Abs(vtxPos.z())>=mMaxVtxZ) return kFALSE;
	if(mFillHisto) hEvent->Fill(6.5);
	if(TMath::Abs(mEvtData.mVertexZ - mEvtData.mVpdVz)>=mMaxVzDiff) return kFALSE;
	if(mFillHisto) hEvent->Fill(7.5);

	// check refmult definition in https://www.star.bnl.gov/webdata/dox/html/StPicoUtilities_8h_source.html
	// https://www.star.bnl.gov/webdata/dox/html/StMuEvent_8cxx_source.html
	mEvtData.mGRefMult        = mMuEvent->grefmult();        // gRefMult (|eta|<0.5)
	mEvtData.mRefMult         = mMuEvent->refMult();         // RefMult (|eta|<0.5) 
	// check how to calculate mRefMult2, mRefMult2East, mRefMult2West, mRefMultHalfEast, and mRefMultHalfWest in StPicoUtilities.h if needed
	mEvtData.mOfflineBtofMult = mMuEvent->btofTrayMultiplicity();
	mEvtData.mOnlineBtofMult  = mMuEvent->triggerData()->tofMultiplicity();

	if(Debug()){
		LOG_INFO<<"Online TofMult: "<<mEvtData.mOnlineBtofMult<<" \tOffline TofMult: "<<mEvtData.mOfflineBtofMult<<endm;
	}

	// VPD hits
	if(mBTofHeader){
		mEvtData.mNVpdHitsEast = mBTofHeader->numberOfVpdHits(east);
		mEvtData.mNVpdHitsWest = mBTofHeader->numberOfVpdHits(west);
	}

	Int_t ie = 0, iw = 0;

	// BBC information
	// Note, all variables are preset to be 0 in memset(&mEvtData, 0, sizeof(mEvtData));
	StBbcTriggerDetector &bbc = mMuEvent->bbcTriggerDetector();
	for(UInt_t iPMT=0; iPMT<bbc.numberOfPMTs(); ++iPMT){
		UInt_t const eastWest = (iPMT<24) ? 0 : 1;  // East: 0-23, West: 24-47

		if(bbc.adc(iPMT) > 0){
			if(eastWest == 0){ //East
				mEvtData.mBbcAdcEast[ie]   = bbc.adc(iPMT);
				mEvtData.mTotalBbcAdcEast += bbc.adc(iPMT);

				if(mEvtData.mMaxBbcAdcEast < bbc.adc(iPMT)){
					mEvtData.mMaxBbcAdcEast = bbc.adc(iPMT);
				}

				ie++;
			}
			else{ //West
				mEvtData.mBbcAdcWest[iw]   = bbc.adc(iPMT);
				mEvtData.mTotalBbcAdcWest += bbc.adc(iPMT);

				if(mEvtData.mMaxBbcAdcWest < bbc.adc(iPMT)){
					mEvtData.mMaxBbcAdcWest = bbc.adc(iPMT);
				}

				iw++;
			}
		}
	}
	mEvtData.mNBbcHitsEast = ie;
	mEvtData.mNBbcHitsWest = iw;

	StZdcTriggerDetector &zdc = mMuEvent->zdcTriggerDetector();
	mEvtData.mZdcSumAdcEast = zdc.adcSum(east);
	mEvtData.mZdcSumAdcWest = zdc.adcSum(west);
	mEvtData.mZdcUnAttenuatedEast = mMuEvent->triggerData()->zdcUnAttenuated(east);
	mEvtData.mZdcUnAttenuatedWest = mMuEvent->triggerData()->zdcUnAttenuated(west);

	//Loop over all zdc strips
	for(int iStrip=1; iStrip<mNZdcStrips+1; ++iStrip) {
		mEvtData.mZdcSmdEastHorizontal[iStrip-1] = zdc.zdcSmd(east, 1, iStrip);
		mEvtData.mZdcSmdEastVertical[iStrip-1]   = zdc.zdcSmd(east, 0, iStrip);
		mEvtData.mZdcSmdWestHorizontal[iStrip-1] = zdc.zdcSmd(west, 1, iStrip);
		mEvtData.mZdcSmdWestVertical[iStrip-1]   = zdc.zdcSmd(west, 0, iStrip);
	} 

	//refMultCorr->init(mEvtData.mRunId);
	//refMultCorr->initEvent(mEvtData.mRefMult, mEvtData.mVertexZ, mEvtData.mZdcRate);
	//mEvtData.mRefMultCorr   = refMultCorr->getRefMultCorr(); 
	//mEvtData.mEvtWeight     = refMultCorr->getWeight();
	//mEvtData.mCentrality    = refMultCorr->getCentralityBin16();
	//if(Debug()) LOG_INFO<<"refMult: "<<mEvtData.mRefMult<<" \t refMultCorr: "<<mEvtData.mRefMultCorr<<" \t mCentrality: "<<mEvtData.mCentrality<<endm;

	//if(mFillHisto){
	//	hRefMultVsRefMultCorr->Fill(mEvtData.mRefMultCorr, mEvtData.mRefMult);
	//	hCentrality->Fill(mEvtData.mCentrality);
	//}

	// EPD information
	// check details in https://www.star.bnl.gov/webdata/dox/html/classStPicoEpdHit.html
	// Note, all variables are preset to be 0 in memset(&mEvtData, 0, sizeof(mEvtData));
	
	ie = 0; iw = 0;

	TVector3 pVtxPos(mEvtData.mVertexX, mEvtData.mVertexY, mEvtData.mVertexZ);

	mEvtData.mNEpdHits = mMuDst->numberOfEpdHit();
	for(UInt_t i=0; i<mMuDst->numberOfEpdHit(); i++){
		StMuEpdHit *epdHit = mMuDst->epdHit(i);
		if(!epdHit) continue;

		mEvtData.mSide[i]     = epdHit->side();
		mEvtData.mPosition[i] = epdHit->position();
		mEvtData.mTile[i]     = epdHit->tile();
		mEvtData.mRow[i]      = epdHit->tile()/2 + 1;
		mEvtData.mADC[i]      = epdHit->adc();
		mEvtData.mTAC[i]      = epdHit->tac();
		mEvtData.mTDC[i]      = epdHit->tdc();
		mEvtData.mHasTAC[i]   = epdHit->hasTac();
		mEvtData.mNMIP[i]     = epdHit->nMIP();
		//mEvtData.mTnMIP[i]    = epdHit->TnMIP(2, 0.3); // you can re-evaluate this variable offline, see this function in https://www.star.bnl.gov/webdata/dox/html/StPicoEpdHit_8h_source.html. NOTE, this function requires STAR library >= SL20d
		if(epdHit->nMIP()<0.3) mEvtData.mTnMIP[i] = 0;
		else                   mEvtData.mTnMIP[i] = epdHit->nMIP()<2 ? epdHit->nMIP() : 2;
		mEvtData.mStatusIsGood[i] = epdHit->isGood();

		TVector3 StraightLine = mEpdGeom->TileCenter(epdHit->id()) - pVtxPos;
		//TVector3 StraightLine = mEpdGeom->TileCenter(epdHit->position(), epdHit->tile(), epdHit->side()) - pVtxPos;
		mEvtData.mEpdEta[i] = StraightLine.Eta();
		mEvtData.mEpdPhi[i] = StraightLine.Phi();

		if(epdHit->side()<0){
			mEvtData.mTotalEpdAdcEast += epdHit->adc();
			if(mEvtData.mMaxEpdAdcEast < epdHit->adc()){
				mEvtData.mMaxEpdAdcEast = epdHit->adc();
			}

			ie++;
		}

		if(epdHit->side()>0){
			mEvtData.mTotalEpdAdcWest += epdHit->adc();
			if(mEvtData.mMaxEpdAdcWest < epdHit->adc()){
				mEvtData.mMaxEpdAdcWest = epdHit->adc();
			}

			iw++;
		}
	}
	mEvtData.mNEpdHitsEast = ie;
	mEvtData.mNEpdHitsWest = iw;

	if(mFillHisto){
		hVPDVzVsTPCVz->Fill(mEvtData.mVertexZ, mEvtData.mVpdVz);
		hVzDiff->Fill(mEvtData.mVertexZ - mEvtData.mVpdVz);
		hGRefMultVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mGRefMult);
		hNEpdHitsVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mNEpdHits);
		hNBtofMatchVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mNBtofMatch);
		hOfflineTofMultVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mOfflineBtofMult);
		hOnlineTofMultVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mOnlineBtofMult);
		hOnlineTofMultVsOfflineTofMult->Fill(mEvtData.mOfflineBtofMult, mEvtData.mOnlineBtofMult);
		hNEpdHitsVsOfflineBtofMult->Fill(mEvtData.mOfflineBtofMult, mEvtData.mNEpdHits);
		hNEpdHitsVsOnlineBtofMult->Fill(mEvtData.mOnlineBtofMult, mEvtData.mNEpdHits);
		hTotalBbcAdcVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mTotalBbcAdcEast + mEvtData.mTotalBbcAdcWest);
		hTotalEpdAdcVsRefMult->Fill(mEvtData.mRefMult, mEvtData.mTotalEpdAdcEast + mEvtData.mTotalEpdAdcWest);
		hBbcAdcWestVsBbcAdcEast->Fill(mEvtData.mTotalBbcAdcEast, mEvtData.mTotalBbcAdcWest);
		hEpdAdcWestVsEpdAdcEast->Fill(mEvtData.mTotalEpdAdcEast, mEvtData.mTotalEpdAdcWest);
		hBbcAdcEastVsEpdAdcEast->Fill(mEvtData.mTotalEpdAdcEast, mEvtData.mTotalBbcAdcEast);
		hBbcAdcWestVsEpdAdcWest->Fill(mEvtData.mTotalEpdAdcWest, mEvtData.mTotalBbcAdcWest);
	}

	if(!mFillTrkInfo)  return kTRUE;

	Int_t nNodes = mMuDst->numberOfPrimaryTracks();
	if(Debug()){
		LOG_INFO<<"# of primary Tracks in muDst: "<<nNodes<<endm;
	}

	Short_t nTrks    = 0;
	Short_t nBEMCTrks = 0;
	for(Int_t i=0; i<nNodes; i++){
		StMuTrack* pMuTrack = mMuDst->primaryTracks(i);
		if(!pMuTrack) continue;

		StMuTrack* gMuTrack = (StMuTrack *)pMuTrack->globalTrack();
		if(!gMuTrack) continue;

		if(!isValidTrack(pMuTrack)) continue;

		mEvtData.mTrkId[nTrks]            = i;  
		mEvtData.mTPCeTrkFlag[nTrks]      = kFALSE;
		mEvtData.mBEMCTraitsIndex[nTrks]  = -999;

		mEvtData.mCharge[nTrks]           = pMuTrack->charge();

		// Calculate global momentum and position at point of DCA to the primaryVertex
		StThreeVectorF pMom               = pMuTrack->p();
		StPhysicalHelixD gHelix           = gMuTrack->helix(); // Return inner helix (first measured point)
		gHelix.moveOrigin(gHelix.pathLength(vtxPos));
		StThreeVectorF gMom               = gHelix.momentum(mEvtData.mBField*kilogauss);
		StThreeVectorF origin             = gHelix.origin();

		mEvtData.mPt[nTrks]               = pMom.perp();
		mEvtData.mEta[nTrks]              = pMom.pseudoRapidity();
		mEvtData.mPhi[nTrks]              = pMom.phi();
		mEvtData.mgPt[nTrks]              = gMom.perp();
		mEvtData.mgEta[nTrks]             = gMom.pseudoRapidity();
		mEvtData.mgPhi[nTrks]             = gMom.phi();
		mEvtData.mgOriginX[nTrks]         = origin.x();
		mEvtData.mgOriginY[nTrks]         = origin.y();
		mEvtData.mgOriginZ[nTrks]         = origin.z();

		mEvtData.mNHitsFit[nTrks]         = pMuTrack->nHitsFit(kTpcId);
		mEvtData.mNHitsMax[nTrks]         = pMuTrack->nHitsPoss(kTpcId);
		mEvtData.mNHitsDedx[nTrks]        = pMuTrack->nHitsDedx();
		mEvtData.mDedx[nTrks]             = pMuTrack->dEdx()*1.e6; 
		mEvtData.mNSigmaE[nTrks]          = pMuTrack->nSigmaElectron();
		mEvtData.mDca[nTrks]              = pMuTrack->dcaGlobal().mag();

		//UInt_t  mMap0                     = (UInt_t)(gMuTrack->topologyMap().data(0));
		//UChar_t mHftHitsMap               = mMap0>>1 & 0x7F;
		//Bool_t  mHasPxl1Hit               = mHftHitsMap>>0 & 0x1;
		//Bool_t  mHasPxl2Hit               = mHftHitsMap>>1 & 0x3;
		//Bool_t  mHasIstHit                = mHftHitsMap>>3 & 0x3;
		//Bool_t  mHasSstHit                = mHftHitsMap>>5 & 0x3;
		//mEvtData.mIsHFTTrk[nTrks]         = mHasPxl1Hit && mHasPxl2Hit && (mHasIstHit || mHasSstHit);
		//mEvtData.mHasHFT4Layers[nTrks]    = mHasPxl1Hit && mHasPxl2Hit && mHasIstHit && mHasSstHit;

		if(mFillHisto){
			hChargePt->Fill(mEvtData.mPt[nTrks]*mEvtData.mCharge[nTrks]);
			hdEdxVsP->Fill(pMom.mag(), mEvtData.mDedx[nTrks]);
			hnSigEVsP->Fill(pMom.mag(), mEvtData.mNSigmaE[nTrks]);
		}

		mEvtData.mTOFMatchFlag[nTrks] = -1;
		mEvtData.mTOFLocalY[nTrks] = -999.;
		mEvtData.mBeta2TOF[nTrks] = -999.;
		if( &(pMuTrack->btofPidTraits()) ){
			const StMuBTofPidTraits& btofPidTraits = pMuTrack->btofPidTraits();
			mEvtData.mTOFMatchFlag[nTrks] = btofPidTraits.matchFlag(); 
			mEvtData.mTOFLocalY[nTrks] = btofPidTraits.yLocal();
			mEvtData.mBeta2TOF[nTrks] = btofPidTraits.beta();
			if(mFillHisto){
				hBetaVsP->Fill(pMom.mag(), 1./mEvtData.mBeta2TOF[nTrks]);
				if(mEvtData.mBeta2TOF[nTrks]>0 && TMath::Abs(1.-1./mEvtData.mBeta2TOF[nTrks])<=0.025){
					hnSigEVsP_wBetaCut->Fill(pMom.mag(), mEvtData.mNSigmaE[nTrks]);
				}
			}
		}

		if(
				TMath::Abs(mEvtData.mNSigmaE[nTrks])<=mMaxnSigmaE
				&& mEvtData.mBeta2TOF[nTrks]>0.
				&& TMath::Abs(1.-1./mEvtData.mBeta2TOF[nTrks])<=mMaxBeta2TOF
		  ){
			mEvtData.mTPCeTrkFlag[nTrks] = kTRUE;
		}

		//if(
		//		mEvtData.mPt[nTrks]>1.5
		//		&& TMath::Abs(mEvtData.mNSigmaE[nTrks])<=mMaxnSigmaE
		//  ){
		//	getBemcInfo(pMuTrack,nTrks,nBEMCTrks,kFALSE);
		//}
		getBemcInfo(pMuTrack,nTrks,nBEMCTrks,kFALSE);

		if(
				mEvtData.mTPCeTrkFlag[nTrks] 
				|| mEvtData.mBEMCTraitsIndex[nTrks]>=0
		  ){
			nTrks++;
		}
	}

	//if(nTrks==0 ) return kFALSE;

	mEvtData.mNTrks         = nTrks;
	mEvtData.mNBEMCTrks     = nBEMCTrks;
	if(Debug()){
		LOG_INFO<<"# of primary tracks stored: "<<mEvtData.mNTrks<<endm;
		LOG_INFO<<"# of EMC matched Tracks stored: "<<mEvtData.mNBEMCTrks<<endm;
	}

	return kTRUE;
}
//_____________________________________________________________________________
Bool_t StMiniTreeMaker::isValidTrack(StPicoTrack *pTrack, TVector3 vtxPos) const
{
	Float_t pt  = pTrack->pMom().Perp();
	Float_t eta = pTrack->pMom().PseudoRapidity();
	Float_t dca = pTrack->gDCA(vtxPos).Mag();

	if(pt<mMinTrkPt)                            return kFALSE;
	if(TMath::Abs(eta)>mMaxTrkEta)              return kFALSE;
	if(pTrack->nHitsFit()<mMinNHitsFit)         return kFALSE;
	if(pTrack->nHitsFit()*1./pTrack->nHitsMax()<mMinNHitsFitRatio) return kFALSE;
	if(pTrack->nHitsDedx()<mMinNHitsDedx)       return kFALSE;
	if(dca>mMaxDca)                             return kFALSE;

	return kTRUE;
}
//_____________________________________________________________________________
Bool_t StMiniTreeMaker::isValidTrack(StMuTrack *pMuTrack) const
{
	Float_t pt  = pMuTrack->pt();
	Float_t eta = pMuTrack->eta(); 
	Float_t dca = pMuTrack->dcaGlobal().mag();

	if(pt<mMinTrkPt)                            return kFALSE;
	if(TMath::Abs(eta)>mMaxTrkEta)              return kFALSE;
	if(pMuTrack->nHitsFit(kTpcId)<mMinNHitsFit) return kFALSE;
	if(pMuTrack->nHitsFit(kTpcId)*1./pMuTrack->nHitsPoss(kTpcId)<mMinNHitsFitRatio)  return kFALSE;
	if(pMuTrack->nHitsDedx()<mMinNHitsDedx)     return kFALSE;
	if(dca>mMaxDca)                             return kFALSE;

	return kTRUE;
}
//_____________________________________________________________________________
Bool_t StMiniTreeMaker::getBemcInfo(StMuTrack *pMuTrack, const Short_t nTrks, Short_t &nBEMCTrks, Bool_t flag)
{

	Float_t maxtowerE = -999., energy = 0.;
	Float_t zdist = -999., phidist = -999., mindist = 999.;
	Int_t mod = -1, eta=-1, sub=-1;
	Int_t neta = -1, nphi=-1;
	UInt_t maxadc = 0;

	mEmcCollection = mMuDst->emcCollection();
	if(!mEmcCollection) {
		LOG_WARN << " No Emc Collection for this event " << endm;
		return kFALSE;
	}

	StThreeVectorD position, momentum;
	StThreeVectorD positionBSMDE, momentumBSMDE;
	StThreeVectorD positionBSMDP, momentumBSMDP;

	Double_t bField = mMuDst->event()->runInfo().magneticField()/10.; //Tesla
	Bool_t ok       = kFALSE;
	Bool_t okBSMDE  = kFALSE;
	Bool_t okBSMDP  = kFALSE;
	if(mEmcPosition) {
		ok      = mEmcPosition->projTrack(&position, &momentum, pMuTrack, bField, mEmcGeom[0]->Radius());
		okBSMDE = mEmcPosition->projTrack(&positionBSMDE, &momentumBSMDE, pMuTrack, bField, mEmcGeom[2]->Radius());
		okBSMDP = mEmcPosition->projTrack(&positionBSMDP, &momentumBSMDP, pMuTrack, bField, mEmcGeom[3]->Radius());
	}
	if(!ok) {
		LOG_WARN << " Projection failed for this track ... " << endm;
		return kFALSE;
	}

	Bool_t bemcMatchFlag = kFALSE;
	if(ok && okBSMDE && okBSMDP){

		StSPtrVecEmcPoint& bEmcPoints = mEmcCollection->barrelPoints();
		mindist=1.e9;
		mEmcGeom[0]->getBin(positionBSMDP.phi(), positionBSMDE.pseudoRapidity(), mod, eta, sub); //project on SMD plan
		for(StSPtrVecEmcPointIterator it = bEmcPoints.begin(); it != bEmcPoints.end(); it++) {
			Bool_t associatedPoint = kFALSE;
			StPtrVecEmcCluster& bEmcClusters = (*it)->cluster(kBarrelEmcTowerId);
			if(bEmcClusters.size()==0 ) continue;
			if(bEmcClusters[0]==NULL) continue;
			for(StPtrVecEmcClusterIterator cIter = bEmcClusters.begin(); cIter != bEmcClusters.end(); cIter++){
				Bool_t associatedCluster = kFALSE;
				StPtrVecEmcRawHit& bEmcHits = (*cIter)->hit();
				for(StPtrVecEmcRawHitIterator hIter = bEmcHits.begin(); hIter != bEmcHits.end(); hIter++) {
					if(mod == (Int_t)(*hIter)->module() && eta == (Int_t)(*hIter)->eta() && sub == (Int_t)(*hIter)->sub()) {
						bemcMatchFlag = kTRUE;
						associatedPoint = kTRUE;
						associatedCluster = kTRUE;
						break;
					}
				}
				if(associatedCluster) {
					for(StPtrVecEmcRawHitIterator hitit=bEmcHits.begin(); hitit!=bEmcHits.end();hitit++) {
						if((*hitit)->energy()>maxtowerE) maxtowerE = (*hitit)->energy();
						if((*hitit)->adc()>maxadc) maxadc = (*hitit)->adc();
					}
				}
			}

			StPtrVecEmcCluster& smdeClusters = (*it)->cluster(kBarrelSmdEtaStripId);
			StPtrVecEmcCluster& smdpClusters = (*it)->cluster(kBarrelSmdPhiStripId);

			if(associatedPoint) {
				energy += (*it)->energy(); //use point's energy, not tower cluster's energy

				float deltaphi=(*it)->position().phi()-positionBSMDP.phi();
				if(deltaphi>=TMath::Pi()) deltaphi=deltaphi-TMath::TwoPi();
				if(deltaphi<-TMath::Pi()) deltaphi=deltaphi+TMath::TwoPi();

				float rsmdp=mEmcGeom[3]->Radius();
				float pointz=(*it)->position().z();
				float deltaz=pointz-positionBSMDE.z();
				if(sqrt(deltaphi*deltaphi*rsmdp*rsmdp+deltaz*deltaz)<mindist) {
					phidist=deltaphi;
					zdist  =deltaz;
					if(smdeClusters.size()>=1) neta=smdeClusters[0]->nHits();
					if(smdpClusters.size()>=1) nphi=smdpClusters[0]->nHits();
					mindist=sqrt(deltaphi*deltaphi*rsmdp*rsmdp+deltaz*deltaz);
				}
			}//associated
		}
	} // end if (ok && okBSMDE && okBSMDP)

	if(flag) return bemcMatchFlag;

	if(bemcMatchFlag && !flag){
		mEvtData.mBEMCTraitsIndex[nTrks]  = nBEMCTrks;
		mEvtData.mBEMCTrkIndex[nBEMCTrks] = nTrks;
		mEvtData.mBEMCAdc0[nBEMCTrks]     = maxadc;
		mEvtData.mBEMCE0[nBEMCTrks]       = maxtowerE;
		mEvtData.mBEMCE[nBEMCTrks]        = energy;
		mEvtData.mBEMCZDist[nBEMCTrks]    = zdist;	
		mEvtData.mBEMCPhiDist[nBEMCTrks]  = phidist;
		mEvtData.mBEMCnEta[nBEMCTrks]     = neta;
		mEvtData.mBEMCnPhi[nBEMCTrks]     = nphi;
		mEvtData.mNSigmaPi[nBEMCTrks]     = pMuTrack->nSigmaPion();
		mEvtData.mNSigmaK[nBEMCTrks]      = pMuTrack->nSigmaKaon();
		mEvtData.mNSigmaP[nBEMCTrks]      = pMuTrack->nSigmaProton();

		if(Debug()){
			LOG_INFO<<"BEMC associated trkId: "<<pMuTrack->id()<<endm;
			LOG_INFO<<"BEMC associated trkPt: "<<pMuTrack->pt()<<endm;
			LOG_INFO<<"BEMC associated trkEta: "<<pMuTrack->eta()<<endm;
			LOG_INFO<<"BEMC associated trkPhi: "<<pMuTrack->phi()<<endm;
			LOG_INFO<<"BEMC associated trkNHitsFit: "<<pMuTrack->nHitsFit(kTpcId)<<endm;
			LOG_INFO<<"BEMC associated trkNHitsPoss: "<<pMuTrack->nHitsPoss(kTpcId)<<endm;
			LOG_INFO<<"BEMC associated trkNHitsFration: "<<pMuTrack->nHitsFit(kTpcId)*1./pMuTrack->nHitsPoss(kTpcId)<<endm;
			LOG_INFO<<"BEMC associated trkNHitsDedx: "<<pMuTrack->nHitsDedx()<<endm;
			LOG_INFO<<"BEMC associated trkDca: "<<pMuTrack->dcaGlobal().mag()<<endm;
			LOG_INFO<<"BEMC Adc0: "<<mEvtData.mBEMCAdc0[nBEMCTrks]<<endm;
			LOG_INFO<<"BEMC E0: "<<mEvtData.mBEMCE0[nBEMCTrks]<<endm;
			LOG_INFO<<"BEMC E: "<<mEvtData.mBEMCE[nBEMCTrks]<<endm;
			LOG_INFO<<"BEMC ZDist: "<<mEvtData.mBEMCZDist[nBEMCTrks]<<endm;
			LOG_INFO<<"BEMC PhiDist: "<<mEvtData.mBEMCPhiDist[nBEMCTrks]<<endm;
			LOG_INFO<<"BEMC nEta: "<<(Int_t)mEvtData.mBEMCnEta[nBEMCTrks]<<endm;
			LOG_INFO<<"BEMC nPhi: "<<(Int_t)mEvtData.mBEMCnPhi[nBEMCTrks]<<endm;
		}

		nBEMCTrks++;
	};

	return bemcMatchFlag;
}
//_____________________________________________________________________________
void StMiniTreeMaker::bookTree()
{
	LOG_INFO << "StMiniTreeMaker:: book the event tree to be filled." << endm;

	mEvtTree = new TTree("miniDst","miniDst");
	mEvtTree->SetAutoSave(10000000); // 10 MB

	// event information
	mEvtTree->Branch("mRunId", &mEvtData.mRunId, "mRunId/I");
	mEvtTree->Branch("mEventId", &mEvtData.mEventId, "mEventId/I");
	mEvtTree->Branch("mNTrigs", &mEvtData.mNTrigs, "mNTrigs/S");
	mEvtTree->Branch("mTrigId", mEvtData.mTrigId, "mTrigId[mNTrigs]/I");
	mEvtTree->Branch("mBField", &mEvtData.mBField, "mBField/F");
	mEvtTree->Branch("mBbcRate", &mEvtData.mBbcRate, "mBbcRate/F");
	mEvtTree->Branch("mZdcRate", &mEvtData.mZdcRate, "mZdcRate/F");
	mEvtTree->Branch("mVpdVz", &mEvtData.mVpdVz, "mVpdVz/F");
	mEvtTree->Branch("mVertexX", &mEvtData.mVertexX, "mVertexX/F");
	mEvtTree->Branch("mVertexY", &mEvtData.mVertexY, "mVertexY/F");
	mEvtTree->Branch("mVertexZ", &mEvtData.mVertexZ, "mVertexZ/F");
	mEvtTree->Branch("mVertexRanking", &mEvtData.mVertexRanking, "mVertexRanking/F");

	mEvtTree->Branch("mGRefMult", &mEvtData.mGRefMult, "mGRefMult/I");
	mEvtTree->Branch("mRefMult", &mEvtData.mRefMult, "mRefMult/I");
	mEvtTree->Branch("mRefMult2", &mEvtData.mRefMult2, "mRefMult2/I");
	mEvtTree->Branch("mRefMult2East", &mEvtData.mRefMult2East, "mRefMult2East/I");
	mEvtTree->Branch("mRefMult2West", &mEvtData.mRefMult2West, "mRefMult2West/I");
	mEvtTree->Branch("mRefMultHalfEast", &mEvtData.mRefMultHalfEast, "mRefMultHalfEast/I");
	mEvtTree->Branch("mRefMultHalfWest", &mEvtData.mRefMultHalfWest, "mRefMultHalfWest/I");
	mEvtTree->Branch("mNBemcMatch", &mEvtData.mNBemcMatch, "mNBemcMatch/s");
	mEvtTree->Branch("mNBtofMatch", &mEvtData.mNBtofMatch, "mNBtofMatch/s");
	mEvtTree->Branch("mOnlineBtofMult", &mEvtData.mOnlineBtofMult, "mOnlineBtofMult/s");
	mEvtTree->Branch("mOfflineBtofMult", &mEvtData.mOfflineBtofMult, "mOfflineBtofMult/s");
	mEvtTree->Branch("mRefMultCorr", &mEvtData.mRefMultCorr, "mRefMultCorr/F");
	mEvtTree->Branch("mEvtWeight", &mEvtData.mEvtWeight, "mEvtWeight/F");
	mEvtTree->Branch("mCentrality", &mEvtData.mCentrality, "mCentrality/S");

	mEvtTree->Branch("mNVpdHitsEast", &mEvtData.mNVpdHitsEast, "mNVpdHitsEast/s");
	mEvtTree->Branch("mNVpdHitsWest", &mEvtData.mNVpdHitsWest, "mNVpdHitsWest/s");

	// BBC ADC information
	mEvtTree->Branch("mNBbcHitsEast", &mEvtData.mNBbcHitsEast, "mNBbcHitsEast/s");
	mEvtTree->Branch("mNBbcHitsWest", &mEvtData.mNBbcHitsWest, "mNBbcHitsWest/s");
	mEvtTree->Branch("mBbcAdcEast", mEvtData.mBbcAdcEast, "mBbcAdcEast[mNBbcHitsEast]/s");
	mEvtTree->Branch("mBbcAdcWest", mEvtData.mBbcAdcWest, "mBbcAdcWest[mNBbcHitsWest]/s");
	mEvtTree->Branch("mMaxBbcAdcEast", &mEvtData.mMaxBbcAdcEast, "mMaxBbcAdcEast/s");
	mEvtTree->Branch("mMaxBbcAdcWest", &mEvtData.mMaxBbcAdcWest, "mMaxBbcAdcWest/s");
	mEvtTree->Branch("mTotalBbcAdcEast", &mEvtData.mTotalBbcAdcEast, "mTotalBbcAdcEast/i");
	mEvtTree->Branch("mTotalBbcAdcWest", &mEvtData.mTotalBbcAdcWest, "mTotalBbcAdcWest/i");

	// ZDC ADC information
	mEvtTree->Branch("mZdcSumAdcEast", &mEvtData.mZdcSumAdcEast, "mZdcSumAdcEast/F");
	mEvtTree->Branch("mZdcSumAdcWest", &mEvtData.mZdcSumAdcWest, "mZdcSumAdcWest/F");
	mEvtTree->Branch("mZdcUnAttenuatedEast", &mEvtData.mZdcUnAttenuatedEast, "mZdcUnAttenuatedEast/F");
	mEvtTree->Branch("mZdcUnAttenuatedWest", &mEvtData.mZdcUnAttenuatedWest, "mZdcUnAttenuatedWest/F");
	mEvtTree->Branch("mZdcSmdEastHorizontal", mEvtData.mZdcSmdEastHorizontal, "mZdcSmdEastHorizontal[8]/F");
	mEvtTree->Branch("mZdcSmdEastVertical", mEvtData.mZdcSmdEastVertical, "mZdcSmdEastVertical[8]/F");
	mEvtTree->Branch("mZdcSmdWestHorizontal", mEvtData.mZdcSmdWestHorizontal, "mZdcSmdWestHorizontal[8]/F");
	mEvtTree->Branch("mZdcSmdWestVertical", mEvtData.mZdcSmdWestVertical, "mZdcSmdWestVertical[8]/F");

	// EPD ADC information
	mEvtTree->Branch("mNEpdHits", &mEvtData.mNEpdHits, "mNEpdHits/s");
	mEvtTree->Branch("mNEpdHitsEast", &mEvtData.mNEpdHitsEast, "mNEpdHitsEast/s");
	mEvtTree->Branch("mNEpdHitsWest", &mEvtData.mNEpdHitsWest, "mNEpdHitsWest/s");
	mEvtTree->Branch("mSide", mEvtData.mSide, "mSide[mNEpdHits]/S");
	mEvtTree->Branch("mPosition", mEvtData.mPosition, "mPosition[mNEpdHits]/S");
	mEvtTree->Branch("mTile", mEvtData.mTile, "mTile[mNEpdHits]/S");
	mEvtTree->Branch("mRow", mEvtData.mRow, "mRow[mNEpdHits]/S");
	mEvtTree->Branch("mEpdEta", mEvtData.mEpdEta, "mEpdEta[mNEpdHits]/F");
	mEvtTree->Branch("mEpdPhi", mEvtData.mEpdPhi, "mEpdPhi[mNEpdHits]/F");
	mEvtTree->Branch("mADC", mEvtData.mADC, "mADC[mNEpdHits]/S");
	mEvtTree->Branch("mTAC", mEvtData.mTAC, "mTAC[mNEpdHits]/S");
	mEvtTree->Branch("mTDC", mEvtData.mTDC, "mTDC[mNEpdHits]/S");
	mEvtTree->Branch("mHasTAC", mEvtData.mHasTAC, "mHasTAC[mNEpdHits]/O");
	mEvtTree->Branch("mNMIP", mEvtData.mNMIP, "mNMIP[mNEpdHits]/F");
	mEvtTree->Branch("mTnMIP", mEvtData.mTnMIP, "mTnMIP[mNEpdHits]/F");
	mEvtTree->Branch("mStatusIsGood", mEvtData.mStatusIsGood, "mStatusIsGood[mNEpdHits]/O");
	mEvtTree->Branch("mMaxEpdAdcEast", &mEvtData.mMaxEpdAdcEast, "mMaxEpdAdcEast/S");
	mEvtTree->Branch("mMaxEpdAdcWest", &mEvtData.mMaxEpdAdcWest, "mMaxEpdAdcWest/S");
	mEvtTree->Branch("mTotalEpdAdcEast", &mEvtData.mTotalEpdAdcEast, "mTotalEpdAdcEast/i");
	mEvtTree->Branch("mTotalEpdAdcWest", &mEvtData.mTotalEpdAdcWest, "mTotalEpdAdcWest/i");

	//all tracks information
	if(mFillTrkInfo){
		mEvtTree->Branch("mNTrks", &mEvtData.mNTrks, "mNTrks/s");
		mEvtTree->Branch("mTrkId", mEvtData.mTrkId, "mTrkId[mNTrks]/S");
		mEvtTree->Branch("mTPCeTrkFlag", mEvtData.mTPCeTrkFlag, "mTPCeTrkFlag[mNTrks]/O");
		mEvtTree->Branch("mBEMCTraitsIndex", mEvtData.mBEMCTraitsIndex,"mBEMCTraitsIndex[mNTrks]/S");
		mEvtTree->Branch("mCharge", mEvtData.mCharge, "mCharge[mNTrks]/S");
		mEvtTree->Branch("mPt", mEvtData.mPt, "mPt[mNTrks]/F");
		mEvtTree->Branch("mEta", mEvtData.mEta, "mEta[mNTrks]/F");
		mEvtTree->Branch("mPhi", mEvtData.mPhi, "mPhi[mNTrks]/F");
		//mEvtTree->Branch("mgPt", mEvtData.mgPt, "mgPt[mNTrks]/F");
		//mEvtTree->Branch("mgEta", mEvtData.mgEta, "mgEta[mNTrks]/F");
		//mEvtTree->Branch("mgPhi", mEvtData.mgPhi, "mgPhi[mNTrks]/F");
		//mEvtTree->Branch("mgOriginX", mEvtData.mgOriginX, "mgOriginX[mNTrks]/F");
		//mEvtTree->Branch("mgOriginY", mEvtData.mgOriginY, "mgOriginY[mNTrks]/F");
		//mEvtTree->Branch("mgOriginZ", mEvtData.mgOriginZ, "mgOriginZ[mNTrks]/F");
		mEvtTree->Branch("mNHitsFit", mEvtData.mNHitsFit, "mNHitsFit[mNTrks]/S");
		mEvtTree->Branch("mNHitsMax", mEvtData.mNHitsMax, "mNHitsMax[mNTrks]/S");
		mEvtTree->Branch("mNHitsDedx", mEvtData.mNHitsDedx, "mNHitsDedx[mNTrks]/S");
		mEvtTree->Branch("mDedx", mEvtData.mDedx, "mDedx[mNTrks]/F");
		mEvtTree->Branch("mNSigmaE", mEvtData.mNSigmaE, "mNSigmaE[mNTrks]/F");
		mEvtTree->Branch("mDca", mEvtData.mDca, "mDca[mNTrks]/F");
		//mEvtTree->Branch("mIsHFTTrk", mEvtData.mIsHFTTrk, "mIsHFTTrk[mNTrks]/O)");
		//mEvtTree->Branch("mHasHFT4Layers", mEvtData.mHasHFT4Layers, "mHasHFT4Layers[mNTrks]/O");
		mEvtTree->Branch("mTOFMatchFlag", mEvtData.mTOFMatchFlag, "mTOFMatchFlag[mNTrks]/S");
		mEvtTree->Branch("mTOFLocalY", mEvtData.mTOFLocalY, "mTOFLocalY[mNTrks]/F");
		mEvtTree->Branch("mBeta2TOF", mEvtData.mBeta2TOF, "mBeta2TOF[mNTrks]/F");

		//BEMC pidTrait information
		mEvtTree->Branch("mNBEMCTrks", &mEvtData.mNBEMCTrks, "mNBEMCTrks/s");
		mEvtTree->Branch("mBEMCTrkIndex", mEvtData.mBEMCTrkIndex, "mBEMCTrkIndex[mNBEMCTrks]/S");
		mEvtTree->Branch("mBEMCAdc0", mEvtData.mBEMCAdc0, "mBEMCAdc0[mNBEMCTrks]/S");
		mEvtTree->Branch("mBEMCE0", mEvtData.mBEMCE0, "mBEMCE0[mNBEMCTrks]/F");
		mEvtTree->Branch("mBEMCE", mEvtData.mBEMCE, "mBEMCE[mNBEMCTrks]/F");
		mEvtTree->Branch("mBEMCZDist", mEvtData.mBEMCZDist, "mBEMCZDist[mNBEMCTrks]/F");
		mEvtTree->Branch("mBEMCPhiDist", mEvtData.mBEMCPhiDist, "mBEMCPhiDist[mNBEMCTrks]/F");
		mEvtTree->Branch("mBEMCnEta", mEvtData.mBEMCnEta, "mBEMCnEta[mNBEMCTrks]/S");
		mEvtTree->Branch("mBEMCnPhi", mEvtData.mBEMCnPhi, "mBEMCnPhi[mNBEMCTrks]/S");
		mEvtTree->Branch("mNSigmaPi", mEvtData.mNSigmaPi, "mNSigmaPi[mNBEMCTrks]/F");
		mEvtTree->Branch("mNSigmaK", mEvtData.mNSigmaK, "mNSigmaK[mNBEMCTrks]/F");
		mEvtTree->Branch("mNSigmaP", mEvtData.mNSigmaP, "mNSigmaP[mNBEMCTrks]/F");
	}

	//AddObj(mEvtTree,".hist");
}
//_____________________________________________________________________________
void StMiniTreeMaker::bookHistos()
{
	hEvent = new TH1D("hEvent","Event statistics",25,0,25);
	//hEvent->GetXaxis()->SetBinLabel(1, "All events");
	//hEvent->GetXaxis()->SetBinLabel(3, "Selelcted Trigger(s)");
	//hEvent->GetXaxis()->SetBinLabel(5, "None-Zero Vertex");
	//hEvent->GetXaxis()->SetBinLabel(6, Form("|V_{r}|<%1.2f cm",mMaxVtxR));
	//hEvent->GetXaxis()->SetBinLabel(7, Form("|V_{z}|<%1.2f cm",mMaxVtxZ));
	//hEvent->GetXaxis()->SetBinLabel(8, Form("|V_{z}Diff|<%1.2f cm",mMaxVzDiff));

	hVPDVzVsTPCVz = new TH2D("hVPDVzVsTPCVz","hVPDVzVsTPCVz; TPC V_{z} (cm); VPD V_{z} (cm)",200,-50,50,200,-50,50);
	hVzDiff = new TH1D("hVzDiff","hVzDiff; TPC V_{z} - VPD V_{z} (cm)",80,-20,20);
	hGRefMultVsRefMult      = new TH2D("hGRefMultVsRefMult","hGRefMultVsRefMult; refMult; grefMult",1000,0,1000,1000,0,1000);
	hNEpdHitsVsRefMult      = new TH2D("hNEpdHitsVsRefMult","hNEpdHitsVsRefMult; refMult; nEpdHits",1000,0,1000,1000,0,1000);
	hNBtofMatchVsRefMult    = new TH2D("hNBtofMatchVsRefMult","hNBtofMatchVsRefMult; refMult; nBtofMatch",1000,0,1000,1000,0,1000);
	hOnlineTofMultVsRefMult  = new TH2D("hOnlineTofMultVsRefMult","hOnlineTofMultVsRefMult; refMult; tofMult^{online}",1000,0,1000,1000,0,1000);
	hOfflineTofMultVsRefMult = new TH2D("hOfflineTofMultVsRefMult","hOfflineTofMultVsRefMult; refMult; tofMult^{offline}",1000,0,1000,1000,0,1000);
	hOnlineTofMultVsOfflineTofMult = new TH2D("hOnlineTofMultVsOfflineTofMult","hOnlineTofMultVsOfflineTofMult; tofMult^{offline}; tofMult^{online}",1000,0,1000,1000,0,1000);
	hNEpdHitsVsOfflineBtofMult     = new TH2D("hNEpdHitsVsOfflineBtofMult","hNEpdHitsVsOfflineBtofMult; btofMult^{offline}; nEpdHits",1000,0,1000,1000,0,1000);
	hNEpdHitsVsOnlineBtofMult     = new TH2D("hNEpdHitsVsOnlineBtofMult","hNEpdHitsVsOnlineBtofMult; btofMult^{online}; nEpdHits",1000,0,1000,1000,0,1000);
	hTotalBbcAdcVsRefMult   = new TH2D("hTotalBbcAdcVsRefMult","hTotalBbcAdcVsRefMult; refMult; Total BBC ADC",1000,0,1000,1000,0,100000);
	hTotalEpdAdcVsRefMult   = new TH2D("hTotalEpdAdcVsRefMult","hTotalEpdAdcVsRefMult; refMult; Total EPD ADC",1000,0,1000,1000,0,100000);
	hBbcAdcWestVsBbcAdcEast = new TH2D("hBbcAdcWestVsBbcAdcEast","hBbcAdcWestVsBbcAdcEast; East Total BBC ADC; West Total BBC ADC",1000,0,50000,1000,0,50000);
	hEpdAdcWestVsEpdAdcEast = new TH2D("hEpdAdcWestVsEpdAdcEast","hEpdAdcWestVsEpdAdcEast; East Total EPD ADC; West Total EPD ADC",1000,0,50000,1000,0,50000);
	hBbcAdcEastVsEpdAdcEast = new TH2D("hBbcAdcEastVsEpdAdcEast","hBbcAdcEastVsEpdAdcEast; East Total EPD ADC; East Total BBC ADC",1000,0,50000,1000,0,50000);
	hBbcAdcWestVsEpdAdcWest = new TH2D("hBbcAdcWestVsEpdAdcWest","hBbcAdcWestVsEpdAdcWest; West Total EPD ADC; West Total BBC ADC",1000,0,50000,1000,0,50000);
	//hRefMultVsRefMultCorr = new TH2D("hRefMultVsRefMultCorr","hRefMultVsRefMultCorr; refMultCorr; refMult",1000,0,1000,1000,0,1000);
	//hCentrality = new TH1D("hCentrality","hCentrality; mCentrality",16,0,16);

	if(mFillTrkInfo){
		hChargePt = new TH1D("hChargePt", "hChargePt; charge*p_{T} (GeV/c)", 600, -15, 15);
		hdEdxVsP = new TH2D("hdEdxVsP","hdEdxVsP; p (GeV/c); dE/dx (KeV/cm)",300,0,15,400,0,20);
		hnSigEVsP = new TH2D("hnSigEVsP","hnSigEVsP; p (GeV/c); n#sigma_{e}",300,0,15,700,-15,20);
		hnSigEVsP_wBetaCut = new TH2D("hnSigEVsP_wBetaCut","hnSigEVsP_wBetaCut; p (GeV/c); n#sigma_{e}",300,0,15,700,-15,20);
		hBetaVsP = new TH2D("hBetaVsP","hBetaVsP; p (GeV/c); 1/#beta",300,0,15,800,0,4);
	}
}
//_____________________________________________________________________________
void StMiniTreeMaker::printConfig()
{
	const char *decision[2] = {"no","yes"};
	printf("=== Configuration for StMiniTreeMaker ===\n");
	printf("Fill the miniDst tree: %s\n",decision[mFillTree]);
	printf("Fill the QA histo: %s\n",decision[mFillHisto]);
	printf("Use default vertex: %s\n",decision[mDefaultVtx]);
	printf("Maximum |Vr|: %1.2f\n",mMaxVtxR);
	printf("Maximum |Vz|: %1.2f\n",mMaxVtxZ);
	printf("Maximum |VzDiff|: %1.2f\n",mMaxVzDiff);
	if(mFillTrkInfo){
		printf("Minimum track pt: %1.2f\n",mMinTrkPt);
		printf("Maximum track |eta| : %1.2f\n",mMaxTrkEta);
		printf("Minimum number of fit hits: %d\n",mMinNHitsFit);
		printf("Minimum ratio of fit hits: %1.2f\n",mMinNHitsFitRatio);
		printf("Minimum number of dedx hits: %d\n",mMinNHitsDedx);
		printf("Maximum dca: %1.2f\n",mMaxDca);
		printf("Maximum |nSigmaE| for TPCe: %1.2f\n",mMaxnSigmaE);
		printf("Maximum |1-1/beta| for TPCe: %1.2f\n",mMaxBeta2TOF);
	}
	printf("=======================================\n");
}
