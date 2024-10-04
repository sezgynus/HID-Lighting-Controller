#include <Microsoft_HidForWindows.h>
#include <Adafruit_NeoPixel.h>
#define NEO_PIXEL1_PIN A0
#define NEO_PIXEL2_PIN A3

#define LAMP_ARRAY1_COUNT 47

#define NEO_PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

Adafruit_NeoPixel ledStrip1 = Adafruit_NeoPixel(LAMP_ARRAY1_COUNT, NEO_PIXEL1_PIN, NEO_PIXEL_TYPE);

#define NEO_PIXEL_LAMP_UPDATE_LATENCY (0x04)

#include "lamp_attributes.h"

static_assert(((sizeof(LampAttributes1) / sizeof(LampAttributes)) == LAMP_ARRAY1_COUNT), "LampAttributes1 must have LAMP_ARRAY1_COUNT items.");

// All lengths in millimeters.
// All times in milliseconds.
Microsoft_HidLampArray lampArray1 = Microsoft_HidLampArray(LAMP_ARRAY1_COUNT, 360, 376, 1, LampArrayKindPeripheral, 33, LampAttributes1);

uint32_t lampArrayAutonomousColor = ledStrip1.Color(0, 0, 0);

void setup() {

  ledStrip1.begin();
  ledStrip1.clear();

  ledStrip1.fill(lampArrayAutonomousColor, 0, LAMP_ARRAY1_COUNT - 1);
  ledStrip1.show();
}

void loop() {
  LampArrayColor currentLampArrayState[LAMP_ARRAY1_COUNT];
  bool isAutonomousMode = lampArray1.getCurrentState(currentLampArrayState);
  bool update = false;
  for (uint16_t i = 0; i < LAMP_ARRAY1_COUNT; i++) {
    uint32_t newColor = isAutonomousMode ? lampArrayAutonomousColor : lampArrayColorToNeoPixelColor(currentLampArrayState[i]);
    if (newColor != ledStrip1.getPixelColor(i)) {
      ledStrip1.setPixelColor(i, newColor);
      update = true;
    }
  }
  if (update) {
    ledStrip1.show();
  }
}

uint32_t lampArrayColorToNeoPixelColor(LampArrayColor lampArrayColor) {
  return ledStrip1.Color(lampArrayColor.RedChannel, lampArrayColor.GreenChannel, lampArrayColor.BlueChannel);
}