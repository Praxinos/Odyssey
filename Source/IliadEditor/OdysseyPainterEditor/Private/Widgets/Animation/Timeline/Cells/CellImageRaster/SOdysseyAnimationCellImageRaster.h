// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyRenderingAbility.h"

class UOdysseyAnimationCellImageRaster;

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

private:
    UOdysseyAnimationCellImageRaster* mCell = nullptr;
    TSharedPtr<FAssetThumbnail> mAssetThumbnail;
    TAttribute<bool> mShowContent;
};
