// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyShape.h"
#include "Tools/RasterBaseTool/OdysseyPainterEditorRasterBaseTool.h"
#include "OdysseyPainterEditorRasterSelectionTool.generated.h"

class UOdysseyShape;

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

    virtual FText GetTooltip() const override;

public:
    UFUNCTION(BlueprintCallable, Category="Actions", CallInEditor)
    void Deselect();

protected:
    TOptional<FMouseCursor> GetMouseCursorOverride() const override;

private:
    // Internal - Callbacks
    void OnShapeBegin();
    void OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset);
    void OnShapeAbort();

protected:
    /** The list of available shapes. */
    UPROPERTY(EditAnywhere, Category="Shape", meta = (IgnoreToolConfiguration))
    FOdysseyShapes Shapes;

    EOdysseySelectionState mSelectionState;
    TSharedPtr<FOdysseyHUDElement> mShapeHUD;
};
