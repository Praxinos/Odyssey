// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyPaintEngine.h"
#include "RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
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
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterTransformTool : public UOdysseyPainterEditorRasterBaseTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterTransformTool();

    //Constructor
    UOdysseyPainterEditorRasterTransformTool();

    virtual bool IsActivable() const override;
    virtual TSharedRef<SWidget> CreateTopTabWidget() override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Tick(float iDeltaTime) override;

    virtual void Load() override;
    virtual void Unload() override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

    virtual FText GetTooltip() const override;

    bool FlipHorizontal();
    bool FlipVertical();

private:
    int GetRotationAngleFromLastReference(FVector2D iPointInTexture);

    void CreateTransformAreaFromSelection();
    EOdysseyTransformCapture DetectCaptureMode( FVector2D iPoint );
    void ConstrainToRectangle( FVector2D iPosition );
    void ConstrainToParallelogram(FVector2D iPosition);

    void CreateTransformBlockFromSelectionBlock();
    ::ULIS::FRectI GetTransformAreaBoundingRect();
    
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
    
    void OnRasterSelectionChanged();

    void UpdateRasterSelection();
    void ResetRasterSelection();

public:
    UPROPERTY(EditAnywhere, Category = "Transform options")
    bool Perspective = false;

    UPROPERTY(EditAnywhere, Category = "Transform options")
    bool Uniform = false;

private: 
    FOdysseyPaintEngine mPaintEngine;
    FOdysseyRasterBlockMutator mRasterMutator;

    EOdysseyTransformCapture mTransformCaptureMode;
    TSharedPtr<FOdysseyHUDElement> mTransformHUD;
    TSharedPtr<FOdysseyHUDPolygon> mTransformArea;
    TArray<TSharedPtr<FOdysseyHUDHandle>> mHandles;

    int mLastReferenceRotation;
    FVector2D mMouseLastReferencePoint;

    EMouseCursor::Type mMouseCursor;

    TSharedPtr<::ULIS::FBlock> mSelectionBlock;
    TSharedPtr<::ULIS::FBlock> mTransformSelectionBlock;
    ::ULIS::FRectI mSelectionBoundingBox;
};
