// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "IOdysseyViewportElement.h"

/////////////////////////////////////////////////////
// IOdysseyViewportElement
class ODYSSEYWIDGETS_API IOdysseyHUDViewportElement : public IOdysseyViewportElement
{
public:
    IOdysseyHUDViewportElement();

public:
    virtual void Draw( FViewport* iViewport, FCanvas* ioCanvas ) = 0;
};
