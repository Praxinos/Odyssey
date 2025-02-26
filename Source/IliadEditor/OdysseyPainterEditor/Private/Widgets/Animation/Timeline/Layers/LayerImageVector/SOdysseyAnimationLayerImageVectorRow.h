// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerRow.h"

class SOdysseyAnimationTimelineInbetweeningHeader;

class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyPainterEditor;
/**
 * Implements a layer row widget
 */
class SOdysseyAnimationLayerImageVectorRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorRow)
        : _PainterEditor(nullptr)
        {}
        SLATE_ATTRIBUTE(int, CurrentFrame)
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE(FOdysseyPainterEditor*, PainterEditor)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
    );

public:
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> GetInbetweeningHeader();

protected:
    virtual TSharedRef<SWidget> GenerateWidget( const FName& iRow, const FName& iColumn ) override;
    virtual EVisibility GetRowVisibility(FName iRow) const;
    virtual TArray<TSharedPtr<SWidget>> GenerateMainRowHeaderOptionWidgets() override;
    TSharedRef<SWidget> GenerateInbetweeningRowHeaderWidget();

private:
    void OnIsColoredCheckStateChanged( ECheckBoxState iState );
    void OnIsWireframeCheckStateChanged( ECheckBoxState iState );
    ECheckBoxState GetIsColoredIsChecked() const;
    ECheckBoxState GetIsWireframeIsChecked() const;

private:
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> mInbetweeningHeader;
    class UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
    TAttribute<FOdysseyPainterEditor*> mEditor;
};
