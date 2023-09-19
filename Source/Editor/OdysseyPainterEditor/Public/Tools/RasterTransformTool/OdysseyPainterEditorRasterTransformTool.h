// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorRasterTransformTool.generated.h"

class UOdysseyHUDPolygon;
class UOdysseyHUDHandle;

UENUM()
enum class EOdysseyTransformConstrain
{
    NoConstrain,
    Rectangle,
    Parallelogram
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

private:
    void ConstrainToRectangle( FVector2D iPosition );
    void ConstrainToParallelogram(FVector2D iPosition);

private: 
    FOdysseyPaintEngine                 mPaintEngine;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mReferenceBlock;

    UOdysseyHUDPolygon* mTransformArea;
    TArray<UOdysseyHUDHandle*> mHandles;

    EOdysseyTransformConstrain mAreaConstrain;

    FVector2D mMouseReferencePoint;
};
