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
    Sides,
    Rotation
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
    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Tick(float iDeltaTime) override;

    virtual void Load() override;
    virtual void Unload() override;

    virtual void PostEditChangeProperty(FPropertyChangedEvent& iPropertyChangedEvent) override;
    virtual EMouseCursor::Type GetMouseCursor() override;

private:
    double GetRotationAngleFromLastReference(FVector2D iPointInTexture);

    void CreateTransformAreaFromSelection();
    EOdysseyTransformCapture DetectCaptureMode( FVector2D iPoint );
    void ConstrainToRectangle( FVector2D iPosition );
    void ConstrainToParallelogram(FVector2D iPosition);

    void CreateTransformBlockFromSelectionBlock();
    ::ULIS::FRectI GetTransformAreaBoundingRect();

    TArray<::ULIS::FRectI> GetTransformAreaAsScanlines(); //Returns rectangles with height of 1 that cover the entire transform area. Useful for freehand selection
    
    void BlendTransformAreaToPaintBlock();

    void CommitTransform();
    void ClearTransform();
    void ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

public:
    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    EOdysseySelectionShape SelectionShape;

    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    bool Uniform = false;

private: 
    UOdysseyPainterEditorRasterSelection* mSelection;

    FOdysseyPaintEngine mPaintEngine;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mTransformedBlock;
    FOdysseyRasterBlockMutator mRasterMutator;

    EOdysseyTransformConstrain mTransformAreaConstrain;
    EOdysseyTransformCapture mTransformCaptureMode;
    FOdysseyHUDPolygon* mTransformArea;
    TArray<FOdysseyHUDHandle*> mHandles;
    FVector2D mPivot;

    double mLastReferenceRotation;
    FVector2D mMouseLastReferencePoint;
    ::ULIS::FRectI mInitialSelectionBoundingBox;

    double mRotation;
};
