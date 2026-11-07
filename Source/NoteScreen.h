#pragma once
#include <JuceHeader.h>
#include <array>
#include "UI.h"

// Medidor de notas: nota detectada grande, objetivo, cents, historial de afinación y teclado de 12 notas.
// Todas las letras de las notas son rosas.
class NoteScreen : public juce::Component, private juce::Timer
{
public:
    explicit NoteScreen(MelodiesProcessor& p) : proc(p) { hDet.fill(-1.f); hTgt.fill(-1.f); setOpaque(true); startTimerHz(30); }
    ~NoteScreen() override { stopTimer(); }

    void paint(juce::Graphics& g) override
    {
        using namespace juce;
        g.fillAll(Pal::navy);
        g.setColour(Pal::notePink.withAlpha(0.10f));                       // rejilla de puntos
        for (int y = 8; y < getHeight(); y += 12) for (int x = 8; x < getWidth(); x += 12) g.fillRect(x, y, 2, 2);

        const float det = proc.detMidi.load(), tgt = proc.tgtMidi.load();
        const int key = (int) std::lround(proc.apvts.getRawParameterValue(Param::key)->load());
        const int scale = (int) std::lround(proc.apvts.getRawParameterValue(Param::scale)->load());
        const auto names = keyNames();

        // --- nota grande
        Px::draw(g, "NOTE", 20, 12, 2, Pal::notePink);
        if (det < 0.f) Px::draw(g, "--", 20, 28, 14, Pal::notePink.withAlpha(0.5f), true);
        else
        {
            const int m = (int) std::lround(det);
            Px::draw(g, names[((m % 12) + 12) % 12], 20, 28, 14, Pal::notePink, true);
            Px::draw(g, String(m / 12 - 1), 20 + Px::width(names[((m % 12) + 12) % 12], 14) + 14, 84, 6, Pal::notePink, true);
        }
        Px::draw(g, "TARGET", 20, 134, 2, Pal::notePink.withAlpha(0.8f));
        if (tgt >= 0.f) { const int t = (int) std::lround(tgt); Px::draw(g, names[t % 12] + String(t / 12 - 1), 20, 150, 4, Pal::white, true); }
        else Px::draw(g, "--", 20, 150, 4, Pal::white.withAlpha(0.4f), true);
        const int cents = det >= 0.f && tgt >= 0.f ? (int) std::lround((det - tgt) * 100.f) : 0;
        Px::draw(g, (det >= 0.f ? (cents > 0 ? "+" : "") + String(cents) + " CENTS" : String("0 CENTS")), 120, 158, 2, Pal::notePink);

        // --- historial (cada punto = 3 px). Líneas = notas permitidas de la escala
        const Rectangle<int> gr(232, 14, 384, 124);
        g.setColour(Pal::notePink.withAlpha(0.28f)); g.drawRect(gr, 2);
        const float centre = centreMidi;
        for (int m = (int) centre - 12; m <= (int) centre + 12; ++m)
            if (Pitch::allowed(((m % 12) + 12) % 12, key, scale))
            {
                const int y = gr.getCentreY() - (int) std::lround((m - centre) * 5.f);
                if (y > gr.getY() && y < gr.getBottom()) { g.setColour(Pal::white.withAlpha(0.10f)); g.fillRect(gr.getX() + 2, y, gr.getWidth() - 4, 1); }
            }
        for (int i = 0; i < (int) hDet.size(); ++i)
        {
            const int idx = (head + i) % (int) hDet.size(), x = gr.getX() + 4 + i * 3;
            if (hTgt[(size_t) idx] >= 0.f) { g.setColour(Pal::white.withAlpha(0.85f)); g.fillRect(x, yFor(hTgt[(size_t) idx], gr), 3, 3); }
            if (hDet[(size_t) idx] >= 0.f) { g.setColour(Pal::notePink); g.fillRect(x, yFor(hDet[(size_t) idx], gr), 3, 3); }
        }

        // --- aguja de cents (-50..+50)
        const Rectangle<int> cb(232, 146, 384, 14);
        g.setColour(Pal::notePink.withAlpha(0.3f)); for (int i = 0; i <= 10; ++i) g.fillRect(cb.getX() + i * (cb.getWidth() - 4) / 10, cb.getY() + (i == 5 ? 0 : 4), 4, i == 5 ? 14 : 6);
        if (det >= 0.f && tgt >= 0.f)
        {
            const float f = jlimit(-50.f, 50.f, (float) cents) / 50.f;
            g.setColour(Pal::notePink); g.fillRect(cb.getCentreX() - 5 + (int) (f * (cb.getWidth() / 2 - 8)), cb.getY(), 10, 14);
        }

        // --- 12 notas (rosas); la detectada se rellena, la objetivo lleva marco, las que no son de la escala se apagan
        const int cellW = (getWidth() - 40) / 12;
        for (int i = 0; i < 12; ++i)
        {
            const Rectangle<int> r(20 + i * cellW, 192, cellW - 4, 54);
            const bool ok = Pitch::allowed(i, key, scale);
            const bool isDet = det >= 0.f && ((int) std::lround(det) % 12) == i, isTgt = tgt >= 0.f && ((int) std::lround(tgt) % 12) == i;
            g.setColour(isDet ? Pal::notePink : Pal::notePink.withAlpha(ok ? 0.16f : 0.05f)); g.fillRect(r);
            if (isTgt) { g.setColour(Pal::white); g.drawRect(r, 3); }
            Px::drawIn(g, names[i], r, 3, isDet ? Pal::navy : Pal::notePink.withAlpha(ok ? 1.f : 0.35f), Justification::centred, true);
        }
    }

private:
    static int yFor(float midi, const juce::Rectangle<int>& gr, float centre) { return juce::jlimit(gr.getY() + 2, gr.getBottom() - 5, gr.getCentreY() - (int) std::lround((midi - centre) * 5.f) - 1); }
    int yFor(float midi, const juce::Rectangle<int>& gr) const { return yFor(midi, gr, centreMidi); }

    void timerCallback() override
    {
        const float det = proc.detMidi.load();
        hDet[(size_t) head] = det; hTgt[(size_t) head] = proc.tgtMidi.load();
        head = (head + 1) % (int) hDet.size();
        if (det >= 0.f) centreMidi += (det - centreMidi) * 0.08f;
        repaint();
    }
    MelodiesProcessor& proc;
    std::array<float, 128> hDet {}, hTgt {};
    int head = 0; float centreMidi = 60.f;
};
