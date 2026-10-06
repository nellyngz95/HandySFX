/*
  ==============================================================================

    PitchShift.h
    Created: 18 Aug 2026 5:51:17pm
    Author:  Nelly Victoria Alexandra Garcia Sihuay

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class PitchShift
{
public:
    PitchShift();

    void prepareShift(double sampleRate, int samplesPerBlock);
    void processShift(juce::AudioBuffer<float>& buffer);

    void setPitchRatio(float ratio);

private:
    float pitchRatio = 1.0f;
    
};
