// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "IDetailCustomization.h"
#include "Widgets/SWidget.h"
#include "OdysseyAnimationEditorOutOfPegsTool.generated.h"

class FOdysseyPaintEngine;
class UOdysseyAnimationLayer;
class FOdysseyHUDPolygon;
class FOdysseyHUDHandle;
class FOdysseyHUDLine;
class UOdysseyAnimationCell;

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyAnimationEditorOutOfPegsTool :
    public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyAnimationEditorOutOfPegsTool();

    //Constructor
    UOdysseyAnimationEditorOutOfPegsTool();

public:
    virtual void Load();

    //OdysseyPainterEditorTool overrides
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;

    virtual EMouseCursor::Type GetMouseCursor() const override;

public:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& iEvent) override;

public:
    UOdysseyAnimationCell* GetCell() const;
    void SetCell(UOdysseyAnimationCell* iCell);

private:
    void OnCellOutOfPegsChanged(bool iIsInteractive);
    void OnLightTableChanged();
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
    UPROPERTY(EditAnywhere, Category="Out Of Pegs", meta = (LinearDeltaSensitivity="1") )
    FVector2D Pan = FVector2D(0, 0);

    UPROPERTY( EditAnywhere, Category="Out Of Pegs", meta = ( Units="Degrees", LinearDeltaSensitivity="1" ) )
    float Rotation = 0.f;

    UPROPERTY( EditAnywhere, Category="Out Of Pegs", meta = ( ClampMin = "0", UIMin = "0", Units="Percent" ) )
    float Zoom = 100.f;

private:
    UOdysseyAnimationLayer* mLayer;
    UOdysseyAnimationCell* mCell;
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

class FOdysseyAnimationEditorOutOfPegsToolDetails : public IDetailCustomization
{
public:
    /** Makes a new instance of this detail layout class for a specific detail view requesting it */
    static TSharedRef<IDetailCustomization> MakeInstance();

    // IDetailCustomization interface
    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;
    // End of IDetailCustomization interface
};
