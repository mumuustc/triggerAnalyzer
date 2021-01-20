#include "headers.h"
#include "constants.h"
#include "MINIEVENT.h"

void bookHistograms();
void writeHistograms(char* outFile);

const Int_t   nInterestTrigs = 4;
const Int_t   interestTrigID[nInterestTrigs] = {650000, 650005, 650006, 650007};
const TString interestTrigName[nInterestTrigs] = {"MB", "MB_EPD", "MB_VPD", "MB_ZDC"};

//********* define function and histograms *********
TH1D *hEvent;
TH2D *hVertexYvsX;
TH2D *hVpdVzvsTpcVz;

TH2D *hBtofMultVsRefMult;
TH2D *hGRefMultVsRefMult;
TH2D *hRefMult2VsRefMult;
TH1D *hRefMult[nInterestTrigs];

TH2D *hNVpdHitsVsRefMult;
TH3D *hNVpdHitsWestVsNVpdHitsEastVsRefMult;

TH2D *hNBbcHitsVsRefMult;
TH3D *hNBbcHitsWestVsNBbcHitsEastVsRefMult;

TH2D *hNRawEpdHitsVsBtofMult;
TH2D *hNRawEpdHitsVsRefMult;
TH3D *hNRawEpdHitsWestVsNRawEpdHitsEastVsRefMult;

TH2D *hEpdAdcVsChn;  // check EPD ADC distribution to define good EPD hit
TH2D *hEpdNMipVsChn; // check EPD MIP distribution to define good EPD hit
TH2D *hNEpdHitsVsBtofMult;
TH2D *hNEpdHitsVsRefMult;
TH2D *hNInnerEpdHitsVsRefMult;
TH2D *hNOuterEpdHitsVsRefMult;
TH3D *hNEpdHitsWestVsNEpdHitsEastVsRefMult;
TH3D *hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult;
TH3D *hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult;

TH2D *hBtofMultVsRefMult_EpdSel;
TH2D *hNEpdHitsVsBtofMult_EpdSel;
TH2D *hNEpdHitsVsRefMult_EpdSel;
TH2D *hNInnerEpdHitsVsRefMult_EpdSel;
TH2D *hNOuterEpdHitsVsRefMult_EpdSel;
TH3D *hNEpdHitsWestVsNEpdHitsEastVsRefMult_EpdSel;
TH3D *hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult_EpdSel;
TH3D *hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult_EpdSel;

TH2D *hVpdVzvsTpcVz_Band;
TH2D *hNVpdHitsWestVsNVpdHitsEast_Band;
TH2D *hNEpdHitsWestVsNEpdHitsEast_Band;
TH2D *hNInnerEpdHitsWestVsNInnerEpdHitsEast_Band;
TH2D *hNOuterEpdHitsWestVsNOuterEpdHitsEast_Band;

int main(int argc, char** argv)
{
    if(argc!=1&&argc!=3) return -1;

    TString inFile="test.list";
    char outFile[1024];
    sprintf(outFile,"test");
    if(argc==3){
        inFile = argv[1];
        sprintf(outFile,"%s",argv[2]);
    }

    //+---------------------------------+
    //| open files and add to the chain |
    //+---------------------------------+
    TChain *chain = new TChain("miniDst");

    Int_t ifile=0;
    char filename[512];
    ifstream *inputStream = new ifstream;
    inputStream->open(inFile.Data());
    if (!(inputStream)) {
        printf("can not open list file\n");
        return 0;
    }
    for(;inputStream->good();){
        inputStream->getline(filename,512);
        if(inputStream->good()) {
            TFile *ftmp = new TFile(filename);
            if(!ftmp||!(ftmp->IsOpen())||!(ftmp->GetNkeys())) {
                cout<<"something wrong"<<endl;
            } else {
                cout<<"read in "<<ifile<<"th file: "<<filename<<endl;
                chain->Add(filename);
                ifile++;
            }
            delete ftmp;
        }
    }
    delete inputStream;

    bookHistograms();

    //+-------------+
    //| loop events |
    //+-------------+
    MINIEVENT *event = new MINIEVENT(chain);
    Int_t nEvts = chain->GetEntries();
    cout<<nEvts<<" events"<<endl;
    for(int ievt=0; ievt<nEvts; ievt++){
        if(nEvts<10){
            cout << "begin " << ievt << "th entry...." << endl;
        }
        else if(ievt % (nEvts / 10) == 0){
            cout << "begin " << ievt << "th entry...." << endl;
        }

        event->GetEntry(ievt);

        hEvent->Fill(0.5);

        Bool_t interestTrig[nInterestTrigs];
        memset(interestTrig, 0, sizeof(interestTrig));

        Int_t mNTrigs = event->mNTrigs;
        for(Int_t itrig=0; itrig<mNTrigs; itrig++){
            for(Int_t i=0; i<nInterestTrigs; i++){
                if(interestTrigID[i] == event->mTrigId[itrig]){
                    interestTrig[i] = kTRUE;
                    hEvent->Fill(2.5+i);

                    break;
                }
            }
        }

        if(!interestTrig[0]) continue;  // Run19 AuAu@14.5 GeV MB trigger

        Double_t bField = event->mBField;
        Double_t vx = event->mVertexX;
        Double_t vy = event->mVertexY;
        Double_t vz = event->mVertexZ;
        Double_t vr = sqrt(vx*vx + vy*vy);
        Double_t vpdVz  = event->mVpdVz;
        Double_t vzDiff = vz - vpdVz;

        hVertexYvsX->Fill(vx, vy);
        hVpdVzvsTpcVz->Fill(vz, vpdVz);

        //if(TMath::Abs(vz)>mVzCut)         continue;
        //if(vr>mVrCut)                     continue;
        //if(TMath::Abs(vzDiff)>mVzDiffCut) continue;

        Int_t grefMult   = event->mGRefMult;
        Int_t refMult    = event->mRefMult;
        Int_t refMult2   = event->mRefMult2;
        Int_t offlineBtofMult = event->mOfflineBtofMult;

        hBtofMultVsRefMult->Fill(refMult, offlineBtofMult);
        hGRefMultVsRefMult->Fill(refMult, grefMult);
        hRefMult2VsRefMult->Fill(refMult, refMult2);
        for(Int_t i=0; i<nInterestTrigs; i++){
            if(interestTrig[i]) hRefMult[i]->Fill(refMult);
        }

        Int_t nVpdHitsEast = event->mNVpdHitsEast;
        Int_t nVpdHitsWest = event->mNVpdHitsWest;
        hNVpdHitsWestVsNVpdHitsEastVsRefMult->Fill(refMult, nVpdHitsEast, nVpdHitsWest);
        hNVpdHitsVsRefMult->Fill(refMult, nVpdHitsEast + nVpdHitsWest);

        Int_t nBbcHitsEast = event->mNBbcHitsEast;
        Int_t nBbcHitsWest = event->mNBbcHitsWest;
        //for(Int_t ie=0; ie<nBbcHitsEast; ie++){
        //    Int_t bbcAdcEast = event->mBbcAdcEast[ie];
        //}
        hNBbcHitsVsRefMult->Fill(refMult, nBbcHitsEast + nBbcHitsWest);
        hNBbcHitsWestVsNBbcHitsEastVsRefMult->Fill(refMult, nBbcHitsEast, nBbcHitsWest);

        Int_t  nGoodEpdHitsEast = 0, nGoodEpdHitsWest = 0;
        Int_t  nGoodInnerEpdHitsEast = 0, nGoodInnerEpdHitsWest = 0;
        Int_t  nGoodOuterEpdHitsEast = 0, nGoodOuterEpdHitsWest = 0;

        UInt_t totalGoodEpdAdcEast = 0, totalGoodEpdAdcWest = 0;
        UInt_t totalGoodInnerEpdAdcEast = 0, totalGoodInnerEpdAdcWest = 0;
        UInt_t totalGoodOuterEpdAdcEast = 0, totalGoodOuterEpdAdcWest = 0;

        Int_t nEpdHits     = event->mNEpdHits;
        Int_t nEpdHitsEast = event->mNEpdHitsEast;
        Int_t nEpdHitsWest = event->mNEpdHitsWest;
        for(Int_t iepd=0; iepd<nEpdHits; iepd++){
            Int_t   side     = event->mSide[iepd];
            Int_t   poistion = event->mPosition[iepd];
            Int_t   tile     = event->mTile[iepd];
            Int_t   row      = tile/2 + 1;
            Int_t   adc      = event->mADC[iepd];
            Int_t   tac      = event->mTAC[iepd];
            Bool_t  hasTac   = event->mHasTAC[iepd];
            Float_t nMIP     = event->mNMIP[iepd];
            Float_t TnMIP    = event->mTnMIP[iepd];

            Int_t chnIdx = side * ( (poistion-1)*31 + tile );
            hEpdAdcVsChn->Fill(chnIdx, adc);
            hEpdNMipVsChn->Fill(chnIdx, nMIP);

            if(adc > mEpdAdcTh){  // OR nMIP > mNMipTh
                if(side < 0){
                    nGoodEpdHitsEast++;
                    totalGoodEpdAdcEast += adc;

                    if(row <= mEpdInnerOuterBoundary){
                        nGoodInnerEpdHitsEast++;
                        totalGoodInnerEpdAdcEast += adc;
                    }
                    else{
                        nGoodOuterEpdHitsEast++;
                        totalGoodOuterEpdAdcEast += adc;
                    }
                }

                if(side > 0){
                    nGoodEpdHitsWest++;
                    totalGoodEpdAdcWest += adc;

                    if(row <= mEpdInnerOuterBoundary){
                        nGoodInnerEpdHitsWest++;
                        totalGoodInnerEpdAdcWest += adc;
                    }
                    else{
                        nGoodOuterEpdHitsWest++;
                        totalGoodOuterEpdAdcWest += adc;
                    }
                }
            }
        }

        hNRawEpdHitsVsBtofMult->Fill(offlineBtofMult, nEpdHits);
        hNRawEpdHitsVsRefMult->Fill(refMult, nEpdHits);
        hNRawEpdHitsWestVsNRawEpdHitsEastVsRefMult->Fill(refMult, nEpdHitsEast, nEpdHitsWest);

        hNEpdHitsVsBtofMult->Fill(offlineBtofMult, nGoodEpdHitsEast + nGoodEpdHitsWest);
        hNEpdHitsVsRefMult->Fill(refMult, nGoodEpdHitsEast + nGoodEpdHitsWest);
        hNEpdHitsWestVsNEpdHitsEastVsRefMult->Fill(refMult, nGoodEpdHitsEast, nGoodEpdHitsWest);

        hNInnerEpdHitsVsRefMult->Fill(refMult, nGoodInnerEpdHitsEast + nGoodInnerEpdHitsWest);
        hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult->Fill(refMult, nGoodInnerEpdHitsEast, nGoodInnerEpdHitsWest);

        hNOuterEpdHitsVsRefMult->Fill(refMult, nGoodOuterEpdHitsEast + nGoodOuterEpdHitsWest);
        hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult->Fill(refMult, nGoodOuterEpdHitsEast, nGoodOuterEpdHitsWest);

        if(nGoodOuterEpdHitsEast > mNOuterEpdHitsEastTh  &&  nGoodOuterEpdHitsWest > mNOuterEpdHitsWestTh){
            hBtofMultVsRefMult_EpdSel->Fill(refMult, offlineBtofMult);
            hNEpdHitsVsBtofMult_EpdSel->Fill(offlineBtofMult, nGoodEpdHitsEast + nGoodEpdHitsWest);

            hNEpdHitsVsRefMult_EpdSel->Fill(refMult, nGoodEpdHitsEast + nGoodEpdHitsWest);
            hNEpdHitsWestVsNEpdHitsEastVsRefMult_EpdSel->Fill(refMult, nGoodEpdHitsEast, nGoodEpdHitsWest);

            hNInnerEpdHitsVsRefMult_EpdSel->Fill(refMult, nGoodInnerEpdHitsEast + nGoodInnerEpdHitsWest);
            hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult_EpdSel->Fill(refMult, nGoodInnerEpdHitsEast, nGoodInnerEpdHitsWest);

            hNOuterEpdHitsVsRefMult_EpdSel->Fill(refMult, nGoodOuterEpdHitsEast + nGoodOuterEpdHitsWest);
            hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult_EpdSel->Fill(refMult, nGoodOuterEpdHitsEast, nGoodOuterEpdHitsWest);
        }

        if(refMult <= 5  &&  offlineBtofMult >= 50){
            hVpdVzvsTpcVz_Band->Fill(vz, vpdVz);
            hNVpdHitsWestVsNVpdHitsEast_Band->Fill(nVpdHitsEast, nVpdHitsWest);
            hNEpdHitsWestVsNEpdHitsEast_Band->Fill(nGoodEpdHitsEast, nGoodEpdHitsWest);
            hNInnerEpdHitsWestVsNInnerEpdHitsEast_Band->Fill(nGoodInnerEpdHitsEast, nGoodInnerEpdHitsWest);
            hNOuterEpdHitsWestVsNOuterEpdHitsEast_Band->Fill(nGoodOuterEpdHitsEast, nGoodOuterEpdHitsWest);
        }
    }

    writeHistograms(outFile);
    delete chain;

    cout<<"end of program"<<endl;
    return 0;
}

//____________________________________________________________
void bookHistograms()
{
    const Int_t    nRefMultBin = 500;
    const Double_t mMaxRefMult = 500;
    const Int_t    nBtofMultBin = 600;
    const Double_t mMaxBtofMult = 1200;
    const Int_t    nEpdHitsBin  = 300;
    const Double_t mMaxEpdHits  = 600;
    const Int_t    nInnerEpdHitsBin = 150;
    const Double_t mMaxInnerEpdHits = 300;
    const Int_t    nOuterEpdHitsBin = 200;
    const Double_t mMaxOuterEpdHits = 400;

    hEvent = new TH1D("hEvent", "hEvent;;Entries", 10, 0, 10);
    hEvent->GetXaxis()->SetBinLabel(1, "All StPhys Events");
    for(Int_t i=0; i<nInterestTrigs; i++){
        hEvent->GetXaxis()->SetBinLabel(i+3, interestTrigName[i].Data());
    }

    hVertexYvsX   = new TH2D("hVertexYvsX", "hVertexYvsX; VertexX (cm); VertexY (cm)", 500, -2.5, 2.5, 500, -2.5, 2.5);
    hVpdVzvsTpcVz = new TH2D("hVpdVzvsTpcVz", "hVpdVzvsTpcVz; TPC V_{z} (cm); VPD V_{z} (cm)", 800, -200, 200, 800, -200, 200);

    hBtofMultVsRefMult = new TH2D("hBtofMultVsRefMult", "hBtofMultVsRefMult; refMult; btofMult", nRefMultBin, 0, mMaxRefMult, nBtofMultBin, 0, mMaxBtofMult);
    hGRefMultVsRefMult = new TH2D("hGRefMultVsRefMult", "hGRefMultVsRefMult; refMult; grefMult", nRefMultBin, 0, mMaxRefMult, nRefMultBin, 0, mMaxRefMult);
    hRefMult2VsRefMult = new TH2D("hRefMult2VsRefMult", "hRefMult2VsRefMult; refMult; refMult2", nRefMultBin, 0, mMaxRefMult, nRefMultBin, 0, mMaxRefMult);
    for(Int_t i=0; i<nInterestTrigs; i++){
        hRefMult[i] = new TH1D(Form("hRefMult_%s", interestTrigName[i].Data()), "hRefMult; refMult", nRefMultBin, 0, mMaxRefMult);
    }

    hNVpdHitsVsRefMult = new TH2D("hNVpdHitsVsRefMult", "hNVpdHitsVsRefMult; refMult; nVpdHits", nRefMultBin, 0, mMaxRefMult, 40, 0, 40);
    hNVpdHitsWestVsNVpdHitsEastVsRefMult = new TH3D("hNVpdHitsWestVsNVpdHitsEastVsRefMult", "hNVpdHitsWestVsNVpdHitsEastVsRefMult; refMult; nVpdHits East; nVpdHits West", nRefMultBin, 0, mMaxRefMult, 20, 0, 20, 20, 0, 20);

    hNBbcHitsVsRefMult = new TH2D("hNBbcHitsVsRefMult", "hNBbcHitsVsRefMult; refMult; nBbcHits", nRefMultBin, 0, mMaxRefMult, 50, 0, 50);
    hNBbcHitsWestVsNBbcHitsEastVsRefMult = new TH3D("hNBbcHitsWestVsNBbcHitsEastVsRefMult", "hNBbcHitsWestVsNBbcHitsEastVsRefMult; refMult; nBbcHits East; nBbcHits West", nRefMultBin, 0, mMaxRefMult, 25, 0, 25, 25, 0, 25);

    hNRawEpdHitsVsBtofMult = new TH2D("hNRawEpdHitsVsBtofMult", "hNRawEpdHitsVsBtofMult; offline btofMult; nRawEpdHits", nBtofMultBin, 0, mMaxBtofMult, nEpdHitsBin, 0, mMaxEpdHits);
    hNRawEpdHitsVsRefMult  = new TH2D("hNRawEpdHitsVsRefMult", "hNRawEpdHitsVsRefMult; refMult; nRawEpdHits", nRefMultBin, 0, mMaxRefMult, nEpdHitsBin, 0, mMaxEpdHits);
    hNRawEpdHitsWestVsNRawEpdHitsEastVsRefMult = new TH3D("hNRawEpdHitsWestVsNRawEpdHitsEastVsRefMult", "hNRawEpdHitsWestVsNRawEpdHitsEastVsRefMult; refMult; nRawEpdHits East; nRawEpdHits West", nRefMultBin, 0, mMaxRefMult, nEpdHitsBin/2, 0, mMaxEpdHits/2, nEpdHitsBin/2, 0, mMaxEpdHits/2);

    hEpdAdcVsChn  = new TH2D("hEpdAdcVsChn", "hEpdAdcVsChn; channel index; ADC", 745, -372.5, 372.5, 400, 0, 4096);
    hEpdNMipVsChn = new TH2D("hEpdNMipVsChn", "hEpdNMipVsChn; channel index; nMIP", 745, -372.5, 372.5, 200, 0, 10);

    hNEpdHitsVsBtofMult = new TH2D("hNEpdHitsVsBtofMult", "hNEpdHitsVsBtofMult; offline btofMult; nEpdHits", nBtofMultBin, 0, mMaxBtofMult, nEpdHitsBin, 0, mMaxEpdHits);
    hNEpdHitsVsRefMult  = new TH2D("hNEpdHitsVsRefMult", "hNEpdHitsVsRefMult; refMult; nEpdHits", nRefMultBin, 0, mMaxRefMult, nEpdHitsBin, 0, mMaxEpdHits);
    hNEpdHitsWestVsNEpdHitsEastVsRefMult = new TH3D("hNEpdHitsWestVsNEpdHitsEastVsRefMult", "hNEpdHitsWestVsNEpdHitsEastVsRefMult; refMult; nEpdHits East; nEpdHits West", nRefMultBin, 0, mMaxRefMult, nEpdHitsBin/2, 0, mMaxEpdHits/2, nEpdHitsBin/2, 0, mMaxEpdHits/2);
    hNInnerEpdHitsVsRefMult = new TH2D("hNInnerEpdHitsVsRefMult", "hNInnerEpdHitsVsRefMult; refMult; nInnerEpdHits", nRefMultBin, 0, mMaxRefMult, nInnerEpdHitsBin, 0, mMaxInnerEpdHits);
    hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult = new TH3D("hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult", "hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult; refMult; nInnerEpdHits East; nInnerEpdHits West", nRefMultBin, 0, mMaxRefMult, nInnerEpdHitsBin/2, 0, mMaxInnerEpdHits/2, nInnerEpdHitsBin/2, 0, mMaxInnerEpdHits/2);
    hNOuterEpdHitsVsRefMult = new TH2D("hNOuterEpdHitsVsRefMult", "hNOuterEpdHitsVsRefMult; refMult; nOuterEpdHits", nRefMultBin, 0, mMaxRefMult, nOuterEpdHitsBin, 0, mMaxOuterEpdHits);
    hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult = new TH3D("hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult", "hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult; refMult; nOuterEpdHits East; nOuterEpdHits West", nRefMultBin, 0, mMaxRefMult, nOuterEpdHitsBin/2, 0, mMaxOuterEpdHits/2, nOuterEpdHitsBin/2, 0, mMaxOuterEpdHits/2);

    hBtofMultVsRefMult_EpdSel  = new TH2D("hBtofMultVsRefMult_EpdSel", "hBtofMultVsRefMult_EpdSel; refMult; btofMult", nRefMultBin, 0, mMaxRefMult, nBtofMultBin, 0, mMaxBtofMult);
    hNEpdHitsVsBtofMult_EpdSel = new TH2D("hNEpdHitsVsBtofMult_EpdSel", "hNEpdHitsVsBtofMult_EpdSel; offline btofMult; nEpdHits", nBtofMultBin, 0, mMaxBtofMult, nEpdHitsBin, 0, mMaxEpdHits);
    hNEpdHitsVsRefMult_EpdSel  = new TH2D("hNEpdHitsVsRefMult_EpdSel", "hNEpdHitsVsRefMult_EpdSel; refMult; nEpdHits", nRefMultBin, 0, mMaxRefMult, nEpdHitsBin, 0, mMaxEpdHits);
    hNEpdHitsWestVsNEpdHitsEastVsRefMult_EpdSel = new TH3D("hNEpdHitsWestVsNEpdHitsEastVsRefMult_EpdSel", "hNEpdHitsWestVsNEpdHitsEastVsRefMult_EpdSel; refMult; nEpdHits East; nEpdHits West", nRefMultBin, 0, mMaxRefMult, nEpdHitsBin/2, 0, mMaxEpdHits/2, nEpdHitsBin/2, 0, mMaxEpdHits/2);
    hNInnerEpdHitsVsRefMult_EpdSel = new TH2D("hNInnerEpdHitsVsRefMult_EpdSel", "hNInnerEpdHitsVsRefMult_EpdSel; refMult; nInnerEpdHits", nRefMultBin, 0, mMaxRefMult, nInnerEpdHitsBin, 0, mMaxInnerEpdHits);
    hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult_EpdSel = new TH3D("hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult_EpdSel", "hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult_EpdSel; refMult; nInnerEpdHits East; nInnerEpdHits West", nRefMultBin, 0, mMaxRefMult, nInnerEpdHitsBin/2, 0, mMaxInnerEpdHits/2, nInnerEpdHitsBin/2, 0, mMaxInnerEpdHits/2);
    hNOuterEpdHitsVsRefMult_EpdSel = new TH2D("hNOuterEpdHitsVsRefMult_EpdSel", "hNOuterEpdHitsVsRefMult_EpdSel; refMult; nOuterEpdHits", nRefMultBin, 0, mMaxRefMult, nOuterEpdHitsBin, 0, mMaxOuterEpdHits);
    hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult_EpdSel = new TH3D("hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult_EpdSel", "hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult_EpdSel; refMult; nOuterEpdHits East; nOuterEpdHits West", nRefMultBin, 0, mMaxRefMult, nOuterEpdHitsBin/2, 0, mMaxOuterEpdHits/2, nOuterEpdHitsBin/2, 0, mMaxOuterEpdHits/2);

    hVpdVzvsTpcVz_Band = new TH2D("hVpdVzvsTpcVz_Band", "hVpdVzvsTpcVz_Band; TPC V_{z} (cm); VPD V_{z} (cm)", 800, -200, 200, 700, -1200, 200);
    hNVpdHitsWestVsNVpdHitsEast_Band = new TH2D("hNVpdHitsWestVsNVpdHitsEast_Band", "hNVpdHitsWestVsNVpdHitsEast_Band; nVpdHits East; nVpdHits West", 20, 0, 20, 20, 0, 20);
    hNEpdHitsWestVsNEpdHitsEast_Band = new TH2D("hNEpdHitsWestVsNEpdHitsEast_Band", "hNEpdHitsWestVsNEpdHitsEast_Band; nEpdHits East; nEpdHits West", nEpdHitsBin/2, 0, mMaxEpdHits/2, nEpdHitsBin/2, 0, mMaxEpdHits/2);
    hNInnerEpdHitsWestVsNInnerEpdHitsEast_Band = new TH2D("hNInnerEpdHitsWestVsNInnerEpdHitsEast_Band", "hNInnerEpdHitsWestVsNInnerEpdHitsEast_Band; nInnerEpdHits East; nInnerEpdHits West", nInnerEpdHitsBin/2, 0, mMaxInnerEpdHits/2, nInnerEpdHitsBin/2, 0, mMaxInnerEpdHits/2);
    hNOuterEpdHitsWestVsNOuterEpdHitsEast_Band = new TH2D("hNOuterEpdHitsWestVsNOuterEpdHitsEast_Band", "hNOuterEpdHitsWestVsNOuterEpdHitsEast_Band; nOuterEpdHits East; nOuterEpdHits West", nOuterEpdHitsBin/2, 0, mMaxOuterEpdHits/2, nOuterEpdHitsBin/2, 0, mMaxOuterEpdHits/2);
}

//____________________________________________________________
void writeHistograms(char* outFile)
{
    char buf[1024];
    sprintf(buf,"%s.histo.root",outFile);
    cout<<"Writing histograms into "<<buf<<endl;
    TFile *mFile = new TFile(buf,"recreate");
    mFile->cd();

    hEvent->Write();
    hVertexYvsX->Write();
    hVpdVzvsTpcVz->Write();

    hBtofMultVsRefMult->Write();
    hGRefMultVsRefMult->Write();
    hRefMult2VsRefMult->Write();
    for(Int_t i=0; i<nInterestTrigs; i++) hRefMult[i]->Write();

    hNVpdHitsVsRefMult->Write();
    hNVpdHitsWestVsNVpdHitsEastVsRefMult->Write();

    hNBbcHitsVsRefMult->Write();
    hNBbcHitsWestVsNBbcHitsEastVsRefMult->Write();

    hNRawEpdHitsVsBtofMult->Write();
    hNRawEpdHitsVsRefMult->Write();
    hNRawEpdHitsWestVsNRawEpdHitsEastVsRefMult->Write();

    hEpdAdcVsChn->Write();
    hEpdNMipVsChn->Write();
    hNEpdHitsVsBtofMult->Write();
    hNEpdHitsVsRefMult->Write();
    hNInnerEpdHitsVsRefMult->Write();
    hNOuterEpdHitsVsRefMult->Write();
    hNEpdHitsWestVsNEpdHitsEastVsRefMult->Write();
    hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult->Write();
    hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult->Write();

    hBtofMultVsRefMult_EpdSel->Write();
    hNEpdHitsVsBtofMult_EpdSel->Write();
    hNEpdHitsVsRefMult_EpdSel->Write();
    hNInnerEpdHitsVsRefMult_EpdSel->Write();
    hNOuterEpdHitsVsRefMult_EpdSel->Write();
    hNEpdHitsWestVsNEpdHitsEastVsRefMult_EpdSel->Write();
    hNInnerEpdHitsWestVsNInnerEpdHitsEastVsRefMult_EpdSel->Write();
    hNOuterEpdHitsWestVsNOuterEpdHitsEastVsRefMult_EpdSel->Write();

    hVpdVzvsTpcVz_Band->Write();
    hNVpdHitsWestVsNVpdHitsEast_Band->Write();
    hNEpdHitsWestVsNEpdHitsEast_Band->Write();
    hNInnerEpdHitsWestVsNInnerEpdHitsEast_Band->Write();
    hNOuterEpdHitsWestVsNOuterEpdHitsEast_Band->Write();

    mFile->Close();
}
