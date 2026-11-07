#pragma once
#include <JuceHeader.h>

namespace Param
{
    inline constexpr const char* key    = "key";
    inline constexpr const char* scale  = "scale";
    inline constexpr const char* speed  = "speed";    // velocidad de afinado en ms (0 = afinado duro)
    inline constexpr const char* amount = "amount";   // cuánto se corrige hacia la nota objetivo
    inline constexpr const char* mix    = "mix";
}

inline juce::StringArray keyNames()   { return { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }; }
inline juce::StringArray scaleNames() { return { "CHROM", "MAJOR", "MINOR", "P.MAJ", "P.MIN" }; }

inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    p.push_back(std::make_unique<AudioParameterChoice>(ParameterID{ Param::key, 1 }, "Key", keyNames(), 0));
    p.push_back(std::make_unique<AudioParameterChoice>(ParameterID{ Param::scale, 1 }, "Scale", scaleNames(), 0));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ Param::speed, 1 }, "Retune Speed",
        NormalisableRange<float>(0.f, 400.f, 0.1f, 0.5f), 20.f,
        AudioParameterFloatAttributes().withStringFromValueFunction([](float v, int) { return String(roundToInt(v)) + " ms"; })));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ Param::amount, 1 }, "Amount",
        NormalisableRange<float>(0.f, 100.f, 0.1f), 100.f,
        AudioParameterFloatAttributes().withStringFromValueFunction([](float v, int) { return String(roundToInt(v)) + "%"; })));
    p.push_back(std::make_unique<AudioParameterFloat>(ParameterID{ Param::mix, 1 }, "Mix",
        NormalisableRange<float>(0.f, 100.f, 0.1f), 100.f,
        AudioParameterFloatAttributes().withStringFromValueFunction([](float v, int) { return String(roundToInt(v)) + "%"; })));
    return { p.begin(), p.end() };
}
