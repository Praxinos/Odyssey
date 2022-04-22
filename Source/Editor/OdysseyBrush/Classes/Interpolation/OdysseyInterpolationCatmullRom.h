// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Interpolation/IOdysseyInterpolation.h"

class ODYSSEYBRUSH_API FOdysseyInterpolationCatmullRom : public IOdysseyInterpolation
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
