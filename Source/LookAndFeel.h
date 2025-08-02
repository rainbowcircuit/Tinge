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
    inline juce::Colour backgroundFill = juce::Colour(34, 35, 36);
    inline juce::Colour backgroundFillAlt = juce::Colour(34, 35, 36);
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
        
    juce::Colour addColors(const juce::Colour& c1, const juce::Colour& c2, const juce::Colour& c3)
    {
        float a1 = c1.getFloatAlpha();
        float a2 = c2.getFloatAlpha();
        float a3 = c3.getFloatAlpha();

        float outAlpha = 1.0f - (1.0f - a1) * (1.0f - a2) * (1.0f - a3);

        // Blend RGB with premultiplied alpha
        float r = (c1.getFloatRed()   * a1 +
                   c2.getFloatRed()   * a2 * (1.0f - a1) +
                   c3.getFloatRed()   * a3 * (1.0f - a1) * (1.0f - a2)) / (outAlpha == 0.0f ? 1.0f : outAlpha);

        float g = (c1.getFloatGreen() * a1 +
                   c2.getFloatGreen() * a2 * (1.0f - a1) +
                   c3.getFloatGreen() * a3 * (1.0f - a1) * (1.0f - a2)) / (outAlpha == 0.0f ? 1.0f : outAlpha);

        float b = (c1.getFloatBlue()  * a1 +
                   c2.getFloatBlue()  * a2 * (1.0f - a1) +
                   c3.getFloatBlue()  * a3 * (1.0f - a1) * (1.0f - a2)) / (outAlpha == 0.0f ? 1.0f : outAlpha);

        return juce::Colour::fromFloatRGBA(r, g, b, outAlpha);
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
        colorAB = addColors(colorA, colorB).withAlpha((float)0.51f);
        colorBC = addColors(colorB, colorC).withAlpha((float)0.51f);
        colorAC = addColors(colorA, colorC).withAlpha((float)0.51f);
        colorABC = addColors(colorA, colorB, colorC).withAlpha((float)0.657f);
    }
    
    juce::Colour colorA;
    juce::Colour colorB;
    juce::Colour colorC;
    juce::Colour colorAB;
    juce::Colour colorBC;
    juce::Colour colorAC;
    juce::Colour colorABC;
};

