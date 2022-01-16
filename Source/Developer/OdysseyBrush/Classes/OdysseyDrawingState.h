// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

/////////////////////////////////////////////////////
// FOdysseyBrushState
class ODYSSEYBRUSH_API FOdysseyDrawingState //TODO: rename to I* (?)
{
public:
    virtual ~FOdysseyDrawingState() {};
    FOdysseyDrawingState();

public:
    virtual const FName& Id() = 0;
};

