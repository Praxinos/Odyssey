// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

/////////////////////////////////////////////////////
// IOdysseyViewportElement
class ODYSSEYWIDGETS_API IOdysseyViewportElement
{
public:
    virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) = 0;
};
