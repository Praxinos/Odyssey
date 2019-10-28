// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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

