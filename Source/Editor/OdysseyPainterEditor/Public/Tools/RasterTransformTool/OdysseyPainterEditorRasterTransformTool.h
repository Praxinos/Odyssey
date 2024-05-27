// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorRasterTransformTool.generated.h"

class FOdysseyHUDPolygon;
class FOdysseyHUDHandle;
class UOdysseyPainterEditorRasterSelectionTool;

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
    virtual EMouseCursor::Type GetMouseCursor() const override;

    virtual FText GetTooltip() const override;

private:
    int GetRotationAngleFromLastReference(FVector2D iPointInTexture);

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

    //Copy from GeomTools.cpp
    bool IsPolygonConvex(const TArray<FVector2D>& Points);

private:
    void OnTopLeftHandleDragged();
    void OnTopRightHandleDragged();
    void OnBottomRightHandleDragged();
    void OnBottomLeftHandleDragged();
    void OnPivotHandleDragged();

public:
    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    EOdysseySelectionShape SelectionShape;

    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    bool Perspective = false;

    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    bool Uniform = false;

private: 
    FOdysseyPaintEngine mPaintEngine;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mTransformedBlock;
    FOdysseyRasterBlockMutator mRasterMutator;

    EOdysseyTransformCapture mTransformCaptureMode;
    TSharedPtr<FOdysseyHUDPolygon> mTransformArea;
    TArray<TSharedPtr<FOdysseyHUDHandle>> mHandles;
    FVector2D mPivot;

    int mLastReferenceRotation;
    FVector2D mMouseLastReferencePoint;

    int mRotation;

    EMouseCursor::Type mMouseCursor;
};
