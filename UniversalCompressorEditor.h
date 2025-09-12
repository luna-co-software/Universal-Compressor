#pragma once

#include "UniversalCompressor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
#include <memory>

//==============================================================================
// Custom LookAndFeel for modern styling
class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel();
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                         juce::Slider& slider) override;
                         
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& box) override;
                     
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
                         
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                             bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class UniversalCompressorEditor : public juce::AudioProcessorEditor,
                                  private juce::Timer,
                                  private juce::AudioProcessorValueTreeState::Listener
{
public:
    UniversalCompressorEditor(UniversalCompressor&);
    ~UniversalCompressorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    // Custom look and feel
    ModernLookAndFeel modernLookAndFeel;
    
    // Custom components  
    class GainReductionMeter;
    class VUMeter;
    
    // Reference to processor
    UniversalCompressor& processor;
    
    // Meters (right column)
    std::unique_ptr<VUMeter> inputVUMeter;
    std::unique_ptr<GainReductionMeter> gainReductionMeter;
    std::unique_ptr<VUMeter> outputVUMeter;
    
    // Global controls (top row)
    std::unique_ptr<juce::ComboBox> modeCombo;
    std::unique_ptr<juce::ToggleButton> bypassToggle;
    std::unique_ptr<juce::ToggleButton> oversampleToggle;
    
    // ModePanel struct for organized mode-specific controls
    struct ModePanel
    {
        std::unique_ptr<juce::Component> panel;
        std::vector<std::unique_ptr<juce::Slider>> knobs;
        std::vector<std::unique_ptr<juce::ComboBox>> combos;
        std::vector<std::unique_ptr<juce::ToggleButton>> toggles;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
        std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> buttonAttachments;
    };
    
    // Mode panels
    std::array<ModePanel, 4> modePanels; // Opto, FET, VCA, Bus
    int currentMode = 0;
    
    // Global parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> oversampleAttachment;
    
    // Static helper functions for control creation
    static juce::Slider*       addKnob(ModePanel& panel, const juce::String& text, float min, float max, 
                                       float defaultVal, const juce::String& suffix = "");
    static juce::ComboBox*     addCombo(ModePanel& panel, const juce::String& text, 
                                       const juce::StringArray& items);
    static juce::ToggleButton* addToggle(ModePanel& panel, const juce::String& text);
    
    // Helper methods
    void updateMeters();
    void updateModePanel();
    void setupModePanel(int mode);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UniversalCompressorEditor)
};