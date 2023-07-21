// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#include "OdysseyPainterEditorVectorTransformTool.generated.h"

class FOdysseyPainterEditorVectorTransformToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorTransformTool : public UOdysseyPainterEditorVectorPickTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorTransformTool();

    //Constructor
    UOdysseyPainterEditorVectorTransformTool();

    virtual bool IsActivable() const override;
    virtual void Load() override;
    virtual void Unload() override;

    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual bool OnKeyDown( const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;

    virtual void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
    virtual void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
    virtual bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey ) override;
    virtual void OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                   , FOdysseyVectorScene* iScene
                                   , const FOdysseyPoint& iPointInTexture );
    virtual void OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture ) override;

    virtual TArray<TSharedPtr<SWidget>> CreateTopTabWidgets() override;


    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
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
    std::vector<FOdysseyVectorPoint*> mSelectedPoints;

    ::ULIS::FVec2D* mPickedPivot;
    bool mDragging;
    FVector2D mScreenMouseAtDown;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey Transform Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double PickingRadius;

    UPROPERTY(EditAnywhere, Category="Odyssey ObjectScale Tool")
    bool Uniform;
    bool UniformAtKeyDown;
};
