#pragma once
#include <JuceHeader.h>

// Fuente pixel 5x7 propia (sin archivos externos). Todo el texto del plugin usa esta fuente.
namespace Px
{
    inline const char* kChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 #.-+/:<>%!(),@?";
    inline const uint8_t kGlyph[][7] = {
        {0b01110,0b10001,0b10001,0b11111,0b10001,0b10001,0b10001}, // A
        {0b11110,0b10001,0b10001,0b11110,0b10001,0b10001,0b11110}, // B
        {0b01110,0b10001,0b10000,0b10000,0b10000,0b10001,0b01110}, // C
        {0b11110,0b10001,0b10001,0b10001,0b10001,0b10001,0b11110}, // D
        {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b11111}, // E
        {0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b10000}, // F
        {0b01110,0b10001,0b10000,0b10111,0b10001,0b10001,0b01111}, // G
        {0b10001,0b10001,0b10001,0b11111,0b10001,0b10001,0b10001}, // H
        {0b01110,0b00100,0b00100,0b00100,0b00100,0b00100,0b01110}, // I
        {0b00111,0b00010,0b00010,0b00010,0b00010,0b10010,0b01100}, // J
        {0b10001,0b10010,0b10100,0b11000,0b10100,0b10010,0b10001}, // K
        {0b10000,0b10000,0b10000,0b10000,0b10000,0b10000,0b11111}, // L
        {0b10001,0b11011,0b10101,0b10101,0b10001,0b10001,0b10001}, // M
        {0b10001,0b11001,0b10101,0b10011,0b10001,0b10001,0b10001}, // N
        {0b01110,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110}, // O
        {0b11110,0b10001,0b10001,0b11110,0b10000,0b10000,0b10000}, // P
        {0b01110,0b10001,0b10001,0b10001,0b10101,0b10010,0b01101}, // Q
        {0b11110,0b10001,0b10001,0b11110,0b10100,0b10010,0b10001}, // R
        {0b01111,0b10000,0b10000,0b01110,0b00001,0b00001,0b11110}, // S
        {0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b00100}, // T
        {0b10001,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110}, // U
        {0b10001,0b10001,0b10001,0b10001,0b10001,0b01010,0b00100}, // V
        {0b10001,0b10001,0b10001,0b10101,0b10101,0b11011,0b10001}, // W
        {0b10001,0b10001,0b01010,0b00100,0b01010,0b10001,0b10001}, // X
        {0b10001,0b10001,0b01010,0b00100,0b00100,0b00100,0b00100}, // Y
        {0b11111,0b00001,0b00010,0b00100,0b01000,0b10000,0b11111}, // Z
        {0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110}, // 0
        {0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110}, // 1
        {0b01110,0b10001,0b00001,0b00010,0b00100,0b01000,0b11111}, // 2
        {0b11110,0b00001,0b00001,0b01110,0b00001,0b00001,0b11110}, // 3
        {0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010}, // 4
        {0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110}, // 5
        {0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110}, // 6
        {0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000}, // 7
        {0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110}, // 8
        {0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100}, // 9
        {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000}, // espacio
        {0b01010,0b01010,0b11111,0b01010,0b11111,0b01010,0b01010}, // #
        {0b00000,0b00000,0b00000,0b00000,0b00000,0b01100,0b01100}, // .
        {0b00000,0b00000,0b00000,0b11111,0b00000,0b00000,0b00000}, // -
        {0b00000,0b00100,0b00100,0b11111,0b00100,0b00100,0b00000}, // +
        {0b00001,0b00010,0b00010,0b00100,0b01000,0b01000,0b10000}, // /
        {0b00000,0b01100,0b01100,0b00000,0b01100,0b01100,0b00000}, // :
        {0b00010,0b00100,0b01000,0b10000,0b01000,0b00100,0b00010}, // <
        {0b01000,0b00100,0b00010,0b00001,0b00010,0b00100,0b01000}, // >
        {0b11001,0b11010,0b00010,0b00100,0b01000,0b01011,0b10011}, // %
        {0b00100,0b00100,0b00100,0b00100,0b00100,0b00000,0b00100}, // !
        {0b00010,0b00100,0b01000,0b01000,0b01000,0b00100,0b00010}, // (
        {0b01000,0b00100,0b00010,0b00010,0b00010,0b00100,0b01000}, // )
        {0b00000,0b00000,0b00000,0b00000,0b01100,0b00100,0b01000}, // ,
        {0b01110,0b10001,0b10111,0b10101,0b10111,0b10000,0b01110}, // @
        {0b01110,0b10001,0b00001,0b00010,0b00100,0b00000,0b00100}  // ?
    };

    static_assert(sizeof(kGlyph) / sizeof(kGlyph[0]) == 52, "glyph count must match kChars");

    inline int indexOf(juce::juce_wchar c)
    {
        c = juce::CharacterFunctions::toUpperCase(c);
        for (int i = 0; kChars[i] != 0; ++i) if ((juce::juce_wchar) kChars[i] == c) return i;
        return -1;
    }
    inline int width(const juce::String& t, int s) { return t.isEmpty() ? 0 : t.length() * 6 * s - s; }

    inline void draw(juce::Graphics& g, const juce::String& text, int x, int y, int s, juce::Colour c, bool bold = false)
    {
        g.setColour(c);
        const int cw = bold ? s + juce::jmax(1, s / 2) : s;
        int cx = x;
        for (auto ch : text)
        {
            const int gi = indexOf(ch);
            if (gi >= 0)
                for (int r = 0; r < 7; ++r)
                {
                    const uint8_t row = kGlyph[gi][r];
                    for (int col = 0; col < 5; ++col)
                        if (row & (0x10 >> col)) g.fillRect(cx + col * s, y + r * s, cw, s);
                }
            cx += 6 * s;
        }
    }

    inline void drawIn(juce::Graphics& g, const juce::String& text, juce::Rectangle<int> a, int s, juce::Colour c,
                       juce::Justification j = juce::Justification::centred, bool bold = false)
    {
        const int w = width(text, s), h = 7 * s;
        int x = a.getX();
        if (j.testFlags(juce::Justification::horizontallyCentred)) x = a.getCentreX() - w / 2;
        else if (j.testFlags(juce::Justification::right)) x = a.getRight() - w;
        draw(g, text, x, a.getCentreY() - h / 2, s, c, bold);
    }
}
