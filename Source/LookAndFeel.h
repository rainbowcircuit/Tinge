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
    inline juce::Colour backgroundFill = { 32, 32, 32 };
    inline juce::Colour backgroundFillAlt = { 42, 42, 42 };

    inline juce::Colour graphicRed = { 248, 72, 56 };
    inline juce::Colour graphicBlue = { 2, 129, 217 };
    inline juce::Colour graphicGreen = { 90, 219, 90 };
    inline juce::Colour graphicYellow = { 247, 234, 79 };
    inline juce::Colour graphicCyan = { 93, 214, 230 };
    inline juce::Colour graphicMagenta = { 217, 75, 182 };;
    inline juce::Colour graphicWhite = { 232, 242, 241 };
    inline juce::Colour graphicWhiteAlt = { 222, 232, 231 };
        
    inline juce::Colour graphicHoverRed = { 255, 92, 76 };
    inline juce::Colour graphicHoverBlue = { 22, 149, 237 };
    inline juce::Colour graphicHoverGreen = { 110, 239, 110 };
    inline juce::Colour graphicHoverYellow = { 255, 254, 99 };
    inline juce::Colour graphicHoverCyan = { 113, 234, 250 };
    inline juce::Colour graphicHoverMagenta = { 237, 95, 202 };;

    
    
    //
};


struct Palette
{
    juce::Colour addColors(const juce::Colour& color1, const juce::Colour& color2, const juce::Colour& color3)
    {
        auto r = juce::jlimit(0, 255, color1.getRed()   + color2.getRed() + color3.getRed());
        auto g = juce::jlimit(0, 255, color1.getGreen() + color2.getGreen() + color3.getGreen());
        auto b = juce::jlimit(0, 255, color1.getBlue()  + color2.getBlue() + color3.getBlue());
        auto a = juce::jlimit(0, 255, color1.getAlpha() + color2.getAlpha() + color3.getAlpha());

        return juce::Colour((juce::uint8)r, (juce::uint8)g, (juce::uint8)b, (juce::uint8)a);
    }
    
    juce::Colour addColors(juce::Colour& c1, juce::Colour& c2)
    {
        return addColors(c1, c2, { (juce::uint8)0, (juce::uint8)0, (juce::uint8)0, (juce::uint8)0 });
    }

    juce::Colour getColorFromHue(float hueValue)
    {
        float hue = juce::jlimit(0.0f, 1.0f, fmodf(hueValue/100.0f, 1.0f));
        return juce::Colour::fromHSV(hue, 1.0f, 1.0f, 1.0f);
    }
    
    void setColors(float baseHue, float hueOffset, float alpha0, float alpha1, float alpha2)
    {
        colorA = getColorFromHue(baseHue).withAlpha((float)alpha0);
        colorB = getColorFromHue(baseHue + hueOffset).withAlpha((float)alpha1);
        colorC = getColorFromHue(baseHue - hueOffset).withAlpha((float)alpha2);
        colorAB = addColors(colorA, colorB);
        colorBC = addColors(colorB, colorC);
        colorAC = addColors(colorA, colorC);
        colorABC = addColors(colorA, colorB, colorC);
    }
    
    juce::Colour colorA;
    juce::Colour colorB;
    juce::Colour colorC;
    juce::Colour colorAB;
    juce::Colour colorBC;
    juce::Colour colorAC;
    juce::Colour colorABC;
};

