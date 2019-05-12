#include "testApp.h"
#include "DAQ.h"

//--------------------------------------------------------------
void testApp::setup(){
    TO.start();
    initGraphics();
    initSounds();
    calibrationFlag=true;
    calibrationDone=false;
    dotFlag=false;
    trigger(0);
}

//--------------------------------------------------------------
void testApp::update(){
    if(calibrationFlag){
        TO.calibration();
    }
    for (int i = 0; i < 320; i++){
		for (int j = 0; j < 240; j++){
		    float tmp=(ofRandomuf() * 255);
			noisePixels[(j*320+i)*4 + 0] = tmp;// r
			noisePixels[(j*320+i)*4 + 1] = tmp;// g
			noisePixels[(j*320+i)*4 + 2] = tmp;// b
			noisePixels[(j*320+i)*4 + 3] = 15; 	// alpha
		}
	}
	texNoise.loadData(noisePixels, 320,240, GL_RGBA);
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    ofSetColor(128);
    TO.lock();
    if((!calibrationFlag)&&(!calibrationDone)){orderFont.drawString("Calibration needed.", ofGetWindowWidth()/2-orderFont.stringWidth("Calibration needed.")/2, ofGetWindowHeight()/2-50);};
    if(calibrationFlag){
        orderFont.drawString("Calibration", ofGetWindowWidth()/2-orderFont.stringWidth("Calibration")/2, ofGetWindowHeight()/2-20);
        /*volume = 1;
        pan = 0.5;
        targetFrequency = 1000.0f * (TO.humPos+2)/2;
        phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;*/

        ofLine(3*ofGetWindowWidth()/8,ofGetWindowHeight()/2-3,5*(float)ofGetWindowWidth()/8,ofGetWindowHeight()/2-3);
        ofLine(min(max((TO.humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8),ofGetWindowHeight()/2+5-18,min(max((TO.humPos+1)*ofGetWindowWidth()/2,3*(float)ofGetWindowWidth()/8),5*(float)ofGetWindowWidth()/8),ofGetWindowHeight()/2+25-18);
    };
    /*if((calibrationDone)&&(!calibrationFlag)){
        //volume=0;
        orderFont.drawString("+", ofGetWindowWidth()/2-orderFont.stringWidth("+")/2, ofGetWindowHeight()/2-orderFont.stringHeight("+")/2);
        };*/
    if(TO.trialStart){
		if(!TO.metronome){
			if(fingerFlag){
				if(TO.turing){
                    drawFinger((TO.hum2Pos)*4);
				}else{
                    if(TO.agent.model==1){
                        drawFinger(TO.agent.x*4);
                    }else if(TO.agent.model==2){
                        drawFinger(TO.agent.x/1.5);
                    }else{
                        drawFinger(TO.agent.x/2);
                    }
				}
			}
		}else {
			ofSetColor(100,255,100);
			/*volume = 1;
            pan = 0.5;
            targetFrequency = 1000.0f * (TO.agent.x+2)/2;
            phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;*/


            if ((TO.lastTime-TO.lastBip)>=(int)((1000.*2.*PI)/TO.agent.omega)) {
                bip.play();
                TO.lastBip=TO.lastTime;
            }
		}

    }
    if(TO.trigFlag){
        trigger(0);
        TO.trigFlag=false;
    }
    TO.unlock();
    ofSetColor(128);
    TO.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key=='v'){
        TO.lock();
        if(!TO.trialStart){
            TO.intervalPos[0][0]=9999;
            TO.intervalPos[0][1]=-9999;
            TO.intervalPos[1][0]=9999;
            TO.intervalPos[1][1]=-9999;
            calibrationFlag=true;
        }
        TO.unlock();
    } else if(key=='c'){
        calibrationFlag=false;
        calibrationDone=true;
    } else if(key =='a'){
        if(TO.automaticMode){
            TO.report=false;
        }
        TO.automaticMode=!TO.automaticMode;
    }else if(key=='p'){
        TO.HUD();
    } else if(key==' '){
        if(calibrationDone&&!TO.metronome){
            TO.trial();
        }
    } else if(key=='1'){
        TO.changeDynModel(1);
    } else if(key=='2'){
        TO.changeDynModel(2);
    } else if(key=='3'){
        TO.changeDynModel(3);
    } else if(key=='r'){
        TO.agent.runge=!TO.agent.runge;
    } else if(key=='h'){
        fingerFlag=!fingerFlag;
    } else if(key=='m'){
        TO.agent.flip();
    } else if(key=='t'){
        TO.TRAJ();
    } else if(key=='z'){
        TO.turing=!TO.turing;
    } else if(key=='d'){
        dotFlag=!dotFlag;
    } else if(key=='f'){
        int mode = ofGetWindowMode();
        if(mode == OF_WINDOW){
            ofSetFullscreen(true);
        } else {
            ofSetFullscreen(false);
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void testApp::exit(){
    TO.stop();
    trigger(0);
    ofSleepMillis(500);
}

//--------------------------------------------------------------
void testApp::initGraphics(){
    ofSetLineWidth(2);
    fingerFlag=true;
    orderFont.loadFont("ressources/Minecraftia.ttf",20, true, true);

    fingerImg.loadImage("ressources/montage.jpg");
    //map image regions into sequence of integers
	int k=0;
	for (int i=0;i<11;i++){
		for (int j=0;j<11;j++){
			image_coordinates[k][0]=j;
			image_coordinates[k][1]=i;
			k++;
		}
	}
	//crosshair at end, leave off last image
	image_coordinates[120][0]=image_coordinates[119][0];

	texNoise.allocate(320,240,GL_RGBA);
	noisePixels	= new unsigned char [320*240*4];

	for (int i = 0; i < 320; i++){
		for (int j = 0; j < 240; j++){
		    float tmp = (ofRandomuf() * 255);
			noisePixels[(j*320+i)*4 + 0] = tmp;	// r
			noisePixels[(j*320+i)*4 + 1] = tmp; // g
			noisePixels[(j*320+i)*4 + 2] = tmp; // b
			noisePixels[(j*320+i)*4 + 3] = 20; 	// alpha
		}
	}
	texNoise.loadData(noisePixels, 320,240, GL_RGBA);
}

//--------------------------------------------------------------
void testApp::initSounds(){
            //Sound
            bip.loadSound("ressources/bip.wav");
            /*int bufferSize		= 512;
            sampleRate 			= 44100;
            phase 				= 0;
            phaseAdder 			= 0.0f;
            phaseAdderTarget 	= 0.0f;
            volume				= 0.1f;
            bNoise 				= false;
            lAudio.assign(bufferSize, 0.0);
            rAudio.assign(bufferSize, 0.0);
            soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);*/
}
//--------------------------------------------------------------
void testApp::drawFinger(float x){
    if(dotFlag){
        ofEllipse(x*150+ofGetWindowWidth()/2, ofGetWindowHeight()/2, 20, 20);
    }else{
        float a=-2.5;
        float b=2.5;
        int i=static_cast<int>(120+(-120)*(x-a)/(b-a));
        if(i>120)
            i=120;
        if(i<0)
            i=0;
            int iX=image_coordinates[i][0]*640;
            int iY=image_coordinates[i][1]*480;

        fingerImg.drawSubsection(ofGetWindowWidth()/2-640/2, ofGetWindowHeight()/2-480/2, 640, 480, iX, iY, 640,480);
        ofEnableAlphaBlending();
        texNoise.draw(ofGetWindowWidth()/2-640/2, ofGetWindowHeight()/2-480/2, 640, 480);
        ofDisableAlphaBlending();
    }
}

//--------------------------------------------------------------
/*void testApp::audioOut(float * output, int bufferSize, int nChannels){
    //pan = 0.5f;
    float leftScale = 1 - pan;
    float rightScale = pan;

    // sin (n) seems to have trouble when n is very large, so we
    // keep phase in the range of 0-TWO_PI like this:
    while (phase > TWO_PI){
        phase -= TWO_PI;
    }

    if ( bNoise == true){
        // ---------------------- noise --------------
        for (int i = 0; i < bufferSize; i++){
            lAudio[i] = output[i*nChannels    ] = ofRandom(0, 1) * volume * leftScale;
            rAudio[i] = output[i*nChannels + 1] = ofRandom(0, 1) * volume * rightScale;
        }
    } else {
        phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
        for (int i = 0; i < bufferSize; i++){
            phase += phaseAdder;
            float sample = sin(phase);
            lAudio[i] = output[i*nChannels    ] = sample * volume * leftScale;
            rAudio[i] = output[i*nChannels + 1] = sample * volume * rightScale;
        }
    }
}*/
