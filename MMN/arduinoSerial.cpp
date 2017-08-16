//#include "ofMain.h"
#include "arduinoSerial.h"

       void ArduinoSerial::writeArduino(unsigned char ch){
          serial.flush();
          serial.writeByte(ch);
       }
		
		void ArduinoSerial::trigger(int n){
		  switch(n){
			case 0: serial.writeByte('0'); 
			        printf("flag 0\n");
			        break;
			case 1: serial.writeByte('1'); 
			        printf("flag 1\n");
					break;
			case 2: serial.writeByte('2'); 
			        printf("flag 2\n");
					break;
			case 3: serial.writeByte('3'); 
			        printf("flag 3\n");
					break;
			case 4: serial.writeByte('4'); 
			        printf("flag 4\n");
					break;
			case 5: serial.writeByte('5'); 
			        printf("flag 5\n");
					break;	
			case 6: serial.writeByte('6'); 
			        printf("flag 5\n");
					break;	
		  }
		  
		}

       int ArduinoSerial::readArduino(){
          unsigned char bytesReturned;
          short conti=0;		  
		  //escribir "i" para tomar lectura
          while(serial.available()>0){
            //if(){
              bytesReturned=serial.readByte();
              nTimesRead=locate(bytesReturned);
			  conti++;
              if ((nTimesRead==3)&(conti>2)){
                 wordn=calculate(bytesReadString);
                 //readTime = ofGetElapsedTimef();
				 return wordn;
              }
          }
		  return wordn;
       }
	   
        bool ArduinoSerial::startSerial(){
            serial.listDevices();
            vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
            nTimesRead=0;
            int baud = 57600;
			if(!serial.setup(0, baud))
			{
				return false;
			}
            bSendSerialMessage = true;   //esta no es tan necesaria!!!
            memset(bytesReadString, 0, 3);
			return true;
        }

//-----------------------------------------------------------------------------------
// ------------------ To calculate the data received from arduino -------------------
        int ArduinoSerial::locate(unsigned char ch){
            switch ((ch&0xC0)>>6){
               case 0:bytesReadString[0]=ch&0x3F;
					  return 1;
               case 1:bytesReadString[1]=ch&0x3F;
                      return 2;
               case 3:bytesReadString[2]=ch&0x0F;
                      return 3;
            }
        }
// -------------------The same mentioned above---------------------------------------
        int ArduinoSerial::calculate(unsigned char arr[]){
             int value;
             value = arr[0]+(arr[1]<<6)+((arr[2]&0x03)<<12);
             if ((arr[2]&0x08)!=0)
                 value=value*(-1);
             return value;
        }
