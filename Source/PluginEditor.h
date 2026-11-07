#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI.h"
#include "Backdrop.h"
#include "NoteScreen.h"
#include "LeafLayer.h"

class MelodiesEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit MelodiesEditor(MelodiesProcessor&);
    ~MelodiesEditor() override { stopTimer(); setLookAndFeel(nullptr); }
    void resized() override;

private:
    void timerCallback() override;

    MelodiesProcessor& proc;
    PixelLnF lnf;
    Backdrop backdrop;
    NoteScreen screen { proc };
    Picker kKey { proc.apvts, Param::key, "KEY", keyNames() }, kScale { proc.apvts, Param::scale, "SCALE", scaleNames() };
    Knob kSpeed { proc.apvts, Param::speed, "SPEED" }, kAmount { proc.apvts, Param::amount, "AMOUNT" }, kMix { proc.apvts, Param::mix, "MIX" };
    LeafLayer leaves;
    int lastChanges = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MelodiesEditor)
};
