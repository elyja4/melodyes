#pragma once
#include <JuceHeader.h>
#include <atomic>
#include "Parameters.h"
#include "Pitch.h"

class MelodiesProcessor : public juce::AudioProcessor
{
public:
    MelodiesProcessor();
    ~MelodiesProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "2far melodies"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

    // para la GUI
    std::atomic<float> detMidi { -1.f };   // nota detectada (MIDI con decimales), -1 = sin voz
    std::atomic<float> tgtMidi { -1.f };   // nota objetivo
    std::atomic<int>   noteChanges { 0 };  // +1 cada vez que cambia la nota (lanza las hojas)
    std::atomic<float> outLevel { 0.f };

private:
    void onDetection(const Pitch::Detector::Result&, int key, int scale, float speedMs, float amount);

    Pitch::Detector detector;
    Pitch::Shifter shifter;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> ratioSm;

    double sr = 44100.0;
    int dryDelay = 550;
    float hopSec = 0.0116f, shiftSemis = 0.f, dispMidi = -1.f;
    int curTarget = -1, stableTarget = -1, candTarget = -1, candCount = 0, unvoicedHops = 0;
    int sinceChange = 1000;
    std::array<float, 3> f0Hist { 0.f, 0.f, 0.f };
    int f0Count = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MelodiesProcessor)
};
