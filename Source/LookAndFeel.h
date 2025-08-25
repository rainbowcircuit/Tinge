/*
  ==============================================================================

    LookAndFeel.h
    Created: 27 Jun 2025 4:21:34pm
    Author:  Takuma Matsui

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Colors {
    inline juce::Colour backgroundFill = juce::Colour(210, 210, 210);
    inline juce::Colour backgroundFillAlt = juce::Colour(203, 209, 195);
    inline juce::Colour backgroundFillAltShadow = juce::Colour(200, 206, 192);

    inline juce::Colour graphicWhite = { 244, 244, 244 };
    inline juce::Colour graphicBlack = { 60, 60, 60 };

    inline juce::Array<juce::Colour> primaryColor = {
        juce::Colour::fromRGB(212, 24, 112),   // yellow
        juce::Colour::fromRGB(212, 212, 0),  // chartreuse
        juce::Colour::fromRGB(3, 148, 194),  // yellow-green
    };
};


struct Palette
{
    
    juce::Colour interpolateColors(const juce::Colour& c1, const juce::Colour& c2, float t)
    {
        t = juce::jlimit(0.0f, 1.0f, t);
        float r = (1 - t) * c1.getFloatRed() + t * c2.getFloatRed();
        float g = (1 - t) * c1.getFloatGreen() + t * c2.getFloatGreen();
        float b = (1 - t) * c1.getFloatBlue() + t * c2.getFloatBlue();

        return juce::Colour::fromFloatRGBA(r, g, b, 1.0f);
    }

    juce::Colour darkenColors(const juce::Colour& c1, const juce::Colour& c2)
    {
        float floor = 0.2f;
        float r = std::min(c1.getFloatRed(), c2.getFloatRed()) + floor;
        float g = std::min(c1.getFloatGreen(), c2.getFloatGreen()) + floor;
        float b = std::min(c1.getFloatBlue(), c2.getFloatBlue()) + floor;

        return juce::Colour::fromFloatRGBA(r, g, b, 1.0f);
    }

    juce::Colour darkenColors(const juce::Colour& c1, const juce::Colour& c2, const juce::Colour& c3)
    {
        return darkenColors(darkenColors(c1, c2), c3);
    }
    

    juce::Colour getColorFromHue(float hueValue)
    {
        float hue = juce::jlimit(0.0f, 1.0f, fmodf(hueValue/100.0f, 1.0f));
        return juce::Colour::fromHSV(hue, 1.0f, 1.0f, 1.0f);
    }
    
    void setColors(int colorAIndex, int colorBIndex, int colorCIndex, float alphaA, float alphaB, float alphaC)
    {
        alphaA = juce::jlimit(0.0f, 1.0f, alphaA);
        alphaB = juce::jlimit(0.0f, 1.0f, alphaB);
        alphaC = juce::jlimit(0.0f, 1.0f, alphaC);
        
        colorA = interpolateColors(Colors::backgroundFill, Colors::primaryColor[0], alphaA);
        colorB = interpolateColors(Colors::backgroundFill, Colors::primaryColor[1], alphaB);
        colorC = interpolateColors(Colors::backgroundFill, Colors::primaryColor[2], alphaC);
        colorAB = darkenColors(colorA, colorB);
        colorBC = darkenColors(colorB, colorC);
        colorAC = darkenColors(colorA, colorC);
        colorABC = darkenColors(colorA, colorB, colorC);
    }
    
    juce::Colour colorA;
    juce::Colour colorB;
    juce::Colour colorC;
    juce::Colour colorAB;
    juce::Colour colorBC;
    juce::Colour colorAC;
    juce::Colour colorABC;
};

