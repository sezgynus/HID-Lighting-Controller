# HID Lighting Controller

[English](README.md) | [Türkçe](README-tr.md)

**USB HID LampArray firmware for driving two independent addressable RGB LED installations from a USB-native Arduino-compatible controller.**

The firmware exposes **two separate HID LampArray devices** to the host. Host-provided per-lamp RGB states are translated into NeoPixel output for a 47-LED installation and a 15-LED installation, for a total of **62 individually addressable lamps**.

The implementation uses `Microsoft_HidForWindows` for the HID Lighting and Illumination interface and `Adafruit_NeoPixel` for the physical LED outputs.

```text
                 USB host / lighting software
                           │
                           │ USB HID LampArray
                           ▼
              ┌──────────────────────────┐
              │   USB-native controller  │
              │                          │
              │  Microsoft_HidLampArray  │
              │       #1       #2        │
              │        │         │        │
              │        ▼         ▼        │
              │   RGB state   RGB state   │
              │        │         │        │
              │   NeoPixel   NeoPixel     │
              └───────┬─────────┬────────┘
                      │         │
                     A0        A3
                      │         │
                      ▼         ▼
                  47 LEDs    15 LEDs
                 L-shaped     circular
                  layout       layout
```

## Project Overview

HID Lighting Controller implements the host-to-lighting path rather than a proprietary serial or vendor-specific RGB protocol.

Each physical LED is described to the host with a `LampAttributes` record containing its identifier, three-dimensional position, update latency, purpose, supported color channels, intensity gain, programmability flag, and key association.

The host can therefore treat the installation as spatially described lamps rather than as an anonymous LED strip.

At runtime the firmware:

1. initializes both NeoPixel outputs;
2. clears both physical LED arrays;
3. starts them in the autonomous color, currently black;
4. repeatedly obtains the current state of LampArray 1 from the HID library;
5. translates its RGB values into NeoPixel colors;
6. updates only pixels whose color changed;
7. calls `show()` only when at least one pixel in that array changed;
8. repeats the same process independently for LampArray 2.

## At a Glance

| Area | LampArray 1 | LampArray 2 |
|---|---:|---:|
| Physical LEDs | 47 | 15 |
| Data pin | A0 | A3 |
| Reported dimensions | 360 × 376 × 1 mm | 120 × 120 × 1 mm |
| Geometry | Two-edge / L-shaped path | Circular path |
| Lamp IDs | `0x00`–`0x2E` | `0x00`–`0x0E` |
| Lamp purpose | Accent | Accent |
| Per-lamp update latency | 4 ms | 4 ms |
| LampArray minimum update interval | 33 ms | 33 ms |
| Programmable | Yes | Yes |
| RGB logical maximum | 255 / 255 / 255 | 255 / 255 / 255 |
| Intensity gain | 1 | 1 |
| NeoPixel format | GRB, 800 kHz | GRB, 800 kHz |

Total physical lamp count: **62**.

## HID LampArray Architecture

Two independent `Microsoft_HidLampArray` objects are constructed:

```cpp
Microsoft_HidLampArray lampArray1 = Microsoft_HidLampArray(
    47, 360, 376, 1,
    LampArrayKindPeripheral,
    33,
    LampAttributes1
);

Microsoft_HidLampArray lampArray2 = Microsoft_HidLampArray(
    15, 120, 120, 1,
    LampArrayKindPeripheral,
    33,
    LampAttributes2
);
```

The two arrays are not merged into one 62-lamp coordinate system. Each has its own dimensions, attribute table, HID state, and NeoPixel output.

Both are declared as `LampArrayKindPeripheral`.

## Physical LED Outputs

The firmware creates two independent `Adafruit_NeoPixel` objects:

| Output | Pin | LED count | Format |
|---|---:|---:|---|
| `ledStrip1` | A0 | 47 | `NEO_GRB + NEO_KHZ800` |
| `ledStrip2` | A3 | 15 | `NEO_GRB + NEO_KHZ800` |

The code assumes LEDs compatible with an 800 kHz NeoPixel-style protocol and GRB channel ordering.

The repository defines the signal pins and logical LED topology, but it does **not** document the LED supply voltage, power-distribution design, level shifting, controller board revision, or maximum current budget. Those electrical details should therefore be selected for the actual LED hardware rather than inferred from this firmware.

## Lamp Geometry

### LampArray 1 — 47 LEDs

The first array describes an L-shaped/two-edge installation.

IDs `0x00` through `0x16` progress along the X axis with Y fixed at zero:

```text
(0,0) → (16,0) → (32,0) → ... → (352,0)
```

The remaining LEDs turn the corner and progress down the Y axis at X = 360:

```text
(360,8)
(360,24)
(360,40)
   ...
(360,376)
```

Conceptually:

```text
0x00 ─ 0x01 ─ 0x02 ─ ... ─ 0x16
                              │
                            0x17
                              │
                            0x18
                              │
                              ⋮
                              │
                            0x2E
```

The `Microsoft_HidLampArray` dimensions are declared as **360 × 376 × 1 mm**.

### LampArray 2 — 15 LEDs

The second array describes 15 positions distributed around a roughly circular 120 × 120 mm area.

Its coordinates begin near the right side at `(116, 51)`, progress clockwise through the lower half, left side and upper half, and finish near `(108, 30)`.

Conceptually:

```text
             0x0B  0x0C
        0x0A             0x0D
    0x09                     0x0E
 0x08                           0x00
 0x07                           0x01
    0x06                     0x02
        0x05             0x03
              0x04
```

The positions are explicit integer coordinates from `lamp_attributes.h`; the diagram above is only a visualization of their ordering.

## Lamp Attributes

Every lamp entry has the same capability configuration apart from its ID and position:

| Attribute | Value |
|---|---|
| Z coordinate | 0 |
| Update latency | 4 ms |
| Purpose | `LampPurposeAccent` |
| Red logical maximum | `0xFF` |
| Green logical maximum | `0xFF` |
| Blue logical maximum | `0xFF` |
| Intensity gain | `0x01` |
| Programmability | `LAMP_IS_PROGRAMMABLE` |
| Input key association | `0x00` |

The coordinate comments in the source define positions in millimeters from the upper-left corner of the device.

## Runtime Data Flow

For each LampArray, the main loop performs the same sequence:

```text
Microsoft_HidLampArray
        │
        │ getCurrentState()
        ▼
LampArrayColor[N]
        │
        │ autonomous?
        ├────────────── yes ──► black
        │
        └────────────── no
                        │
                        ▼
             Red / Green / Blue
                        │
                        ▼
          NeoPixel packed GRB color
                        │
                        ▼
          compare with current pixel
                        │
             changed? ──┴── no → skip
                │
               yes
                ▼
         setPixelColor()
                │
                ▼
       at least one change?
                │
               yes
                ▼
              show()
```

This change-detection step avoids calling `show()` when the complete array already matches the requested state.

The two arrays are processed sequentially and maintain separate `update` flags.

## Autonomous Mode

The firmware defines:

```cpp
uint32_t lampArrayAutonomousColor = ledStrip1.Color(0, 0, 0);
```

When `getCurrentState()` indicates autonomous mode, the corresponding physical array is driven with this color instead of the host-provided lamp states.

Because the autonomous color is currently **RGB(0, 0, 0)**, autonomous mode means **LEDs off**.

There is no standalone animation engine, effect selector, serial command interface, or local color-control UI in this repository.

## Color Conversion

Host colors are represented as `LampArrayColor`. The firmware converts them with:

```cpp
return ledStrip1.Color(
    lampArrayColor.RedChannel,
    lampArrayColor.GreenChannel,
    lampArrayColor.BlueChannel
);
```

Although the helper uses `ledStrip1.Color()` for both arrays, the function only packs the supplied RGB components into the NeoPixel library's color representation; the resulting packed value is then used for either physical strip.

The physical NeoPixel objects themselves are configured for **GRB** ordering.

## Update Timing

There are two distinct timing values in the source:

- `NEO_PIXEL_LAMP_UPDATE_LATENCY = 0x04` → **4 ms**, reported in every lamp's attributes.
- LampArray constructor interval → **33 ms** for each HID LampArray.

These values describe different layers and should not be treated as interchangeable.

The main Arduino loop itself contains no explicit `delay()`.

## Initialization Sequence

`setup()` initializes the arrays independently:

```text
ledStrip1.begin()
ledStrip1.clear()
ledStrip1.fill(black, 0, 46)
ledStrip1.show()

ledStrip2.begin()
ledStrip2.clear()
ledStrip2.fill(black, 0, 14)
ledStrip2.show()
```

Both outputs therefore begin in the configured autonomous color before normal host-state processing starts.

## Compile-Time Consistency Checks

The firmware deliberately checks that each physical LED count matches its attribute table:

```cpp
static_assert(
    sizeof(LampAttributes1) / sizeof(LampAttributes)
        == LAMP_ARRAY1_COUNT
);

static_assert(
    sizeof(LampAttributes2) / sizeof(LampAttributes)
        == LAMP_ARRAY2_COUNT
);
```

If an LED count is changed without updating the corresponding `LampAttributes` array, compilation fails instead of silently exposing inconsistent HID metadata.

## Customizing the Layout

When adapting the firmware to another installation, keep four layers synchronized:

1. **Physical count** — `LAMP_ARRAY1_COUNT` / `LAMP_ARRAY2_COUNT`.
2. **Output wiring** — `NEO_PIXEL1_PIN` / `NEO_PIXEL2_PIN`.
3. **HID geometry** — every entry in `LampAttributes1` / `LampAttributes2`.
4. **Reported enclosure dimensions** — width, height and depth passed to each `Microsoft_HidLampArray` constructor.

The order of each attribute array should match the physical order of the addressable LEDs. A host uses these coordinates to understand where each reported lamp exists in the installation.

If the number of lamps changes, update both the count constant and the corresponding attribute table; the compile-time assertions will catch a count mismatch.

## Requirements

The source directly depends on:

- `Microsoft_HidForWindows`;
- `Adafruit_NeoPixel`;
- an Arduino-compatible USB-native target supported by the HID library;
- two addressable RGB LED chains compatible with the selected NeoPixel timing/order;
- a host environment/application capable of controlling HID LampArray devices.

The repository does **not** pin:

- a specific board definition;
- Arduino core version;
- `Microsoft_HidForWindows` version;
- `Adafruit_NeoPixel` version.

The firmware has been structured around native USB HID support; boards that only expose a USB-to-serial converter cannot provide the same HID device behavior without different hardware/USB support.

## Building

1. Open `HID_Lighting_Controller/HID_Lighting_Controller.ino` in the Arduino development environment.
2. Install `Microsoft_HidForWindows` and `Adafruit_NeoPixel`.
3. Select a compatible USB-native board/core.
4. Connect the first LED data input to A0 and the second to A3.
5. Provide the LED installation with an electrically appropriate power supply and common reference with the controller as required by the hardware.
6. Compile and upload the firmware.
7. Connect the controller to the host over USB.
8. Use a HID LampArray-capable host application to take control of the lamps.

## Troubleshooting

### The HID lighting device does not appear

Check the selected board/core for native USB HID support and compatibility with `Microsoft_HidForWindows`. Also verify that the USB cable carries data rather than power only.

### LEDs remain off

Black is the configured autonomous color. Verify that the host has taken control of the LampArray and is actually sending non-zero lamp colors.

### Colors are incorrect

The physical outputs are configured as `NEO_GRB + NEO_KHZ800`. Confirm that the installed LEDs use the same channel ordering and timing.

### Spatial effects appear in the wrong locations

Compare the physical LED order against the corresponding `LampAttributes` table. HID coordinates describe logical lamp positions, while array order determines which physical LED receives each state.

### Compilation fails after changing LED count

Update the corresponding `LampAttributes` array. The `static_assert` checks intentionally reject count mismatches.

## Source Map

| File | Responsibility |
|---|---|
| `HID_Lighting_Controller/HID_Lighting_Controller.ino` | Creates the two HID LampArrays and NeoPixel outputs, initializes LEDs, reads host state, performs color conversion and updates changed pixels |
| `HID_Lighting_Controller/lamp_attributes.h` | Defines the ID, physical coordinates and HID capabilities of all 62 lamps |
| `.github/workflows/sign-commits.yml` | Repository commit-signing workflow |

## Repository Structure

```text
HID-Lighting-Controller/
├── .github/
│   └── workflows/
│       └── sign-commits.yml
├── HID_Lighting_Controller/
│   ├── HID_Lighting_Controller.ino
│   └── lamp_attributes.h
├── README.md
└── README-tr.md
```

## Current Scope

The repository implements the firmware path from **USB HID LampArray state → two physical NeoPixel arrays**.

It does not currently contain hardware schematics, PCB files, a desktop control application, custom USB VID/PID configuration, a standalone effect engine, or pinned build-environment metadata. Those items are therefore not inferred by this documentation.
