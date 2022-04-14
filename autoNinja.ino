/*
autoNinja Vehicle control system

DIGITAL INPUTS
AUTOSTART - Pin for start and stop signals
NEUTRAL/PARK - Must be in neutral or park else stop sequence
BRAKE - Stop / Turn off 
Door Trigger
Hood Trigger
Trunk Trigger
Prox Trigger
Motion Trigger
Aux Trigger 1
Aux Trigger 2

ANALOG INPUTS
RPM - Detect when car is started

DIGITAL OUTPUTS all OUTPUTS Must go to seperate relays
Accessory - 
Ignition On -
Start - 
Horn
Parking Light
HeadLights

PWM OUTPUTS


*/
const int led = 2;
const int ignitionPin = 32;
const int accessoryPin = 33;
const int starterPin = 25;
const int brakePin = 26;
const int parkingPin = 27;
const int RPM = 14;
const int parkingLightsPin = 23;
const int headLampsPin = 22;
const int hornPin = 21;
const int doorLock = 19; //Also sets security active if enabled
const int doorUnlock = 18; // disable security if enabled


char buffer[200];
unsigned long interval=200;
String hoodPinState;
String trunkPinState;
String proxPinState;
String doorPinStateAsText;
String hoodPinStateAsText;
String trunkPinStateAsText;
String proxPinStateAsText;
int ledState=HIGH;
//Door Sensor Varibles
const int doorPin = 34; // used if security system is enabled. Enabled or Disabled. Sends Fault if enabled.
int doorPinReading;
int doorPinState;
int lastDoorPinState=LOW;
unsigned long lastDoorChange;

//Hood Sensor Varibles
const int hoodPin = 35; // used if security system is enabled. Enabled or Disabled. Sends Fault if enabled.
int hoodPinReading;
int lastHoodPinbState=LOW;
unsigned long lastHoodChange;

//Trunk Sensor Varibles
const int trunkPin = 36; // used if security system is enabled. Enabled or Disabled. Sends Fault if enabled.
int trunkPinReading;
int lastTrunkPinState=LOW;
unsigned long lastTrunkChange;

//Proximity Sensor Varibles
const int proxPin = 39; // used if security system is enabled. Enabled or Disabled. Sends Fault if enabled.
int proxPinReading;
int lastProxPinState=LOW;
unsigned long lastProxChange;

unsigned long alertBuffer=1000;
    unsigned long lastBlink;
    unsigned long blinkInterval=100;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(lastDoorPinState);
pinMode(led,OUTPUT);
pinMode(doorPin,INPUT);
pinMode(hoodPin,INPUT);
pinMode(trunkPin,INPUT);
pinMode(proxPin,INPUT);

}
void checkInputs(){
  //read door pin
  doorPinReading=digitalRead(doorPin);
 

  // set time of last change
if (doorPinReading!=lastDoorPinState){ //if door pin isn't what it was last
lastDoorChange=millis(); // set when door was open or closed at current time.
}

if ((millis()-lastDoorChange) > alertBuffer){

  if (doorPinReading!=doorPinState){
     sprintf(buffer,"doorPinReading %d : ledState %d : doorPinState %d",doorPinReading,ledState,doorPinState);
Serial.println(buffer);
delay(100);
    doorPinState=doorPinReading;
  
  if (doorPinState==LOW){
    ledState=!ledState;
    
  }
  }
}



    lastDoorPinState=doorPinReading;
}
unsigned long currentMillis = millis();
void loop() {
  
  if(doorPinReading==LOW){
ledState=!ledState;
Serial.println(ledState);
digitalWrite(led,ledState);
doBlink;

  } else {
//digitalWrite(led,ledState);
  }
 checkInputs();
}
void doBlink(){
   

  if (currentMillis - lastBlink >= interval) {
    // save the last time you blinked the LED
    lastBlink = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);
  }
}
