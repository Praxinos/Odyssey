// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "OdysseyStyleSet.h"
#include "OdysseyVector.h"

#define LOCTEXT_NAMESPACE "SOdysseyPainterEditorVectorSceneTreeView"

FVectorSceneTreeViewItem::~FVectorSceneTreeViewItem()
{
}

FVectorSceneTreeViewItem::FVectorSceneTreeViewItem( FOdysseyVectorObject* iVectorObject )
{
    mVectorObject = iVectorObject;
}

FOdysseyVectorObject*
FVectorSceneTreeViewItem::GetVectorObject()
{
    return mVectorObject;
}

SOdysseyPainterEditorVectorSceneTreeViewRow::~SOdysseyPainterEditorVectorSceneTreeViewRow()
{
}

SOdysseyPainterEditorVectorSceneTreeViewRow::SOdysseyPainterEditorVectorSceneTreeViewRow()
    : mDropZone( DROPZONE_NONE )
{
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::Construct( const typename STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments& InArgs
                                                      , const TSharedRef< STableViewBase >& InOwnerTableView )
{
    STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::Construct( InArgs, InOwnerTableView );
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDrop( const FGeometry& iGeometry
                                                   , const FDragDropEvent& iDragDropEvent )
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();

    const TSharedPtr<FVectorSceneTreeViewItem>* item = GetItemForThis( OwnerTablePtr.Pin().ToSharedRef() );
    FOdysseyVectorObject* vectorObject = item->Get()->GetVectorObject();
    FOdysseyVectorScene* vectorScene = vectorObject->GetScene();
    std::list<FOdysseyVectorObject*>& selectedObjectList = vectorScene->GetSelectedObjectList();

    for( FOdysseyVectorObject* selectedObject : selectedObjectList )
    {
        vectorObject->TransferChild( selectedObject );
    }

    UE_LOG(LogTemp, Warning, TEXT("Some warning message %s"), *(vectorObject->GetName()) );

    mDropZone = DROPZONE_NONE;

    vectorScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    vectorScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                                    | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                                    | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                                    | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return FReply::Handled();
}

int32
SOdysseyPainterEditorVectorSceneTreeViewRow::OnPaint( const FPaintArgs& Args
                                                    , const FGeometry& AllottedGeometry
                                                    , const FSlateRect& MyCullingRect
                                                    , FSlateWindowElementList& OutDrawElements
                                                    , int32 LayerId
                                                    , const FWidgetStyle& InWidgetStyle
                                                    , bool bParentEnabled ) const
{
    //const FSlateBrush* DropIndicatorBrush = &InWidgetStyle.DropIndicator_Above;

    FSlateBrush myBrush = FSlateImageBrush(
                TEXT(""),
                FVector2D(1,1),
                FSlateColor(FColor::Red),
                ESlateBrushTileType::NoTile,
                ESlateBrushImageType::NoImage
    );

    int32 rowLayerId = STableRow<TSharedPtr<FVectorSceneTreeViewItem>>::OnPaint( Args
                                                                               , AllottedGeometry
                                                                               , MyCullingRect
                                                                               , OutDrawElements
                                                                               , LayerId
                                                                               , InWidgetStyle
                                                                               , bParentEnabled );
    if( mDropZone )
    {
        FSlateDrawElement::MakeBox
        (
            OutDrawElements,
            rowLayerId++,
            AllottedGeometry.ToPaintGeometry(),
            &myBrush,
            ESlateDrawEffect::None,
            myBrush.GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
        );
    }

    return rowLayerId;
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragEnter( const FGeometry& MyGeometry
                                                        , const FDragDropEvent& DragDropEvent )
{
    const TSharedPtr<FVectorSceneTreeViewItem>* item = GetItemForThis( OwnerTablePtr.Pin().ToSharedRef() );

    mDropZone = DROPZONE_ON;
}

void
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragLeave( const FDragDropEvent& DragDropEvent )
{
    const TSharedPtr<FVectorSceneTreeViewItem>* item = GetItemForThis( OwnerTablePtr.Pin().ToSharedRef() );

    mDropZone = DROPZONE_NONE;
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragOver( const FGeometry& iGeometry
                                                       , const FDragDropEvent& iDragDropEvent )
{
    TSharedPtr<FDragDropOperation> Operation = iDragDropEvent.GetOperation();
    //FVector2D position = iGeometry.GetAbsolutePosition();

    return FReply::Handled();
}

FReply
SOdysseyPainterEditorVectorSceneTreeViewRow::OnDragDetected ( const FGeometry& iGeometry,
                                                              const FPointerEvent& iMouseEvent )
{
    TSharedRef<FDragDropOperation> Operation = MakeShared<FDragDropOperation>();

    return FReply::Handled().BeginDragDrop(Operation);
}

#undef LOCTEXT_NAMESPACE
