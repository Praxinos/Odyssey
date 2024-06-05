// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorRasterSelectionTool.generated.h"

class FOdysseyHUDPolygon;

UENUM()
enum class EOdysseySelectionShape : uint8
{
    Rectangle,
    Freehand,
    Ellipse
};

UENUM()
enum class EOdysseySelectionState : uint8
{
    Normal,
    Add,
    Substract
};

//This is already a tool to prepare for the moment we'll separate transform and selection. When we'll have a "mask" feature in Odyssey
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterSelectionTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterSelectionTool();

    //Constructor
    UOdysseyPainterEditorRasterSelectionTool();
    
    virtual bool IsActivable() const override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Load() override;
    virtual void Unload() override;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetSelectionBlock();
    ::ULIS::FRectI GetSelectionAreaBoundingRect();

    void ClearSelection();

protected:
    bool IsSelectionValid(::ULIS::FRectI iSelectionArea);
    void ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

private:
    void ConstrainSelectionToEllipse( const FOdysseyPoint& iPointInTexture );
    void ConstrainSelectionToRectangle(FVector2D iPosition);


protected:
    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    EOdysseySelectionShape SelectionShape;

    EOdysseySelectionState SelectionState;

    UPROPERTY(EditAnywhere, Category = "Selection Shape")
    bool Uniform;

    TSharedPtr<FOdysseyHUDElement> mSelectionHUD;
    TSharedPtr<FOdysseyHUDPolygon> mToolSelectionArea;
    FOdysseyPaintEngine mPaintEngine;

    FVector2D mDownReference;
};
