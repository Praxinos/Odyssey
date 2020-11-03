// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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

private:
    static TSharedRef< FOdysseyStyleDefault > Create();

private:
    static TSharedPtr< FOdysseyStyleDefault > smStyleDefaultInstance;
};
