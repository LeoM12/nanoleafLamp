#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include "credentials.h"
#include "FastLED.h"
#include <BlynkSimpleEsp8266.h>
#include <Espalexa.h>

#define SERIAL_BAUDRATE     115200
#define BLYNK_PRINT Serial
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define LENGTH 271
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define PIN1 1

#define FARBWECHSEL "Farbwechsel"
#define REGENBOGEN "Regenbogen"
#define WELLE "Welle"

void farbwechselAlexa(EspalexaDevice* dev);
void regenbogenAlexa(EspalexaDevice* dev);
void welleAlexa(EspalexaDevice* dev);
Espalexa espalexa;

#define SAMPLES 1024
#define SAMPLING_FREQ 1000
#define NOISE         1200
double vReal[SAMPLES];
double vImag[SAMPLES];
uint8_t step1 = 0;
uint8_t step2 = 0;
boolean registeredBeat = false;
unsigned long newTime;
unsigned int sampling_period_us;

CRGB strip[LENGTH];
char auth[] = "3HXA0D0uJSabprgF4xiITKAXGJrLe8Jp";
char ssid[] = "M MAIN";
char pass[] = "21832040298146194648";

int data = 255;

int r, g, b;
int interval = 10;
int lastUpdate;
int lastUpdate2;
uint8_t step = 0;
double speed = 1;
int activePattern = 0;
const int rainbowUpPat = 1;
const int rainbowRightPat = 2;
const int rainbowOutPat = 3;
const int fadePat = 4;
const int wavePat = 5;
const int fill2Pat = 6;
const int musicPat = 7;
boolean firstCall = false;
int onOrOff = 1;
int brightness = 255;
uint8_t count;
int history[100];
int historyCounter;
int rInt;
//Length: 172
int stripRight[173][6] =  {{265}, {266, 264}, {263, 267}, {268}, {262}, {269}, {261}, {270, 260}, {0, 259}, {1, 258}, {2}, {257}, {3}, {256}, {4, 255}, {5}, {254}, {6},
  {253, 239, 252, 240}, {16}, {15, 7}, {238, 251, 241}, {17}, {8}, {14}, {242}, {18, 250, 237}, {9}, {13}, {249, 236}, {243, 19}, {12, 10},
  {235}, {244, 248}, {20, 11}, {234}, {247, 245}, {21}, {246}, {233}, {22}, {43}, {42, 232}, {23}, {44}, {41}, {231}, {24}, {45}, {40}, {25, 230},
  {46}, {39}, {229, 26}, {38, 47}, {228}, {27}, {37, 48, 205}, {227, 226, 206, 58}, {28}, {36, 204, 57, 49}, {225}, {59, 207, 29}, {203, 35, 50, 56}, {224, 30, 208},
  {60, 202, 55, 34, 51}, {54, 52, 31, 61, 209, 223}, {33, 201}, {32, 222}, {62, 53, 210}, {200}, {221}, {211, 63}, {199}, {220}, {212}, {64}, {198}, {219}, {213}, {65}, {197},
  {218}, {214}, {66}, {196}, {217}, {215}, {67}, {216}, {195}, {182}, {68}, {194, 183}, {181}, {78, 69}, {91, 193, 184}, {180, 77, 90}, {70, 79}, {192, 185, 92}, {179},
  {76, 89}, {80}, {191, 186, 71, 93}, {178}, {88, 75}, {94, 72, 81, 190, 187}, {177}, {87, 74}, {82, 73}, {188, 189, 95, 176}, {175, 86}, {83, 96}, {174, 85}, {84}, {97},
  {173, 152}, {98}, {153}, {151}, {172}, {99}, {154}, {150, 171}, {155, 100}, {149, 170}, {101, 156, 148}, {169}, {147, 157}, {102}, {168}, {146, 112, 158}, {167, 103},
  {113, 111}, {159}, {145, 104}, {166}, {114, 110}, {160}, {144, 105}, {165}, {109}, {115, 161}, {143, 106, 164}, {108}, {116}, {142, 107, 162}, {163}, {141, 117},
  {140}, {118}, {139}, {119}, {138}, {120}, {137}, {121}, {136}, {122}, {135}, {123}, {134}, {124}, {133}, {125}, {132}, {126}, {131}, {127}, {130}, {128}, {129}
};

int stripUp[144][11] =  {{199, 198, 197, 196, 195}, {194}, {200}, {193}, {201}, {192}, {202}, {191}, {203}, {190}, {204}, {189}, {205}, {188}, {206}, {187}, {207},
  {208, 186}, {185, 209}, {184}, {210}, {215, 214, 213, 212, 211, 183}, {216}, {182},
  {220, 219, 218, 217}, {221}, {181}, {222}, {180}, {223}, {179}, {224}, {178}, {225}, {177}, {176, 226}, {230, 231, 232, 233, 229, 173, 172, 171, 170, 169, 174},
  {168, 234}, {175, 228}, {227}, {58, 73}, {235}, {167}, {72}, {57, 74, 59}, {236}, {166}, {56, 60, 71}, {75}, {237}, {165}, {61, 70, 55}, {76}, {238},
  {164}, {62, 54}, {69}, {77}, {239, 163}, {64, 63, 68, 67, 66, 65}, {53}, {78}, {240}, {52, 162}, {79}, {241}, {51, 161, 80}, {242}, {160}, {81, 50}, {243}, {159},
  {49}, {82}, {244}, {158}, {48}, {83}, {245}, {44, 157, 156, 155, 154, 153, 47, 46, 45}, {43, 84, 152, 246, 39, 42, 41, 40}, {85, 38, 151, 150, 149, 148, 147}, {247},
  {86}, {146, 37}, {248}, {87, 36}, {145}, {249}, {35, 88}, {144, 250}, {34}, {89}, {143}, {251}, {33}, {90}, {142}, {252}, {32}, {135, 91, 139, 140, 138, 137, 136},
  {254, 259, 258, 257, 256, 255, 141}, {253}, {134}, {260}, {31, 107, 16}, {92}, {17, 133}, {106}, {261, 15}, {108, 30}, {93}, {132, 105, 18}, {262}, {14}, {29, 94, 109},
  {131}, {104}, {19}, {13, 263}, {28, 110, 95}, {130}, {20, 103}, {111, 264, 12, 27}, {96}, {129, 21, 102}, {23, 24, 22, 101, 100, 99, 98, 97, 26, 25}, {112, 11, 265},
  {128}, {266, 113}, {10}, {127, 114, 267}, {9}, {268}, {126, 115}, {8}, {269, 116}, {125, 7}, {270}, {124, 117}, {6}, {118, 119, 123, 122, 121, 120}, {0, 5, 4, 3, 2, 1}
};

int testRight[][7] = {{265}, {266}, {264}, {267}, {263}, {268}, {262}, {269, 261}, {270}, {260}, {259}, {0}, {258}, {1}, {2, 257}, {3, 256}, {4, 255}, {5, 254}, {6}, {253, 252},
  {240}, {239, 16}, {7}, {15}, {251}, {241, 238, 17}, {14, 8}, {250}, {242}, {237, 18}, {13, 9, 249}, {243}, {19, 236}, {10}, {12}, {248}, {235, 20, 244}, {11}, {247},
  {234, 245, 21}, {246}, {233}, {22}, {43}, {42}, {232}, {23}, {44}, {41}, {231}, {24}, {45}, {40}, {230}, {25}, {39, 46}, {229}, {26}, {38}, {47}, {228}, {27}, {48, 37},
  {205, 227, 226}, {58, 206, 28}, {49}, {36, 204, 57}, {207, 225}, {29}, {50, 59}, {203, 35, 56}, {208, 224}, {30, 51, 60}, {55, 34}, {202}, {223, 209}, {31}, {61, 33, 54, 52},
  {201}, {222}, {210, 32, 53}, {62}, {200}, {221, 211}, {63}, {199}, {212, 220}, {64}, {198}, {219, 213}, {65}, {197}, {218, 214}, {66}, {196}, {217}, {215}, {67}, {216}, {195},
  {182}, {68}, {194, 183}, {181}, {78, 69, 193, 184}, {91}, {90}, {180, 77, 79, 185, 70, 192}, {92, 89}, {179, 76}, {80, 71, 191, 186}, {93}, {75, 178, 88}, {81}, {72, 190, 187, 94},
  {177, 87}, {74, 82}, {188}, {95, 189, 73}, {176, 175}, {86, 83}, {96}, {174, 85}, {84}, {97}, {173}, {152}, {98}, {153, 151}, {172}, {154, 150, 99}, {171}, {100}, {149, 155}, {170},
  {101, 156}, {148}, {169}, {147, 157, 102}, {168}, {103, 158}, {112, 167, 146}, {111}, {113}, {104, 166, 145, 159}, {110}, {114}, {105, 144, 160, 165}, {109}, {106, 115, 161},
  {143, 164}, {108}, {116}, {163, 107, 162, 142}, {141}, {117}, {140}, {118}, {139}, {119}, {138}, {120}, {137}, {121}, {136}, {122}, {135}, {123}, {124, 134}, {133, 125}, {132, 126},
  {131}, {127}, {130, 128}, {129}
};

int stripCircle[][15] =  {{81, 80, 66, 65, 67, 50, 51, 79, 82, 64, 88, 87, 35, 52, 49}, {34, 36, 86, 89, 83, 78, 68, 63, 48}, {37, 53, 33},
  {90, 85}, {47, 32, 38}, {62}, {77, 69}, {91, 84, 54}, {39, 46}, {152}, {31, 61, 76}, {92, 70}, {153, 55}, {151}, {40}, {45}, {30, 60}, {75},
  {93, 71, 56}, {154, 150}, {41}, {44}, {29, 59}, {94, 74, 72, 57}, {155}, {149}, {42}, {43, 58}, {73, 28}, {95}, {175, 156, 227, 148}, {246},
  {228, 174, 27}, {176}, {226, 147, 157}, {245, 96}, {229}, {247, 173}, {177, 158}, {225, 146, 97}, {244, 26}, {248, 230}, {172}, {178, 159},
  {145, 98, 25}, {243, 224}, {249, 231, 171}, {179, 160}, {99, 24}, {242, 144}, {223}, {250, 232, 170}, {180, 161}, {100, 23}, {241, 143}, {222},
  {251, 233}, {181, 169, 162}, {218, 219, 217, 166, 165, 101, 22}, {240, 236, 237, 221, 220, 167, 164, 142}, {238, 235, 216, 168, 163},
  {239, 234, 213, 214, 182, 104, 106, 105}, {252, 215, 212, 103, 17, 19, 18}, {107, 102, 20}, {211, 183, 141, 21, 16}, {253, 109, 110, 108, 14, 15},
  {111, 13, 12}, {140, 112, 11}, {254}, {210, 184}, {139, 113}, {10}, {255}, {209, 185}, {138}, {114}, {9}, {256}, {208, 186}, {137}, {115, 8}, {257},
  {187, 207}, {136, 116}, {7}, {258}, {206}, {188}, {135, 117}, {6}, {259}, {205}, {189}, {134, 260, 118, 5, 204}, {190}, {261, 133, 119, 4}, {203},
  {191}, {132, 120, 3}, {262, 202}, {192}, {131, 121, 2}, {263, 201}, {193}, {1}, {130, 122, 264, 200}, {196, 199, 197, 198, 195, 194}, {0},
  {268, 129, 125, 128, 127, 126, 123}, {270, 267, 266, 269, 265, 124}
};

int hexagons[][33] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270},
  {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 247, 248, 249, 250, 251, 252},
  {43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245},
  {59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 176, 177, 178, 179, 180, 181, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226},
  {74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174},
  {85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151},
  {108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140},
  {183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215}};

  int test[33][1] = {{266}, {267}, {265}, {268}, {264}, {269}, {263}, {270}, {262}, {271}, {261}, {0}, {263}, {1}, {259}, {2}, {258}, {3},
    {257}, {4}, {256}, {5}, {255}, {6}, {15}, {7}, {8}, {14}, {9}, {13}, {10}, {12}, {11}
  };

  int test2[] = {266, 267, 265, 268, 264, 269, 263, 270, 262, 271, 261, 0, 260, 1, 259, 2, 258, 3, 257, 4, 256, 5, 255, 6, 15, 7, 8, 14, 9, 13, 10, 12, 11};

  // FFT ---------------------------------------------------------------------------------------

  /*for (int i = 0; i < SAMPLES; i++) {
        newTime = micros();
        vReal[i] = analogRead(0); // A conversion takes about 9.7uS on an ESP32
        vImag[i] = 0;
        while ((micros() - newTime) < sampling_period_us) {  chill  }
      }
      // Compute FFT
      FFT.DCRemoval();
      FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(FFT_FORWARD);
      FFT.ComplexToMagnitude();
  */

  // RAINBOW ---------------------------------------------------------------------------------------

  void rainbowSetup() {
    interval = 25;
    step = 255;
    firstCall = true;
  }

  void rainbowRightUpdate() {
    step++;
    for (int i = 0; i < 175; i++) { //173
      for (int k = 0; k < 7; k++) { //6
        strip[testRight[i][k]] = CHSV(step + i, 255, 255);
      }
    }
    FastLED.show();
  }

  void rainbowUpUpdate() {
    step++;
    for (int i = 0; i < 144; i++) { //170
      for (int k = 0; k < 11; k++) { //8
        strip[stripUp[i][k]] = CHSV(step + i, 255, 255);
      }
    }
    FastLED.show();
  }

  void rainbowOutUpdate() {
    if (firstCall) {
      step = 255;
      firstCall = false;
    }
    step--;
    for (int i = 0; i < 110; i++) {
      for (int k = 0; k < 15; k++) {
        strip[stripCircle[i][k]] = CHSV(step + i, 255, 255);
      }
    }
    FastLED.show();
  }

  // FADE --------------------------------------------------------------------------------------------
  void fadeSetup() {
    interval = 500;
    step = 0;
    fill_solid(strip, LENGTH, CHSV(0, 255, 255));
    FastLED.show();
    Serial.println("Farbwechsel gestartet.");
  }

  void fadeUpdate() {
    step++;
    fill_solid(strip, LENGTH, CHSV(step, 255, 255));
    FastLED.show();
  }

  // FILL  --------------------------------------------------------------------------------------------

  void waveSetup() {
    interval = 80;
    step = 0;
    fill_solid(strip, LENGTH, CRGB(0, 0, 0));
    FastLED.show();
  }


  void waveUpdate() {

    if (step == 170) {
      step = 0;
      count++;
    }
    if (count % 2 == 0) {
      for (int k = 0; k < 8; k++) {
        strip[stripRight[step][k]] = CHSV(step * 255 / 170, 255, 255);
      }
    } else {
      strip[step] = CHSV(step * 2, 255, 0);
      strip[LENGTH - step] = CHSV(step * 2, 255, 0);
    }

    step++;
    FastLED.show();
  }

  // FILL 2  --------------------------------------------------------------------------------------------

  void fill2Setup() {
    interval = 500;
    step = 1;
    fill_solid(strip, LENGTH, CRGB(0, 0, 0));
    FastLED.show();
    for (int i = 0; i < 100; i++) {
      history[i] = 10;
    }
  }

  void fill2Update() {
    rInt = rand() % 12;
    if (rInt < 8) {
      for (int i = 0; i < 33; i++) {
        strip[hexagons[rInt][i]] = CRGB(255, 0, 0);
        if (historyCounter > 3) {
          strip[hexagons[history[historyCounter - 3]][i]] = CRGB(0, 0, 0);
        }
      }
      history[historyCounter] = rInt;
      historyCounter++;
      if (historyCounter >= 99) {
        for (int i = 0; i < 100; i++) {
          history[i] = 10;
        }
        historyCounter = 0;
      }
      
    }
    FastLED.show();

  }

  // SUNRISE TIMER --------------------------------------------------------------------------------------------

  BLYNK_WRITE(V6) {
    sunriseSetup3();
    activePattern = 20;
  }
  void sunriseSetup3() {
    interval = 500;
    step = 0;
    fill_solid(strip, LENGTH, CHSV(31, 210, 255));
    FastLED.show();
  }

  void sunriseUpdate() {
    if (step < 255) {
      fill_solid(strip, LENGTH, CHSV(31, 210, step));
      step++;
      FastLED.show();
    } else {
      activePattern = 0;
    }
  }

  // ON / OFF BUTTON -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  void onOffButton() {
    uint8_t previousBrightness = brightness;
    if (onOrOff) {
      FastLED.setBrightness(previousBrightness);
    } else {
      previousBrightness = FastLED.getBrightness();
      FastLED.setBrightness(0);
    }
    FastLED.show();
    Serial.println("Button");
  }

  BLYNK_WRITE(V3) {
    onOrOff = param.asInt();
    onOffButton();
  }

  // BRIGHTNESS SLIDER  ----------------------------------------------------------------------------------------------------------------------------------------------------------------
  void brightnessUpdate() {
    FastLED.setBrightness(brightness);
    if (onOrOff) {
      FastLED.show();
    }
  }

  BLYNK_WRITE(V4) {
    brightness = param.asInt();
    brightnessUpdate();

  }

  // SPEED SLIDER ---------------------------------------------------------------------------------------------------------------------------------------------------------------------

  BLYNK_WRITE(V5) {
    speed = param.asDouble();
  }

  // ZERGBA ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  BLYNK_WRITE(V1)
  {
    activePattern = 0;
    r = param[0].asInt();
    g = param[1].asInt();
    b = param[2].asInt();
    Serial.println("ZERGBA");
    farbeAnzeigen(r, g, b);
  }
  void farbeAnzeigen(int r, int g, int b)
  {
    FastLED.setBrightness(brightness);
    for (int i = 0; i < LENGTH; i++ )
    {
      strip[i] = CRGB(r, g, b);
    }
    Serial.println("ZERGBA called");
    if (onOrOff) {
      FastLED.show();
    }
  }

  //Alexa Methods -----------------------------------------------------------------------------------------------------------------------------------------------------------
  void regenbogenAlexa(EspalexaDevice * d) {
    if (d == nullptr) return;
    if (d->getValue()) {
      brightness = ((d->getPercent() * 255) / 100);
      brightnessUpdate();
      Serial.println("Alexa: Regenbogen");
      Serial.println(d->getPercent());
      Serial.println(brightness);
    } else {
      onOrOff = false;
      onOffButton();
    }

  }
  void farbwechselAlexa(EspalexaDevice * d) {
    if (d == nullptr) return;
    if (d->getValue()) {
      brightness = ((d->getPercent() * 255) / 100);
      brightnessUpdate();
      fadeSetup();
      activePattern = fadePat;
      Serial.println("Alexa: Farbwechsel");
    } else {
      onOrOff = false;
      onOffButton();
    }

  }
  void welleAlexa(EspalexaDevice * d) {
    if (d == nullptr) return;
    if (d->getValue()) {
      brightness = ((d->getPercent() * 255) / 100);
      brightnessUpdate();
      waveSetup();
      activePattern = wavePat;
      Serial.println("Alexa: Welle");
    } else {
      onOrOff = false;
      onOffButton();
    }

  }

  // MENU ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  BLYNK_WRITE(V2) {
    int menuParam = param.asInt();
    switch (menuParam) {
      //Light
      case rainbowUpPat: {
          rainbowSetup();
          break;
        }
      //Rainbow
      case rainbowRightPat: {
          rainbowSetup();
          break;
        }
      case rainbowOutPat: {
          rainbowSetup();
          break;
        }
      //Fade
      case fadePat: {
          fadeSetup();
          break;
        }
      case wavePat: {
          waveSetup();
          break;
        }
      case fill2Pat: {
          fill2Setup();
          break;
        }

      default: {
          break;
        }
    }
    activePattern = menuParam;
  }

  // SETUP ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  }

  void setup()
  { delay(200);

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // LEDs
    FastLED.addLeds<LED_TYPE, PIN1, COLOR_ORDER>(strip, LENGTH).setCorrection(TypicalLEDStrip);
    FastLED.setCorrection(TypicalPixelString);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 11600);

    // Wifi
    wifiSetup();

    Blynk.begin(auth, ssid, pass);

    //Alexa Initialisation
    espalexa.addDevice("Regenbogen", regenbogenAlexa, EspalexaDeviceType::dimmable);
    espalexa.addDevice("Farbwechsel", farbwechselAlexa, EspalexaDeviceType::dimmable);
    espalexa.addDevice("Welle", welleAlexa, EspalexaDeviceType::dimmable);
    espalexa.begin();

    sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQ));

    fill_solid(strip, LENGTH, CRGB(0, 0, 0));

  }

  void loop()
  {
    espalexa.loop();

    Blynk.run();
    if (((millis() - lastUpdate) * 10)  > interval / speed) {
      lastUpdate = millis();

      switch (activePattern) {
        case rainbowUpPat: {
            rainbowUpUpdate();
            break;
          }
        case rainbowRightPat: {
            rainbowRightUpdate();
            break;
          }
        case rainbowOutPat: {
            rainbowOutUpdate();
            break;
          }
        case fadePat: {
            fadeUpdate();
            break;
          }
        case wavePat: {
            waveUpdate();
            break;
          }
        case fill2Pat: {
            fill2Update();
            break;
          }
        case musicPat: {
            //musicUpdate();
            break;
          }
        case 20: {
            sunriseUpdate();
            break;
          }
        default: {
            break;
          }
      }
    }

  }
