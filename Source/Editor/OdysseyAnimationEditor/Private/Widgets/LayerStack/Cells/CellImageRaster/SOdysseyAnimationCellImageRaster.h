// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationCellImageRaster;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationCellImageRaster
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCellImageRaster)
        : _ShowContent(true)
        {}
        SLATE_ATTRIBUTE(bool, ShowContent)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, UOdysseyAnimationCellImageRaster* iCell);

private:
    void OnThumbnailDirtied();
    void OnThumbnailChanged();

    EVisibility GetContentVisibility() const;

private:
    UOdysseyAnimationCellImageRaster* mCell = nullptr;
    TSharedPtr<FAssetThumbnail> mAssetThumbnail;
    TAttribute<bool> mShowContent;
};