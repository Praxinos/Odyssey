// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "PaintEngine/OdysseyPaintEngine.h"
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

    virtual void ExtendToolbar( FToolBarBuilder& iBuilder ) override;

    bool FlipHorizontal();
    bool FlipVertical();

    void UndoTransformTransaction();
    void RedoTransformTransaction();

private:
    int GetRotationAngleFromLastReference(FVector2D iPointInTexture);

    EOdysseyTransformCapture DetectCaptureMode( FVector2D iPoint );
    void ConstrainToRectangle( FVector2D iPosition );
    void ConstrainToParallelogram(FVector2D iPosition);

    void UpdateTransformHUD(); // Update the HUD of the transform based on current selection
    void UpdateTransformBlock(); // Update the pixel block of the transform based on current selection
    ::ULIS::FRectI GetTransformAreaBoundingRect();

    void BlendTransformAreaToPaintBlock();

    void CommitTransform();
    void ClearTransform( bool iInactivateTransformTool = false );
    void ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

    //Copy from GeomTools.cpp
    bool IsPolygonConvex(const TArray<FVector2D>& Points);

    void RecordTransformTransaction();

private:
    void OnTopLeftHandleDragged();
    void OnTopRightHandleDragged();
    void OnBottomRightHandleDragged();
    void OnBottomLeftHandleDragged();
    void OnPivotHandleDragged();

    void OnRasterSelectionChanged();

    void UpdateRasterSelection( bool iCreateNewIfEmpty = false );
    void ResetRasterSelection();

public:
    UPROPERTY(EditAnywhere, Category="Transform options")
    bool Perspective = false;

    UPROPERTY(EditAnywhere, Category="Transform options")
    bool Uniform = false;

private:
    FOdysseyPaintEngine mPaintEngine;
    FOdysseyRasterBlockMutator mRasterMutator;

    EOdysseyTransformCapture mTransformCaptureMode;
    TSharedPtr<FOdysseyHUDElement> mTransformToolHUD; // The whole HUD displayed by the tool, selection + transform
    TSharedPtr<FOdysseyHUDPolygon> mTransformAreaHUD; // The "rectangle" HUD with four handles used for the transformation
    TArray<TSharedPtr<FOdysseyHUDHandle>> mHandles;

    int mLastReferenceRotation;
    FVector2D mMouseLastReferencePoint;

    EMouseCursor::Type mMouseCursor;

    TSharedPtr<::ULIS::FBlock> mSelectionBlock; // The base block we transform
    TSharedPtr<::ULIS::FBlock> mTransformedBlock; // The transformed block
    ::ULIS::FRectI mSelectionBoundingBox;

    FInputChord mToolChord; //The current state of the keyboard keys that are pressed while using this tool

    int mIndexTransaction; //Current index in the transactions array
    TArray<TStaticArray<FVector2D, 5>> mTransformTransactions; //Undos/Redos for the current transform, contains 4 handles and 1 pivot for each transaction
};
