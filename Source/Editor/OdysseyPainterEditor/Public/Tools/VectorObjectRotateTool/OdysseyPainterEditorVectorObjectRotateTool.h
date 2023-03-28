// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorObjectRotateTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectRotateTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectRotateTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectRotateTool();
 
    void Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnMouseDown( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    void OnMouseDrag( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture);
    bool OnMouseUp( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

private:
    FOdysseyVectorHUDRotate *mTransformHUD;
    ::ULIS::FVec2D* mPickedPivot;

protected:

};
