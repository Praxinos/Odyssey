// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "FreehandShape/Interpolation/IOdysseyInterpolation.h"

class ODYSSEYSHAPES_API FOdysseyInterpolationCatmullRom : public IOdysseyInterpolation
{
public:
    // Construction / Destruction
    virtual ~FOdysseyInterpolationCatmullRom();
    FOdysseyInterpolationCatmullRom();

public:
    // Public API
    virtual bool IsReady() const override;
    virtual int MinimumRequiredPoints() const override;
    virtual const TArray< FOdysseyPoint >& ComputePoints() override;
};