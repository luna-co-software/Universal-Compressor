# Universal Compressor - Dynamic Analog UI Implementation

## Overview
This implementation provides a complete analog-style, dynamic UI system for the Universal Compressor VST3 plugin. The UI dynamically changes its appearance and controls based on the selected compressor type, emulating classic hardware units.

## Features

### 1. Four Distinct Compressor Modes

#### Opto Mode (LA-2A Style)
- **Color Scheme**: Warm cream background with vintage red accents
- **Controls**: Peak Reduction knob, Gain knob, Compress/Limit switch
- **Aesthetic**: Vintage bakelite-style knobs with cream pointers

#### FET Mode (1176 Style)
- **Color Scheme**: Blackface panel with blue accents
- **Controls**: Input, Output, Attack, Release knobs, Ratio button group
- **Aesthetic**: Metallic knobs with precise white markings

#### VCA Mode (DBX 160 Style)
- **Color Scheme**: Retro beige with orange accents
- **Controls**: Threshold, Ratio, Attack, Release, Output, Over Easy switch
- **Aesthetic**: Professional studio look with LED indicators

#### Bus Mode (SSL G Style)
- **Color Scheme**: Modern dark blue-gray with gold accents
- **Controls**: Threshold, Ratio, Attack/Release selectors, Makeup gain, Auto Release
- **Aesthetic**: High-end console appearance

### 2. Metering System

#### Analog VU Meter (Center)
- Classic needle-style meter showing gain reduction
- Realistic ballistics (300ms attack/release)
- Red zone indication for heavy compression
- Glass reflection effect for authenticity

#### LED Meters (Sides)
- Vertical LED bargraph meters for input/output levels
- Color-coded segments (green → yellow → orange → red)
- Glow effects for active LEDs

### 3. Dynamic UI Components

#### Custom LookAndFeel Classes
Each mode has its own `LookAndFeel` class with unique:
- Knob rendering (metallic vs vintage)
- Button styles (rectangular vs toggle switches)
- Color schemes matching the hardware
- Typography and label styles

#### Responsive Layout
- Controls reposition based on selected mode
- Smooth transitions between modes
- Proper spacing for analog aesthetic

## Implementation Files

### Core Components

1. **AnalogLookAndFeel.h/cpp**
   - Base class for analog styling
   - Mode-specific LookAndFeel implementations
   - Custom meter components

2. **EnhancedCompressorEditor.h/cpp**
   - Main editor implementation
   - Dynamic panel management
   - Mode switching logic

3. **UniversalCompressorEditor.h/cpp**
   - Original editor (kept for compatibility)
   - Can be swapped with enhanced version

## Usage

### To Use Enhanced Editor

In your processor's `createEditor()` method:

```cpp
juce::AudioProcessorEditor* UniversalCompressor::createEditor()
{
    return new EnhancedCompressorEditor(*this);
}
```

### Required Parameters

The editor expects these parameters in your AudioProcessorValueTreeState:

**Global:**
- `mode` (0-3): Compressor type selection
- `bypass`: Bypass toggle
- `oversample`: Oversampling toggle

**Mode-Specific Parameters:**

Opto:
- `opto_peak_reduction` (0-100%)
- `opto_gain` (-20 to +20 dB)
- `opto_limit` (toggle)

FET:
- `fet_input` (0-10)
- `fet_output` (-20 to +20 dB)
- `fet_attack` (0.02-0.8 ms)
- `fet_release` (0.05-1.1 s)
- `fet_ratio` (combo: 4:1, 8:1, 12:1, 20:1)

VCA:
- `vca_threshold` (-40 to 0 dB)
- `vca_ratio` (1-20:1)
- `vca_attack` (0.1-100 ms)
- `vca_release` (10-1000 ms)
- `vca_output` (-20 to +20 dB)
- `vca_overeasy` (toggle)

Bus:
- `bus_threshold` (-20 to 0 dB)
- `bus_ratio` (2-10:1)
- `bus_attack` (combo: 0.1/0.3/1/3/10/30 ms)
- `bus_release` (combo: 0.1/0.3/0.6/1.2 s, Auto)
- `bus_makeup` (-10 to +20 dB)
- `bus_autorelease` (toggle)

### Meter Integration

The processor should provide these methods:
```cpp
float getInputLevel();    // Returns input level in dB
float getGainReduction(); // Returns GR in dB (negative)
float getOutputLevel();   // Returns output level in dB
```

## Build Instructions

1. Add the new files to your CMakeLists.txt:
```cmake
target_sources(UniversalCompressor
    PRIVATE
        AnalogLookAndFeel.cpp
        EnhancedCompressorEditor.cpp
        # ... other sources
)
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
make -j4
```

## Customization

### Adding New Compressor Types

1. Create a new LookAndFeel class inheriting from `AnalogLookAndFeelBase`
2. Define color scheme and drawing methods
3. Add a new panel struct in the editor
4. Update `updateMode()` method to handle the new type

### Modifying Aesthetics

Each LookAndFeel class can be customized:
- Adjust color schemes in constructors
- Modify knob rendering in `drawRotarySlider()`
- Change button styles in respective draw methods
- Add textures or gradients for more realism

## Performance Considerations

- Meters update at 30 Hz (configurable in timer)
- VU meter uses realistic ballistics simulation
- Efficient repainting only when values change significantly
- Background texture created once and reused

## Thread Safety

- All parameter changes use AudioProcessorValueTreeState attachments
- Meter updates are timer-based (not audio thread)
- Mode switching is handled on the message thread

## Best Practices

1. **Parameter Mapping**: Ensure all parameters exist before creating attachments
2. **Look Consistency**: Each mode maintains its unique aesthetic throughout
3. **Responsive Design**: Controls scale properly with window resizing
4. **User Experience**: Smooth transitions and intuitive control placement

## Testing

Test each mode for:
- Visual consistency with hardware inspiration
- Control responsiveness
- Meter accuracy
- Mode switching smoothness
- Parameter persistence

## Future Enhancements

Potential improvements:
- Sidechain input visualization
- Spectrum analyzer overlay
- Preset management UI
- A/B comparison mode
- Resizable window support
- Additional vintage compressor emulations