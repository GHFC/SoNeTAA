/* Comunication Firmware with the computer, 
This reads the goniometer, and in each interruption with the timer,
the board sends the lecture to the computer by rs232.
This is the same firmware for both cases, the MMN and the HDC test.*/
////  HDC-Arduino Firmare
//// _______________________________________________

#include <Arduino.h>
#include <digitalWriteFast.h>
#include <DueTimer.h>
int PinA = 3;
int PinB = 2;
int PinI = 4;
int PinTrigger0 = 5;
int PinTrigger1 = 6;
int PinTrigger2 = 7;//cambiar por pin necesario
int PinTrigger3 = 8;
int Triggerin = 9;
int counter = 0;
int media;
int alta;
int baja;

boolean starts=false;
volatile boolean moving;
volatile boolean up;

void timerInterrupt(){
  starts=true;
  }

void timerInterrupt0(){
  digitalWrite(PinTrigger0, LOW);
  digitalWrite(PinTrigger1, LOW);
  digitalWrite(PinTrigger2, LOW);
  digitalWrite(PinTrigger3, LOW);
  Timer0.stop();
}

// Interruption when PinA get down
void scriptInterruptAF ()  {
  if (digitalReadFast(PinA))
    up = digitalReadFast(PinB);
  else
    up = !digitalReadFast(PinB);
  
  if (up)
    counter++;
  else
    counter--;
}

void scriptInterruptPush() {
  if (digitalReadFast (Triggerin))
    Serial.write(0xFF);
  }

void scriptInterruptBF ()  {
  if (digitalReadFast(PinB))
    up = !digitalReadFast(PinA);
  else
    up = digitalReadFast(PinA);
  if (up)
    counter++;
  else
    counter--;
}
//inline int digitalReadDirect(int pin){
//  return !!(g_APinDescription[pin].pPort -> PIO_PDSR & g_APinDescription[pin].ulPin);
//}
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);
  Serial.setTimeout(10);
  // make the pushbutton's pin an input:
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  pinMode(PinI, INPUT);
  pinMode(PinTrigger0, OUTPUT);
  pinMode(PinTrigger1, OUTPUT);
  pinMode(PinTrigger2, OUTPUT);
  pinMode(PinTrigger3, OUTPUT);
  attachInterrupt (digitalPinToInterrupt(PinA),scriptInterruptAF,FALLING);
  attachInterrupt (digitalPinToInterrupt(PinB),scriptInterruptBF,FALLING);
  attachInterrupt (digitalPinToInterrupt (Triggerin), scriptInterruptPush, RISING);
  Timer3.attachInterrupt(timerInterrupt);
  Timer0.attachInterrupt(timerInterrupt0);
}

// the loop routine runs over and over again forever:
void loop() {
  if (starts){
    if (counter>0){
      baja=counter & 0x3F;
      media=((counter>>6) & 0x3F)|0x40;
      alta=((counter>> 12) & 0x0F)|0xC0;
    }
    else {
      baja=abs(counter) & 0x3F;
      media=((abs(counter)>>6) & 0x3F)|0x40;
      alta=((abs(counter)>> 12) & 0x0F)|0xC8;
    }
    Serial.write(baja);
    Serial.write(media);
    Serial.write(alta);
    starts=false;
  }
}

void serialEvent(){
  while (Serial.available()){
     char inchar = (char)Serial.read();
     if (inchar=='i'){
        Timer3.start(2000); //timer interrupts each 2000 microsecs
     }
     else if(inchar=='o'){
        starts=false;
        Timer3.stop();
     }
     else if(inchar=='0'){
        digitalWrite(PinTrigger0, HIGH);   // turn the LED on (HIGH is the voltage level)
        Timer0.start(9000);
     }
     else if(inchar=='1'){
        digitalWrite(PinTrigger1, HIGH);   // turn the LED on (HIGH is the voltage level)
        Timer0.start(9000);
     }
     else if(inchar=='2'){
        digitalWrite(PinTrigger0, HIGH);   // turn the LED on (HIGH is the voltage level)
        digitalWrite(PinTrigger1, HIGH);
        Timer0.start(9000);                       // wait for 3 miliseconds
     }
	   else if(inchar=='3'){
	      digitalWrite(PinTrigger2, HIGH);
		    Timer0.start(9000);
	 }
     else if(inchar=='4'){
       digitalWrite(PinTrigger2, HIGH);
       digitalWrite(PinTrigger0, HIGH);
        Timer0.start(9000);
   }
    else if(inchar=='5'){
       digitalWrite(PinTrigger2, HIGH);
       digitalWrite(PinTrigger1, HIGH);
        Timer0.start(5000);
   }
     else if(inchar=='6'){
       digitalWrite(PinTrigger2, HIGH);
       digitalWrite(PinTrigger1, HIGH);
       digitalWrite(PinTrigger0, HIGH);
        Timer0.start(5000);
   }
  }
}

