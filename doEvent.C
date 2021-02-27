#include <TSystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void load(){
	//Load all the System libraries
	gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
	loadSharedLibraries();

	gSystem->Load("StTpcDb");
	gSystem->Load("StEvent");
	gSystem->Load("StMcEvent");
	gSystem->Load("StMcEventMaker");
	gSystem->Load("StDaqLib");
	gSystem->Load("libgen_Tables");
	gSystem->Load("libsim_Tables");
	gSystem->Load("libglobal_Tables");
	gSystem->Load("StMagF");

	gSystem->Load("St_g2t.so");
	gSystem->Load("St_geant_Maker.so");
	gSystem->Load("StAssociationMaker");
	gSystem->Load("StMcAnalysisMaker");
	gSystem->Load("libgeometry_Tables");   
	gSystem->Load("StTriggerUtilities");

	gSystem->Load("StEmcUtil");
	gSystem->Load("StEmcRawMaker");
	gSystem->Load("StEmcADCtoEMaker");
	gSystem->Load("StPreEclMaker");
	gSystem->Load("StEpcMaker");
	gSystem->Load("StEmcSimulatorMaker");

	gSystem->Load("StDbLib");
	gSystem->Load("StDbUtilities");
	gSystem->Load("StDbBroker");
	gSystem->Load("StDetectorDbMaker");
	gSystem->Load("St_db_Maker");

	gSystem->Load("StMtdHitMaker");
	gSystem->Load("StMtdUtil");
	gSystem->Load("StMtdMatchMaker");
	gSystem->Load("StMtdCalibMaker");
	gSystem->Load("StBTofUtil");
	gSystem->Load("StVpdCalibMaker");

	gSystem->Load("StEpdUtil");

	gSystem->Load("StPicoEvent");
	gSystem->Load("StPicoDstMaker");
	//gSystem->Load("StRefMultCorr");
	gSystem->Load("StMiniTreeMaker");
}

void doEvent(Int_t nEvents=1000, const Char_t *inputFile="test_MuDst.list", const TString outputFile="test.root", const Bool_t debug = kTRUE)
{
	load();

	StChain *chain = new StChain();
	chain->SetDebug(0);
	StMuDebug::setLevel(0); // switch of some debug output

	StMuTimer timer;
	timer.start();

	Bool_t iMuDst = 0;

	ifstream infile;
	infile.open(inputFile);
	string name;
	getline(infile,name);
	infile.close();
	std::size_t found = name.find("MuDst.root");
	if(found!=std::string::npos) iMuDst = 1;


	if(iMuDst){
		char theFilter[80];
		sprintf(theFilter,".MuDst.root:MuDst.root");
		StMuDstMaker *microMaker = new StMuDstMaker(0,0,"",inputFile,theFilter,1000);
		microMaker->Init();
		microMaker->SetStatus("*",1);

		//StMagFMaker *magfMk = new StMagFMaker; //add this line when run StMtdMatchMaker 

		//StMtdCalibMaker *mtdCalibMaker = new StMtdCalibMaker("mtdcalib");

		StEmcADCtoEMaker *adc2e = new StEmcADCtoEMaker();
		adc2e->setPrint(false);
		adc2e->saveAllStEvent(true);//Set to kTRUE if all hits are to be saved on StEvent

		StPreEclMaker *pre_ecl = new StPreEclMaker();
		pre_ecl->setPrint(kFALSE);

		StEpcMaker *epc = new StEpcMaker();
		epc->setPrint(kFALSE);

		outputFile.ReplaceAll(".root", "_MuDst.root");
	}else{
		StPicoDstMaker *picoMaker = new StPicoDstMaker(StPicoDstMaker::IoRead,inputFile,"picoDst"); 
		//StPicoDstMaker *picoMaker = new StPicoDstMaker(0,inputFile,"picoDst"); 

		outputFile.ReplaceAll(".root", "_picoDst.root");
	}

	St_db_Maker *dbMk = new St_db_Maker("StarDb", "MySQL:StarDb", "$STAR/StarDb","StarDb");
	//if(!iMuDst) dbMk->SetDateTime(20180101,0); //for run18 picoDst

	// run19 AuAu 14.5(6) GeV st_physics 
	vector<Int_t> processTrigIDs;
	processTrigIDs.clear();
	processTrigIDs.push_back(650000);  // minbias                                
	processTrigIDs.push_back(650002);  // minbias-hltgood
	processTrigIDs.push_back(650004);  // minbias-allvtx 
	processTrigIDs.push_back(650005);  // mb_epdcomponent                        
	processTrigIDs.push_back(650006);  // mb_vpdcomponent
	processTrigIDs.push_back(650007);  // mb_zdccomponent  

	StMiniTreeMaker *miniTreeMaker = new StMiniTreeMaker();
	miniTreeMaker->setOutFileName(outputFile);
	//miniTreeMaker->setTriggerIDs(processTrigIDs);
	miniTreeMaker->setFillTree(1);
	miniTreeMaker->setFillTrkInfo(0);
	miniTreeMaker->setFillHisto(1);
	miniTreeMaker->setUseDefaultVtx(1);
	//miniTreeMaker->setMaxVtxR(2.);
	//miniTreeMaker->setMaxVtxZ(100.);
	//miniTreeMaker->setMaxVzDiff(20.);
	//miniTreeMaker->setMinNHitsFit(15);
	//miniTreeMaker->setMinNHitsFitRatio(0.);
	//miniTreeMaker->setMinNHitsDedx(10);
	//miniTreeMaker->setMaxDca(5);
	//miniTreeMaker->setPrintMemory(1);
	//miniTreeMaker->setPrintCpu(1);
	//miniTreeMaker->setPrintConfig(1);
	if(debug) miniTreeMaker->SetDebug(1);

	if(chain->Init()==kStERR) return;
	cout<<"chain->Init();"<<endl;

	if(nEvents<0){
		if(iMuDst) nEvents = microMaker->chain()->GetEntries();
		else       nEvents = picoMaker->chain()->GetEntries();
	}

	cout << "****************************************** " << endl;
	cout << "total number of events  " << nEvents << endl;
	cout << "****************************************** " << endl;

	for(Int_t i=0; i<nEvents; i++) {
		if(debug) {
			cout<<endl;
			cout<<"Working on eventNumber "<< i <<endl;
		} else {
			if(i%1000==0)
				cout << "Working on eventNumber " << i << endl;
		}

		chain->Clear();
		int iret = chain->Make(i);

		if(iret) { cout << "Bad return code!" << iret << endl; break;}
	}

	chain->Finish();
	delete chain;

	timer.stop();
	cout << "Total time = " << timer.elapsedTime() << " s" << endl;

	cout << "****************************************** " << endl;
	cout << "Work done... now its time to close up shop!"<< endl;
	cout << "****************************************** " << endl;
}
