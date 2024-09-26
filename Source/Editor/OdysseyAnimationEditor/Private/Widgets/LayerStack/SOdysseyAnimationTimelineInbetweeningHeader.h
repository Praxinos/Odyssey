// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class FInbetweeningListViewItem;
class FOdysseyVectorSharedEnv;
class FOdysseyAnimationEditorExtension;
class UOdysseyAnimationLayerImageVector;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineInbetweeningHeader
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineInbetweeningHeader
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
    public:
        SOdysseyAnimationTimelineInbetweeningHeader();

    public:
	    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineInbetweeningHeader)
            {}
        SLATE_END_ARGS()

	    void Construct( const FArguments& InArgs
                      , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                      , FOdysseyAnimationEditorExtension* iAnimationEditorExtension );
        FOdysseyAnimationEditorExtension* GetAnimationEditorExtension();
        UOdysseyAnimationLayerImageVector* GetAnimationLayerImageVector();
        virtual FReply OnKeyUp ( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;
        void Update();

    protected:
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                           , const TSharedRef<STableViewBase>& iOwnerTable );
        TSharedPtr<SWidget> OnContextMenuOpening();
        void RemoveInbetweenerTags();

    private :
        virtual bool Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const override;

    protected:
        UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
	    FOdysseyAnimationEditorExtension* mAnimationEditorExtension;
        TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
};
