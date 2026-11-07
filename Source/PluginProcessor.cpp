#include "PluginProcessor.h"
#include "PluginEditor.h"

MelodiesProcessor::MelodiesProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createLayout())
{
}

bool MelodiesProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    const auto in = l.getMainInputChannelSet(), out = l.getMainOutputChannelSet();
    return in == out && (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo());
}

void MelodiesProcessor::prepareToPlay(double sampleRate, int)
{
    sr = sampleRate;
    detector.prepare(sr); shifter.prepare(sr);
    hopSec = detector.hopSeconds(sr);
    ratioSm.reset(sr, hopSec); ratioSm.setCurrentAndTargetValue(1.f);
    dryDelay = (int) (0.0125 * sr);
    setLatencySamples(dryDelay);            // retardo aproximado del camino procesado (informado al host)
    shiftSemis = 0.f; curTarget = stableTarget = candTarget = -1; candCount = 0; unvoicedHops = 0; f0Count = 0;
    shifter.setWindowTarget((float) (0.025 * sr));
}

// Se ejecuta cada ~11.6 ms con el resultado del detector.
void MelodiesProcessor::onDetection(const Pitch::Detector::Result& r, int key, int scale, float speedMs, float amount)
{
    ++sinceChange;
    if (r.voiced)
    {
        unvoicedHops = 0;
        // mediana de 3 para evitar saltos de octava
        f0Hist = { f0Hist[1], f0Hist[2], r.f0 }; f0Count = juce::jmin(3, f0Count + 1);
        std::array<float, 3> s = f0Hist; std::sort(s.begin(), s.end());
        const float f0 = f0Count >= 3 ? s[1] : r.f0;

        const float midi = 69.f + 12.f * std::log2(f0 / 440.f);
        dispMidi = dispMidi < 0.f ? midi : dispMidi + (midi - dispMidi) * 0.5f;
        curTarget = Pitch::nearestTarget(midi, key, scale, curTarget);

        const float want = ((float) curTarget - midi) * amount;
        const float coef = speedMs <= 0.5f ? 1.f : 1.f - std::exp(-hopSec / (speedMs * 0.001f));
        shiftSemis += (want - shiftSemis) * coef;

        // ventana del cambiador = 2 * k * periodo (síncrona al tono)
        const float period = (float) sr / f0, k = std::ceil(0.010f * f0);
        shifter.setWindowTarget(juce::jlimit((float) (0.016 * sr), (float) (0.040 * sr), 2.f * k * period));

        // cambio de nota con antirrebote (~35 ms) para disparar las hojas
        if (curTarget == stableTarget) candCount = 0;
        else if (curTarget == candTarget) { if (++candCount >= 3 && (sinceChange > 8 || stableTarget < 0)) { stableTarget = curTarget; candCount = 0; sinceChange = 0; noteChanges.fetch_add(1); } }
        else { candTarget = curTarget; candCount = 1; }

        detMidi.store(dispMidi); tgtMidi.store((float) curTarget);
    }
    else
    {
        if (++unvoicedHops > 3) shiftSemis *= 0.8f;
        if (unvoicedHops > 13)
        {
            stableTarget = curTarget = -1; dispMidi = -1.f; f0Count = 0;
            shifter.setWindowTarget((float) (0.025 * sr));
            detMidi.store(-1.f); tgtMidi.store(-1.f);
        }
    }
    ratioSm.setTargetValue(std::pow(2.f, shiftSemis / 12.f));
}

void MelodiesProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples(), nc = juce::jmin(buffer.getNumChannels(), 2);
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch) buffer.clear(ch, 0, n);

    auto P = [this](const char* id) { return apvts.getRawParameterValue(id)->load(); };
    const int key = (int) std::lround(P(Param::key)), scale = (int) std::lround(P(Param::scale));
    const float speedMs = P(Param::speed), amount = P(Param::amount) * 0.01f, mix = P(Param::mix) * 0.01f;

    Pitch::Detector::Result res;
    float in[2] = { 0.f, 0.f }, wet[2] = { 0.f, 0.f }, dry[2] = { 0.f, 0.f };
    for (int i = 0; i < n; ++i)
    {
        float mono = 0.f;
        for (int ch = 0; ch < nc; ++ch) { in[ch] = buffer.getReadPointer(ch)[i]; mono += in[ch]; }
        if (detector.push(mono / (float) nc, res)) onDetection(res, key, scale, speedMs, amount);

        shifter.process(in, nc, ratioSm.getNextValue(), dryDelay, wet, dry);
        for (int ch = 0; ch < nc; ++ch) buffer.getWritePointer(ch)[i] = dry[ch] * (1.f - mix) + wet[ch] * mix;
    }
    outLevel.store(buffer.getMagnitude(0, n));
}

juce::AudioProcessorEditor* MelodiesProcessor::createEditor() { return new MelodiesEditor(*this); }

void MelodiesProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary(*xml, dest);
}
void MelodiesProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new MelodiesProcessor(); }
