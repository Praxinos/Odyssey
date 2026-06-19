// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Engine/TextureRenderTarget2D.h"

#include "OdysseyRenderingAbility.h"

class UOdysseyAnimationCellImageRaster;
class UTextureRenderTarget2D;

class SOdysseyAnimationCellImageRaster
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCellImageRaster)
        : _ShowContent(true)
        {}
        SLATE_ATTRIBUTE(bool, ShowContent)
    SLATE_END_ARGS()

public:
    ~SOdysseyAnimationCellImageRaster();
    void Construct(const FArguments& iArgs, UOdysseyAnimationCellImageRaster* iCell);

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
    EVisibility GetContentVisibility() const;
    void RefreshCellRenderTarget();
    void CreateCheckerboardTexture();

private:
    TObjectPtr<UOdysseyAnimationCellImageRaster> mCell;
    TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;

    struct FSlateImageBrush* mCellBrush;
    struct FSlateImageBrush* mCheckerboardBrush;

    TAttribute<bool> mShowContent;
};
