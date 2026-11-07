#pragma once
#include <JuceHeader.h>
#include <BinaryData.h>
#include "UI.h"

// Fondo estático con estética de revista anime / tipografía (papel blanco, azul marino, bloque rosa fuerte,
// filetes finos, textos diminutos, código de barras) en letras pixel. Se dibuja una vez (buffered).
class Backdrop : public juce::Component
{
public:
    static constexpr const char* credit = "DEV BY ELYJA4";    // <- cambia aquí el texto de la esquinita
    static juce::Rectangle<int> treeRect() { return { 588, 8, 154, 132 }; }

    Backdrop()
    {
        tree = juce::ImageCache::getFromMemory(BinaryData::tree_jpg, BinaryData::tree_jpgSize);
        setOpaque(true); setBufferedToImage(true);
    }

    void paint(juce::Graphics& g) override
    {
        using namespace juce;
        g.fillAll(Pal::paper);
        const auto navy = Pal::navy;

        // cabecera
        g.setColour(navy); g.fillRect(40, 14, 540, 2);
        Px::drawIn(g, "(C)2026", { 440, 18, 140, 14 }, 2, navy, Justification::right);
        Px::draw(g, "ISSUE 01", 40, 18, 2, navy);
        Px::draw(g, "2FAR MELODIES", 42, 46, 7, Pal::hot, true);       // sombra rosa
        Px::draw(g, "2FAR MELODIES", 36, 40, 7, navy, true);
        Px::draw(g, "AUTO-TUNE / PITCH CORRECTION", 40, 102, 3, navy, true);
        g.setColour(navy); g.fillRect(40, 128, 540, 2);
        Px::draw(g, "SAKURA SERIES - PINK EDITION - NOTE LEAVES FALL ON EVERY CHANGE", 40, 140, 1, navy);

        // franja de información (como el encabezado de la revista)
        g.setColour(navy); g.fillRect(40, 166, 700, 1); g.fillRect(40, 204, 700, 1);
        struct Cell { int x; const char* head; const char* l1; const char* l2; };
        const Cell cells[] = { { 40, "SOME INFO", "PITCH DETECTION", "YIN + SCALE LOCK" },
                               { 200, "ABOUT VOICE", "SET KEY AND SCALE", "SING. IT SNAPS." },
                               { 380, "RETUNE", "0 MS = HARD TUNE", "400 MS = NATURAL" } };
        for (const auto& c : cells)
        {
            Px::draw(g, c.head, c.x + 6, 172, 2, navy, true);
            Px::draw(g, c.l1, c.x + 6, 188, 1, navy); Px::draw(g, c.l2, c.x + 6, 196, 1, navy);
            g.setColour(navy); g.fillRect(c.x + 150 + (c.x == 380 ? 20 : 0), 166, 1, 38);
        }
        Px::drawIn(g, "PIXEL TYPE.", { 560, 168, 176, 34 }, 3, navy, Justification::right, true);

        // bloque rosa + barra azul marino (como la portada de referencia), con borde difuminado en píxeles
        g.setColour(Pal::hot); g.fillRect(40, 214, 664, 286);
        g.setColour(navy);     g.fillRect(712, 214, 28, 286);
        for (int r = 0; r < 5; ++r)
            for (int x = 40; x < 704; x += 6)
                if (((x / 6 + r) & 1) == 0 && r < 4 - (x * 7 % 4)) { g.setColour(Pal::notePink); g.fillRect(x, 216 + r * 5, 3, 3); }
        for (int i = 0; i < 6; ++i) { g.setColour(Pal::paper); g.fillRect(719, 224 + i * 44, 14, 14); }

        // zona de controles
        g.setColour(navy); g.fillRect(40, 512, 700, 1); g.fillRect(40, 702, 700, 1);
        Px::draw(g, "CONTROLS", 40, 516, 1, navy);
        for (int i = 1; i < 5; ++i) g.fillRect(40 + i * 140, 520, 1, 176);

        // pie: código de barras, logo y créditos
        Random rng(7); int x = 40; bool on = true;
        while (x < 230) { const int w = 1 + rng.nextInt(4); if (on) g.fillRect(x, 722, w, 52); x += w; on = !on; }
        g.setColour(Pal::hot); g.fillEllipse(260.f, 724.f, 22.f, 22.f);
        g.setColour(navy);     g.fillEllipse(272.f, 724.f, 22.f, 22.f);
        Px::draw(g, "2FAR MELODIES", 260, 756, 2, navy, true);
        Px::drawIn(g, "08/06/ MADE", { 480, 722, 260, 16 }, 2, navy, Justification::right);
        Px::drawIn(g, "WITH LOVE", { 480, 742, 260, 16 }, 2, navy, Justification::right);
        Px::drawIn(g, "VST3 - AUTOTUNE", { 480, 764, 260, 14 }, 2, navy, Justification::right);

        // árbol de cerezo en la esquina + créditos
        const auto t = treeRect();
        if (tree.isValid()) g.drawImage(tree, t.toFloat(), RectanglePlacement::centred);
        g.setColour(navy); g.drawRect(t.expanded(2), 2);
        const Rectangle<int> tag(t.getX() - 6, t.getBottom() + 4, t.getWidth() + 12, 16);
        g.setColour(Pal::hot); g.fillRect(tag);
        Px::drawIn(g, credit, tag, 2, Pal::white, Justification::centred, true);
    }
private:
    juce::Image tree;
};
