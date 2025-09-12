// Test program for Universal Compressor Plugin
// Compile with: g++ -o test_enhanced test_enhanced_plugin.cpp `pkg-config --cflags --libs juce_audio_processors juce_core juce_audio_basics`

#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include "UniversalCompressor.h"

// Test signal generators
class TestSignalGenerator
{
public:
    static void generateSineWave(float* buffer, int numSamples, float frequency, float sampleRate, float amplitude = 1.0f)
    {
        static float phase = 0.0f;
        float phaseIncrement = 2.0f * M_PI * frequency / sampleRate;
        
        for (int i = 0; i < numSamples; ++i)
        {
            buffer[i] = amplitude * std::sin(phase);
            phase += phaseIncrement;
            if (phase >= 2.0f * M_PI)
                phase -= 2.0f * M_PI;
        }
    }
    
    static void generateTransient(float* buffer, int numSamples, float attackTime, float sampleRate)
    {
        int attackSamples = static_cast<int>(attackTime * sampleRate);
        
        for (int i = 0; i < numSamples; ++i)
        {
            if (i < attackSamples)
                buffer[i] = static_cast<float>(i) / attackSamples;
            else
                buffer[i] = std::exp(-3.0f * (i - attackSamples) / static_cast<float>(numSamples - attackSamples));
        }
    }
    
    static float calculateRMS(const float* buffer, int numSamples)
    {
        float sum = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            sum += buffer[i] * buffer[i];
        return std::sqrt(sum / numSamples);
    }
    
    static float calculatePeak(const float* buffer, int numSamples)
    {
        float peak = 0.0f;
        for (int i = 0; i < numSamples; ++i)
            peak = std::max(peak, std::abs(buffer[i]));
        return peak;
    }
};

// Test each compressor mode
void testCompressorMode(UniversalCompressor& compressor, int mode, const std::string& modeName)
{
    std::cout << "\n=== Testing " << modeName << " Mode ===" << std::endl;
    
    const float sampleRate = 44100.0f;
    const int blockSize = 512;
    const int numChannels = 2;
    
    // Prepare the processor
    compressor.prepareToPlay(sampleRate, blockSize);
    
    // Set mode
    auto& params = compressor.getParameters();
    if (auto* modeParam = params.getParameter("mode"))
        modeParam->setValueNotifyingHost(mode / 3.0f); // Normalize to 0-1
    
    // Create test buffers
    juce::AudioBuffer<float> inputBuffer(numChannels, blockSize);
    juce::AudioBuffer<float> outputBuffer(numChannels, blockSize);
    juce::MidiBuffer midiBuffer;
    
    // Test 1: Steady-state sine wave (test threshold and ratio)
    std::cout << "Test 1: Steady-state compression" << std::endl;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        TestSignalGenerator::generateSineWave(
            inputBuffer.getWritePointer(ch), blockSize, 1000.0f, sampleRate, 0.5f);
    }
    outputBuffer.makeCopyOf(inputBuffer);
    
    // Process
    compressor.processBlock(outputBuffer, midiBuffer);
    
    float inputRMS = TestSignalGenerator::calculateRMS(inputBuffer.getReadPointer(0), blockSize);
    float outputRMS = TestSignalGenerator::calculateRMS(outputBuffer.getReadPointer(0), blockSize);
    float gainReduction = 20.0f * std::log10(outputRMS / inputRMS);
    
    std::cout << "  Input RMS: " << inputRMS << " (" << 20*std::log10(inputRMS) << " dB)" << std::endl;
    std::cout << "  Output RMS: " << outputRMS << " (" << 20*std::log10(outputRMS) << " dB)" << std::endl;
    std::cout << "  Gain Reduction: " << gainReduction << " dB" << std::endl;
    std::cout << "  Meter GR: " << compressor.getGainReduction() << " dB" << std::endl;
    
    // Test 2: Transient response (test attack/release)
    std::cout << "\nTest 2: Transient response" << std::endl;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        TestSignalGenerator::generateTransient(
            inputBuffer.getWritePointer(ch), blockSize, 0.001f, sampleRate);
    }
    outputBuffer.makeCopyOf(inputBuffer);
    
    compressor.processBlock(outputBuffer, midiBuffer);
    
    float inputPeak = TestSignalGenerator::calculatePeak(inputBuffer.getReadPointer(0), blockSize);
    float outputPeak = TestSignalGenerator::calculatePeak(outputBuffer.getReadPointer(0), blockSize);
    
    std::cout << "  Input Peak: " << inputPeak << " (" << 20*std::log10(inputPeak) << " dB)" << std::endl;
    std::cout << "  Output Peak: " << outputPeak << " (" << 20*std::log10(outputPeak) << " dB)" << std::endl;
    std::cout << "  Peak Reduction: " << 20*std::log10(outputPeak/inputPeak) << " dB" << std::endl;
    
    // Test 3: Stereo linking
    std::cout << "\nTest 3: Stereo linking" << std::endl;
    
    // Generate different signals in L/R
    TestSignalGenerator::generateSineWave(
        inputBuffer.getWritePointer(0), blockSize, 1000.0f, sampleRate, 0.8f); // Left loud
    TestSignalGenerator::generateSineWave(
        inputBuffer.getWritePointer(1), blockSize, 2000.0f, sampleRate, 0.2f); // Right quiet
    
    outputBuffer.makeCopyOf(inputBuffer);
    compressor.processBlock(outputBuffer, midiBuffer);
    
    float leftGR = 20*std::log10(TestSignalGenerator::calculateRMS(outputBuffer.getReadPointer(0), blockSize) /
                                  TestSignalGenerator::calculateRMS(inputBuffer.getReadPointer(0), blockSize));
    float rightGR = 20*std::log10(TestSignalGenerator::calculateRMS(outputBuffer.getReadPointer(1), blockSize) /
                                   TestSignalGenerator::calculateRMS(inputBuffer.getReadPointer(1), blockSize));
    
    std::cout << "  Left channel GR: " << leftGR << " dB" << std::endl;
    std::cout << "  Right channel GR: " << rightGR << " dB" << std::endl;
    std::cout << "  Stereo image preserved: " << (std::abs(leftGR - rightGR) < 1.0f ? "Yes" : "No") << std::endl;
}

// Performance test
void performanceTest(UniversalCompressor& compressor)
{
    std::cout << "\n=== Performance Test ===" << std::endl;
    
    const float sampleRate = 96000.0f; // High sample rate test
    const int blockSize = 2048;
    const int numChannels = 2;
    const int numBlocks = 1000;
    
    compressor.prepareToPlay(sampleRate, blockSize);
    
    juce::AudioBuffer<float> buffer(numChannels, blockSize);
    juce::MidiBuffer midiBuffer;
    
    // Fill with test signal
    for (int ch = 0; ch < numChannels; ++ch)
    {
        TestSignalGenerator::generateSineWave(
            buffer.getWritePointer(ch), blockSize, 1000.0f, sampleRate);
    }
    
    // Measure processing time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numBlocks; ++i)
    {
        compressor.processBlock(buffer, midiBuffer);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    float totalSamples = blockSize * numBlocks;
    float totalSeconds = totalSamples / sampleRate;
    float processingSeconds = duration.count() / 1000000.0f;
    float realtimeFactor = totalSeconds / processingSeconds;
    
    std::cout << "  Processed " << totalSeconds << " seconds of audio in " << processingSeconds << " seconds" << std::endl;
    std::cout << "  Realtime factor: " << realtimeFactor << "x" << std::endl;
    std::cout << "  CPU usage estimate: " << (100.0f / realtimeFactor) << "%" << std::endl;
    
    if (realtimeFactor < 10.0f)
        std::cout << "  WARNING: Performance may be insufficient for realtime use!" << std::endl;
}

int main()
{
    std::cout << "Universal Compressor Plugin Test Suite" << std::endl;
    std::cout << "======================================" << std::endl;
    
    try
    {
        // Create processor instance
        auto processor = std::make_unique<UniversalCompressor>();
        
        // Test each mode
        testCompressorMode(*processor, 0, "LA-2A (Opto)");
        testCompressorMode(*processor, 1, "1176 (FET)");
        testCompressorMode(*processor, 2, "DBX 160 (VCA)");
        testCompressorMode(*processor, 3, "SSL Bus");
        
        // Performance test
        performanceTest(*processor);
        
        std::cout << "\n=== All Tests Complete ===" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}