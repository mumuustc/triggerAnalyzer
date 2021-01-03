#include "headers.h"
#include "constants.h"
#include "MINIEVENT.h"

void bookHistograms();
void writeHistograms(char* outFile);

//********* define function and histograms *********
TH2D *hVertexYvsX;
TH2D *hVpdVzvsTpcVz;
TH2D *hGRefMultVsRefMult;
TH2D *hNEpdHitsVsRefMult;
TH2D *hNBtofMatchVsRefMult;
TH2D *hNEpdHitsVsBtofMult;
TH2D *hTotalBbcAdcVsRefMult;
TH2D *hTotalEpdAdcVsRefMult;
TH2D *hNGoodEpdHitsVsRefMult;
TH2D *hNGoodEpdHitsVsBtofMult;

TH2D *hEpdAdcVsChn;  // check EPD ADC distribution to define good EPD hit
TH2D *hEpdNMipVsChn; // check EPD MIP distribution to define good EPD hit

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

		Bool_t validTrig = kFALSE;
		Int_t mNTrigs = event->mNTrigs;
		for(Int_t itrig=0; itrig<mNTrigs; itrig++){
			if(650000 == event->mTrigId[itrig]) validTrig = kTRUE; // Run19 AuAu@14.5 GeV MB trigger
		}
		if(!validTrig) continue;

		Double_t bField = event->mBField;
		Double_t vx = event->mVertexX;
		Double_t vy = event->mVertexY;
		Double_t vz = event->mVertexZ;
		Double_t vr = sqrt(vx*vx + vy*vy);
		Double_t vpdVz  = event->mVpdVz;
		Double_t vzDiff = vz - vpdVz;

		hVertexYvsX->Fill(vx,vy);
		hVpdVzvsTpcVz->Fill(vz,vpdVz);

		//if(TMath::Abs(vz)>mVzCut)         continue;
		//if(vr>mVrCut)                     continue;
		//if(TMath::Abs(vzDiff)>mVzDiffCut) continue;

		Double_t grefMult   = event->mGRefMult;
		Double_t refMult    = event->mRefMult;
		Double_t refMult2   = event->mRefMult2;
		Int_t    nBtofMatch = event->mNBtofMatch;

		Int_t nBbcHitsEast = event->mNBbcHitsEast;
		Int_t nBbcHitsWest = event->mNBbcHitsWest;
		for(Int_t ie=0; ie<nBbcHitsEast; ie++){
			Int_t bbcAdcEast = event->mBbcAdcEast[ie];
		}

		Int_t  nGoodEpdHitsEast = 0, nGoodEpdHitsWest = 0;
		UInt_t totalGoodEpdAdcEast = 0, totalGoodEpdAdcWest = 0;

		Int_t nEpdHits     = event->mNEpdHits;
		Int_t nEpdHitsEast = event->mNEpdHitsEast;
		Int_t nEpdHitsWest = event->mNEpdHitsWest;
		for(Int_t iepd=0; iepd<nEpdHits; iepd++){
			Int_t   side     = event->mSide[iepd];
			Int_t   poistion = event->mPosition[iepd];
			Int_t   tile     = event->mTile[iepd];
			Int_t   adc      = event->mADC[iepd];
			Int_t   tac      = event->mTAC[iepd];
			Bool_t  hasTac   = event->mHasTAC[iepd];
			Float_t nMIP     = event->mNMIP[iepd];
			Float_t TnMIP    = event->mTnMIP[iepd];

			Int_t chnIdx = side * ( (poistion-1)*31 + tile );
			hEpdAdcVsChn->Fill(chnIdx, adc);
			hEpdNMipVsChn->Fill(chnIdx, nMIP);

			if(nMIP > mNMipTh){
				if(side < 0){
					nGoodEpdHitsEast++;
					totalGoodEpdAdcEast += adc;
				}

				if(side > 0){
					nGoodEpdHitsWest++;
					totalGoodEpdAdcWest += adc;
				}
			}
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
	hVertexYvsX   = new TH2D("hVertexYvsX", "hVertexYvsX; VertexX (cm); VertexY (cm)", 500, -2.5, 2.5, 500, -2.5, 2.5);
	hVpdVzvsTpcVz = new TH2D("hVpdVzvsTpcVz", "hVpdVzvsTpcVz; TPC VertexZ (cm); VPD VertexZ (cm)", 2000, -200, 200, 2000, -200, 200);

	hEpdAdcVsChn  = new TH2D("hEpdAdcVsChn", "hEpdAdcVsChn; channel index; ADC", 745, -372.5, 372.5, 1000, 0, 4096);
	hEpdNMipVsChn = new TH2D("hEpdNMipVsChn", "hEpdNMipVsChn; channel index; nMIP", 745, -372.5, 372.5, 1000, 0, 10);
}

//____________________________________________________________
void writeHistograms(char* outFile)
{
	char buf[1024];
	sprintf(buf,"%s.histo.root",outFile);
	cout<<"Writing histograms into "<<buf<<endl;
	TFile *mFile = new TFile(buf,"recreate");
	mFile->cd();

	hVertexYvsX->Write();
	hVpdVzvsTpcVz->Write();

	hEpdAdcVsChn->Write();
	hEpdNMipVsChn->Write();

	mFile->Close();
}
