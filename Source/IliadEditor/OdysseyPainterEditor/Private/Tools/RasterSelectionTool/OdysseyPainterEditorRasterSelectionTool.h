// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyPainterEditorRasterSelectionTool.generated.h"

class UOdysseyShape;
class UOdysseyMagicWandShape;

UENUM()
enum class EOdysseySelectionState : uint8
{
    Normal,
    Add,
    Substract
};

//This is already a tool to prepare for the moment we'll separate transform and selection. When we'll have a "mask" feature in Odyssey
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterSelectionTool : public UOdysseyPainterEditorRasterBaseTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterSelectionTool();

    //Constructor
    UOdysseyPainterEditorRasterSelectionTool();


public:
    //TOOL
    template<class T> T* CreateShape(FName iName);

public:
    virtual bool IsActivable() const override;

    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyDown(const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey) override;

    virtual void Load() override;
    virtual void Unload() override;

    virtual void Tick(float iDeltaTime) override;

    void SetFeatherRadius(int iRadius);
    virtual FText GetTooltip() const override;

public:
    UFUNCTION(BlueprintCallable, Category="Actions", CallInEditor)
    void Deselect();

protected:
    TOptional<FMouseCursor> GetMouseCursorOverride() const override;
    void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

private:
    // Internal - Callbacks
    void OnShapeBegin();
    void OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset);
    void OnContoursCommit(const TArray<TArray<FOdysseyPoint>>& iContours);
    void OnSelectionBlockCommit(TSharedPtr<::ULIS::FBlock> iMaskBlock);
    void OnShapeAbort();

protected:
    /** The list of available shapes. */
    UPROPERTY(EditAnywhere, Category="Shape", meta = (IgnoreToolConfiguration))
    FOdysseyShapes Shapes;

    /** The feather radius. */
    UPROPERTY(EditAnywhere, Category = "Feather", meta = (ClampMin = "0", ClampMax = "1000", UIMin = "0", UIMax = "1000", Delta = "1"))
    float FeatherRadius = 0;

    EOdysseySelectionState mSelectionState;
    TSharedPtr<FOdysseyHUDElement> mShapeHUD;

    UOdysseyMagicWandShape* mMagicWandShape;
};
