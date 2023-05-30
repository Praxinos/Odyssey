// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageRasterCell
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnBuildContextMenu, FMenuBuilder&)
    DECLARE_DELEGATE_OneParam(FOnMapActions, TSharedPtr<FUICommandList>)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageRasterCell)
        {}
        SLATE_EVENT(FOnBuildContextMenu, OnBuildContextMenu)
        SLATE_EVENT(FOnMapActions, OnMapActions)
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        FOdysseyAnimationEditor* iEditor,
        UOdysseyAnimationLayerImageRaster* iLayer,
        int iCellIndex
    );

public:
    virtual bool SupportsKeyboardFocus() const override;
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;

private:
    FOdysseyAnimationEditor* mEditor;
    UOdysseyAnimationLayerImageRaster* mLayer;
    int mCellIndex;

    FOnBuildContextMenu mOnBuildContextMenu;
    FOnMapActions mOnMapActions;
};