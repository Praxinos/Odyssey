// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include "OdysseyVector.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#include "OdysseyPainterEditorVectorObjectRotateTool.generated.h"

class FOdysseyPainterEditorVectorObjectRotateToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectRotateTool : public UOdysseyPainterEditorVectorObjectPickTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectRotateTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectRotateTool();

    virtual void UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
    virtual void LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
    virtual bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey ) override;
    virtual ::ULIS::FRectI OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                            , FOdysseyVectorScene* iScene
                                            , const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                , FOdysseyVectorScene* iScene
                                , const FOdysseyPoint& iPointInTexture
                                , const FKey& iKey ) override;

    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;
    // usually for undos. (we need to reset the HUD as if the tool was activated)
    void OnRefresh( FOdysseyVectorScene* iScene );

    bool IsDragging();
    bool IsRotating();
    double GetCumulAngle();

protected:
    void FitHUD( FOdysseyVectorScene* iScene );

private:
    FOdysseyPainterEditorVectorObjectRotateToolHUD* mObjectRotateHUD;
    std::vector<FObjectTransform> mObjectTransformArray;
    bool mDragging;
    bool mRotating;
    ::ULIS::FVec2D* mPickedPivot;
    ::ULIS::FVec2D mStartAtVector;
    double mCumulAngle;
};
