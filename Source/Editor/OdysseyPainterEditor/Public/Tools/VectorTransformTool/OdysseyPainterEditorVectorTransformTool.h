// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/DefaultTool/OdysseyPainterEditorDefaultTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#include "OdysseyPainterEditorVectorTransformTool.generated.h"

class FOdysseyPainterEditorVectorTransformToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorTransformTool : public UOdysseyPainterEditorDefaultTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorTransformTool();

    //Constructor
    UOdysseyPainterEditorVectorTransformTool();
    void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    bool OnKeyDownVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FKey& iKey );
    bool OnKeyUpVector( FOdysseyVectorEngine* iEngine
                      , FOdysseyVectorScene* iScene
                      , const FKey& iKey );
    bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture
                          , const FKey& iKey );
    ::ULIS::FRectI OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                     , FOdysseyVectorScene* iScene
                                     , const FOdysseyPoint& iPointInTexture );
    void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                          , FOdysseyVectorScene* iScene
                          , const FOdysseyPoint& iPointInTexture );
    bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                        , FOdysseyVectorScene* iScene
                        , const FOdysseyPoint& iPointInTexture
                        , const FKey& iKey );
    void PropertyChangedVector( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FName& iPropertyName );

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

private:
    void TranslateObjectSelection( FOdysseyVectorEngine* iEngine
                                 , FOdysseyVectorScene* iScene
                                 , const FOdysseyPoint& iPointInTexture );
    void RotateObjectSelection( FOdysseyVectorEngine* iEngine
                              , FOdysseyVectorScene* iScene
                              , const FOdysseyPoint& iPointInTexture );
    double GetRotationAngle( const FOdysseyPoint& iPointInTexture );
    void ScaleObjectSelection( FOdysseyVectorEngine* iEngine
                             , FOdysseyVectorScene* iScene
                             , const FOdysseyPoint& iPointInTexture );

private:
    FOdysseyPainterEditorVectorTransformToolHUD* mTransformHUD;
    std::vector<FObjectTransform> mObjectTransformArray;
    ::ULIS::FVec2D* mPickedPivot;
    bool mDragging;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey Transform Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double PickingRadius;

    UPROPERTY( EditAnywhere, Category="Odyssey ObjectMove Tool" )
    bool World;

    UPROPERTY(EditAnywhere, Category="Odyssey ObjectScale Tool")
    bool Uniform;
    bool UniformAtKeyDown;
};
