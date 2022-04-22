// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "StrokeEngine/Smoothing/IOdysseySmoothing.h"

class ODYSSEYPAINTEREDITOR_API FOdysseySmoothingPull : public IOdysseySmoothing
{
public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingPull();
    FOdysseySmoothingPull(FOdysseyStrokeOptions* iStrokeOptions);

public:
    // Public API
    virtual bool IsReady() const override;
    virtual FOdysseyPoint ComputePoint() override;
};
