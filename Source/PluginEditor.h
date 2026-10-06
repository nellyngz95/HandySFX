/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PitchShift.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class HandySFXAudioProcessorEditor: public juce::AudioProcessorEditor, public juce::Timer
{
public:
    HandySFXAudioProcessorEditor(HandySFXAudioProcessor&);
    ~HandySFXAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
   void setupKnob(juce::Slider& knob,juce::Label& label,const juce::String& name,double min, double max,double defaultVal);
    void hideAllKnobs();
    void showRadioKnobs();
    void showMonsterKnobs();
    void showChipmunkKnobs();
    void showCaveKnobs();
    void showMegaphoneKnobs();
    void showRobotKnobs();
    
    void selectPreset(Character preset);

    juce::TextButton radioBtn { "RADIO" },monsterBtn { "MONSTER" },chipmunkBtn { "CHIPMUNK" },caveBtn { "CAVE" },megaphoneBtn { "MEGAPHONE" },robotBtn { "ROBOT" };
    //RAdio
    juce::Slider radioHPFKnob, radioLPFKnob, radioDriveKnob;
    juce::Label  radioHPFLabel, radioLPFLabel, radioDriveLabel;
    // Monster
    juce::Slider monsterPitchKnob, monsterDriveKnob, monsterShelfKnob;
    juce::Label  monsterPitchLabel, monsterDriveLabel, monsterShelfLabel;

    // Chipmunk
    juce::Slider chipmunkPitchKnob, chipmunkShelfKnob;
    juce::Label  chipmunkPitchLabel, chipmunkShelfLabel;

    // Cave
    juce::Slider caveRoomKnob, caveDelayKnob, caveWetKnob;
    juce::Label  caveRoomLabel, caveDelayLabel, caveWetLabel;

    // Megaphone
    juce::Slider megaphoneHPFKnob, megaphoneLPFKnob, megaphoneDriveKnob;
    juce::Label  megaphoneHPFLabel, megaphoneLPFLabel, megaphoneDriveLabel;

    // Robot
    juce::Slider robotBitKnob, robotRingKnob, robotShelfKnob;
    juce::Label  robotBitLabel, robotRingLabel, robotShelfLabel;
   //Attachments
    std::unique_ptr<Attachment> radioHPFAttach,radioLPFAttach,radioDriveAttach;
    // Monster
    std::unique_ptr<Attachment> monsterPitchAttach,monsterDriveAttach,monsterShelfAttach;
    // Chipmunk
    std::unique_ptr<Attachment> chipmunkPitchAttach,chipmunkShelfAttach;
    // Cave
    std::unique_ptr<Attachment> caveRoomAttach,caveDelayAttach,caveWetAttach;
    // Megaphone
    std::unique_ptr<Attachment> megaphoneHPFAttach,megaphoneLPFAttach,megaphoneDriveAttach;
    // Robot
    std::unique_ptr<Attachment> robotBitAttach,robotRingAttach,robotShelfAttach;
private:
    HandySFXAudioProcessor& audioProcessor;
    HandyLookAndFeel lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HandySFXAudioProcessorEditor)
};
