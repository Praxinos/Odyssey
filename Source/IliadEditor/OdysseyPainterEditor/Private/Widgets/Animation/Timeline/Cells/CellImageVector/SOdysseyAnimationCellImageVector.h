// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationCellImageVector;

class SOdysseyAnimationCellImageVector
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCellImageVector)
        : _ShowContent(true)
        {}
        SLATE_ATTRIBUTE(bool, ShowContent)
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, UOdysseyAnimationCellImageVector* iCell);

private:
    void OnThumbnailDirtied();
    void OnThumbnailChanged();

    EVisibility GetContentVisibility() const;

private:
    UOdysseyAnimationCellImageVector* mCell = nullptr;
    TSharedPtr<FAssetThumbnail> mAssetThumbnail;
    TAttribute<bool> mShowContent;
};
