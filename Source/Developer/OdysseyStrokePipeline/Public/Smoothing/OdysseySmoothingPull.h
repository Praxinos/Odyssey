// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Smoothing/IOdysseySmoothing.h"

class ODYSSEYSTROKEPIPELINE_API FOdysseySmoothingPull : public IOdysseySmoothing
{
public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingPull();
    FOdysseySmoothingPull();

public:
    // Public API
    virtual bool IsReady() const override;
    virtual int MinimumRequiredPoints() const override;
    virtual FOdysseyStrokePoint ComputePoint() override;
};
