#include "iostream.h"

void split(const Char_t *infile = "test.list", const Int_t NUM = 5)
{
	gROOT->Reset();

	system("mkdir -p filelist4Job");
	system("rm -rf filelist4Job/*");
	system("rm -rf datalist4Job");

	ifstream* inputStream = new ifstream;
	inputStream->open(infile);
	if (!(inputStream)) {
		cout << "can not open list file" << endl;
		return;
	}
	char line[512];
	char outputfile[100];
	ofstream outDataList;
	outDataList.open("datalist4Job");
	ofstream outData;
	for (int i=0;inputStream->good();i++) {
		inputStream->getline(line,512);
		if  ( inputStream->good() ) {
			if(i%NUM==0) {
				if(outData.is_open()) outData.close();
				sprintf(outputfile,"filelist4Job/%d.list",i/NUM);
				outData.open(outputfile);
				outDataList << outputfile << endl;
			}
			cout << " read in file " << line << endl;
			outData << line << endl;
		}
	}
	if(outData.is_open()) outData.close();
	outDataList.close();
}
