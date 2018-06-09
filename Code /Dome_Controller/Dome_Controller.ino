//#include <OSCBoards.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <OSCMatch.h>
#include <OSCMessage.h>
#include <OSCTiming.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

#define ENCODER_USE_INTERRUPTS

 #define ENCODER_OPTIMIZE_INTERRUPTS
// #include <EEPROM.h>
#include <Encoder.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            13

#define NUMPIXELS      21

// encoder pins
Encoder rotaryEnc(11, 12);
//   avoid using pins with LEDs attached

// OSC NETWORK STUFF
int status = WL_IDLE_STATUS;
char ssid[] = "PaulSwift"; //  your network SSID (name)
char pass[] = "thankyou";    // your network password (use for WPA, or use as key for WEP)

//the Arduino's IP (only for recieving)
// IPAddress ip(128, 32, 122, 252);
//destination IP
IPAddress outIp1(192, 168, 1, 9 );
IPAddress outIp2(192, 168, 1, 9);
IPAddress outIp3(192, 168, 1, 11);
const unsigned int outPort = 9999;

WiFiUDP Udp;


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//set a framerate like at 30 fps using a delay at end of the loop
int trig[NUMPIXELS]; 
int fade[NUMPIXELS];
int panel[NUMPIXELS];
int irPins[] = {15, 16, 17, 18, 19};

//Mux control pins
int s0 = 5;
int s1 = 6;
int s2 = 9;
int s3 = 10;

//Mux in “SIG” pin
int SIG_pin = 14;


byte r;
byte g;
byte b;

int fadeTime = 5; //fadetime in frames

int delayval = 0; //Seconds that it takes for Neopixel to fade 

int frameCounter = 0;

void setup() {

  pixels.begin();
  pixels.show();

  pinMode(s0, OUTPUT);
pinMode(s1, OUTPUT);
pinMode(s2, OUTPUT);
pinMode(s3, OUTPUT);

digitalWrite(s0, LOW);
digitalWrite(s1, LOW);
digitalWrite(s2, LOW);
digitalWrite(s3, LOW);

digitalWrite(2, HIGH);

  Serial.begin(9600);
//  delay(200);
  //  while (!Serial) {
  //    Serial.println("waiting for serial..."); // wait for serial port to connect. Needed for native USB port only
  //  }

  WiFi.setPins(8, 7, 4); // CS, irq, rst

//  delay(1000);

    //  Serial.println("Serial port connected");
  // check for the presence of the shield:
  //Serial.println(WiFi.status());
  if (WiFi.status() == WL_NO_SHIELD) {
    //Serial.println("WiFi shield not present");
    // don't continue:
    //    while (true);

  }


  // Serial.println(WiFi.status());
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    // Serial.print("Attempting to connect to SSID: ");
    //  Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 3 seconds for connection:
//   delay(2000);
  }

  // Serial.println("Connected to wifi");
  // printWiFiStatus();

  //  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(outPort);


//  connection successful code (blink LED x3)
  digitalWrite(13, HIGH);
//  delay(100);
  digitalWrite(13, LOW);
//  delay(100);
  digitalWrite(13, HIGH);
//  delay(100);
  digitalWrite(13, LOW);
//  delay(100);
  digitalWrite(13, HIGH);
//  delay(1000);
  digitalWrite(13, LOW);
}

// HSV Stuff taken and edited from https://gist.github.com/hdznrrd/656996
void HSV_to_RGB(float h, float s, float v, byte *r, byte *g, byte *b)
{
  int i;
  float f,p,q,t;
  
  h = max(0.0, min(360.0, h));
  s = max(0.0, min(100.0, s));
  v = max(0.0, min(100.0, v));
  
  s /= 100;
  v /= 100;
  
  if(s == 0) {
    // Achromatic (grey)
    *r = *g = *b = round(v*255);
    return;
  }

  h /= 60; // sector 0 to 5
  i = floor(h);
  f = h - i; // factorial part of h
  p = v * (1 - s);
  q = v * (1 - s * f);
  t = v * (1 - s * (1 - f));
  switch(i) {
    case 0:
      *r = round(255*v);
      *g = round(255*t);
      *b = round(255*p);
      break;
    case 1:
      *r = round(255*q);
      *g = round(255*v);
      *b = round(255*p);
      break;
    case 2:
      *r = round(255*p);
      *g = round(255*v);
      *b = round(255*t);
      break;
    case 3:
      *r = round(255*p);
      *g = round(255*q);
      *b = round(255*v);
      break;
    case 4:
      *r = round(255*t);
      *g = round(255*p);
      *b = round(255*v);
      break;
    default: // case 5:
      *r = round(255*v);
      *g = round(255*p);
      *b = round(255*q);
    }
}

long rotaryPos  = -999;
long rotaryVal = 0;

void loop() {
  // put your main code here, to run repeatedly:

//Loop through and read all 16 values
//Reports back Value at channel 6 is: 346
//int val = readMux(15);
//  for(int i = 0; i < 16; i ++){
////    Serial.print("Value at channel ");
////    Serial.print(i); Serial.print("is : ");
////    Serial.println(readMux(i));
////    delay(1000);
//
//      float newHue = map(frameCounter, 0, 600, 0, 359);
//      //Serial.println(newHue);
//      HSV_to_RGB(newHue, 100, fade[i], &r, &g, &b);
//      pixels.setPixelColor(i, pixels.Color(r,g,b));
//      pixels.show();
//      if (fade[i] > 0){
//        fade[i]--;
//   }
//  }

    
    frameCounter = frameCounter%2400;
    for (int i=0;i<NUMPIXELS;i++){
      if (i <= 15){
        panel[i] = readMux(i);
        if (panel[i] < 100){        
          trig[i] = HIGH;
          if (trig[i] = HIGH){
            fade[i] = 100;
          }
        }else{
          trig[i] = LOW;
        }
      }//end of  if (i <= 15){
      
      if (i > 15){
        panel[i] = analogRead(i-1);
        //Serial.println (panel[i]);
        if (panel[i] < 100){
          trig[i] = HIGH;
          if (trig[i] = HIGH){
            fade[i] = 100;
          }
         }else{
            trig[i] = LOW;
          }
        }
      
      float newHue = map(frameCounter, 0, 2400, 0, 359);
//      Serial.println(newHue);
      if (fade[i] > 0){
        fade[i] = fade[i] - fadeTime;
      }
      HSV_to_RGB(newHue, 100, fade[i], &r, &g, &b);
      pixels.setPixelColor(i, pixels.Color(r,g,b));
      pixels.show();
      //delay(delayval);
//      Serial.println (fade[i]);
      frameCounter++;
  }//end for loop
  long newRotaryPos;
    newRotaryPos = rotaryEnc.read();
  
    if (newRotaryPos > rotaryPos) {
      // Serial.print("up");
      // Serial.print(newRotaryPos);
      rotaryVal++;
      // rotaryPos = newRotaryPos;
      delay(10);
      //the message wants an OSC address as first argument
      OSCMessage msg("/rotary");
      msg.add(2);
      Udp.beginPacket(outIp1, outPort);
      msg.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      Udp.beginPacket(outIp3, outPort);
      msg.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      msg.empty(); // free space occupied by message
    }

  else if (newRotaryPos < rotaryPos) {
    //  Serial.print("down");
    //  Serial.print(newRotaryPos);
    rotaryVal--;
    delay(10);
    OSCMessage msg("/rotary");
    msg.add(-2);
    Udp.beginPacket(outIp1, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    Udp.beginPacket(outIp3, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    msg.empty(); // free space occupied by message

  }
  rotaryPos = newRotaryPos;
  OSCMessage msg("/IR");
  for (int i = 0; i < NUMPIXELS; i++){
    msg.add(trig[i]);
  }
  Udp.beginPacket(outIp1, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  Udp.beginPacket(outIp3, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
  delay(10);
}
  
int readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};
  int muxChannel[16][4]={ 
    {0,0,0,0}, //channel 0 
    {1,0,0,0}, //channel 1 
    {0,1,0,0}, //channel 2 
    {1,1,0,0}, //channel 3 
    {0,0,1,0}, //channel 4 
    {1,0,1,0}, //channel 5 
    {0,1,1,0}, //channel 6 
    {1,1,1,0}, //channel 7 
    {0,0,0,1}, //channel 8 
    {1,0,0,1}, //channel 9 
    {0,1,0,1}, //channel 10 
    {1,1,0,1}, //channel 11 
    {0,0,1,1}, //channel 12 
    {1,0,1,1}, //channel 13 
    {0,1,1,1}, //channel 14 
    {1,1,1,1} //channel 15
  }; 
  //loop through the 4 sig 
  for(int i = 0; i < 4; i ++){ 
    digitalWrite(controlPin[i], muxChannel[channel][i]); 
  } 
  //read the value at the SIG pin 
  int val = analogRead(SIG_pin); 
  //return the value 
  return val; 
  }



