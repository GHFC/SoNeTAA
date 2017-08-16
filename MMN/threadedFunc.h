#ifndef _THREADED_OBJECT
#define _THREADED_OBJECT

#include "arduinoSerial.h"


class MyThread : public ofThread{
    public:

    ArduinoSerial as;
    ofSoundPlayer lstd,durd,freqd,combd;
    int initialtime;
    int count;
    ofFile file;
    ofBuffer buff;
	ofstream trialReportFile;
	bool comunication;
    
    void start(){
		comunication=as.startSerial();
        lstd.load("1000Hz_50ms.wav");
        lstd.setVolume(0.5);
	    durd.load("1000Hz_100ms.wav");
        durd.setVolume(0.5);
	    freqd.load("1500Hz_50ms.wav");
        freqd.setVolume(0.5);
        combd.load("1500Hz_100ms.wav");
        combd.setVolume(0.5);
        count=0;
        file.open(ofToDataPath("LEAP.csv"),ofFile::ReadWrite,false);
        buff=file.readToBuffer();
		trialReportFile.open(ofToDataPath(ofToString(ofGetYear())+"-"+ofToString(ofGetMonth())+"-"+ofToString(ofGetDay())+"-"+ofToString(ofGetHours())+"-"+ofToString(ofGetMinutes())+"_report.txt").c_str(),ios::out);
        startThread(true,false);
        initialtime=ofGetElapsedTimeMillis();
    }
    void threadedFunction(){
        while(isThreadRunning()){
            if(!buff.isLastLine()){
                string line=buff.getNextLine();
                vector <string> codes =ofSplitString(line,",");
                sleep(500+((int)ofRandom(0,100)));
				int readTime=ofGetElapsedTimeMillis()-initialtime;
                ofLog()<<"Time "<< readTime << " Stimuli number: " << count << " " << codes[2];
				initialtime=ofGetElapsedTimeMillis();
                switch(ofToInt(codes[2])){
                 case 201: lstd.play();
				           trialReportFile << "STD," << ofToString(initialtime) << "\n";
				           if(comunication)
						       as.trigger(0);
                           break;
                 case 202: freqd.play();
				           trialReportFile << "FREQ_D," << ofToString(initialtime) << "\n";
						   if(comunication)
				               as.trigger(1);
                           break;
                 case 203: durd.play();
				           trialReportFile << "DUR_D," << ofToString(initialtime) << "\n";
                           if(comunication)
						       as.trigger(2);
						   break;
                 case 204: combd.play();
				           trialReportFile << "COMB_D," << ofToString(initialtime) << "\n";
				           if(comunication)
						       as.trigger(3);
                           break;
                }
                count++;
            }
            else{
                stopThread();
            }
        }
    }
};
#endif
