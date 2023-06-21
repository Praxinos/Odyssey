// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorObjectScaleTool.generated.h"

class FOdysseyPainterEditorVectorObjectScaleToolHUD;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorObjectScaleTool : public UOdysseyPainterEditorVectorObjectPickTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorVectorObjectScaleTool();

    //Constructor
    UOdysseyPainterEditorVectorObjectScaleTool();

    virtual void UnloadVector ( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
    virtual void LoadVector ( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene ) override;
    virtual bool OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                  , FOdysseyVectorScene* iScene
                                  , const FOdysseyPoint& iPointInTexture
                                  , const FKey& iKey ) override;
    ::ULIS::FRectI OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                     , FOdysseyVectorScene* iScene
                                     , const FOdysseyPoint& iPointInTexture );
    virtual ::ULIS::FRectI OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                            , FOdysseyVectorScene* iScene
                                            , const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                , FOdysseyVectorScene* iScene
                                , const FOdysseyPoint& iPointInTexture
                                , const FKey& iKey ) override;
    //OdysseyPainterEditorTool overrides
    virtual void Commit() override;

protected:
    void FitHUD( FOdysseyVectorScene* iScene );

private:
    FOdysseyPainterEditorVectorObjectScaleToolHUD* mObjectScaleHUD;
    std::vector<FObjectTransform> mObjectTransformArray;
    bool mDragging;
    double mOldLocalMouseX;
    double mOldLocalMouseY;
    uint32 mHandleFlags;

public:
    UPROPERTY(EditAnywhere, Category="Odyssey ObjectScale Tool")
    bool Uniform;

    //UPROPERTY( EditAnywhere, Category="Odyssey ObjectMove Tool", meta = (ClampMin = "0.0", UIMin = "0.0") )
    double PickingRadius;
};
