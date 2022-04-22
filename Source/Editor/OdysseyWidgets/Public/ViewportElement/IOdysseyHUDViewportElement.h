// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPaintEngineHUD.h"
#include "IOdysseyViewportElement.h"

/////////////////////////////////////////////////////
// IOdysseyViewportElement
class ODYSSEYWIDGETS_API IOdysseyHUDViewportElement : public IOdysseyViewportElement
{
public:
    virtual void Draw() = 0;

//Getters/setters
public:
    void SetPaintEngineHUD( FOdysseyPaintEngineHUD* iPaintEngineHUD);
    FOdysseyPaintEngineHUD* GetPaintEngineHUD();

protected:
    FOdysseyPaintEngineHUD* mPaintEngineHUD;
};
