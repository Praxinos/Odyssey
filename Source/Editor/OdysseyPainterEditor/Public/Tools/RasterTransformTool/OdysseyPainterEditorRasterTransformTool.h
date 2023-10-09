// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorRasterTransformTool.generated.h"

class FOdysseyHUDPolygon;
class FOdysseyHUDHandle;

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
    void CreateTransformBlockFromReferenceBlock();
    ::ULIS::FRectI GetTransformAreaBoundingRect();
    TArray<::ULIS::FRectI> GetTransformAreaAsRectangles();
    void BlendTransformAreaToPaintBlock();

    void ClearBlock( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock );

    void CommitTransform();
    void AbortTransform();

    //Resets the tool and its HUD
    void ClearTransform();

public:
    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    EOdysseySelectionShape SelectionShape;

private: 

    FOdysseyPaintEngine mPaintEngine;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mReferenceBlock;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mTransformedBlock;

    FOdysseyHUDPolygon* mTransformArea;
    bool mTransformAreaSet;
    TArray<FOdysseyHUDHandle*> mHandles;

    FOdysseyRasterBlockMutator mRasterMutator;

    EOdysseyTransformConstrain mAreaConstrain;
    EOdysseyTransformCapture mTransformCaptureMode;

    FVector2D mMouseLastReferencePoint;

};
