#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "mmsystem.h"

//========================================================================
int main( ){
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 640,480, OF_FULLSCREEN);
	ofSetVerticalSync(false);
    timeBeginPeriod(1);
    ofSetWindowTitle("VPI 4.0");
    ofHideCursor();
	ofRunApp( new testApp());
    timeEndPeriod(1);
}
