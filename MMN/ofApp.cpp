#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    timescreen=false;
    ofBackground(0,0,0);
    ofSetFullscreen(true);
    HUDFont.loadFont("Minecraftia.ttf",10,true,true);
    //ofSetVerticalSync(true);
    video.load("mmnpingu_silent.m4v");
    video.setLoopState(OF_LOOP_NONE);
    video.play();
    thread.start();
    //ofToggleFullscreen();
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    int totalf,currentf;
	int srelation=2;
	int sizex=video.getWidth()*srelation;
	int sizey=video.getHeight()*srelation;
    video.draw((ofGetWindowWidth()/2)-(sizex/2),(ofGetWindowHeight()/2)-(sizey/2),sizex,sizey);
    currentf=video.getCurrentFrame();
    totalf=video.getTotalNumFrames();
    if(video.getIsMovieDone())
	    {thread.stopThread();
	     HUDFont.drawString("THE END",100, 100);
		}
	if(timescreen){
        float percent=(float)currentf/totalf;
        HUDFont.drawString("Time (s): "+ofToString((int)(percent*video.getDuration())),20, 100);
    }
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key=='s'){
        if (timescreen)
            timescreen=false;
        else
            timescreen=true;
    }
	if (key=='f'){
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
