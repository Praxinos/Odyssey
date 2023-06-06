// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorObjectScaleTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectScaleTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectScaleTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectScaleTool();

    void UnloadVector ( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector ( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
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

protected:
    void FitHUD( FOdysseyVectorScene* iScene );

private:
    FOdysseyVectorHUDScale *mTransformHUD;

    double mOldLocalMouseX;
    double mOldLocalMouseY;
    int32 mPickedHandle;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey ObjectScale Tool")
    bool Uniform;

};
