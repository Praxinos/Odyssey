// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <list>

//#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class FInbetweeningListViewItem;
class FOdysseyAnimationEditorExtension;
class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorTagInbetweener;
class FOdysseyVectorEngine;

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
                      , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                      , FOdysseyAnimationEditorExtension* iAnimationEditorExtension );

        void Update();
        FOdysseyAnimationEditorExtension* GetAnimationEditorExtension();
        UOdysseyAnimationLayerImageVector* GetAnimationLayerImageVector();
        void SetCursorPos( FVector2D iCursorPos );

    protected:
        TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                           , const TSharedRef<STableViewBase>& iOwnerTable );
        TSharedPtr<SWidget> OnContextMenuOpening();
        void AddBreakdown();
        void RemoveBreakdown();
        void ChangeDirection();
        void GetSelectedInbetweenerTags( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList
                                       , std::list<FOdysseyVectorEngine*>& oEngineList );

    private :
        virtual bool Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const override;

    protected:
        UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
	    FOdysseyAnimationEditorExtension* mAnimationEditorExtension;
        TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
        FVector2D mCursorPos;
};
