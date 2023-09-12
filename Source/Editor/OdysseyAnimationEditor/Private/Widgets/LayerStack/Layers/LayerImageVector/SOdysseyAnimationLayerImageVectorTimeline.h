// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationCell;
class UOdysseyAnimationLayerImageVector;

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimeline
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorTimeline)
        {}
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyAnimationLayerImageVectorTimeline();
    SOdysseyAnimationLayerImageVectorTimeline();
    void Construct(
        const FArguments& iArgs, 
        FOdysseyAnimationEditorExtension* iExtension,
        UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
    );

public:
    virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent) override;
    virtual FReply OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent ) override;
    
    virtual bool SupportsKeyboardFocus() const override;

private:
    TSharedRef<FOdysseyAnimationCell> OnCreateCell();
    TSharedRef<SWidget> OnCreateCellWidget(TSharedPtr<FOdysseyAnimationCell> iCell);
    EVisibility GetFrameSelectorVisibility() const;
    void OnBuildCellsContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);
    void OnBuildFrameSelectorContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);

    void SelectAllFrames();
    void DeleteSelectedFrames();
    void BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);
    void MapActions(TSharedPtr<FUICommandList> iCommandList);

private:
    FOdysseyAnimationEditorExtension* mExtension;
    UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
	
	TSharedRef<FUICommandList> mCommandList;
};
