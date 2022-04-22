// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Tools/DrawingTool/Shapes/Freehand/Smoothing/IOdysseySmoothing.h"

class ODYSSEYPAINTEREDITOR_API FOdysseySmoothingPull : public IOdysseySmoothing
{
public:
    // Construction / Destruction
    virtual ~FOdysseySmoothingPull();
    FOdysseySmoothingPull(FOdysseySmoothingOptions* iSmoothingOptions);

public:
    // Public API
    virtual bool IsReady() const override;
    virtual FOdysseyPoint ComputePoint() override;
};
