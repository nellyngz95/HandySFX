//================= HANDY SFX======================
/**
 HandySFX is a plug-in that a llows you to change your voice over time.
 It's an idea of using some of the most common voices. for post production
 
 
*/

#pragma once

#include <JuceHeader.h>
#include "PitchShift.h"
#include <mutex> 


using Filter = juce::dsp::IIR::Filter<float>;
using Coefficients = juce::dsp::IIR::Coefficients<float>;
using FilterDuplicator = juce::dsp::ProcessorDuplicator<Filter, Coefficients>;

enum class Character
{
    Radio,
    Monster,
    Chipmunk,
    Cave,
    Megaphone,
    Robot,
    None
};

class HandySFXAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    HandySFXAudioProcessor();
    ~HandySFXAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
// Creating different functions for the different voices
 //Radio, monster, chipmunk,cave, megaphone and a robot
    void Radio(juce::AudioBuffer<float>& buffer, double sampleRate);//send the information of the buffer, gets num channels start sample and numsamples. Radio is filters Low pass and high pass.
    void Monster(juce::AudioBuffer<float>& buffer);// Pitch shift, Low shelf filter, Saturation distortion and gain compensation.
    void Chipmunk(juce::AudioBuffer<float>& buffer);
    void Cave(juce::AudioBuffer<float>& buffer);
    void Megaphone(juce::AudioBuffer<float>& buffer);
    void Robot(juce::AudioBuffer<float>& buffer);
    void None(juce::AudioBuffer<float>& buffer);
    

//Elements for Radio 2 Filters
//juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,juce::dsp::IIR::Coefficients<float>> Variable
    FilterDuplicator RadioHPF;
    FilterDuplicator RadioLPF;
//Elements for Monster Low Shelve Filter
    FilterDuplicator MonsterLS;
//Elements for the Chipmunks:
    FilterDuplicator ChipmunkHS;
//Elements for the Cave
    juce::dsp::Reverb RCave;
    FilterDuplicator CaveF;
    juce::dsp::DelayLine<float,juce::dsp::DelayLineInterpolationTypes::Linear> DCave{ 44100 };
    void setCaveParameters(float roomSize,float damping,float wetLevel,float dryLevel,float width);
//Elements for the Megaphone
    FilterDuplicator MHPF;
    FilterDuplicator MLPF;
//Elements for the Robot
    FilterDuplicator RobotLS;
    float robotPhase=0.0f;

    //Karen inspired waveform
    juce::Rectangle<int> oscilloscopeArea;
    juce::Timer* repaintTimer;
    std::array<float, 1024> waveformBuffer { {} };
    std::atomic<int> waveformWritePos { 0 };
    void pushSamplesToWaveform( const juce::AudioBuffer<float>& buffer);
    Character SelectionCharacter = Character::None;

private:
    //==============================================================================
    
    PitchShift pitchShifter;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HandySFXAudioProcessor)
};
