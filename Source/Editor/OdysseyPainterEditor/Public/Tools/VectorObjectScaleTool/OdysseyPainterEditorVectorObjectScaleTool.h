// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorObjectScaleTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectScaleTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectScaleTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectScaleTool();
 
    void Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnMouseDown( FOdysseyVectorEngine* iEngine
                    , FOdysseyVectorScene* iScene
                    , FOdysseyVectorUndo** iUndo
                    , const FOdysseyPoint& iPointInTexture
                    , const FKey& iKey );
    void OnMouseDrag( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture );
    bool OnMouseUp( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;
    // usually for undos. (we need to reset the HUD as if the tool was activated)
    void OnRefresh( FOdysseyVectorScene* iScene );

protected:
    void FitHUD( FOdysseyVectorScene* iScene );

private:
    // the "undo transform" object is used in both the MouseDown and MouseUp events. We need to remember it.
    FOdysseyVectorUndoObjectTransform* mUndoObjectTransform;
    FOdysseyVectorHUDScale *mTransformHUD;

    double mOldLocalMouseX;
    double mOldLocalMouseY;
    int32 mPickedHandle;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey ObjectScale Tool")
    bool Uniform;

};
