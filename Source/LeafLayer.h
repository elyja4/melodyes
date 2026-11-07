#pragma once
#include <JuceHeader.h>
#include <vector>
#include "UI.h"

// Capa transparente por encima de todo: hojas/pétalos de cerezo en píxeles que caen del árbol
// cada vez que cambia la nota.
class LeafLayer : public juce::Component, private juce::Timer
{
public:
    LeafLayer() { setInterceptsMouseClicks(false, false); startTimerHz(60); }
    ~LeafLayer() override { stopTimer(); }

    void spawn(juce::Rectangle<float> region, int count)
    {
        for (int i = 0; i < count; ++i)
        {
            Petal p;
            p.x = region.getX() + rng.nextFloat() * region.getWidth();
            p.y = region.getY() + rng.nextFloat() * region.getHeight();
            p.vy = 0.9f + rng.nextFloat() * 1.3f; p.phase = rng.nextFloat() * 6.28f; p.sway = 0.4f + rng.nextFloat() * 0.8f;
            p.drift = -0.15f + rng.nextFloat() * 0.3f; p.delay = rng.nextInt(40); p.colour = rng.nextInt(3); p.frame = rng.nextInt(3);
            petals.push_back(p);
        }
    }

    void paint(juce::Graphics& g) override
    {
        static const uint8_t sprite[3][5] = { { 0b01100, 0b11110, 0b11111, 0b01110, 0b00100 },
                                              { 0b00110, 0b01111, 0b11111, 0b01110, 0b01000 },
                                              { 0b01000, 0b11110, 0b11111, 0b11110, 0b00110 } };
        const juce::Colour cols[3] = { Pal::softPink, juce::Colour(0xffff7fb0), Pal::hot };
        const int cell = 3;
        for (const auto& p : petals)
        {
            if (p.delay > 0) continue;
            g.setColour(cols[p.colour]);
            const int bx = (int) p.x / cell * cell, by = (int) p.y / cell * cell;
            for (int r = 0; r < 5; ++r)
                for (int c = 0; c < 5; ++c)
                    if (sprite[p.frame][r] & (0x10 >> c)) g.fillRect(bx + c * cell, by + r * cell, cell, cell);
        }
    }
private:
    struct Petal { float x, y, vy, phase, sway, drift; int delay, colour, frame; };
    void timerCallback() override
    {
        if (petals.empty()) return;
        for (auto& p : petals)
        {
            if (p.delay > 0) { --p.delay; continue; }
            p.phase += 0.06f; p.y += p.vy; p.x += std::sin(p.phase) * p.sway + p.drift;
            if (((int) (p.phase * 10.f)) % 11 == 0) p.frame = (p.frame + 1) % 3;
        }
        petals.erase(std::remove_if(petals.begin(), petals.end(), [this](const Petal& p) { return p.y > (float) getHeight() + 20.f; }), petals.end());
        repaint();
    }
    std::vector<Petal> petals; juce::Random rng;
};
