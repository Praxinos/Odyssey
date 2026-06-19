// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyStyle.h"

class FOdysseyStyleDefault;

/**
 * Declares the Editor's visual style.
 */
class FSlateOdysseyStyle
    : public FOdysseyStyle
{
public:
    static void Initialize();
    static void Shutdown();

    static const FSlateStyleSet& Get();
private:
    static TSharedRef< FOdysseyStyleDefault > Create();

private:
    static TSharedPtr< FOdysseyStyleDefault > smStyleDefaultInstance;
};
