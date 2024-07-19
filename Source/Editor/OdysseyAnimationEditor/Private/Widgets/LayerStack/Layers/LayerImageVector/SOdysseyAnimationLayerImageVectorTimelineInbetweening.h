// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class FInbetweeningListViewItem;
class FOdysseyVectorSharedEnv;
class FOdysseyAnimationEditorExtension;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationLayerImageVectorTimelineInbetweening
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimelineInbetweening
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        SOdysseyAnimationLayerImageVectorTimelineInbetweening();

    public:
	    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorTimelineInbetweening)
            {}
        SLATE_END_ARGS()

	    void Construct( const FArguments& InArgs
                      , FOdysseyVectorSharedEnv* iVectorSharedEnv
                      , FOdysseyAnimationEditorExtension* iAnimationEditorExtension );

        void Update();
        FOdysseyAnimationEditorExtension* GetAnimationEditorExtension();

    protected:
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                           , const TSharedRef<STableViewBase>& iOwnerTable );

    protected:
	    FOdysseyAnimationEditorExtension* mAnimationEditorExtension;
        TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
        FOdysseyVectorSharedEnv* mVectorSharedEnv;
};
