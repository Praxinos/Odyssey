// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
