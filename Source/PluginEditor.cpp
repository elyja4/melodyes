#include "PluginEditor.h"

MelodiesEditor::MelodiesEditor(MelodiesProcessor& p) : AudioProcessorEditor(&p), proc(p)
{
    setLookAndFeel(&lnf);
    for (juce::Component* c : std::initializer_list<juce::Component*>{ &backdrop, &screen, &kKey, &kScale, &kSpeed, &kAmount, &kMix, &leaves })
        addAndMakeVisible(c);
    lastChanges = proc.noteChanges.load();
    setSize(780, 800);
    startTimerHz(30);
}

void MelodiesEditor::resized()
{
    backdrop.setBounds(getLocalBounds());
    screen.setBounds(56, 230, 632, 254);
    kKey.setBounds(40, 526, 140, 120);   kScale.setBounds(180, 526, 140, 120);
    kSpeed.setBounds(320, 526, 140, 168); kAmount.setBounds(460, 526, 140, 168); kMix.setBounds(600, 526, 140, 168);
    leaves.setBounds(getLocalBounds());
}

// Cada cambio de nota -> hojas cayendo desde la copa del árbol
void MelodiesEditor::timerCallback()
{
    const int c = proc.noteChanges.load();
    if (c != lastChanges)
    {
        const int n = juce::jmin(3, c - lastChanges);
        lastChanges = c;
        const auto t = Backdrop::treeRect().toFloat();
        leaves.spawn({ t.getX() + 8.f, t.getY() + 6.f, t.getWidth() - 16.f, t.getHeight() * 0.5f }, 10 + 4 * n);
    }
}
