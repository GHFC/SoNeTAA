#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

#include "threadObject.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        // Graphics
        void initGraphics();
        ofTrueTypeFont orderFont;
        ofImage fingerImg;
        bool fingerFlag;
        bool dotFlag;

        int image_coordinates[121][2];
        void drawFinger(float x);
        ofTexture		texNoise;
        unsigned char 	* noisePixels;

        // Sounds
        void initSounds();
        ofSoundPlayer  bip;

        /*void audioOut(float * output, int bufferSize, int nChannels);
		ofSoundStream soundStream;
		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;
		vector <float> lAudio;
		vector <float> rAudio;
		float 	targetFrequency;
		float 	phase;
		float 	phaseAdder;
		float 	phaseAdderTarget;*/

        // VPI
        threadedObject	TO;
        bool calibrationFlag;
        bool calibrationDone;
};
