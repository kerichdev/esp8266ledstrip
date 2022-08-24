// LIBRARIES
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Arduino.h>
#include <FastLED.h>

// SECRETS ( Make sure to fill out. )
#include <secrets.h>

// DEBUG
#define BLYNK_PRINT Serial

// LED STRIP CONFIGURATION
#define LED_DT 14 // Data pin
#define LED_TYPE NEOPIXEL // Type of the LED strip
#define NUM_LEDS 60 // The number of LEDs in the strip

uint8_t max_bright = 255; // Maximum brightness of the LED - adjust lower if you get a weird tint of Red/Green/Blue

struct CRGB leds[NUM_LEDS];
BlynkTimer timer;

// Built-in palette
CRGB deadBlack(0, 0, 0); // Pure Black
CRGB deadWhite(255, 255, 255); // Pure White
CRGB deadGreen(0, 255, 0); // Pure Green
CRGB deadRed(255, 0, 0); // Pure Red

int assistantOverride = 1; // Assistant Control Switch - Learn more about the Google Assistant integration in the README.md

int currentMode = 0; // Current LED Strip mode - Blynk Switch: 4
int currentSpeed = 10; // Current LED Strip speed - Blynk Switch: 5

int colorH = 0; // Current color HUE - Blynk Switch: 6
int colorS = 0; // Current color SATURATION - Blynk Switch: 7
int colorV = 0; // Current color VALUE - Blynk Switch: 8

int colorH_g1 = 0; // Gradient: HUE 1
int colorS_g1 = 0; // Gradient: SATURATION 1
int colorV_g1 = 0; // Gradient: VALUE 1

int colorH_g2 = 0; // Gradient: HUE 2
int colorS_g2 = 0; // Gradient: SATURATION 2
int colorV_g2 = 0; // Gradient: VALUE 2

int gradientMode = 0; // Current gradient mode

int colorSpread = 15; // Spread of the color palette in the Color Cradle mode

String presetLights = "none";

// Blynk Switches declaration
BLYNK_WRITE(V19) { currentMode = 20; }
BLYNK_WRITE(V3) { assistantOverride = param.asInt(); }
BLYNK_WRITE(V4) { currentMode = param.asInt(); }
BLYNK_WRITE(V5) { currentSpeed = param.asInt(); }
BLYNK_WRITE(V6) { colorH = param.asInt(); }
BLYNK_WRITE(V7) { colorS = param.asInt(); }
BLYNK_WRITE(V8) { colorV = param.asInt(); }
BLYNK_WRITE(V9) { colorSpread = param.asInt(); }
BLYNK_WRITE(V10) {
      for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = deadBlack;
        FastLED.delay(30);
        FastLED.show();
      }
      ESP.reset();
}

BLYNK_WRITE(V12) { colorH_g1 = param.asInt(); }
BLYNK_WRITE(V14) { colorS_g1 = param.asInt(); }
BLYNK_WRITE(V16) { colorV_g1 = param.asInt(); }
BLYNK_WRITE(V13) { colorH_g2 = param.asInt(); }
BLYNK_WRITE(V15) { colorS_g2 = param.asInt(); }
BLYNK_WRITE(V17) { colorV_g2 = param.asInt(); }
BLYNK_WRITE(V18) { gradientMode = param.asInt(); }
BLYNK_WRITE(V11) { currentMode = (assistantOverride) ? param.asInt() : currentMode; }

// Preset list - only for assistant functionality - add as much as needed.
// CASE SWITCH *DOES NOT WORK* - this issue is yet to be resolved but please don't make PRs with case switch instead of an if else tree
BLYNK_WRITE(V20) {
    presetLights = (assistantOverride) ? param.asString() : "none";

    if (presetLights == "gaming"){
        currentMode = 4;
        currentSpeed = 15;
        presetLights = "none";
    }
    else if (presetLights == "ultrakill") {
        currentMode = 3;
        colorSpread = 10;
        currentSpeed = 20;
        colorH = 10;
        colorS = 255;
        colorV = 255;
        presetLights = "none";
    }
    else if (presetLights == "candy") {
        currentMode = 5;
        colorH_g1 = 40;
        colorH_g2 = 0;
        colorS_g1 = 255;
        colorS_g2 = 255;
        colorV_g1 = 255;
        colorV_g2 = 255;
        gradientMode = 1;
        presetLights = "none";
    }
    else if (presetLights == "retro") {
        currentMode = 5;
        colorH_g1 = 210;
        colorH_g2 = 140;
        colorS_g1 = 255;
        colorS_g2 = 255;
        colorV_g1 = 255;
        colorV_g2 = 255;
        gradientMode = 2;
        presetLights = "none";
    }
    else if (presetLights == "rainbow") {
        currentMode = 2;
        currentSpeed = 15;
        presetLights = "none";
    }
    else if (presetLights == "mint") {
        currentMode = 3;
        currentSpeed = 15;
        colorSpread = 20;
        colorH = 125;
        colorS = 190;
        colorV = 255;
        presetLights = "none";
    }
    else if (presetLights == "night") {
        currentMode = 3;
        currentSpeed = 15;
        colorSpread = 20;
        colorH = 170;
        colorS = 200;
        colorV = 100;
        presetLights = "none";
    }
    else if (presetLights == "rainy") {
        currentMode = 1;
        colorH = 160;
        colorS = 255;
        colorV = 75;
        presetLights = "none";
    }
    else if (presetLights == "sunny") {
        currentMode = 3;
        currentSpeed = 20;
        colorSpread = 5;
        colorH = 60;
        colorS = 255;
        colorV = 255;
        presetLights = "none";
    }
} // TODO: Move the preset lights switchback outside of the IFs, may or may not work

BLYNK_CONNECTED() {
    Blynk.syncAll();
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = deadGreen;
        FastLED.show();
        FastLED.delay(30);
    }
}

// Uptime timer
void blynkUptime() {
      Blynk.virtualWrite(V2, millis() / 1000);
}

// Rainbow wave stuff
void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) { 
    uint8_t thisHue = beat8(thisSpeed,255);
    fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);
    FastLED.show();
}

// Color Cradle
void color_cradle(uint8_t thisHue, uint8_t colorRange, uint8_t thisSpeed, int thisS, int thisV){
    uint8_t lowHue = thisHue - colorRange;
    uint8_t highHue = thisHue + colorRange;
    uint8_t currentHue = beatsin8(thisSpeed, lowHue, highHue);
    fill_solid(leds, NUM_LEDS, CHSV(currentHue, thisS, thisV));
    FastLED.show();
}

// Rainbow with one-color-at-a-time
void rainbow_constant(uint8_t thisSpeed, int thisS, int thisV){
    uint8_t thisHue = beat8(thisSpeed, 255);
    fill_solid(leds, NUM_LEDS, CHSV(thisHue, thisS, thisV));
    FastLED.show();
}

// This thing isnt supposed to work but it somehow does so here be dragons
TGradientDirectionCode getGradMode(){
    switch(gradientMode) {
        case 0: return FORWARD_HUES;
        case 1: return BACKWARD_HUES;
        case 2: return SHORTEST_HUES;
        case 3: return LONGEST_HUES;
        default: return SHORTEST_HUES;
    }
}

// SETUP
void setup() {
    Serial.begin(115200);
    LEDS.addLeds<LED_TYPE, LED_DT>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
    FastLED.setBrightness(max_bright); // Tweak max_bright in case of color tints - refer to the comment at the declaration
    delay(500);
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = deadRed;
        FastLED.show();
        FastLED.delay(30);
    }
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(1000L, blynkUptime);
}

// LOOP - SWITCHER
void loop() {
    // MAKE SURE TO KEEP THESE 2
    Blynk.run();
    timer.run();
    //
    switch(currentMode) {
        case 0: fill_solid(leds, NUM_LEDS, deadBlack); //lights off mode
                break;
        case 1: fill_solid(leds, NUM_LEDS, CHSV(colorH, colorS, colorV)); //solid color
                break;
        case 2: rainbow_constant(currentSpeed, colorS, colorV); //solid color scroll
                break;
        case 3: color_cradle(colorH, colorSpread, currentSpeed, colorS, colorV); //solid color sine
                break;
        case 4: rainbow_wave(currentSpeed, 1); //rgb mode
                break;
        case 5: fill_gradient(leds, NUM_LEDS, CHSV(colorH_g1, colorS_g1, colorV_g1), CHSV(colorH_g2, colorS_g2, colorV_g2), getGradMode());
                break;
    }
    FastLED.show();
}
