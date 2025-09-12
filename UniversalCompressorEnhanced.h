#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <memory>
#include <atomic>

enum class CompressorMode : int
{
    Opto = 0,    // LA-2A style optical compressor
    FET = 1,     // 1176 style FET compressor  
    VCA = 2,     // DBX 160 style VCA compressor
    Bus = 3      // SSL Bus style compressor
};

enum class DetectionMode : int
{
    Peak = 0,
    RMS = 1,
    Hybrid = 2   // Blend of peak and RMS
};

enum class TopologyMode : int  
{
    FeedForward = 0,
    FeedBack = 1
};

class UniversalCompressor : public juce::AudioProcessor
{
public:
    UniversalCompressor();
    ~UniversalCompressor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Universal Compressor"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override;
    double getLatencyInSamples() const;

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Metering
    float getInputLevel() const { return inputMeter.load(); }
    float getOutputLevel() const { return outputMeter.load(); }
    float getGainReduction() const { return grMeter.load(); }
    
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    CompressorMode getCurrentMode() const;

private:
    // Enhanced DSP classes with stereo linking
    class StereoLinkProcessor
    {
    public:
        void prepare(int numChannels);
        float getLinkedDetection(const float* channelLevels, int numChannels, float linkAmount);
        void processLinkedGainReduction(float* reductions, int numChannels, float linkAmount);
        
    private:
        std::vector<float> linkedReductions;
    };
    
    // Core DSP classes
    class OptoCompressor;
    class FETCompressor;
    class VCACompressor;
    class BusCompressor;
    class AntiAliasing;
    class LookaheadDelay;
    class SidechainFilter;
    
    // Parameter state
    juce::AudioProcessorValueTreeState parameters;
    
    // DSP components
    std::unique_ptr<OptoCompressor> optoCompressor;
    std::unique_ptr<FETCompressor> fetCompressor;
    std::unique_ptr<VCACompressor> vcaCompressor;
    std::unique_ptr<BusCompressor> busCompressor;
    std::unique_ptr<AntiAliasing> antiAliasing;
    std::unique_ptr<LookaheadDelay> lookahead;
    std::unique_ptr<SidechainFilter> sidechainFilter;
    std::unique_ptr<StereoLinkProcessor> stereoLink;
    
    // Mix control for parallel compression
    juce::dsp::DryWetMixer<float> dryWetMixer;
    
    // Metering
    std::atomic<float> inputMeter{-60.0f};
    std::atomic<float> outputMeter{-60.0f};
    std::atomic<float> grMeter{0.0f};
    
    // Processing state
    double currentSampleRate{44100.0};
    int currentBlockSize{512};
    bool isProcessing{false};
    
    // Parameter smoothing
    juce::SmoothedValue<float> mixSmoothed;
    juce::SmoothedValue<float> outputGainSmoothed;
    
    // Parameter creation
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    // Helper methods
    void updateLatency();
    float convertParameterValue(const juce::String& paramID, float rawValue);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UniversalCompressor)
};