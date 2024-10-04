// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License.

#include <Microsoft_HidForWindows.h>
#include <Adafruit_NeoPixel.h>

//
// Adafruit NeoPixel-Shield - 40 RGB/W - https://www.adafruit.com/product/3053
//

// NeoPixel-Shield always communicates over PIN6.
// However since the shield operates at 5V, and the SAMD-board at 3.3V, a level-shifter between board-output to shield-input was introduced.
// This necessitated the remapping of PIN5 (board) to PIN6 (shield)
#define NEO_PIXEL1_PIN A0
#define NEO_PIXEL2_PIN A3

// NeoPixel-Shield has 40 neopixels
#define NEO_PIXEL_LAMP_COUNT 47

// NeoPixel-Shield (as used) is GRBW.
// Note: This must be determined for each NeoPixel-Shield used.
#define NEO_PIXEL_TYPE (NEO_GRB + NEO_KHZ800)

Adafruit_NeoPixel neoPixelShield = Adafruit_NeoPixel(NEO_PIXEL_LAMP_COUNT, NEO_PIXEL1_PIN, NEO_PIXEL_TYPE);

// UpdateLatency for all Lamps set to 4msec as it just seems reasonable.
#define NEO_PIXEL_LAMP_UPDATE_LATENCY (0x04)

// The Host needs to know the location of every Lamp in the LampArray (X/Y/Z position) and other metadata.
// See "26.7 LampArray Attributes and Interrogation" https://usb.org/sites/default/files/hut1_4.pdf#page=336
static LampAttributes neoPixelShieldLampAttributes[] PROGMEM = {
  // All positions in millimeters from upper-left corner of device.
  // All times in milliseconds.
  // Id  X     Y     Z     Latency                        Purposes           RED   GRN   BLUE  GAIN  PROGAMMBLE?           KEY
  { 0x00, 0x000, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x01, 0x010, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x02, 0x020, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x03, 0x030, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x04, 0x040, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x05, 0x050, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x06, 0x060, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x07, 0x070, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x08, 0x080, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x09, 0x090, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x0A, 0x0A0, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x0B, 0x0B0, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x0C, 0x0C0, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x0D, 0x0D0, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x0E, 0x0E0, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x0F, 0x0F0, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x10, 0x100, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x11, 0x110, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x12, 0x120, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x13, 0x130, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x14, 0x140, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x15, 0x150, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x16, 0x160, 0x000, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x17, 0x168, 0x008, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x18, 0x168, 0x018, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x19, 0x168, 0x028, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x1A, 0x168, 0x038, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x1B, 0x168, 0x048, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x1C, 0x168, 0x058, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x1D, 0x168, 0x068, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x1E, 0x168, 0x078, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x1F, 0x168, 0x088, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x20, 0x168, 0x098, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x21, 0x168, 0x0A8, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x22, 0x168, 0x0B8, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x23, 0x168, 0x0C8, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x24, 0x168, 0x0D8, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x25, 0x168, 0x0E8, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x26, 0x168, 0x0F8, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x27, 0x168, 0x108, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x28, 0x168, 0x118, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x29, 0x168, 0x128, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x2A, 0x168, 0x138, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x2B, 0x168, 0x148, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x2C, 0x168, 0x158, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x2D, 0x168, 0x168, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
  { 0x2E, 0x168, 0x178, 0x00, NEO_PIXEL_LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, LAMP_IS_PROGRAMMABLE, 0x00 },
};
static_assert(((sizeof(neoPixelShieldLampAttributes) / sizeof(LampAttributes)) == NEO_PIXEL_LAMP_COUNT), "neoPixelShieldLampAttributes must have NEO_PIXEL_LAMP_COUNT items.");

// All lengths in millimeters.
// All times in milliseconds.
Microsoft_HidLampArray lampArray = Microsoft_HidLampArray(NEO_PIXEL_LAMP_COUNT, 360, 376, 1, LampArrayKindPeripheral, 33, neoPixelShieldLampAttributes);

// When the LampArray is in Autonomous-Mode, displays solid blue.
uint32_t lampArrayAutonomousColor = neoPixelShield.Color(0, 0, 0);

void setup() {
  SerialUSB.begin(9600);
  // Initialize the NeoPixel library.
  neoPixelShield.begin();
  neoPixelShield.clear();

  // Always initially in Autonomous-Mode.
  neoPixelShield.fill(lampArrayAutonomousColor, 0, NEO_PIXEL_LAMP_COUNT - 1);
  neoPixelShield.show();
}

void loop() {
  LampArrayColor currentLampArrayState[NEO_PIXEL_LAMP_COUNT];
  bool isAutonomousMode = lampArray.getCurrentState(currentLampArrayState);
  bool update = false;
  for (uint16_t i = 0; i < NEO_PIXEL_LAMP_COUNT; i++) {
    uint32_t newColor = isAutonomousMode ? lampArrayAutonomousColor : lampArrayColorToNeoPixelColor(currentLampArrayState[i]);
    if (newColor != neoPixelShield.getPixelColor(i)) {
      neoPixelShield.setPixelColor(i, newColor);
      update = true;
    }
  }
  if (update) {
    neoPixelShield.show();
  }
}

uint32_t lampArrayColorToNeoPixelColor(LampArrayColor lampArrayColor) {
  return neoPixelShield.Color(lampArrayColor.RedChannel, lampArrayColor.GreenChannel, lampArrayColor.BlueChannel);
}