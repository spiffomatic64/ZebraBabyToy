// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <NeoPixelBus.h>

// Used for Leds
#define PixelCount 15 // this example assumes 4 pixels, making it smaller will cause a failure
#define PixelPin 0    // make sure to set this to the correct pin, ignored for Esp8266

// Used for software SPI
#define LIS3DH_CLK 18 //SCL
#define LIS3DH_MOSI 23 //SDA 
#define LIS3DH_MISO 19 //SDO

// Chip Select pins
#define CS_1 25 //CS
#define CS_2 26
#define CS_3 27
#define CS_4 13
#define CS_5 5
#define CS_NUM 5
#define LED_ACCEL 1
#define BUF 10
#define TOLERANCE 100
#define FADE_TIME 100
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */
#define MOVEMENT 5
#define ACTIVE_COUNTER 150
#define SLEEP_COUNTER 500

// Globals
Adafruit_LIS3DH lis;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
float buf_r, buf_g, buf_b;
bool active = false;
long inactive_counter = ACTIVE_COUNTER + 1;


RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup(void) {

  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");

  Serial.println("Starting neopixels's");
  strip.Begin();
  strip.Show();

  // Accelerometer setup
  Serial.println("Starting LIS3DH");
  lis = Adafruit_LIS3DH(CS_2);
  
  if (!lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
  } else {
    Serial.println("LIS3DH found!");
    lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

    Serial.print("Range = "); Serial.print(2 << lis.getRange());  
    Serial.println("G");
  }
}

void loop() {

  lis.read();      // get X Y and Z data at once
  
  Serial.print(" - X:  "); Serial.print(lis.x); 
  Serial.print("  \tY:  "); Serial.print(lis.y); 
  Serial.print("  \tZ:  "); Serial.println(lis.z);

  Serial.println("About to write leds");
  active = false;
  xyz_all_leds(lis.x, lis.y, lis.z);
  if(!active) {
    inactive_counter++;
  } else {
    inactive_counter = 0;
  }
  Serial.print(inactive_counter);

  if(inactive_counter<ACTIVE_COUNTER) {
    show_averaged_leds();
  } else if(inactive_counter<SLEEP_COUNTER ) {
    wheel_cycle_leds(inactive_counter-ACTIVE_COUNTER,5);
  } else {
    
  }
  
  Serial.println("Done writing leds");
  delay(20); 
}

void check_inactive(int16_t x, int16_t y, int16_t z) {
  goto_sleep();
}

void goto_sleep() {
  fade_out();
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
}

void fade_out() {
  float r = buf_r;
  float g = buf_g;
  float b = buf_b;
  float ratio_r = buf_r / FADE_TIME;
  float ratio_g = buf_g / FADE_TIME;
  float ratio_b = buf_b / FADE_TIME;

  for(int i=0; i<FADE_TIME; i++) {
    r = r - ratio_r;
    g = g - ratio_g;
    b = b - ratio_b;

    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    
    for(uint16_t i=0; i<PixelCount; i++) {
      strip.SetPixelColor(i, RgbColor(uint8_t(r), uint8_t(g), uint8_t(b)));
    }
   
    strip.Show();
    delay(20);
  }
}

void xyz_all_leds(int16_t x, int16_t y, int16_t z) {
  //map(value, fromLow, fromHigh, toLow, toHigh)
  uint8_t r = abs(map(x, -9000, 9000, -255, 255));
  uint8_t g = abs(map(y, -9000, 9000, -255, 255));
  uint8_t b = abs(map(z, -9000, 9000, -255, 255));
  Serial.print("R: "); Serial.print(r); 
  Serial.print(" G: "); Serial.print(g); 
  Serial.print(" B: "); Serial.println(b);
  float ratio=0;

  if (r<g && r<b) {
    r = 0;
  }else if (b<r && b<g) {
    b = 0;
  }else {
    g = 0;
  }

  if (r>g && r>b) {
    ratio=255/r;
    r = 255;
  }else if (b>r && b>g) {
    ratio=255/b;
    b = 255;
  }else {
    if(g==0) g=1;
    ratio=255/g;
    g = 255;
  }

  Serial.print("Ratio: "); Serial.print(ratio); 
  Serial.print("R: "); Serial.print(r); 
  Serial.print(" G: "); Serial.print(g); 
  Serial.print(" B: "); Serial.println(b);

  if(r!=0 && r!=255){
    r=r*ratio;
  }else if(b!=0 && b!=255){
    b=b*ratio;
  }else{
    g=g*ratio;
  }

  Serial.print("Color");
  Serial.print(" - R:  "); Serial.print(r); 
  Serial.print("  \tG:  "); Serial.print(g); 
  Serial.print("  \tB:  "); Serial.println(b);

  average_leds(r, g, b);
}

void average_leds(uint8_t r, uint8_t g, uint8_t b) {
  Serial.print("BUF Color");
  Serial.print(" - R:  "); Serial.print(buf_r); 
  Serial.print("  \tG:  "); Serial.print(buf_g); 
  Serial.print("  \tB:  "); Serial.println(buf_b);
  if(abs(r-buf_r)>MOVEMENT || abs(g-buf_g)>MOVEMENT || abs(b-buf_b)>MOVEMENT) 
  {
    Serial.println("MOVEMENT!");
    active = true;
  }
  buf_r = ((buf_r * BUF) + r) / (BUF + 1);
  buf_g = ((buf_g * BUF) + g) / (BUF + 1);
  buf_b = ((buf_b * BUF) + b) / (BUF + 1);
}

void show_averaged_leds()
{
  for(uint16_t i=0; i<PixelCount; i++) {
    strip.SetPixelColor(i, RgbColor(round(buf_r), round(buf_g), round(buf_b)));
  }
 
  strip.Show();
}

void wheel_all_leds(int pos,int rate) {
  pos = pos * rate;
  RgbColor color = Wheel(pos);

  for(uint16_t i=0; i<PixelCount; i++) {
    strip.SetPixelColor(i, color);
  }
  
  strip.Show();
}

void wheel_cycle_leds(int pos,int rate) {
  
  for(uint16_t i=0; i<PixelCount; i++) {
    strip.SetPixelColor(i, Wheel((pos+(i*rate)) * rate));
  }
  
  strip.Show();
}

//fix for more resolution
RgbColor Wheel(int pos) {
  uint8_t r, g, b;

  pos = pos % 1536;

  if(pos < 256) { // blue->teal
    r = 0;
    g = pos;
    b = 255;
  } else if( pos < 512) { // teal->green
    pos = pos - 256;
    r = 0;
    g = 255;
    b = 255 - pos;
  } else if( pos < 768) { // green->yellow
    pos = pos - 512;
    r = pos;
    g = 255;
    b = 0;
  } else if( pos < 1024) { // yellow->red
    pos = pos - 768;
    r = 255;
    g = 255 - pos;
    b = 0;
  } else if( pos < 1280) { //red->purple
    pos = pos - 1024;
    r = 255;
    g = 0;
    b = pos;
  } else if( pos < 1536) { // purple->blue
    pos = pos - 1280;
    r = 255 - pos;
    g = 0;
    b = 255;
  } 

  return RgbColor(r, g, b);
}

