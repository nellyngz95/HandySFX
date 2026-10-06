#include "PluginProcessor.h"
#include "PluginEditor.h"

HandySFXAudioProcessorEditor::
HandySFXAudioProcessorEditor(
    HandySFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    startTimerHz(30);
    setLookAndFeel(&lookAndFeel);
    for(auto* btn : { &radioBtn, &monsterBtn,&chipmunkBtn, &caveBtn,&megaphoneBtn, &robotBtn })
        addAndMakeVisible(btn);
    // Setup button callbacks
    radioBtn.onClick = [this]() {selectPreset(Character::Radio); };
    monsterBtn.onClick = [this]() {selectPreset(Character::Monster); };
    chipmunkBtn.onClick = [this]() {selectPreset(Character::Chipmunk); };
    caveBtn.onClick = [this]() {selectPreset(Character::Cave); };
    megaphoneBtn.onClick = [this]() {selectPreset(Character::Megaphone); };
    robotBtn.onClick = [this]() {selectPreset(Character::Robot); };

    // Toggle state
    radioBtn.setClickingTogglesState(true);
    monsterBtn.setClickingTogglesState(true);
    chipmunkBtn.setClickingTogglesState(true);
    caveBtn.setClickingTogglesState(true);
    megaphoneBtn.setClickingTogglesState(true);
    robotBtn.setClickingTogglesState(true);

    // Radio group → only one selected at a time
    for(auto* btn : { &radioBtn, &monsterBtn,
        &chipmunkBtn, &caveBtn,
        &megaphoneBtn, &robotBtn })
        btn->setRadioGroupId(1);
    setupKnob(radioHPFKnob, radioHPFLabel,"HPF", 200.0, 2000.0, 500.0);
    setupKnob(radioLPFKnob, radioLPFLabel,"LPF", 1000.0, 6000.0, 3000.0);
    setupKnob(radioDriveKnob, radioDriveLabel,"Drive", 1.0, 10.0, 2.0);
    // Monster
    setupKnob(monsterPitchKnob, monsterPitchLabel,"Pitch", 0.3, 0.9, 0.7);
    setupKnob(monsterDriveKnob, monsterDriveLabel,"Drive", 1.0, 10.0, 4.0);
    setupKnob(monsterShelfKnob, monsterShelfLabel,"Shelf", 0.0, 12.0, 6.0);

    // Chipmunk
    setupKnob(chipmunkPitchKnob, chipmunkPitchLabel,"Pitch", 1.1, 2.0, 1.3);
    setupKnob(chipmunkShelfKnob, chipmunkShelfLabel,"Shelf", 0.0, 12.0, 3.0);

    // Cave
    setupKnob(caveRoomKnob, caveRoomLabel,"Room", 0.0, 1.0, 0.9);
    setupKnob(caveDelayKnob, caveDelayLabel,"Delay", 0.05, 0.5, 0.1);
    setupKnob(caveWetKnob, caveWetLabel,"Wet", 0.0, 1.0, 0.6);

    // Megaphone
    setupKnob(megaphoneHPFKnob, megaphoneHPFLabel,"HPF", 200.0, 2000.0, 500.0);
    setupKnob(megaphoneLPFKnob, megaphoneLPFLabel,"LPF", 1000.0, 6000.0, 3000.0);
    setupKnob(megaphoneDriveKnob, megaphoneDriveLabel,"Drive", 1.0, 10.0, 6.0);

    // Robot
    setupKnob(robotBitKnob, robotBitLabel,"Bits", 2.0, 16.0, 8.0);
    setupKnob(robotRingKnob, robotRingLabel,"Ring", 20.0, 200.0, 50.0);
    setupKnob(robotShelfKnob, robotShelfLabel,"Shelf", 0.0, 12.0, 6.0);
    
    //ATTACHEMENTS
    auto& apvts = audioProcessor.apvts;

    // Radio
    radioHPFAttach = std::make_unique<Attachment>(apvts, "radioHPF", radioHPFKnob);
    radioLPFAttach = std::make_unique<Attachment>(apvts, "radioLPF", radioLPFKnob);
    radioDriveAttach = std::make_unique<Attachment>(apvts, "radioDrive", radioDriveKnob);

    // Monster
    monsterPitchAttach = std::make_unique<Attachment>(apvts, "monsterPitch", monsterPitchKnob);
    monsterDriveAttach = std::make_unique<Attachment>(apvts, "monsterDrive", monsterDriveKnob);
    monsterShelfAttach = std::make_unique<Attachment>(apvts, "monsterShelf", monsterShelfKnob);

    // Chipmunk
    chipmunkPitchAttach = std::make_unique<Attachment>(apvts, "chipmunkPitch", chipmunkPitchKnob);
    chipmunkShelfAttach = std::make_unique<Attachment>(apvts, "chipmunkShelf", chipmunkShelfKnob);

    // Cave
    caveRoomAttach = std::make_unique<Attachment>(apvts, "caveRoom", caveRoomKnob);
    caveDelayAttach = std::make_unique<Attachment>(apvts, "caveDelay", caveDelayKnob);
    caveWetAttach = std::make_unique<Attachment>(apvts, "caveWet", caveWetKnob);

    // Megaphone
    megaphoneHPFAttach = std::make_unique<Attachment>(apvts, "megaphoneHPF", megaphoneHPFKnob);
    megaphoneLPFAttach = std::make_unique<Attachment>(apvts, "megaphoneLPF", megaphoneLPFKnob);
    megaphoneDriveAttach = std::make_unique<Attachment>(apvts, "megaphoneDrive", megaphoneDriveKnob);

    // Robot
    robotBitAttach = std::make_unique<Attachment>(apvts, "robotBit", robotBitKnob);
    robotRingAttach = std::make_unique<Attachment>(apvts, "robotRing", robotRingKnob);
    robotShelfAttach = std::make_unique<Attachment>(apvts, "robotShelf", robotShelfKnob);
    
    hideAllKnobs();
    setSize(700, 400);
}

HandySFXAudioProcessorEditor::~HandySFXAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void HandySFXAudioProcessorEditor::setupKnob(juce::Slider& knob,juce::Label& label,const juce::String& name,double min, double max,double defaultVal)
{
    knob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    knob.setRange(min, max);
    knob.setValue(defaultVal);
    knob.setTextBoxStyle(juce::Slider::NoTextBox,
        false, 0, 0);
    addAndMakeVisible(knob);

    label.setText(name,juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}
void HandySFXAudioProcessorEditor::hideAllKnobs()
{
    std::vector<juce::Component*> components = {&radioHPFKnob, &radioLPFKnob, &radioDriveKnob,&radioHPFLabel, &radioLPFLabel, &radioDriveLabel,
&monsterPitchKnob, &monsterDriveKnob, &monsterShelfKnob,&monsterPitchLabel, &monsterDriveLabel, &monsterShelfLabel,&chipmunkPitchKnob, &chipmunkShelfKnob,
&chipmunkPitchLabel, &chipmunkShelfLabel,&caveRoomKnob, &caveDelayKnob, &caveWetKnob,&caveRoomLabel, &caveDelayLabel, &caveWetLabel,&megaphoneHPFKnob, &megaphoneLPFKnob, &megaphoneDriveKnob,
&megaphoneHPFLabel, &megaphoneLPFLabel, &megaphoneDriveLabel,&robotBitKnob, &robotRingKnob, &robotShelfKnob,&robotBitLabel, &robotRingLabel, &robotShelfLabel
    };

    for(auto* c : components)
        c->setVisible(false);
}
void HandySFXAudioProcessorEditor::showRadioKnobs()
{
    radioHPFKnob.setVisible(true);
    radioLPFKnob.setVisible(true);
    radioDriveKnob.setVisible(true);
    radioHPFLabel.setVisible(true);
    radioLPFLabel.setVisible(true);
    radioDriveLabel.setVisible(true);
}
void HandySFXAudioProcessorEditor::showMonsterKnobs()
{
    monsterPitchKnob.setVisible(true);
    monsterDriveKnob.setVisible(true);
    monsterShelfKnob.setVisible(true);
    monsterPitchLabel.setVisible(true);
    monsterDriveLabel.setVisible(true);
    monsterShelfLabel.setVisible(true);
}

void HandySFXAudioProcessorEditor::showChipmunkKnobs()
{
    chipmunkPitchKnob.setVisible(true);
    chipmunkShelfKnob.setVisible(true);
    chipmunkPitchLabel.setVisible(true);
    chipmunkShelfLabel.setVisible(true);
}

void HandySFXAudioProcessorEditor::showCaveKnobs()
{
    caveRoomKnob.setVisible(true);
    caveDelayKnob.setVisible(true);
    caveWetKnob.setVisible(true);
    caveRoomLabel.setVisible(true);
    caveDelayLabel.setVisible(true);
    caveWetLabel.setVisible(true);
}

void HandySFXAudioProcessorEditor::showMegaphoneKnobs()
{
    megaphoneHPFKnob.setVisible(true);
    megaphoneLPFKnob.setVisible(true);
    megaphoneDriveKnob.setVisible(true);
    megaphoneHPFLabel.setVisible(true);
    megaphoneLPFLabel.setVisible(true);
    megaphoneDriveLabel.setVisible(true);
}

void HandySFXAudioProcessorEditor::showRobotKnobs()
{
    robotBitKnob.setVisible(true);
    robotRingKnob.setVisible(true);
    robotShelfKnob.setVisible(true);
    robotBitLabel.setVisible(true);
    robotRingLabel.setVisible(true);
    robotShelfLabel.setVisible(true);
}

void HandySFXAudioProcessorEditor::selectPreset(Character preset)
{
    hideAllKnobs();

    switch(preset)
    {
        case Character::Radio:
            showRadioKnobs(); break;
        case Character::Monster:
            showMonsterKnobs(); break;
        case Character::Chipmunk:
            showChipmunkKnobs(); break;
        case Character::Cave:
            showCaveKnobs(); break;
        case Character::Megaphone:
            showMegaphoneKnobs(); break;
        case Character::Robot:
            showRobotKnobs(); break;
        default: break;
    }

    audioProcessor.SelectionCharacter = preset;
}
void HandySFXAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xff0d0d0d));

    // Header bar
    g.setColour(juce::Colour(0xff111111));
    g.fillRect(0, 0, getWidth(), 30);

    // Title
    g.setColour(juce::Colour(0xff00ff9f));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText("HANDYSFX",10, 0, 200, 30,juce::Justification::centredLeft);

    // Version
    g.setColour(juce::Colour(0xff004d30));
    g.setFont(juce::Font(11.0f, juce::Font::plain));
    g.drawText("v1.0",getWidth()-50, 0, 40, 30,juce::Justification::centredRight);

    // Dividers
    g.setColour(juce::Colour(0xff00ff9f).withAlpha(0.2f));
    g.drawHorizontalLine(30, 0, getWidth());

    int col1 = getWidth() * 0.45f;
    int col2 = getWidth() * 0.65f;
    g.drawVerticalLine(col1, 30, getHeight());
    g.drawVerticalLine(col2, 30, getHeight());
    // Oscilloscope area
    auto oscArea = juce::Rectangle<int>(5, 35, col1 - 10, getHeight() - 40);

    // Dark green CRT background
    g.setColour(juce::Colour(0xff001a00));
    g.fillRect(oscArea);

    // Draw waveform
    g.setColour(juce::Colour(0xff00ff9f));

    int writePos =audioProcessor.waveformWritePos.load();

    float width    = oscArea.getWidth();
    float height   = oscArea.getHeight();
    float centerY  = oscArea.getCentreY();
    float amplitude = height * 0.45f;

    juce::Path waveformPath;
    bool started = false;

    for(int i = 0; i < 1024; i++)
    {
        int readPos = (writePos + i) % 1024;

        float x = oscArea.getX() +(i / 1024.0f) * width;
        float y = centerY -audioProcessor.waveformBuffer[readPos]* amplitude;

        if(!started)
        {
            waveformPath.startNewSubPath(x, y);
            started = true;
        }
        else
        {
            waveformPath.lineTo(x, y);
        }
    }

    g.strokePath(waveformPath,
    juce::PathStrokeType(2.0f));

    // Scanlines
    g.setColour(juce::Colour(0x10000000));
    for(int y = oscArea.getY();
        y < oscArea.getBottom(); y += 3)
        g.drawHorizontalLine(y,oscArea.getX(), oscArea.getRight());

    // Border
    g.setColour(juce::Colour(0xff00ff9f).withAlpha(0.4f));
    g.drawRect(oscArea, 1);
}

void HandySFXAudioProcessorEditor::resized()
{
    int col1 = getWidth() * 0.45f;
    int col2 = getWidth() * 0.65f;
    int btnW = col2 - col1 - 10;
    int btnX = col1 + 5;
    int btnH = 40;
    int gap  = 8;
    int top  = 55;
    //BUTTONS
    radioBtn.setBounds(btnX, top, btnW, btnH);
    monsterBtn.setBounds(btnX, top + (btnH+gap), btnW, btnH);
    chipmunkBtn.setBounds(btnX, top + (btnH+gap)*2, btnW, btnH);
    caveBtn.setBounds(btnX, top + (btnH+gap)*3, btnW, btnH);
    megaphoneBtn.setBounds(btnX, top + (btnH+gap)*4, btnW, btnH);
    robotBtn.setBounds(btnX, top + (btnH+gap)*5, btnW, btnH);
    //KNOBS
    int knobSize = 55;
    int rightColWidth = getWidth() - col2;
    int knobX = col2 + (rightColWidth - knobSize) / 2;
    int knobY = top + 10;
    int labelH = 20;
    int knobGap = 8;

    // Helper lambda for setting knob bounds
    auto setKnobBounds = [&](
        juce::Slider& knob,
        juce::Label& label,
        int pos) // pos = vertical position
    {
        knob.setBounds(
            knobX,
            knobY + pos * (knobSize + labelH + knobGap),
            knobSize, knobSize);
        label.setBounds(
            knobX,
            knobY + pos * (knobSize + labelH + knobGap)
                + knobSize,
            knobSize, labelH);
    };
    // Radio
    setKnobBounds(radioHPFKnob, radioHPFLabel, 0);
    setKnobBounds(radioLPFKnob, radioLPFLabel, 1);
    setKnobBounds(radioDriveKnob, radioDriveLabel, 2);

    // Monster
    setKnobBounds(monsterPitchKnob, monsterPitchLabel, 0);
    setKnobBounds(monsterDriveKnob, monsterDriveLabel, 1);
    setKnobBounds(monsterShelfKnob, monsterShelfLabel, 2);

    // Chipmunk
    setKnobBounds(chipmunkPitchKnob, chipmunkPitchLabel, 0);
    setKnobBounds(chipmunkShelfKnob, chipmunkShelfLabel, 1);

    // Cave
    setKnobBounds(caveRoomKnob, caveRoomLabel, 0);
    setKnobBounds(caveDelayKnob, caveDelayLabel, 1);
    setKnobBounds(caveWetKnob, caveWetLabel, 2);

    // Megaphone
    setKnobBounds(megaphoneHPFKnob, megaphoneHPFLabel, 0);
    setKnobBounds(megaphoneLPFKnob, megaphoneLPFLabel, 1);
    setKnobBounds(megaphoneDriveKnob, megaphoneDriveLabel, 2);

    // Robot
    setKnobBounds(robotBitKnob, robotBitLabel, 0);
    setKnobBounds(robotRingKnob, robotRingLabel, 1);
    setKnobBounds(robotShelfKnob, robotShelfLabel, 2);
}

void HandySFXAudioProcessorEditor::timerCallback()
{
    repaint();
}
