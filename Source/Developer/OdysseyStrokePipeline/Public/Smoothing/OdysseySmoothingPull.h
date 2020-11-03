// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Smoothing/IOdysseySmoothing.h"

class ODYSSEYSTROKEPIPELINE_API FOdysseySmoothingPull : public IOdysseySmoothing
{
public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingPull();
    FOdysseySmoothingPull(FOdysseySmoothingParameters* iParameters);

public:
    // Public API
    virtual bool IsReady() const override;
    virtual FOdysseyStrokePoint ComputePoint() override;
};
