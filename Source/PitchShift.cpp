/*
  ==============================================================================

    PitchShift.cpp
    Created: 18 Aug 2026 5:49:56pm
    Author:  Nelly Victoria Alexandra Garcia Sihuay

  ==============================================================================
*/

#include "PitchShift.h"

PitchShift::PitchShift() {}


void PitchShift::prepareShift(double sampleRate,int samplesPerBlock)
{(void)sampleRate;(void)samplesPerBlock;}

void PitchShift::processShift(juce::AudioBuffer<float>& buffer)
{
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    // Backup copy of original audio
    juce::AudioBuffer<float> PitchBuffer(numChannels, numSamples);
    
    // Copy ALL channels
    for (int ch = 0; ch < numChannels; ++ch)
        PitchBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
    
    for (int channel = 0;channel < numChannels; ++channel)
    {
        float* WPointer = buffer.getWritePointer(channel);
        const float* RPointer = PitchBuffer.getReadPointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sOriginal = i * pitchRatio;
            int sBefore = (int)sOriginal % numSamples;
            int sAfter = (sBefore + 1) % numSamples;
            float distance = sOriginal - (int)sOriginal;
            
            // Linear interpolation
            WPointer[i] = RPointer[sBefore] *(1.0f - distance) + RPointer[sAfter] * distance;
        }
    }
}

void PitchShift::setPitchRatio(float ratio)
{
    pitchRatio = ratio;
}
