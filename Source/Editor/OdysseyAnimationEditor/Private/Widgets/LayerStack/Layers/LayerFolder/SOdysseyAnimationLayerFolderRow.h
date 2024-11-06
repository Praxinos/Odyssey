// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LayerStack/Layers/SOdysseyAnimationLayerRow.h"

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerFolderRow
    : public SOdysseyAnimationLayerRow
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerFolderRow)
        : _PainterEditor(nullptr)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE(TSharedPtr<FOdysseyPainterEditor>, PainterEditor)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
        class UOdysseyAnimationLayerFolder* iAnimationLayerFolder
    );

private:
    class UOdysseyAnimationLayerFolder* mAnimationLayerFolder;
    TAttribute<TSharedPtr<FOdysseyPainterEditor>> mEditor;
};
