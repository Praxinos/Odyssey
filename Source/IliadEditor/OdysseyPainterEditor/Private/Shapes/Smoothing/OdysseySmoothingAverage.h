// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IOdysseySmoothing.h"

class FOdysseySmoothingAverage : public IOdysseySmoothing
{
public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingAverage();
    FOdysseySmoothingAverage(FOdysseySmoothingOptions* iSmoothingOptions);

public:
    // Public API
    virtual bool IsReady() const override;
    virtual FOdysseyPoint ComputePoint() override;
    virtual void AddPoint( const FOdysseyPoint& iPoint ) override;
    virtual bool CanCatchUp() const override;
};
