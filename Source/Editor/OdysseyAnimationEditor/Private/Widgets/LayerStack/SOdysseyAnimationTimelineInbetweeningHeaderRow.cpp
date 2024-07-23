// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyleSet.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// from module OdysseyVector
#include "OdysseyVector.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FInbetweeningListViewItem::~FInbetweeningListViewItem()
{
}

FInbetweeningListViewItem::FInbetweeningListViewItem( FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mInbetweenerTag = iInbetweenerTag;
}

FOdysseyVectorTagInbetweener*
FInbetweeningListViewItem::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

SOdysseyAnimationTimelineInbetweeningHeaderRow::~SOdysseyAnimationTimelineInbetweeningHeaderRow()
{
}

SOdysseyAnimationTimelineInbetweeningHeaderRow::SOdysseyAnimationTimelineInbetweeningHeaderRow()
{
}

void
SOdysseyAnimationTimelineInbetweeningHeaderRow::Construct( const typename STableRow<TSharedPtr<FInbetweeningListViewItem>>::FArguments& InArgs
                                                         , const TSharedRef< STableViewBase >& InOwnerTableView
                                                         , const TSharedPtr<FInbetweeningListViewItem> iTem )
{
    STableRow<TSharedPtr<FInbetweeningListViewItem>>::Construct( InArgs, InOwnerTableView );
    TSharedPtr<SHorizontalBox> tagBox;
    const FSlateBrush* objectIcon = nullptr;
    const FSlateBrush* inbetweenerTagIcon = nullptr;

    inbetweenerTagIcon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching16" );

    mInbetweenerTag = iTem.Get()->GetInbetweenerTag();

    mTextBlockWidget = SNew(STextBlock)
                       .Text( FText::FromString( mInbetweenerTag->GetOwner()->GetName() ) );

    tagBox = SNew(SHorizontalBox);

    SetContent( SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SImage )
                    .Image( inbetweenerTagIcon )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    mTextBlockWidget.ToSharedRef()
                ] );
}

FReply
SOdysseyAnimationTimelineInbetweeningHeaderRow::OnMouseButtonDown( const FGeometry & MyGeometry
                                                                 , const FPointerEvent & MouseEvent )
{
    FOdysseyVectorSharedEnv* sharedEnv = mInbetweenerTag->GetOwner()->GetEngine()->GetSharedEnv();
    std::list<FOdysseyVectorTag*>& tagList = sharedEnv->GetTagList();

    for( FOdysseyVectorTag* tag : tagList )
    {
        tag->SetSelected( false );
    }

    mInbetweenerTag->SetSelected( true );

    return FReply::Handled();
}

FVector2D
SOdysseyAnimationTimelineInbetweeningHeaderRow::ComputeDesiredSize ( float LayoutScaleMultiplier ) const
{
    return FVector2D( 20.0f * LayoutScaleMultiplier, 20.0f * LayoutScaleMultiplier );
}

#undef LOCTEXT_NAMESPACE
