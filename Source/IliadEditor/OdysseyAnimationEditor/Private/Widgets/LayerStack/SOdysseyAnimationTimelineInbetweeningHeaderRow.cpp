// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "OdysseyLayerStack.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "OdysseyStyleSet.h"
#include "OdysseyPainterEditor.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// from module OdysseyVector
#include "OdysseyVector.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorGroupPaint.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
    const FSlateBrush* inbetweenerTagIcon = FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Matching16" );
    FOdysseyVectorTagInbetweener* inbetweenerTag = iTem.Get()->GetInbetweenerTag();
    uint32 cellIndex = inbetweenerTag->GetOwner()->GetEngine()->GetCell()->GetIndex();

    mInbetweenerTag = inbetweenerTag;

    mTextBlockWidget = SNew(STextBlock)
                       .Text( FText::FromString( FString::Printf( TEXT("Cell %d / "), cellIndex )
                                               + mInbetweenerTag->GetOwner()->GetName() ) );

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
    TSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader> treeView = StaticCastSharedPtr<SOdysseyAnimationTimelineInbetweeningHeader>(OwnerTablePtr.Pin());
    UOdysseyAnimationLayerImageVector* layer = treeView.Get()->GetAnimationLayerImageVector();
    FOdysseyPainterEditor* editor = treeView->GetEditor();
    UOdysseyLayerStack* layerStack = editor->LayerStack();

    if ( layerStack->CurrentLayer.Get() != layer )
    {
        FOdysseyObjectEditorUtils::SetPropertyValue( layerStack
                                                    , "CurrentLayer"
                                                    , TSoftObjectPtr<UOdysseyLayer>( layer ) );
    }

    return STableRow<TSharedPtr<FInbetweeningListViewItem>>::OnMouseButtonDown( MyGeometry, MouseEvent );
}

bool
SOdysseyAnimationTimelineInbetweeningHeaderRow::IsItemSelected() const
{
    return mInbetweenerTag->GetOwner()->IsSelected();
}

FVector2D
SOdysseyAnimationTimelineInbetweeningHeaderRow::ComputeDesiredSize ( float LayoutScaleMultiplier ) const
{
    return FVector2D( 64
                    , UOdysseyAnimationLayerImageVector::GetInbetweeningRowHeight() );
}

#undef LOCTEXT_NAMESPACE
