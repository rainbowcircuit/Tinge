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
    inline juce::Colour backgroundFill = juce::Colour(249, 204, 171);
    inline juce::Colour backgroundFillAlt = juce::Colour(249, 204, 171);
    inline juce::Colour backgroundFillAltShadow = juce::Colour(202, 214, 213);

    inline juce::Colour graphicWhite = { 245, 255, 242 };
    inline juce::Colour graphicWhiteAlt = { 235, 245, 232 };

    inline juce::Colour graphicBlack = { 39, 41, 40 };
    inline juce::Colour graphicBlackAlt = { 84, 86, 85 };

    inline juce::Colour graphicGrey = { 209, 204, 161 };

    inline juce::Array<juce::Colour> primaryColor = {
        juce::Colour::fromRGB(231, 157, 11),   // yellow
        juce::Colour::fromRGB(227, 90, 106),  // chartreuse
        juce::Colour::fromRGB(78, 132, 120),  // yellow-green
    };
    
    
};


struct Palette
{
    
    static juce::Colour interpolateColors(const juce::Colour& c1, const juce::Colour& c2, float t)
    {
        t = juce::jlimit(0.0f, 1.0f, t);
        float r = (1 - t) * c1.getFloatRed() + t * c2.getFloatRed();
        float g = (1 - t) * c1.getFloatGreen() + t * c2.getFloatGreen();
        float b = (1 - t) * c1.getFloatBlue() + t * c2.getFloatBlue();

        return juce::Colour::fromFloatRGBA(r, g, b, 1.0f);
    }

    static juce::Colour darkenColors(const juce::Colour& c1, const juce::Colour& c2)
    {
        float r = std::min(c1.getFloatRed(), c2.getFloatRed());
        float g = std::min(c1.getFloatGreen(), c2.getFloatGreen());
        float b = std::min(c1.getFloatBlue(), c2.getFloatBlue());

        return juce::Colour::fromFloatRGBA(r, g, b, 1.0f);
    }
    
    static juce::Colour darkenColors(const juce::Colour& c1, const juce::Colour& c2, const juce::Colour& c3)
    {
        return darkenColors(darkenColors(c1, c2), c3);
    }
    
    static juce::Colour multiplyColors(const juce::Colour& c1, const juce::Colour& c2)
    {
        float r = juce::jlimit(0.0f, 1.0f, c1.getFloatRed() * c2.getFloatRed());
        float g = juce::jlimit(0.0f, 1.0f, c1.getFloatGreen() * c2.getFloatGreen());
        float b = juce::jlimit(0.0f, 1.0f, c1.getFloatBlue() * c2.getFloatBlue());

        // Return the entire color with higher total brightness
        return juce::Colour::fromFloatRGBA(r + 0.1f, g + 0.1f, b + 0.1f, 1.0f);
    }
    

     
    void setColors(float alphaA, float alphaB, float alphaC)
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

