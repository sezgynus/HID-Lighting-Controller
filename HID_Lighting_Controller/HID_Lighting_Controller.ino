#include <Microsoft_HidForWindows.h>
#include <Adafruit_NeoPixel.h>
#define NEO_PIXEL1_PIN A0
#define NEO_PIXEL2_PIN A3

#define LAMP_ARRAY1_COUNT 47
#define LAMP_ARRAY2_COUNT 15

#define NEO_PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

Adafruit_NeoPixel ledStrip1 = Adafruit_NeoPixel(LAMP_ARRAY1_COUNT, NEO_PIXEL1_PIN, NEO_PIXEL_TYPE);
Adafruit_NeoPixel ledStrip2 = Adafruit_NeoPixel(LAMP_ARRAY2_COUNT, NEO_PIXEL2_PIN, NEO_PIXEL_TYPE);

#define NEO_PIXEL_LAMP_UPDATE_LATENCY (0x04)

#include "lamp_attributes.h"

static_assert(((sizeof(LampAttributes1) / sizeof(LampAttributes)) == LAMP_ARRAY1_COUNT), "LampAttributes1 must have LAMP_ARRAY1_COUNT items.");
static_assert(((sizeof(LampAttributes2) / sizeof(LampAttributes)) == LAMP_ARRAY2_COUNT), "LampAttributes1 must have LAMP_ARRAY2_COUNT items.");

// All lengths in millimeters.
// All times in milliseconds.
Microsoft_HidLampArray lampArray1 = Microsoft_HidLampArray(LAMP_ARRAY1_COUNT, 360, 376, 1, LampArrayKindPeripheral, 33, LampAttributes1);
Microsoft_HidLampArray lampArray2 = Microsoft_HidLampArray(LAMP_ARRAY2_COUNT, 120, 120, 1, LampArrayKindPeripheral, 33, LampAttributes2);

uint32_t lampArrayAutonomousColor = ledStrip1.Color(0, 0, 0);

void setup() {

  ledStrip1.begin();
  ledStrip1.clear();

  ledStrip1.fill(lampArrayAutonomousColor, 0, LAMP_ARRAY1_COUNT - 1);
  ledStrip1.show();

  ledStrip2.begin();
  ledStrip2.clear();

  ledStrip2.fill(lampArrayAutonomousColor, 0, LAMP_ARRAY2_COUNT - 1);
  ledStrip2.show();
}

void loop() {
  LampArrayColor currentLampArrayState1[LAMP_ARRAY1_COUNT];
  bool isAutonomousMode = lampArray1.getCurrentState(currentLampArrayState1);
  bool update = false;
  for (uint16_t i = 0; i < LAMP_ARRAY1_COUNT; i++) {
    uint32_t newColor = isAutonomousMode ? lampArrayAutonomousColor : lampArrayColorToNeoPixelColor(currentLampArrayState1[i]);
    if (newColor != ledStrip1.getPixelColor(i)) {
      ledStrip1.setPixelColor(i, newColor);
      update = true;
    }
  }
  if (update) {
    ledStrip1.show();
  }
  LampArrayColor currentLampArrayState2[LAMP_ARRAY2_COUNT];
  isAutonomousMode = lampArray2.getCurrentState(currentLampArrayState2);
  update = false;
  for (uint16_t i = 0; i < LAMP_ARRAY2_COUNT; i++) {
    uint32_t newColor = isAutonomousMode ? lampArrayAutonomousColor : lampArrayColorToNeoPixelColor(currentLampArrayState2[i]);
    if (newColor != ledStrip2.getPixelColor(i)) {
      ledStrip2.setPixelColor(i, newColor);
      update = true;
    }
  }
  if (update) {
    ledStrip2.show();
  }
}

uint32_t lampArrayColorToNeoPixelColor(LampArrayColor lampArrayColor) {
  return ledStrip1.Color(lampArrayColor.RedChannel, lampArrayColor.GreenChannel, lampArrayColor.BlueChannel);
}