#include "UniversalCompressorEditor.h"
#include "UniversalCompressor.h"
#include <cmath>

//==============================================================================
// ModernLookAndFeel Implementation (keeping existing)
ModernLookAndFeel::ModernLookAndFeel()
{
    // Set modern colors
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF4A9EFF));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF2A2A2A));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xFFFFFFFF));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xFF2A2A2A));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xFFE0E0E0));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xFF4A4A4A));
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF3A3A3A));
    setColour(juce::TextButton::textColourOffId, juce::Colour(0xFFE0E0E0));
    setColour(juce::TextButton::textColourOnId, juce::Colour(0xFFFFFFFF));
}

void ModernLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                                        juce::Slider&)
{
    auto radius = (float) juce::jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // Draw outer ring with gradient shadow
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.fillEllipse(rx - 2, ry - 2, rw + 4, rw + 4);
    
    // Draw main knob body with gradient
    juce::ColourGradient gradient(juce::Colour(0xFF4A4A4A), centreX, ry,
                                 juce::Colour(0xFF2A2A2A), centreX, ry + rw, false);
    g.setGradientFill(gradient);
    g.fillEllipse(rx, ry, rw, rw);
    
    // Draw inner highlight
    g.setColour(juce::Colour(0xFF6A6A6A));
    g.fillEllipse(rx + 2, ry + 2, rw - 4, rw - 4);
    
    // Draw track (background arc)
    juce::Path track;
    track.addCentredArc(centreX, centreY, radius - 2, radius - 2, 0.0f, 
                       rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xFF2A2A2A));
    g.strokePath(track, juce::PathStrokeType(4.0f));
    
    // Draw filled arc (value indication)
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius - 2, radius - 2, 0.0f, 
                          rotaryStartAngle, angle, true);
    g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    g.strokePath(valueArc, juce::PathStrokeType(4.0f));
    
    // Draw pointer
    juce::Path pointer;
    auto pointerLength = radius * 0.6f;
    auto pointerThickness = 3.0f;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius + 6, pointerThickness, pointerLength);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(juce::Colour(0xFFFFFFFF));
    g.fillPath(pointer);
}

void ModernLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                    int, int, int, int,
                                    juce::ComboBox& box)
{
    auto cornerSize = 4.0f;
    juce::Rectangle<int> boxBounds(0, 0, width, height);
    
    // Background with gradient
    juce::ColourGradient gradient(juce::Colour(0xFF3A3A3A), 0, 0,
                                 juce::Colour(0xFF2A2A2A), 0, height, false);
    if (isButtonDown)
        gradient = juce::ColourGradient(juce::Colour(0xFF2A2A2A), 0, 0,
                                       juce::Colour(0xFF1A1A1A), 0, height, false);
    
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
    
    // Border
    g.setColour(findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f), cornerSize, 1.0f);
    
    // Arrow
    juce::Rectangle<int> arrowZone(width - 20, 0, 16, height);
    juce::Path path;
    path.startNewSubPath((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 3.0f);
    path.lineTo((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
    path.lineTo((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 3.0f);
    
    g.setColour(findColour(juce::ComboBox::textColourId).withAlpha(box.isEnabled() ? 0.9f : 0.2f));
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void ModernLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto fontSize = juce::jmin(15.0f, (float) button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 1.1f;
    
    drawTickBox(g, button, 4.0f, ((float) button.getHeight() - tickWidth) * 0.5f,
                tickWidth, tickWidth, button.getToggleState(),
                button.isEnabled(), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    
    g.setColour(button.findColour(juce::ToggleButton::textColourId));
    g.setFont(fontSize);
    
    if (! button.isEnabled())
        g.setOpacity(0.5f);
    
    g.drawFittedText(button.getButtonText(),
                     button.getLocalBounds().withTrimmedLeft(juce::roundToInt(tickWidth) + 10)
                                           .withTrimmedRight(2),
                     juce::Justification::centredLeft, 10);
}

void ModernLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, 
                                           const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto cornerSize = 6.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    
    auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
                                     .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);
    
    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);
    
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerSize);
    
    g.setColour(button.findColour(juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
}

//==============================================================================
// VU Meter Implementation
class UniversalCompressorEditor::VUMeter : public juce::Component
{
public:
    VUMeter() = default;
    
    void setLevel(float newLevel) 
    {
        if (std::abs(newLevel - currentLevel) > 0.1f)
        {
            currentLevel = newLevel;
            repaint();
        }
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto meterBounds = bounds.reduced(2.0f);
        
        // Background
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.fillRoundedRectangle(meterBounds, 3.0f);
        
        // Calculate meter fill based on dB value
        auto normalizedLevel = juce::jlimit(0.0f, 1.0f, (currentLevel + 60.0f) / 60.0f);
        auto fillHeight = meterBounds.getHeight() * normalizedLevel;
        auto fillBounds = juce::Rectangle<float>(meterBounds.getX(), 
                                               meterBounds.getBottom() - fillHeight,
                                               meterBounds.getWidth(), fillHeight);
        
        // Color zones
        juce::Colour meterColor = juce::Colour(0xFF00FF00); // Green
        if (currentLevel > -18.0f) meterColor = juce::Colour(0xFFFFFF00); // Yellow
        if (currentLevel > -6.0f) meterColor = juce::Colour(0xFFFF6600);  // Orange
        if (currentLevel > -3.0f) meterColor = juce::Colour(0xFFFF0000);  // Red
        
        g.setColour(meterColor);
        g.fillRect(fillBounds);
        
        // Peak hold line
        if (peakLevel > -60.0f)
        {
            auto peakNormalized = juce::jlimit(0.0f, 1.0f, (peakLevel + 60.0f) / 60.0f);
            auto peakY = meterBounds.getBottom() - meterBounds.getHeight() * peakNormalized;
            g.setColour(juce::Colours::white);
            g.drawHorizontalLine(int(peakY), meterBounds.getX(), meterBounds.getRight());
        }
        
        // Border
        g.setColour(juce::Colour(0xFF4A4A4A));
        g.drawRoundedRectangle(meterBounds, 3.0f, 1.0f);
    }
    
private:
    float currentLevel = -60.0f;
    float peakLevel = -60.0f;
};

//==============================================================================
// Gain Reduction Meter Implementation
class UniversalCompressorEditor::GainReductionMeter : public juce::Component
{
public:
    GainReductionMeter() = default;
    
    void setGainReduction(float newGR)
    {
        if (std::abs(newGR - gainReduction) > 0.1f)
        {
            gainReduction = newGR;
            repaint();
        }
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto meterBounds = bounds.reduced(2.0f);
        
        // Background
        g.setColour(juce::Colour(0xFF1A1A1A));
        g.fillRoundedRectangle(meterBounds, 3.0f);
        
        // Calculate fill (0dB = no fill, -20dB = full fill)
        auto normalizedGR = juce::jlimit(0.0f, 1.0f, std::abs(gainReduction) / 20.0f);
        auto fillHeight = meterBounds.getHeight() * normalizedGR;
        auto fillBounds = juce::Rectangle<float>(meterBounds.getX(), 
                                               meterBounds.getBottom() - fillHeight,
                                               meterBounds.getWidth(), fillHeight);
        
        // Always blue for gain reduction
        g.setColour(juce::Colour(0xFF4A9EFF));
        g.fillRect(fillBounds);
        
        // Border
        g.setColour(juce::Colour(0xFF4A4A4A));
        g.drawRoundedRectangle(meterBounds, 3.0f, 1.0f);
    }
    
private:
    float gainReduction = 0.0f;
};

//==============================================================================
// Static Helper Functions
juce::Slider* UniversalCompressorEditor::addKnob(ModePanel& panel, const juce::String& text, 
                                                 float min, float max, float defaultVal, const juce::String& suffix)
{
    auto knob = std::make_unique<juce::Slider>();
    knob->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
    knob->setRange(min, max, 0.01);
    knob->setValue(defaultVal);
    knob->setTextValueSuffix(suffix);
    knob->setName(text);
    
    auto* raw = knob.get();
    panel.panel->addAndMakeVisible(raw);
    panel.knobs.emplace_back(std::move(knob));
    return raw;
}

juce::ComboBox* UniversalCompressorEditor::addCombo(ModePanel& panel, const juce::String& text, 
                                                    const juce::StringArray& items)
{
    auto combo = std::make_unique<juce::ComboBox>();
    combo->addItemList(items, 1);
    combo->setSelectedId(1);
    combo->setName(text);
    
    auto* raw = combo.get();
    panel.panel->addAndMakeVisible(raw);
    panel.combos.emplace_back(std::move(combo));
    return raw;
}

juce::ToggleButton* UniversalCompressorEditor::addToggle(ModePanel& panel, const juce::String& text)
{
    auto toggle = std::make_unique<juce::ToggleButton>(text);
    
    auto* raw = toggle.get();
    panel.panel->addAndMakeVisible(raw);
    panel.toggles.emplace_back(std::move(toggle));
    return raw;
}

//==============================================================================
// Constructor
UniversalCompressorEditor::UniversalCompressorEditor(UniversalCompressor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&modernLookAndFeel);
    setSize(800, 550);
    
    // Create meters
    inputVUMeter = std::make_unique<VUMeter>();
    gainReductionMeter = std::make_unique<GainReductionMeter>();
    outputVUMeter = std::make_unique<VUMeter>();
    
    addAndMakeVisible(inputVUMeter.get());
    addAndMakeVisible(gainReductionMeter.get());
    addAndMakeVisible(outputVUMeter.get());
    
    // Create global controls
    modeCombo = std::make_unique<juce::ComboBox>();
    modeCombo->addItemList({"Opto", "FET", "VCA", "Bus"}, 1);
    modeCombo->setSelectedId(1);
    addAndMakeVisible(modeCombo.get());
    
    bypassToggle = std::make_unique<juce::ToggleButton>("Bypass");
    addAndMakeVisible(bypassToggle.get());
    
    oversampleToggle = std::make_unique<juce::ToggleButton>("Oversample");
    addAndMakeVisible(oversampleToggle.get());
    
    // Setup mode panels
    for (int i = 0; i < 4; ++i)
    {
        modePanels[i].panel = std::make_unique<juce::Component>();
        addAndMakeVisible(modePanels[i].panel.get());
        setupModePanel(i);
        modePanels[i].panel->setVisible(false);   // Hide by default
    }
    
    // Global parameter attachments (with null checks)
    auto& params = processor.getParameters();
    if (params.getRawParameterValue("mode"))
        modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(params, "mode", *modeCombo);
    if (params.getRawParameterValue("bypass"))
        bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(params, "bypass", *bypassToggle);
    if (params.getRawParameterValue("oversample"))
        oversampleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(params, "oversample", *oversampleToggle);
    
    // Listen to mode parameter changes
    processor.getParameters().addParameterListener("mode", this);
    
    // Update initial mode
    const auto* modeParam = processor.getParameters().getRawParameterValue("mode");
    currentMode = modeParam ? static_cast<int>(*modeParam) : 2; // Default to VCA mode
    updateModePanel();
    
    // Start timer for meter updates
    startTimerHz(30);
}

void UniversalCompressorEditor::setupModePanel(int mode)
{
    auto& panel = modePanels[mode];
    
    switch (mode)
    {
        case 0: // Opto
        {
            auto* peakKnob = addKnob(panel, "Peak Reduction", 0.0f, 100.0f, 50.0f, "%");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "opto_peak_reduction", *peakKnob));
            
            auto* gainKnob = addKnob(panel, "Gain", -20.0f, 20.0f, 0.0f, " dB");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "opto_gain", *gainKnob));
            
            auto* limitToggle = addToggle(panel, "Limit");
            panel.buttonAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                processor.getParameters(), "opto_limit", *limitToggle));
            break;
        }
        
        case 1: // FET
        {
            auto* inputKnob = addKnob(panel, "Input", 0.0f, 10.0f, 5.0f);
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "fet_input", *inputKnob));
            
            auto* outputKnob = addKnob(panel, "Output", -20.0f, 20.0f, 0.0f, " dB");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "fet_output", *outputKnob));
            
            auto* attackKnob = addKnob(panel, "Attack", 0.02f, 0.8f, 0.02f, " ms");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "fet_attack", *attackKnob));
            
            auto* releaseKnob = addKnob(panel, "Release", 0.05f, 1.1f, 0.4f, " s");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "fet_release", *releaseKnob));
            
            auto* ratioCombo = addCombo(panel, "Ratio", {"4:1", "8:1", "12:1", "20:1"});
            panel.comboAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                processor.getParameters(), "fet_ratio", *ratioCombo));
            break;
        }
        
        case 2: // VCA
        {
            auto* thresholdKnob = addKnob(panel, "Threshold", -40.0f, 0.0f, -12.0f, " dB");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "vca_threshold", *thresholdKnob));
            
            auto* ratioKnob = addKnob(panel, "Ratio", 1.0f, 20.0f, 4.0f, ":1");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "vca_ratio", *ratioKnob));
            
            auto* attackKnob = addKnob(panel, "Attack", 0.1f, 100.0f, 1.0f, " ms");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "vca_attack", *attackKnob));
            
            auto* releaseKnob = addKnob(panel, "Release", 10.0f, 1000.0f, 100.0f, " ms");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "vca_release", *releaseKnob));
            
            auto* outputKnob = addKnob(panel, "Output", -20.0f, 20.0f, 0.0f, " dB");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "vca_output", *outputKnob));
            break;
        }
        
        case 3: // Bus
        {
            auto* thresholdKnob = addKnob(panel, "Threshold", -20.0f, 0.0f, -6.0f, " dB");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "bus_threshold", *thresholdKnob));
            
            auto* ratioKnob = addKnob(panel, "Ratio", 2.0f, 10.0f, 4.0f, ":1");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "bus_ratio", *ratioKnob));
            
            auto* attackCombo = addCombo(panel, "Attack", {"0.1ms", "0.3ms", "1ms", "3ms", "10ms", "30ms"});
            panel.comboAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                processor.getParameters(), "bus_attack", *attackCombo));
            
            auto* releaseCombo = addCombo(panel, "Release", {"0.1s", "0.3s", "0.6s", "1.2s", "Auto"});
            panel.comboAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                processor.getParameters(), "bus_release", *releaseCombo));
            
            auto* makeupKnob = addKnob(panel, "Makeup", -10.0f, 20.0f, 0.0f, " dB");
            panel.sliderAttachments.emplace_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                processor.getParameters(), "bus_makeup", *makeupKnob));
            break;
        }
    }
}

UniversalCompressorEditor::~UniversalCompressorEditor()
{
    setLookAndFeel(nullptr);
    processor.getParameters().removeParameterListener("mode", this);
}

void UniversalCompressorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1E1E1E));
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", 24.0f, juce::Font::bold));
    g.drawText("Universal Compressor", 20, 10, 300, 30, juce::Justification::left);
    
    // Draw mode-specific title
    juce::String modeNames[] = {"Opto Mode", "FET Mode", "VCA Mode", "Bus Mode"};
    g.setFont(juce::Font("Arial", 18.0f, juce::Font::plain));
    g.drawText(modeNames[currentMode], 200, 80, 400, 25, juce::Justification::centred);
    
    // Draw section dividers
    g.setColour(juce::Colour(0xFF4A4A4A));
    g.drawVerticalLine(180, 50, getHeight() - 20);  // Left divider
    g.drawVerticalLine(620, 50, getHeight() - 20);  // Right divider
}

void UniversalCompressorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Global controls row
    auto globalRow = bounds.removeFromTop(70).reduced(20, 10);
    if (modeCombo)
        modeCombo->setBounds(globalRow.removeFromLeft(120));
    globalRow.removeFromLeft(20);
    if (bypassToggle)
        bypassToggle->setBounds(globalRow.removeFromLeft(80));
    globalRow.removeFromLeft(20);
    if (oversampleToggle)
        oversampleToggle->setBounds(globalRow.removeFromLeft(100));
    
    auto remaining = bounds.reduced(20, 10);
    
    // Left column - reserved for future controls
    auto leftColumn = remaining.removeFromLeft(160);
    
    // Right column - meters
    auto rightColumn = remaining.removeFromRight(160);
    auto meterHeight = (rightColumn.getHeight() - 40) / 3;
    
    auto inputMeterArea = rightColumn.removeFromTop(meterHeight);
    if (inputVUMeter)
        inputVUMeter->setBounds(inputMeterArea.reduced(10, 5));
    
    rightColumn.removeFromTop(20);
    auto grMeterArea = rightColumn.removeFromTop(meterHeight);
    if (gainReductionMeter)
        gainReductionMeter->setBounds(grMeterArea.reduced(10, 5));
    
    rightColumn.removeFromTop(20);
    auto outputMeterArea = rightColumn.removeFromTop(meterHeight);
    if (outputVUMeter)
        outputVUMeter->setBounds(outputMeterArea.reduced(10, 5));
    
    // Center column - mode panel
    auto centerColumn = remaining.reduced(20, 0);
    
    // Layout current mode panel
    for (int i = 0; i < 4; ++i)
    {
        if (modePanels[i].panel)
            modePanels[i].panel->setBounds(centerColumn);
        
        // Layout controls in a grid
        auto& panel = modePanels[i];
        auto panelBounds = centerColumn.reduced(10);
        
        int numControls = static_cast<int>(panel.knobs.size() + panel.combos.size() + panel.toggles.size());
        if (numControls > 0)
        {
            int cols = (numControls > 4) ? 3 : 2;
            int rows = (numControls + cols - 1) / cols;
            
            int controlWidth = panelBounds.getWidth() / cols;
            int controlHeight = panelBounds.getHeight() / rows;
            
            int controlIndex = 0;
            
            // Layout knobs
            for (auto& knob : panel.knobs)
            {
                int row = controlIndex / cols;
                int col = controlIndex % cols;
                knob->setBounds(col * controlWidth, row * controlHeight, 
                              controlWidth - 10, controlHeight - 10);
                controlIndex++;
            }
            
            // Layout combos
            for (auto& combo : panel.combos)
            {
                int row = controlIndex / cols;
                int col = controlIndex % cols;
                combo->setBounds(col * controlWidth, row * controlHeight + 20, 
                               controlWidth - 10, 25);
                controlIndex++;
            }
            
            // Layout toggles
            for (auto& toggle : panel.toggles)
            {
                int row = controlIndex / cols;
                int col = controlIndex % cols;
                toggle->setBounds(col * controlWidth, row * controlHeight + 20, 
                                controlWidth - 10, 25);
                controlIndex++;
            }
        }
    }
}

void UniversalCompressorEditor::timerCallback()
{
    updateMeters();
}

void UniversalCompressorEditor::updateMeters()
{
    if (inputVUMeter)
        inputVUMeter->setLevel(processor.getInputLevel());
    if (gainReductionMeter)
        gainReductionMeter->setGainReduction(processor.getGainReduction());
    if (outputVUMeter)
        outputVUMeter->setLevel(processor.getOutputLevel());
}

void UniversalCompressorEditor::parameterChanged(const juce::String& parameterID, float)
{
    if (parameterID == "mode")
    {
        updateModePanel();
    }
}

void UniversalCompressorEditor::updateModePanel()
{
    const auto* modeParam = processor.getParameters().getRawParameterValue("mode");
    const int newMode = modeParam ? static_cast<int>(*modeParam) : 0;

    // Hide all panels
    for (auto& mp : modePanels)
        mp.panel->setVisible(false);

    currentMode = juce::jlimit(0, 3, newMode);
    modePanels[currentMode].panel->setVisible(true);
    repaint();
}