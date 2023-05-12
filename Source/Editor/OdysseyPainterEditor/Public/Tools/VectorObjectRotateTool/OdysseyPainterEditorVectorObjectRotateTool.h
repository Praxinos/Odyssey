// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#include "OdysseyPainterEditorVectorObjectRotateTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectRotateTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectRotateTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectRotateTool();
 
    void ActivateVector( FOdysseyVectorEngine* iEngine
                       , FOdysseyVectorScene* iScene );
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;
    // usually for undos. (we need to reset the HUD as if the tool was activated)
    void OnRefresh( FOdysseyVectorScene* iScene );

protected:
    void FitHUD( FOdysseyVectorScene* iScene );

private:
    FOdysseyVectorHUDRotate *mTransformHUD;
    ::ULIS::FVec2D* mPickedPivot;

};
