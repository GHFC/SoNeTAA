#ifndef _ARDUIN_SERIAL
#define _ARDUIN_SERIAL
#include "ofMain.h"

class ArduinoSerial{
    private:
        unsigned char bytesReadString[3]; //3 data received to convert to int
        int wordn;                //encoder response;
        int nTimesRead;
        int calculate(unsigned char []);  //Calculate the encoder response
        int locate(unsigned char);
    public:
        ofSerial serial;
        bool bSendSerialMessage;
        bool startSerial();
        int readArduino();
        void writeArduino(unsigned char); // a flag for sending serial
		void trigger(int);
};
#endif
