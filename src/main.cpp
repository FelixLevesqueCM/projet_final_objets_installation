#include <Arduino.h>

#include <VL53L0X.h>
VL53L0X  myTOF;

#include <M5_Encoder.h>
M5_Encoder myEncoder;

#include <M5_PbHub.h>
M5_PbHub myPbHub;

#include <MicroOscSlip.h>
MicroOscSlip <128> monOsc(& Serial);

#include <FastLED.h>
CRGB Pixel;
CRGB keyPixel;
#define PIXEL 27

#define CANAL_KEY 0
#define CANAL_ANGLE 1
#define BROCHE_ATOM_FIL_JAUNE 26

unsigned long monChronoDepart ;

int boutonValeurChange;

void setup() {
  Wire.begin();
  myEncoder.begin();
  myPbHub.begin();
  myTOF.init();
  Serial.begin(115200);
  monChronoDepart = millis();
  myPbHub.setPixelCount( CANAL_KEY , 1);
  FastLED.addLeds< WS2812, BROCHE_ATOM_FIL_JAUNE , GRB >(&keyPixel, 1); 
  //distance 
    myTOF.setSignalRateLimit(0.1);
    myTOF.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    myTOF.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    
  //animation de la lumière au démarrage
  Pixel = CRGB(255, 0, 0);
  FastLED.show();
  delay(1000);
  Pixel = CRGB(255, 255, 0);
  FastLED.show();
  delay(1000);
  Pixel = CRGB(0, 255, 0);
  FastLED.show();
  delay(1000);
  Pixel = CRGB(0, 0, 0);
  FastLED.show();
}

//recevoir du osc vers le arduino
void myOscMessageParser(MicroOscMessage & receivedOscMessage) {
  if (receivedOscMessage.checkOscAddress("/")) {
       
   } 
}

void loop() {
  monOsc.onOscMessageReceived(myOscMessageParser);
  if ( millis() - monChronoDepart >= 20 ) {
    monChronoDepart = millis();

    //changer la lumière du key:
    //myPbHub.setPixelColor( CANAL_KEY , 0 , 0,0,0 );

    //key et angle
    int maLectureKey = myPbHub.digitalRead( CANAL_KEY );
    int maLectureAngle = myPbHub.analogRead( CANAL_ANGLE );

    monOsc.sendInt( "/key" , maLectureKey );
    monOsc.sendInt( "/angle" , maLectureAngle );

    //encodeur
    myEncoder.update();
    int changementEncodeur = myEncoder.getEncoderChange();
    int boutonEncodeur = myEncoder.getButtonState();

    if (boutonValeurChange != boutonEncodeur) {
      monOsc.sendInt( "/encodeur_bouton", boutonEncodeur);
    }
    boutonValeurChange = boutonEncodeur;

    monOsc.sendInt( "/encodeur_change", changementEncodeur);

    //tof
    uint16_t mesure = myTOF.readRangeSingleMillimeters();
    monOsc.sendInt( "/tof" ,mesure);
    if (maLectureKey == 0) {
      myPbHub.setPixelColor(CANAL_KEY, 0, 150, 200, 255);
    } else {
      myPbHub.setPixelColor(CANAL_KEY, 0, 0, 0, 0);
    }
  }
}
