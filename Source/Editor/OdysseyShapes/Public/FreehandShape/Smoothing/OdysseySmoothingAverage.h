// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "FreehandShape/Smoothing/IOdysseySmoothing.h"

class ODYSSEYSHAPES_API FOdysseySmoothingAverage : public IOdysseySmoothing
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
