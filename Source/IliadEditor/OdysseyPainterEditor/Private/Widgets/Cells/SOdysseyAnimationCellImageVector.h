// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "OdysseyRenderingAbility.h"

class UOdysseyAnimationCellImageVector;
class UTextureRenderTarget2D;

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
    ~SOdysseyAnimationCellImageVector();
    void Construct(const FArguments& iArgs, UOdysseyAnimationCellImageVector* iCell);

private:
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
    EVisibility GetContentVisibility() const;
    void RefreshCellRenderTarget();
    void CreateCheckerboardTexture();

private:
    TObjectPtr<UOdysseyAnimationCellImageVector> mCell;

    TStrongObjectPtr<UTextureRenderTarget2D> RenderTarget;
    FSlateImageBrush* mCellBrush;

    FSlateImageBrush* mCheckerboardBrush;

    TAttribute<bool> mShowContent;
};
