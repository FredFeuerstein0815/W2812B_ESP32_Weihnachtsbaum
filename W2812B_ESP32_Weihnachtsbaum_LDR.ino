#include <Adafruit_NeoPixel.h>

#define NUM_LEDS         300
#define DATA_PIN         16
#define LDR_PIN          34
#define LDR_SCHWELLE     1000     // 500 = sehr dunkel, 1000 bis 1500 = Dämmerung, 4095 = sehr hell
#define LDR_SAMPLES      5
#define LDR_INTERVALL_MS 60000    // 60000 = 1x pro Minute
#define HELLIGKEIT_LEDS  128      // 1 = fast aus, 255 = maximale Helligkeit

#define NEO_GRB_KHZ800   (NEO_GRB + NEO_KHZ800)

#define DAUER_WEISS      1        // Verzögerung in ms, 1 = schnellste LED
#define DAUER_VIOLETT    26
#define DAUER_BLAU       41
#define DAUER_CYAN       56
#define DAUER_GRUEN      71
#define DAUER_GELB       86
#define DAUER_ORANGE     101
#define DAUER_ROT        116

#define ANIM_TICK_MS   1          // Grundtakt des Animations-Tasks
#define LED_SHOW_MS    10         // maximale Aktualisierungsrate der LEDs

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB_KHZ800);

// Zustand, der zwischen Tasks geteilt wird
volatile bool g_dunkel = false;

// Mutex schützt den Puffer vor gleichzeitigem Zugriff
SemaphoreHandle_t stripMutex;

// Farbdefinition
struct Farbe {
  uint8_t r, g, b;
  uint16_t dauer;
  int pos;
  TickType_t letzte;
};

Farbe farben[] = {
  {255, 255, 255, DAUER_WEISS,   296, 0},
  {110,   0, 165, DAUER_VIOLETT, 259, 0},
  {  0,   0, 255, DAUER_BLAU,    222, 0},
  {  0, 255,  63, DAUER_CYAN,    185, 0},
  {  0, 255,   0, DAUER_GRUEN,   148, 0},
  {236, 255,   0, DAUER_GELB,    111, 0},
  {255, 156,   0, DAUER_ORANGE,   74, 0},
  {255,   0,   0, DAUER_ROT,      37, 0},
};
const int ANZAHL_FARBEN = sizeof(farben) / sizeof(farben[0]);

void sensorTask(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    long summe = 0;
    for (int i = 0; i < LDR_SAMPLES; i++) {
      summe += analogRead(LDR_PIN);
      vTaskDelay(pdMS_TO_TICKS(2));
    }
    bool dunkel = (summe / LDR_SAMPLES) < LDR_SCHWELLE;
    g_dunkel = dunkel;
    Serial.print(F("LDR Mittelwert: "));
    Serial.println(summe / LDR_SAMPLES);
    vTaskDelay(pdMS_TO_TICKS(LDR_INTERVALL_MS));
  }
}

void animationTask(void *pvParameters) {
  (void)pvParameters;

  bool warDunkel = false;
  TickType_t letzteAusgabe = xTaskGetTickCount();
  TickType_t letzterTick   = xTaskGetTickCount();

  for (;;) {
    vTaskDelayUntil(&letzterTick, pdMS_TO_TICKS(ANIM_TICK_MS));
    TickType_t jetzt = xTaskGetTickCount();
    bool dunkel = g_dunkel;

    if (!dunkel) {
      if (warDunkel) {
        Serial.println(F("Zu hell, LEDs aus"));
        xSemaphoreTake(stripMutex, portMAX_DELAY);
        strip.clear();
        strip.show();
        xSemaphoreGive(stripMutex);
      }
      warDunkel = false;
      continue;
    }

    if (!warDunkel) {
      Serial.println(F("Dunkel genug, LEDs an"));
      for (int i = 0; i < ANZAHL_FARBEN; i++) {
        farben[i].letzte = (i == 0) ? jetzt : jetzt - pdMS_TO_TICKS(farben[i].dauer);
      }
    }
    warDunkel = true;
    bool geaendert = false;
    xSemaphoreTake(stripMutex, portMAX_DELAY);
    for (int i = 0; i < ANZAHL_FARBEN; i++) {
      Farbe &f = farben[i];
      if ((jetzt - f.letzte) >= pdMS_TO_TICKS(f.dauer)) {
        f.letzte = jetzt;
        int alt = f.pos;
        f.pos = (f.pos + 1) % NUM_LEDS;
        bool belegt = false;
        for (int j = 0; j < ANZAHL_FARBEN; j++) {
          if (farben[j].pos == alt) { belegt = true; break; }
        }
        if (!belegt) strip.setPixelColor(alt, 0, 0, 0);
        strip.setPixelColor(f.pos, f.r, f.g, f.b);
        geaendert = true;
      }
    }

    if (geaendert && (jetzt - letzteAusgabe) >= pdMS_TO_TICKS(LED_SHOW_MS)) {
      strip.show();
      letzteAusgabe = jetzt;
    }
    xSemaphoreGive(stripMutex);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starte"));
  pinMode(LDR_PIN, INPUT);
  strip.begin();
  strip.setBrightness(HELLIGKEIT_LEDS);
  strip.show();
  stripMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(sensorTask,    "LDR",       2048, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(animationTask, "Animation", 4096, nullptr, 2, nullptr, 1);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
