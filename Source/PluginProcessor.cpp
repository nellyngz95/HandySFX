/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PitchShift.h"

//==============================================================================
HandySFXAudioProcessor::HandySFXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",
                           juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output",
                           juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters",createParameters())
#endif
{
}
HandySFXAudioProcessor::~HandySFXAudioProcessor()
{
}

//==============================================================================
const juce::String HandySFXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HandySFXAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HandySFXAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HandySFXAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HandySFXAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HandySFXAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HandySFXAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HandySFXAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HandySFXAudioProcessor::getProgramName (int index)
{
    return {};
}

void HandySFXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HandySFXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = (juce::uint32) juce::jmax (getTotalNumInputChannels(),getTotalNumOutputChannels());
    pitchShifter.prepareShift(sampleRate, samplesPerBlock);
    
    RadioHPF.prepare(spec);
    RadioLPF.prepare(spec);
    
    *RadioHPF.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 500.0f);
    *RadioLPF.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 3000.0f);
    
    MonsterLS.prepare(spec);
    *MonsterLS.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 200.0f, 1.0f, 6.0f);
    
    ChipmunkHS.prepare(spec);
    *ChipmunkHS.state = *Coefficients::makeHighShelf(sampleRate, 3000.0f, 0.7f, 6.0f);
    
    CaveF.prepare(spec);
    RCave.prepare(spec);
    DCave.prepare(spec);
    //delay samples = delay time (seconds) × sample rate
    DCave.setDelay(0.2f * sampleRate);
    *CaveF.state=*Coefficients::makeLowShelf(sampleRate, 300.0f, 0.7f, 6.0f);
    setCaveParameters(0.9f, 0.3f, 0.6f, 0.4f, 1.0f);//setCaveParameters(roomSizeSlider.getValue(),dampingSlider.getValue()wetSlider.getValue(),drySlider.getValue(),widthSlider.getValue());
    //Megaphone
    MHPF.prepare(spec);
    MLPF.prepare(spec);

    *MHPF.state = *Coefficients::makeHighPass(sampleRate, 500.0f);
    *MLPF.state = *Coefficients::makeLowPass(sampleRate, 3000.0f);
    //Robot
    RobotLS.prepare(spec);
    *RobotLS.state=*Coefficients::makeLowShelf(sampleRate,200.0f, 0.7f, 6.0f);
    
    
    
}

void HandySFXAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HandySFXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
void HandySFXAudioProcessor::Radio(juce::AudioBuffer<float>& buffer,double sampleRate)
{
  
    float hpfFreq = *apvts.getRawParameterValue("radioHPF");
    float lpfFreq = *apvts.getRawParameterValue("radioLPF");
    float drive = *apvts.getRawParameterValue("radioDrive");

    *RadioHPF.state = *Coefficients::makeHighPass(sampleRate, hpfFreq);
    *RadioLPF.state = *Coefficients::makeLowPass(sampleRate, lpfFreq);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float>context(block);

    RadioHPF.process(context);
    RadioLPF.process(context);

    for(int channel = 0;channel < buffer.getNumChannels();++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for(int sample = 0;sample < buffer.getNumSamples();++sample)
                data[sample] = data[sample] /(1.0f + std::abs(data[sample] * drive));
        }
}

void HandySFXAudioProcessor::Monster(juce::AudioBuffer<float>& buffer)
{
    float pitch = *apvts.getRawParameterValue("monsterPitch");
    float drive = *apvts.getRawParameterValue("monsterDrive");
    float shelf = *apvts.getRawParameterValue("monsterShelf");

    pitchShifter.setPitchRatio(pitch);
    pitchShifter.processShift(buffer);

    *MonsterLS.state = *Coefficients::makeLowShelf(getSampleRate(), 250.0f, 0.7f, shelf);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    MonsterLS.process(context);

    float T = 0.8f;
    float softClip = 2.0f;

    for(int channel = 0;channel < buffer.getNumChannels();++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for(int sample = 0;
                sample < buffer.getNumSamples();
                ++sample)
            {
                data[sample] *= drive;
                data[sample] = juce::jlimit(
                    -T, T, data[sample]);
                data[sample] = std::tanh(
                    softClip * data[sample]) /
                    std::tanh(softClip);
            }
        }
    buffer.applyGain(0.5f);
}
void HandySFXAudioProcessor::Chipmunk(juce::AudioBuffer<float>& buffer)
{
   
    float pitch = *apvts.getRawParameterValue("chipmunkPitch");
    float shelf = *apvts.getRawParameterValue("chipmunkShelf");

    pitchShifter.setPitchRatio(pitch);
    pitchShifter.processShift(buffer);

    *ChipmunkHS.state = *Coefficients::makeHighShelf(getSampleRate(), 3000.0f, 0.7f, shelf);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float>
    context(block);
    ChipmunkHS.process(context);

    for(int channel = 0;channel < buffer.getNumChannels();++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for(int sample = 0;
                sample < buffer.getNumSamples();
                ++sample)
                data[sample] = std::tanh(data[sample]);
        }
    buffer.applyGain(0.6f);
}

void HandySFXAudioProcessor::Cave(juce::AudioBuffer<float>& buffer)
{
    
    float room  = *apvts.getRawParameterValue("caveRoom");
    float delay = *apvts.getRawParameterValue("caveDelay");
    float wet   = *apvts.getRawParameterValue("caveWet");

    // Update reverb params
    setCaveParameters(room, 0.3f, wet,1.0f - wet, 1.0f);

    // Update delay time
    DCave.setDelay(delay * getSampleRate());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float>context(block);
    CaveF.process(context);

       for(int channel = 0;channel < buffer.getNumChannels();++channel)
       {
           auto* data = buffer.getWritePointer(channel);
           for(int sample = 0;
               sample < buffer.getNumSamples();
               ++sample)
           {
               float delayedSample =
                   DCave.popSample(channel);
               DCave.pushSample(channel, data[sample]);
               data[sample] = data[sample] * 0.6f
                            + delayedSample * 0.4f;
           }
       }

       juce::dsp::AudioBlock<float> block2(buffer);
       juce::dsp::ProcessContextReplacing<float>
           context2(block2);
       RCave.process(context2);
       buffer.applyGain(0.7f);
}
void HandySFXAudioProcessor::setCaveParameters(float roomSize, float damping,float wetLevel,float dryLevel,float width)
{
    juce::dsp::Reverb::Parameters RParams;
    RParams.roomSize = roomSize;
    RParams.damping = damping;
    RParams.wetLevel = wetLevel;
    RParams.dryLevel = dryLevel;
    RParams.width = width;
    RCave.setParameters(RParams);
    
}

void HandySFXAudioProcessor::Megaphone(juce::AudioBuffer<float>& buffer)
{
    float hpf   = *apvts.getRawParameterValue(
            "megaphoneHPF");
        float lpf   = *apvts.getRawParameterValue(
            "megaphoneLPF");
        float drive = *apvts.getRawParameterValue(
            "megaphoneDrive");

        *MHPF.state =
            *Coefficients::makeHighPass(
                getSampleRate(), hpf);
        *MLPF.state =
            *Coefficients::makeLowPass(
                getSampleRate(), lpf);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float>
            context(block);
        MHPF.process(context);
        MLPF.process(context);

        float T = 0.6f;
        for(int channel = 0;
            channel < buffer.getNumChannels();
            ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for(int sample = 0;
                sample < buffer.getNumSamples();
                ++sample)
            {
                data[sample] *= drive;
                data[sample] = juce::jlimit(
                    -T, T, data[sample]);
                data[sample] = std::tanh(data[sample]);
            }
        }
        buffer.applyGain(0.5f);
}
void HandySFXAudioProcessor::Robot(juce::AudioBuffer<float>& buffer)
{

    float bitDepth = *apvts.getRawParameterValue(
            "robotBit");
        float ringFreq = *apvts.getRawParameterValue(
            "robotRing");
        float shelf    = *apvts.getRawParameterValue(
            "robotShelf");

        *RobotLS.state = *Coefficients::makeLowShelf(
            getSampleRate(), 200.0f, 0.7f, shelf);

        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float>
            context(block);
        RobotLS.process(context);

        float phaseIncrement =
            ringFreq / getSampleRate();
    //Ring modulator
    //output = input × sin(2π × frequency × time)
        for(int channel = 0;
            channel < buffer.getNumChannels();
            ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            float phase = robotPhase;

            for(int sample = 0;
                sample < buffer.getNumSamples();
                ++sample)
            {
                float osc = std::sin(
                    2.0f * M_PI * phase);
                data[sample] *= osc;
                phase += phaseIncrement;
                if(phase >= 1.0f) phase -= 1.0f;
            }
        }

    // Bitcrusher →output = round(input × levels) / levels--> levels = 2^bitDepth
        robotPhase += phaseIncrement *
            buffer.getNumSamples();
        if(robotPhase >= 1.0f) robotPhase -= 1.0f;

        float levels = std::pow(2.0f, bitDepth);
        for(int channel = 0;
            channel < buffer.getNumChannels();
            ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            for(int sample = 0;
                sample < buffer.getNumSamples();
                ++sample)
                data[sample] = std::round(
                    data[sample] * levels) / levels;
        }
        buffer.applyGain(0.8f);


}
void HandySFXAudioProcessor::pushSamplesToWaveform(const juce::AudioBuffer<float>& buffer)
{
    auto* data = buffer.getReadPointer(0);
    int writePos = waveformWritePos.load();
    
    for(int i = 0; i < buffer.getNumSamples(); i++)
    {
        waveformBuffer[writePos] = data[i];
        writePos = (writePos + 1) % 1024;
    }
    waveformWritePos.store(writePos);
}

void HandySFXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    //Voice
    pushSamplesToWaveform(buffer);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    switch(SelectionCharacter)
        {
            case Character::Radio:
                Radio(buffer, getSampleRate()); break;
            case Character::Monster:
                Monster(buffer); break;
            case Character::Chipmunk:
                Chipmunk(buffer); break;
            case Character::Cave:
                Cave(buffer); break;
            case Character::Megaphone:
                Megaphone(buffer); break;
            case Character::Robot:
                Robot(buffer); break;
            default: break;
        }
    
}

//==============================================================================
bool HandySFXAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HandySFXAudioProcessor::createEditor()
{
    return new HandySFXAudioProcessorEditor (*this);
}

//==============================================================================
void HandySFXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void HandySFXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HandySFXAudioProcessor();
}
 juce::AudioProcessorValueTreeState::ParameterLayout HandySFXAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Radio
    params.push_back(std::make_unique<juce::AudioParameterFloat>("radioHPF", "Radio HPF",200.0f, 2000.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("radioLPF", "Radio LPF",1000.0f, 6000.0f, 3000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("radioDrive", "Radio Drive",1.0f, 10.0f, 2.0f));

    // Monster
    params.push_back(std::make_unique<juce::AudioParameterFloat>("monsterPitch", "Monster Pitch",0.3f, 0.9f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("monsterDrive", "Monster Drive",1.0f, 10.0f, 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("monsterShelf", "Monster Shelf",0.0f, 12.0f, 6.0f));

    // Chipmunk
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chipmunkPitch", "Chipmunk Pitch",1.1f, 2.0f, 1.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("chipmunkShelf", "Chipmunk Shelf",0.0f, 12.0f, 3.0f));

    // Cave
    params.push_back(std::make_unique<juce::AudioParameterFloat>("caveRoom", "Cave Room",0.0f, 1.0f, 0.9f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("caveDelay", "Cave Delay",0.05f, 0.5f, 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("caveWet", "Cave Wet",0.0f, 1.0f, 0.6f));

    // Megaphone
    params.push_back(std::make_unique<juce::AudioParameterFloat>("megaphoneHPF", "Megaphone HPF",200.0f, 2000.0f, 500.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("megaphoneLPF", "Megaphone LPF",1000.0f, 6000.0f, 3000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("megaphoneDrive", "Megaphone Drive",1.0f, 10.0f, 6.0f));

    // Robot
    params.push_back(std::make_unique<juce::AudioParameterFloat>("robotBit", "Robot Bit Depth",2.0f, 16.0f, 8.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("robotRing", "Robot Ring Freq",20.0f, 200.0f, 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("robotShelf", "Robot Shelf",0.0f, 12.0f, 6.0f));

    return { params.begin(), params.end() };
}
