#pragma once
// Núcleo de afinación SIN dependencias de JUCE (solo std) para poder probarlo aparte.
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

namespace Pitch
{
    // máscaras de escala (bit i = intervalo i desde la tónica): 0 cromática, 1 mayor, 2 menor, 3 pent. mayor, 4 pent. menor
    inline bool allowed(int pc, int key, int scale)
    {
        static const int masks[5] = { 0xFFF, 0xAB5, 0x5AD, 0x295, 0x4A9 };
        const int rel = ((pc - key) % 12 + 12) % 12;
        return (masks[scale] >> rel) & 1;
    }

    // nota permitida más cercana, con histéresis para que no "tiemble" entre dos notas
    inline int nearestTarget(float midi, int key, int scale, int prev)
    {
        const int base = (int) std::lround(midi);
        int best = base; float bd = 1e9f;
        for (int o = -6; o <= 6; ++o)
        {
            const int c = base + o;
            if (!allowed(((c % 12) + 12) % 12, key, scale)) continue;
            const float d = std::abs(midi - (float) c);
            if (d < bd) { bd = d; best = c; }
        }
        if (prev >= 0 && allowed(prev % 12, key, scale) && std::abs(midi - (float) prev) < bd + 0.25f) return prev;
        return best;
    }

    //==========================================================================
    // Detector de tono tipo YIN sobre audio decimado a ~11 kHz.
    class Detector
    {
    public:
        struct Result { float f0 = 0.f, d = 1.f; bool voiced = false; };

        void prepare(double sr)
        {
            dec = std::max(1, (int) std::lround(sr / 11025.0));
            fs = (float) (sr / dec);
            tauMax = (int) (fs / 60.f) + 1;
            tauMin = std::max(2, (int) (fs / 1000.f));
            N = W + tauMax;
            ring.assign(2048, 0.f); win.assign((size_t) N, 0.f);
            d.assign((size_t) tauMax + 2, 0.f); cm.assign((size_t) tauMax + 2, 0.f);
            idx = 0; acc = 0.f; accCount = 0; sinceHop = 0;
        }
        float hopSeconds(double sr) const { return (float) (hop * dec) / (float) sr; }

        // devuelve true cuando hay un resultado nuevo (cada ~11.6 ms)
        bool push(float x, Result& out)
        {
            acc += x;
            if (++accCount < dec) return false;
            ring[(size_t) idx] = acc / (float) dec; idx = (idx + 1) & 2047; acc = 0.f; accCount = 0;
            if (++sinceHop < hop) return false;
            sinceHop = 0; out = analyze(); return true;
        }
    private:
        Result analyze()
        {
            Result r;
            for (int j = 0; j < N; ++j) win[(size_t) j] = ring[(size_t) ((idx + 2048 - N + j) & 2047)];
            float e = 0.f; for (int j = 0; j < W; ++j) e += win[(size_t) j] * win[(size_t) j];
            if (std::sqrt(e / (float) W) < 0.004f) return r;

            for (int tau = 1; tau <= tauMax; ++tau)
            {
                float s = 0.f;
                for (int j = 0; j < W; ++j) { const float dd = win[(size_t) j] - win[(size_t) (j + tau)]; s += dd * dd; }
                d[(size_t) tau] = s;
            }
            cm[0] = 1.f; float run = 0.f;
            for (int tau = 1; tau <= tauMax; ++tau) { run += d[(size_t) tau]; cm[(size_t) tau] = run > 0.f ? d[(size_t) tau] * (float) tau / run : 1.f; }

            int t = -1;
            for (int tau = tauMin; tau < tauMax; ++tau)
                if (cm[(size_t) tau] < 0.18f)
                {
                    while (tau + 1 < tauMax && cm[(size_t) tau + 1] < cm[(size_t) tau]) ++tau;
                    t = tau; break;
                }
            if (t < 0)
            {
                float mn = 1e9f;
                for (int tau = tauMin; tau < tauMax; ++tau) if (cm[(size_t) tau] < mn) { mn = cm[(size_t) tau]; t = tau; }
                if (mn > 0.35f) return r;
            }
            float tf = (float) t;
            if (t > 1 && t < tauMax)
            {
                const float a = cm[(size_t) t - 1], b = cm[(size_t) t], c = cm[(size_t) t + 1], den = a - 2.f * b + c;
                if (std::abs(den) > 1e-9f) tf = (float) t + 0.5f * (a - c) / den;
            }
            r.f0 = fs / tf; r.d = cm[(size_t) t]; r.voiced = r.f0 >= 60.f && r.f0 <= 1000.f;
            return r;
        }

        static constexpr int W = 384, hop = 128;
        int dec = 4, tauMax = 184, tauMin = 11, N = 570, idx = 0, accCount = 0, sinceHop = 0;
        float fs = 11025.f, acc = 0.f;
        std::vector<float> ring, win, d, cm;
    };

    //==========================================================================
    // Cambio de tono por dos líneas de retardo con fundido (Hann). La longitud de ventana se
    // ajusta a un múltiplo del periodo del sonido detectado (síncrono al tono) para evitar
    // el filtrado en peine típico de este método.
    class Shifter
    {
    public:
        void prepare(double sr)
        {
            size = (int) (sr * 0.07) + 8;
            for (auto& b : buf) b.assign((size_t) size, 0.f);
            wp = 0; p = 0.5f; L = Lt = (float) (0.025 * sr);
        }
        void setWindowTarget(float samples) { Lt = samples; }

        void process(const float* in, int nch, float ratio, int dryDelay, float* wet, float* dry)
        {
            L += (Lt - L) * 0.0008f;
            p += (1.f - ratio) / L; p -= std::floor(p);
            float p2 = p + 0.5f; p2 -= std::floor(p2);
            const float w1 = 0.5f - 0.5f * std::cos(6.2831853f * p), w2 = 1.f - w1;
            const float d1 = p * L, d2 = p2 * L;
            for (int ch = 0; ch < nch; ++ch)
            {
                auto& b = buf[(size_t) ch];
                b[(size_t) wp] = in[ch];
                wet[ch] = w1 * tap(b, d1) + w2 * tap(b, d2);
                dry[ch] = b[(size_t) ((wp - dryDelay + size) % size)];
            }
            wp = (wp + 1) % size;
        }
    private:
        float tap(const std::vector<float>& b, float d) const
        {
            float rp = (float) wp - d; while (rp < 0.f) rp += (float) size;
            const int i0 = (int) rp; const float fr = rp - (float) i0; const int i1 = (i0 + 1) % size;
            return b[(size_t) i0] * (1.f - fr) + b[(size_t) i1] * fr;
        }
        std::array<std::vector<float>, 2> buf;
        int size = 1, wp = 0; float p = 0.5f, L = 1.f, Lt = 1.f;
    };
}
