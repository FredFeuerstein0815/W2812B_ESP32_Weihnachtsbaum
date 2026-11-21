#include <Adafruit_NeoPixel.h>

#define NUM_LEDS        300
#define DATA_PIN        16
#define LDR_PIN         34
#define LDR_SCHWELLE    1500 // 500 = sehr dunkel, 1500 =Dämmerung, 4095 = sehr hell
#define LDR_SAMPLES     5

#define NEO_GRB_KHZ800 (NEO_GRB + NEO_KHZ800)

#define DAUER_WEISS    1
#define DAUER_VIOLETT  20
#define DAUER_BLAU     30
#define DAUER_CYAN     40
#define DAUER_GRUEN    50
#define DAUER_GELB     60
#define DAUER_ORANGE   70
#define DAUER_ROT      80

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB_KHZ800);

int pos_weiss   = 296;
int pos_violett = 259;
int pos_blau    = 222;
int pos_cyan    = 185;
int pos_gruen   = 148;
int pos_gelb    = 111;
int pos_orange  = 74;
int pos_rot     = 37;

unsigned long letzesWeissUpdate   = 0;
unsigned long letzesViolettUpdate = 0;
unsigned long letzesBlauUpdate    = 0;
unsigned long letzesCyanUpdate    = 0;
unsigned long letzesGruenUpdate   = 0;
unsigned long letzesGelbUpdate    = 0;
unsigned long letzesOrangeUpdate  = 0;
unsigned long letzesRotUpdate     = 0;

bool warDunkel = false;

bool istDunkel() {
  long summe = 0;
  for (int i = 0; i < LDR_SAMPLES; i++) {
    summe += analogRead(LDR_PIN);
    delayMicroseconds(10000);
  }
  return (summe / LDR_SAMPLES) < LDR_SCHWELLE;
}

void stripAusschalten() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
  letzesWeissUpdate   = millis();
  letzesViolettUpdate = millis();
  letzesBlauUpdate    = millis();
  letzesCyanUpdate    = millis();
  letzesGruenUpdate   = millis();
  letzesGelbUpdate    = millis();
  letzesOrangeUpdate  = millis();
  letzesRotUpdate     = millis();
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("LED-Streifen startet …"));
  pinMode(LDR_PIN, INPUT);
  strip.begin();
  strip.setBrightness(255);
  strip.show();
}

void loop() {
//  Serial.print("LDR = "); Serial.println(analogRead(LDR_PIN));
  bool dunkel = istDunkel();

  if (!dunkel) {
    if (warDunkel) {
      Serial.println(F("\nZu hell, LEDs aus"));
      stripAusschalten();
      Serial.print("LDR = "); Serial.println(analogRead(LDR_PIN));
    }
    warDunkel = false;
    return;
  }

  if (!warDunkel) {
    Serial.println(F("\nDunkel genug, LEDs an"));
    Serial.print("LDR = "); Serial.println(analogRead(LDR_PIN));
    letzesWeissUpdate   = millis();
    letzesViolettUpdate = millis() - DAUER_VIOLETT;
    letzesBlauUpdate    = millis() - DAUER_BLAU;
    letzesCyanUpdate    = millis() - DAUER_CYAN;
    letzesGruenUpdate   = millis() - DAUER_GRUEN;
    letzesGelbUpdate    = millis() - DAUER_GELB;
    letzesOrangeUpdate  = millis() - DAUER_ORANGE;
    letzesRotUpdate     = millis() - DAUER_ROT;
  }
  warDunkel = true;

  unsigned long now = millis();
  bool changed = false;

  if (now - letzesWeissUpdate >= DAUER_WEISS) {
    letzesWeissUpdate = now;
    int alt = pos_weiss;
    pos_weiss = (pos_weiss + 1) % NUM_LEDS;
    if (alt != pos_rot) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_weiss, 255, 255, 255);
    changed = true;
  }

  if (now - letzesViolettUpdate >= DAUER_VIOLETT) {
    letzesViolettUpdate = now;
    int alt = pos_violett;
    pos_violett = (pos_violett + 1) % NUM_LEDS;
    if (alt != pos_violett) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_violett, 110, 0, 165);
    changed = true;
  }

  if (now - letzesBlauUpdate >= DAUER_BLAU) {
    letzesBlauUpdate = now;
    int alt = pos_blau;
    pos_blau = (pos_blau + 1) % NUM_LEDS;
    if (alt != pos_blau) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_blau, 0, 0, 255);
    changed = true;
  }

  if (now - letzesCyanUpdate >= DAUER_CYAN) {
    letzesCyanUpdate = now;
    int alt = pos_cyan;
    pos_cyan = (pos_cyan + 1) % NUM_LEDS;
    if (alt != pos_cyan) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_cyan, 0, 255, 63);
    changed = true;
  }

  if (now - letzesGruenUpdate >= DAUER_GRUEN) {
    letzesGruenUpdate = now;
    int alt = pos_gruen;
    pos_gruen = (pos_gruen + 1) % NUM_LEDS;
    if (alt != pos_gruen) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_gruen, 0, 255, 0);
    changed = true;
  }

  if (now - letzesGelbUpdate >= DAUER_GELB) {
    letzesGelbUpdate = now;
    int alt = pos_gelb;
    pos_gelb = (pos_gelb + 1) % NUM_LEDS;
    if (alt != pos_gelb) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_gelb, 236, 255, 0);
    changed = true;
  }

  if (now - letzesOrangeUpdate >= DAUER_ORANGE) {
    letzesOrangeUpdate = now;
    int alt = pos_orange;
    pos_orange = (pos_orange + 1) % NUM_LEDS;
    if (alt != pos_orange) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_orange, 255, 156, 0);
    changed = true;
  }

  if (now - letzesRotUpdate >= DAUER_ROT) {
    letzesRotUpdate = now;
    int alt = pos_rot;
    pos_rot = (pos_rot + 1) % NUM_LEDS;
    if (alt != pos_rot) strip.setPixelColor(alt, 0, 0, 0);
    strip.setPixelColor(pos_rot, 255, 0, 0);
    changed = true;
  }

  if (changed) strip.show();
}
