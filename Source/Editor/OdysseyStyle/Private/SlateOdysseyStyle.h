// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyStyleSet.h"

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
