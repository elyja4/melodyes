#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "../Source/Pitch.h"
#include <cstdio>
#include <random>
// Señal tipo voz: fundamental + armónicos con envolvente.
static float voice(double t, double f) { double s = 0; for (int h = 1; h <= 6; ++h) s += std::sin(2 * M_PI * f * h * t + h) / h; return (float) (0.3 * s); }
int main()
{
    const double sr = 44100.0;
    int fails = 0;
    // 1) detector
    for (double f : { 110.0, 220.0, 261.63, 330.0, 440.0, 660.0 })
    {
        Pitch::Detector det; det.prepare(sr); Pitch::Detector::Result r, last;
        for (int i = 0; i < (int) sr; ++i) if (det.push(voice(i / sr, f), r)) last = r;
        const double err = 1200.0 * std::log2(last.f0 / f);
        std::printf("detector %.2f Hz -> %.2f (%.1f cents) voiced=%d\n", f, last.f0, err, (int) last.voiced);
        if (!last.voiced || std::abs(err) > 15.0) { ++fails; std::puts("  FALLO"); }
    }
    // silencio => no voiced
    { Pitch::Detector det; det.prepare(sr); Pitch::Detector::Result r, last; for (int i = 0; i < 20000; ++i) if (det.push(0.f, r)) last = r; if (last.voiced) { ++fails; std::puts("silencio detectado como voz"); } }

    // 2) cuantización a escala
    struct T { float midi; int key, scale, expect; } tests[] = { { 60.4f, 0, 1, 60 }, { 61.2f, 0, 1, 62 }, { 66.0f, 0, 1, 65 }, { 61.0f, 0, 0, 61 }, { 63.3f, 9, 2, 64 } };
    for (auto& t : tests) { int g = Pitch::nearestTarget(t.midi, t.key, t.scale, -1); std::printf("target midi %.1f key %d scale %d -> %d (esperado %d)\n", t.midi, t.key, t.scale, g, t.expect); if (g != t.expect) ++fails; }

    // 3) cambio de tono: 220 Hz * 2^(1/12) y 2^(-3/12); se mide la frecuencia de salida por cruces por cero
    for (double semis : { 1.0, -3.0, 0.0, 5.0 })
    {
        Pitch::Shifter sh; sh.prepare(sr);
        const double f = 220.0, ratio = std::pow(2.0, semis / 12.0);
        sh.setWindowTarget((float) (2.0 * std::ceil(0.010 * f) * sr / f));   // L = 2*k*T
        const int n = (int) (sr * 2.0); int zc = 0; float prev = 0.f; double rms = 0, rmsIn = 0; int cnt = 0;
        for (int i = 0; i < n; ++i)
        {
            float in[2] = { voice(i / sr, f), voice(i / sr, f) }, wet[2], dry[2];
            sh.process(in, 2, (float) ratio, 550, wet, dry);
            if (i > (int) sr) { if (prev < 0.f && wet[0] >= 0.f) ++zc; prev = wet[0]; rms += wet[0] * wet[0]; rmsIn += in[0] * in[0]; ++cnt; }
        }
        // los armónicos añaden cruces: se compara contra una referencia sin cambio (ratio 1 con la misma señal)
        std::printf("shifter %+.0f st: cruces/s=%d  (ref 220Hz=220, esperado ~%.1f)  RMS out/in=%.2f\n", semis, zc, 220.0 * ratio, std::sqrt(rms / rmsIn));
        if (std::abs(zc - 220.0 * ratio) > 0.04 * 220.0 * ratio + 4) { ++fails; std::puts("  FALLO frecuencia"); }
        if (std::sqrt(rms / rmsIn) < 0.6 || std::sqrt(rms / rmsIn) > 1.4) { ++fails; std::puts("  FALLO nivel"); }
    }
    std::printf(fails ? "\n%d FALLOS\n" : "\nTODO OK\n", fails);
    return fails != 0;
}
