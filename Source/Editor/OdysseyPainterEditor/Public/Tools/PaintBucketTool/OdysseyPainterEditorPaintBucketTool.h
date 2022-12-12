// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"

#include "OdysseyPainterEditorPaintBucketTool.generated.h"

class FOdysseyPaintEngine;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorPaintBucketTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorPaintBucketTool();

    //Constructor
    UOdysseyPainterEditorPaintBucketTool();
    
    void Initialize(FOdysseyPaintEngine* iPaintEngine);

    //OdysseyPainterEditorTool overrides
    virtual void Activate() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void Commit() override;

public:
    // Setters
    virtual bool CanDraw();

protected:
    // protected Data Members

    //Resources
    FOdysseyPaintEngine                mPaintEngine;
};
