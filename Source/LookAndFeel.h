/*
  ==============================================================================

    LookAndFeel.h
    Created: 5 Oct 2026 5:00:56pm
    Author:  Nelly Victoria Alexandra Garcia Sihuay

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class HandyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Colors
    const juce::Colour background  =juce::Colour(0xff0d0d0d);
    const juce::Colour accentGreen =juce::Colour(0xff00ff9f);
    const juce::Colour dimGreen    =juce::Colour(0xff004d30);
    const juce::Colour panelDark   =juce::Colour(0xff111111);
    const juce::Colour textColour  =juce::Colour(0xffcccccc);
    // BUTTONS
    void drawButtonBackground(juce::Graphics& g,juce::Button& button,const juce::Colour&,bool isHighlighted,bool isDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);

        if(button.getToggleState())
        {
            g.setColour(accentGreen.withAlpha(0.15f));
            g.fillRoundedRectangle(bounds, 4.0f);
            g.setColour(accentGreen);
            g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        }
        else if(isHighlighted)
        {
            g.setColour(dimGreen.withAlpha(0.3f));
            g.fillRoundedRectangle(bounds, 4.0f);
            g.setColour(dimGreen);
            g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        }
        else
        {
            g.setColour(panelDark);
            g.fillRoundedRectangle(bounds, 4.0f);
            g.setColour(dimGreen);
            g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        }
    }

    void drawButtonText(juce::Graphics& g,juce::TextButton& button,bool, bool) override
    {
        g.setFont(11.0f);
        if(button.getToggleState())
            g.setColour(accentGreen);
        else
            g.setColour(textColour.withAlpha(0.5f));

        g.drawText(
            button.getButtonText().toUpperCase(),button.getLocalBounds(),juce::Justification::centred);
    }
    // SLIDERS
    void drawRotarySlider(juce::Graphics& g,int x, int y, int width, int height,float sliderPos,float rotaryStartAngle,float rotaryEndAngle,juce::Slider&) override
    {
        float radius = juce::jmin(width, height) / 2.0f - 4.0f;
        float centreX = x + width * 0.5f;
        float centreY = y + height * 0.5f;
        float angle = rotaryStartAngle +sliderPos *(rotaryEndAngle - rotaryStartAngle);

        // Background circle
        g.setColour(panelDark);
        g.fillEllipse(centreX - radius,centreY - radius,radius * 2.0f,radius * 2.0f);

        // Arc track
        juce::Path track;
        track.addArc(centreX - radius,centreY - radius,radius * 2.0f,radius * 2.0f,rotaryStartAngle,rotaryEndAngle,true);
        g.setColour(dimGreen);
        g.strokePath(track,juce::PathStrokeType(2.0f));

        // Arc fill → green
        juce::Path fill;
        fill.addArc(centreX - radius,centreY - radius,radius * 2.0f,radius * 2.0f,rotaryStartAngle,angle,true);
        g.setColour(accentGreen);
        g.strokePath(fill,juce::PathStrokeType(2.0f));

        // Pointer line
        juce::Path pointer;
        float pointerLength = radius * 0.6f;
        pointer.startNewSubPath(0.0f,-pointerLength);
        pointer.lineTo(0.0f, -radius * 0.9f);
        g.setColour(accentGreen);
        g.strokePath(pointer,juce::PathStrokeType(2.0f),juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // Outer ring
        g.setColour(accentGreen.withAlpha(0.3f));
        g.drawEllipse(centreX - radius,centreY - radius,radius * 2.0f,radius * 2.0f,1.0f);
    }

    void drawLabel(juce::Graphics& g,juce::Label& label) override
    {
        g.setColour(textColour.withAlpha(0.6f));
        g.setFont(11.0f);
        g.drawText(label.getText().toUpperCase(),label.getLocalBounds(),juce::Justification::centred);
    }
};
