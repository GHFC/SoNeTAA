#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings settings;
        settings.setGLVersion(2,1);
        //ofSetupOpenGL(1366,768,OF_FULLSCREEN);			// <-------- setup the GL context
        settings.width=1920;
        settings.height=1080;
        ofCreateWindow(settings);
		ofSetWindowTitle("MMN");
        ofHideCursor();
        ofSetFrameRate(60);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	    ofRunApp(new ofApp());
}
