#ifndef _OF_VPI
#define _OF_VPI

#include "ofMain.h"
#include "ofxXmlSettings.h"

class VPI{

    public:
        // methods
        float update(float y, float yd, float timeStep);
		void changeModel(int newModel);
		void loadParameters();
		void loadTrial(int trial);
		void flip();
        float g1(float x);
        float g1d(float x);
        float g2(float x, float y);

        // constructor
		VPI();
        VPI(int dynModel);

        // variables
        float x;
        float xd;
		int model;

        ofxXmlSettings XML;
        float omega;
        float om;
        float tau;
        float alpha;
        float beta;
        float gamma;
        float kappa;
		float lambda;
        float A;
        float B;
        float mu;
        float gain;

        float task;
        float transition;

        bool runge;

    private:
};

#endif
