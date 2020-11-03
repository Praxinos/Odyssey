// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Smoothing/IOdysseySmoothing.h"

class ODYSSEYSTROKEPIPELINE_API FOdysseySmoothingAverage : public IOdysseySmoothing
{
public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingAverage();
    FOdysseySmoothingAverage(FOdysseySmoothingParameters* iParameters);

public:
    // Public API
    virtual bool IsReady() const override;
    virtual FOdysseyStrokePoint ComputePoint() override;
    virtual void AddPoint( const FOdysseyStrokePoint& iPoint ) override;
    virtual bool CanCatchUp() const override;
};
