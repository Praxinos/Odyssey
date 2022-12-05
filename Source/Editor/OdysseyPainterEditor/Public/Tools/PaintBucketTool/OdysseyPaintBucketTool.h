// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTool.h"

#include "OdysseyPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPaintBucketTool : public UOdysseyTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPaintBucketTool();

    //Constructor
    UOdysseyPaintBucketTool();
    
    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    //OdysseyTool overrides
    virtual void Activate() override;
    virtual void Inactivate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

    virtual void BindShortcuts(class FBaseToolkit* iToolkit) override;

public:
    // Setters
    bool CanDraw();

    // Sets the PaintEngine used to draw
    void SetPaintEngine(FOdysseyPaintEngine* iPaintEngine);

    // Set wether the tool can draw or not
    void IsDrawingLocked(bool iValue);

    // Get wether the tool can draw or not
    bool IsDrawingLocked();

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine*                mPaintEngine;

    //---

    //Internal
    bool                                mIsDrawingLocked;
};
