#pragma once
#include <JuceHeader.h>
#include "PixelFont.h"
#include "PluginProcessor.h"

namespace Pal
{
    inline const juce::Colour paper(0xfff6f3f7), navy(0xff12123a), hot(0xffe8206a),
        notePink(0xffff8fc0), softPink(0xffffa6c9), white(0xffffffff);
}
using APVTS = juce::AudioProcessorValueTreeState;

//==============================================================================
// Perilla en estilo pixel: anillo de cuadritos + cuerpo de píxeles.
struct PixelLnF : public juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos,
                          float a0, float a1, juce::Slider&) override
    {
        using namespace juce;
        const float r = (float) jmin(w, h) * 0.5f - 3.f;
        const Point<float> c((float) x + (float) w * 0.5f, (float) y + (float) h * 0.5f);
        const float ang = a0 + pos * (a1 - a0);

        const int segs = 21, cell = jmax(4, (int) (r * 0.13f));
        for (int i = 0; i < segs; ++i)
        {
            const float t = (float) i / (float) (segs - 1), a = a0 + t * (a1 - a0);
            const int px = (int) std::round(c.x + std::sin(a) * r * 0.9f) - cell / 2, py = (int) std::round(c.y - std::cos(a) * r * 0.9f) - cell / 2;
            g.setColour(t <= pos + 0.001f ? Pal::hot : Pal::navy.withAlpha(0.18f));
            g.fillRect(px, py, cell, cell);
        }
        // cuerpo circular hecho de píxeles
        const float r0 = r * 0.62f; const int st = 4;
        g.setColour(Pal::navy);
        for (float dy = -r0; dy <= r0; dy += (float) st)
            for (float dx = -r0; dx <= r0; dx += (float) st)
                if (dx * dx + dy * dy <= r0 * r0)
                    g.fillRect((int) (c.x + dx) / st * st, (int) (c.y + dy) / st * st, st, st);
        g.setColour(Pal::notePink.withAlpha(0.35f));
        g.fillRect((int) (c.x - r0 * 0.5f) / st * st, (int) (c.y - r0 * 0.6f) / st * st, st * 3, st);
        // puntero: tres cuadritos hacia afuera
        for (int k = 0; k < 3; ++k)
        {
            const float d = r0 * (0.25f + 0.28f * (float) k);
            g.setColour(k == 2 ? Pal::hot : Pal::notePink);
            g.fillRect((int) std::round(c.x + std::sin(ang) * d) - 2, (int) std::round(c.y - std::cos(ang) * d) - 2, 5, 5);
        }
    }
};

//==============================================================================
class Knob : public juce::Component
{
public:
    Knob(APVTS& apvts, const char* id, const juce::String& name) : title(name)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        s.setRotaryParameters(1.2f * juce::MathConstants<float>::pi, 2.8f * juce::MathConstants<float>::pi, true);
        s.onValueChange = [this] { repaint(); };
        addAndMakeVisible(s);
        att = std::make_unique<APVTS::SliderAttachment>(apvts, id, s);
        if (auto* p = apvts.getParameter(id)) s.setDoubleClickReturnValue(true, p->convertFrom0to1(p->getDefaultValue()));
    }
    void paint(juce::Graphics& g) override
    {
        Px::drawIn(g, title, { 0, 0, getWidth(), 22 }, 2, Pal::navy, juce::Justification::centred, true);
        Px::drawIn(g, s.getTextFromValue(s.getValue()), { 0, getHeight() - 26, getWidth(), 24 }, 3, Pal::hot, juce::Justification::centred, true);
    }
    void resized() override { s.setBounds(8, 24, getWidth() - 16, getHeight() - 24 - 28); }
private:
    juce::String title; juce::Slider s;
    std::unique_ptr<APVTS::SliderAttachment> att;
};

//==============================================================================
// Selector con flechas < > para parámetros de lista (tonalidad y escala).
class Picker : public juce::Component
{
public:
    Picker(APVTS& apvts, const char* id, const juce::String& name, const juce::StringArray& items)
        : title(name), names(items), param(apvts.getParameter(id)),
          att(*apvts.getParameter(id), [this](float v) { index = (int) std::lround(v); repaint(); })
    {
        att.sendInitialUpdate();
    }
    void paint(juce::Graphics& g) override
    {
        Px::drawIn(g, title, { 0, 0, getWidth(), 22 }, 2, Pal::navy, juce::Justification::centred, true);
        const auto box = juce::Rectangle<int>(6, 34, getWidth() - 12, 56);
        g.setColour(Pal::navy); g.fillRect(box);
        g.setColour(Pal::hot); g.fillRect(box.getX(), box.getBottom() - 4, box.getWidth(), 4);
        Px::drawIn(g, "<", box.withWidth(20), 3, Pal::notePink);
        Px::drawIn(g, ">", box.withTrimmedLeft(box.getWidth() - 20), 3, Pal::notePink);
        const juce::String t = names[juce::jlimit(0, names.size() - 1, index)];
        const auto area = box.reduced(20, 0);
        int sc = 5; while (sc > 1 && Px::width(t, sc) > area.getWidth()) --sc;   // el texto más grande que quepa
        Px::drawIn(g, t, area, sc, Pal::notePink, juce::Justification::centred, true);
    }
    void mouseDown(const juce::MouseEvent& e) override { step(e.x < getWidth() / 2 ? -1 : 1); }
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& w) override { step(w.deltaY > 0 ? -1 : 1); }
private:
    void step(int d) { att.setValueAsCompleteGesture((float) ((index + d + names.size()) % names.size())); }
    juce::String title; juce::StringArray names; juce::RangedAudioParameter* param; int index = 0;
    juce::ParameterAttachment att;
};
