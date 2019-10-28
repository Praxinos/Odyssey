// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Smoothing/IOdysseySmoothing.h"

class ODYSSEYSTROKEPIPELINE_API FOdysseySmoothingAverage : public IOdysseySmoothing
{

public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingAverage();
    FOdysseySmoothingAverage();

public:
    // Public API
    virtual bool IsReady() const override;
    virtual int MinimumRequiredPoints() const override;
    virtual FOdysseyStrokePoint ComputePoint() override;
};

