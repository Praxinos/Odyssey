// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorRasterTransformTool.generated.h"

class FOdysseyHUDPolygon;
class FOdysseyHUDHandle;
class UOdysseyPainterEditorRasterSelection;

UENUM()
enum class EOdysseySelectionShape : uint8
{
    Rectangle,
    FreeHand,
    Ellipse
};

enum class EOdysseyTransformConstrain
{
    NoConstrain,
    Rectangle,
    RectangleForcedRatio,
    Parallelogram
};

enum class EOdysseyTransformCapture
{
    NoCapture,
    Inside,
    Sides
};

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterTransformTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterTransformTool();

    //Constructor
    UOdysseyPainterEditorRasterTransformTool();

    virtual bool IsActivable() const override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey);

    virtual void Load() override;
    virtual void Unload() override;

private:
    void ConstrainToRectangle( FVector2D iPosition );
    void ConstrainToParallelogram(FVector2D iPosition);

    void CreateTransformBlockFromReferenceBlock(); //Get a temporary block for the transformation, so we don't lose quality when transforming our reference block
    ::ULIS::FRectI GetTransformAreaBoundingRect();

    TArray<::ULIS::FRectI> GetTransformAreaAsScanlines(); //Returns rectangles with height of 1 that cover the entire transform area. Useful for freehand selection
    
    void BlendTransformAreaToPaintBlock();

    void ClearBlock( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock );

    bool IsSelectionValid(::ULIS::FRectI iSelectionArea); //Checks if the selection has a relevant position in the canvas. ULIS needs it to determine if their operations make sense.

    void CommitTransform();
    void AbortTransform();
    void ClearTransform(); //Resets the tool and its HUD

public:
    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    EOdysseySelectionShape SelectionShape;

private: 
    UOdysseyPainterEditorRasterSelection* mSelection;
    FOdysseyPaintEngine mPaintEngine;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mTransformedBlock;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mReferenceBlock;

    FOdysseyHUDPolygon* mTransformArea;
    bool mTransformAreaSet;
    TArray<FOdysseyHUDHandle*> mHandles;

    FOdysseyRasterBlockMutator mRasterMutator;

    EOdysseyTransformConstrain mAreaConstrain;
    EOdysseyTransformCapture mTransformCaptureMode;

    FVector2D mMouseLastReferencePoint;

};
