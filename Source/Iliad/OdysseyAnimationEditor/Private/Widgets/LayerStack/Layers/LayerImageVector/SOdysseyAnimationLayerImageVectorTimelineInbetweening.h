// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <list>

//#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class FInbetweeningListViewItem;
class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorTagInbetweener;
class FOdysseyVectorEngine;
class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyPainterEditor;
class FOdysseyVectorGroupPaint;

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationLayerImageVectorTimelineInbetweening
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationLayerImageVectorTimelineInbetweening
    : public SListView<TSharedPtr<FInbetweeningListViewItem>>
{
public:
    ~SOdysseyAnimationLayerImageVectorTimelineInbetweening();
    SOdysseyAnimationLayerImageVectorTimelineInbetweening();

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationLayerImageVectorTimelineInbetweening)
        : _PainterEditor(nullptr)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
        SLATE_ATTRIBUTE(FOdysseyPainterEditor*, PainterEditor)
    SLATE_END_ARGS()

    void Construct( const FArguments& InArgs
                    , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector );

    void Update();
    UOdysseyAnimationLayerImageVector* GetAnimationLayerImageVector();
    void SetCursorPos( FVector2D iCursorPos );
    const FSlateBrush *GetForwardArrowBrush();
    const FSlateBrush *GetBackwardArrowBrush();

    FOdysseyPainterEditor* GetEditor() const;
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> GetTimelinePosition() const;

protected:
    TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                        , const TSharedRef<STableViewBase>& iOwnerTable );
    TSharedPtr<SWidget> OnContextMenuOpening();
    void AddBreakdown();
    void RemoveBreakdown();
    void ChangeDirection();
    void GetSelectedInbetweenerTags( std::list<FOdysseyVectorTagInbetweener*>& oSelectedInbetweenerTagList
                                    , std::list<FOdysseyVectorEngine*>& oEngineList );

    void OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iNotificationFlags );

private :
    virtual bool Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const override;

protected:
    UOdysseyAnimationLayerImageVector* mAnimationLayerImageVector;
    TArray<TSharedPtr<FInbetweeningListViewItem>> mItemsSource;
    FVector2D mCursorPos;
    const FSlateBrush *mForwardArrowBrush;
    const FSlateBrush *mBackwardArrowBrush;
    TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    TAttribute<FOdysseyPainterEditor*> mEditor;
};
