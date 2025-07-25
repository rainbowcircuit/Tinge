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
    inline juce::Colour backgroundFill = { 180, 180, 180 };
    inline juce::Colour backgroundFillAlt = { 42, 42, 42 };
    inline juce::Colour graphicWhite = { 244, 244, 244 };

inline juce::Array<juce::Colour> gradientColor = {
    juce::Colour::fromRGB(254, 242, 0),   // yellow
    juce::Colour::fromRGB(202, 219, 44),  // chartreuse
    juce::Colour::fromRGB(142, 198, 63),  // yellow-green
    juce::Colour::fromRGB(60, 181, 74),   // spring green
    juce::Colour::fromRGB(1, 167, 81),    // green
    juce::Colour::fromRGB(1, 167, 116),   // blue-green
    juce::Colour::fromRGB(5, 168, 157),   // aqua green
    juce::Colour::fromRGB(5, 172, 198),   // aqua blue
    juce::Colour::fromRGB(1, 173, 238),   // turquoise blue
    juce::Colour::fromRGB(3, 141, 212),   // cerulean blue
    juce::Colour::fromRGB(0, 114, 187),   // blue
    juce::Colour::fromRGB(0, 84, 164),    // blue violet
    juce::Colour::fromRGB(47, 48, 148),   // violet
    juce::Colour::fromRGB(102, 46, 146),  // red violet
    juce::Colour::fromRGB(146, 40, 143),  // purple
    juce::Colour::fromRGB(188, 27, 141),  // fuchsia
    juce::Colour::fromRGB(238, 0, 141),   // magenta
    juce::Colour::fromRGB(235, 10, 115),  // blue red
    juce::Colour::fromRGB(238, 18, 92),   // red
    juce::Colour::fromRGB(239, 23, 65),   // orange red
    juce::Colour::fromRGB(236, 29, 37),   // orange
    juce::Colour::fromRGB(242, 101, 34),  // yellow orange
    juce::Colour::fromRGB(247, 148, 31),  // orange yellow
    juce::Colour::fromRGB(254, 194, 18)   // golden yellow
};

    
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
    
    void setColors(int colorAIndex, int colorBIndex, int colorCIndex, float alphaA, float alphaB, float alphaC)
    {
        colorAIndex = juce::jlimit(0, 23, colorAIndex);
        colorBIndex = juce::jlimit(0, 23, colorBIndex);
        colorCIndex = juce::jlimit(0, 23, colorCIndex);

        alphaA = juce::jlimit(0.0f, 1.0f, alphaA);
        alphaB = juce::jlimit(0.0f, 1.0f, alphaB);
        alphaC = juce::jlimit(0.0f, 1.0f, alphaC);

        colorA = Colors::gradientColor[colorAIndex].withAlpha((float)alphaA/3);
        colorB = Colors::gradientColor[colorBIndex].withAlpha((float)alphaB/3);
        colorC = Colors::gradientColor[colorCIndex].withAlpha((float)alphaC/3);
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

