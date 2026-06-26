// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "IDetailCustomization.h"
#include "Widgets/SWidget.h"
#include "OdysseyPainterEditorAnimationOutOfPegsTool.generated.h"

class FOdysseyPaintEngine;
class UOdysseyLayer;
class FOdysseyHUDPolygon;
class FOdysseyHUDHandle;
class FOdysseyHUDLine;
class UOdysseyLayerCell;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorAnimationOutOfPegsTool :
    public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorAnimationOutOfPegsTool();

    //Constructor
    UOdysseyPainterEditorAnimationOutOfPegsTool();

public:
    virtual void Load();

    //OdysseyPainterEditorTool overrides
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;

public:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& iEvent) override;

public:
    UOdysseyLayerCell* GetCell() const;
    void SetCell(UOdysseyLayerCell* iCell);

private:
    void OnCellOutOfPegsChanged(bool iIsInteractive);
    void OnLighttableChanged();
    void RebuildHUD();
    void RefreshHUD();
    FVector2D GetCenter() const;

    void OnTopLeftHandleDragBegin();
    void OnTopRightHandleDragBegin();
    void OnBottomRightHandleDragBegin();
    void OnBottomLeftHandleDragBegin();
    void OnRotationHandleDragBegin();

    void OnTopLeftHandleDragged();
    void OnTopRightHandleDragged();
    void OnBottomRightHandleDragged();
    void OnBottomLeftHandleDragged();
    void OnRotationHandleDragged();

    void OnTopLeftHandleDragEnd();
    void OnTopRightHandleDragEnd();
    void OnBottomRightHandleDragEnd();
    void OnBottomLeftHandleDragEnd();
    void OnRotationHandleDragEnd();

public:
    UFUNCTION(BlueprintCallable, Category="Actions", CallInEditor)
    void Reset();

    UFUNCTION(BlueprintCallable, Category="Actions", CallInEditor)
    void ResetAll();

public:
    /** The pan. */
    UPROPERTY(
        EditAnywhere,
        Category="Out Of Pegs",
        meta = (
            Tooltip = "Translation in pixels",
            LinearDeltaSensitivity = "5",
            Delta = "1"
        ))
    FVector2D Pan = FVector2D(0, 0);

    /** The rotation. */
    UPROPERTY(
        EditAnywhere,
        Category="Out Of Pegs",
        meta = (
            Tooltip = "Rotation in degrees",
            LinearDeltaSensitivity="15",
            Delta = "1",
            Units = "Degrees"
        ) )
    float Rotation = 0.f;

    /** The zoom. */
    UPROPERTY(
        EditAnywhere,
        Category="Out Of Pegs",
        meta = (
            Tooltip = "Scale in percent",
            ClampMin = "0.1",
            UIMin = "0.1",
            LinearDeltaSensitivity = "15",
            Delta = "1",
            Units="Percent"
        ) )
    float Zoom = 100.f;

private:
    UOdysseyLayer* mLayer;
    UOdysseyLayerCell* mCell;
    TSharedPtr<FOdysseyHUDPolygon> mTransformHUD = nullptr;
    TSharedPtr<FOdysseyHUDHandle> mTransformTopLeftHandleHUD = nullptr;
    TSharedPtr<FOdysseyHUDHandle> mTransformTopRightHandleHUD = nullptr;
    TSharedPtr<FOdysseyHUDHandle> mTransformBottomRightHandleHUD = nullptr;
    TSharedPtr<FOdysseyHUDHandle> mTransformBottomLeftHandleHUD = nullptr;
    TSharedPtr<FOdysseyHUDHandle> mTransformRotationHandleHUD = nullptr;
    TSharedPtr<FOdysseyHUDLine> mTransformRotationLineHUD = nullptr;

    FVector2D mRotationCenter;

    bool mIsPanning = false;
    FVector2D mPanPointReference;
    FVector2D mPanReference;

    /* class FOdysseyHUDPolygon* mLeftPegHUD = nullptr;
    class FOdysseyHUDPolygon* mRightPegHUD = nullptr;
    class FOdysseyHUDCircle* mCenterPegHUD = nullptr; */

    FVector2D mZoomCenter;
    float mZoomDistanceReference;
    float mZoomReference;
};

class FOdysseyPainterEditorAnimationOutOfPegsToolDetails : public IDetailCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance();

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface
};
