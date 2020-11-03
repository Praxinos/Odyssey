// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Interpolation/IOdysseyInterpolation.h"

class ODYSSEYSTROKEPIPELINE_API FOdysseyInterpolationBezier : public IOdysseyInterpolation
{
public:
    // Construction / Destruction
    virtual ~FOdysseyInterpolationBezier();
    FOdysseyInterpolationBezier();

public:
    // Public API
    virtual bool IsReady() const override;
    virtual int MinimumRequiredPoints() const override;
    virtual const TArray< FOdysseyStrokePoint >& ComputePoints() override;
};
