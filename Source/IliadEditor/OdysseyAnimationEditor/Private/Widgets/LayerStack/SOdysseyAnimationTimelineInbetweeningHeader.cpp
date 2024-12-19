// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Math/UnitConversion.h"
#include "OdysseyStyleSet.h"

// from module OdysseyPainterEditor
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditor.h"

// from module OdysseyVector
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorEngine.h"
#include "Undo/OdysseyVectorUndoTagRemove.h"

#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineInbetweeningHeader::~SOdysseyAnimationTimelineInbetweeningHeader()
{
    FOdysseyVectorEngine::OnNotifyDelegate().RemoveAll( this );
}

SOdysseyAnimationTimelineInbetweeningHeader::SOdysseyAnimationTimelineInbetweeningHeader()
    : mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Construct( const FArguments& InArgs
                                                      , UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector )
{
    mAnimationLayerImageVector = iAnimationLayerImageVector;
    mEditor = InArgs._PainterEditor;

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnNotifyDelegate().AddRaw( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnVectorSceneNotify );

    SListView<TSharedPtr<FInbetweeningListViewItem>>::Construct(
        SListView<TSharedPtr<FInbetweeningListViewItem>>::FArguments()
        // for some reason, SetItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .ListItemsSource(&mItemsSource)
        .OnGenerateRow( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow )
        //.OnGetChildren( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnGetChildren )
        //.OnSelectionChanged( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyAnimationTimelineInbetweeningHeader::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        //.HeaderRow(headerRow)
    );

    Update();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iNotificationFlags )
{
    if( iNotificationFlags & FOdysseyPainterEditor::UI_UPDATE_TIMELINE )
    {
        Update();
    }
}

FReply
SOdysseyAnimationTimelineInbetweeningHeader::OnKeyDown( const FGeometry& iGeometry
                                                      , const FKeyEvent& iKeyEvent )
{
    if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SListView<TSharedPtr<FInbetweeningListViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

FReply
SOdysseyAnimationTimelineInbetweeningHeader::OnKeyUp ( const FGeometry& MyGeometry
                                                        , const FKeyEvent& InKeyEvent )
{
    return FReply::Unhandled();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Update()
{
    mItemsSource.Reset();

    for( FOdysseyVectorTag* tag : mAnimationLayerImageVector->GetSharedEnv()->GetSharedTagList() )
    {
        if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( tag );

            mItemsSource.Insert( MakeShareable( new FInbetweeningListViewItem(inbetweenerTag) ), 0 );
        }
    }

    // Select items if needed
    //SelectedItems.Empty();
    RequestListRefresh();
}

bool
SOdysseyAnimationTimelineInbetweeningHeader::Private_IsItemSelected( const TSharedPtr<FInbetweeningListViewItem>& iItem )  const
{
    UOdysseyLayerStack* layerStack = mAnimationLayerImageVector->GetLayerStack();
    return iItem.Get()->GetInbetweenerTag()->GetOwner()->IsSelected() && ( layerStack->CurrentLayer == mAnimationLayerImageVector );
}

TSharedRef<ITableRow>
SOdysseyAnimationTimelineInbetweeningHeader::OnGenerateRow( TSharedPtr<FInbetweeningListViewItem> iItem
                                                          , const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SOdysseyAnimationTimelineInbetweeningHeaderRow, iOwnerTable, iItem );
}

TSharedPtr<SWidget>
SOdysseyAnimationTimelineInbetweeningHeader::OnContextMenuOpening()
{
    FMenuBuilder menu( true, nullptr );

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.remove-tag.name", "Remove Tag")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.remove-tag.tooltip", "Remove Tag")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.add-breakdown.name", "Commit")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.add-breakdown.tooltip", "Commit")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationTimelineInbetweeningHeader::Commit )));

    menu.AddMenuEntry( LOCTEXT("vector-tool.inbetweening-context-menu.reset-spacing-charts.name", "Reset spacing charts")
                     , LOCTEXT("vector-tool.inbetweening-context-menu.reset-spacing-charts.tooltip", "Reset spacing charts")
                     , FSlateIcon()
                     , FUIAction(FExecuteAction::CreateSP( this, &SOdysseyAnimationTimelineInbetweeningHeader::ResetSpacingCharts )));

    return menu.MakeWidget();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::Commit()
{
    FOdysseyPainterEditor* editor = mEditor.Get();

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::CommitSelectedInbetweenerTag( editor, mAnimationLayerImageVector->GetSharedEnv() );
}

void
SOdysseyAnimationTimelineInbetweeningHeader::ResetSpacingCharts()
{
    FOdysseyPainterEditor* editor = mEditor.Get();

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::ResetInbetweenerTagSpacingChart( editor, mAnimationLayerImageVector->GetSharedEnv() );
}

void
SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag()
{
    FOdysseyPainterEditor* editor = mEditor.Get();

    // note: editor is NULL in the Sequencer
    FOdysseyPainterEditor::RemoveInbetweenerTag( editor, mAnimationLayerImageVector->GetSharedEnv() );
}

UOdysseyAnimationLayerImageVector*
SOdysseyAnimationTimelineInbetweeningHeader::GetAnimationLayerImageVector()
{
    return mAnimationLayerImageVector;
}

FOdysseyPainterEditor*
SOdysseyAnimationTimelineInbetweeningHeader::GetEditor() const
{
    return mEditor.Get();
}

void
SOdysseyAnimationTimelineInbetweeningHeader::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SOdysseyAnimationTimelineInbetweeningHeader::RemoveInbetweenerTag )
    );
}

#undef LOCTEXT_NAMESPACE
