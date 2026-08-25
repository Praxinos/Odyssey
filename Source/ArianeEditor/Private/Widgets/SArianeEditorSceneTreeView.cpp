// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "SArianeEditorSceneTreeView.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
#include "SArianeEditorSceneTreeViewRow.h"
#include "SArianeEditorSceneTreeViewContextMenu.h"
#include "ArianeLayerStack.h"
// Ariane Headers
#include "ArianeGroup.h"
#include "ArianeLayer.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
#include "ArianePainting3DComponent.h"
// Unreal Headers
#include "Editor.h"
#include "Framework/Commands/GenericCommands.h"


#define LOCTEXT_NAMESPACE "ArianeEditor"

SArianeEditorSceneTreeView::~SArianeEditorSceneTreeView()
{
}

SArianeEditorSceneTreeView::SArianeEditorSceneTreeView()
    : Editor( nullptr )
    , CommandList(MakeShared<FUICommandList>())
    , bDoUpdate ( false )
{
    MapActionsToCommandList();
}

void
SArianeEditorSceneTreeView::Construct( const FArguments& InArgs, FArianeEditor* InEditor )
{
    TSharedPtr<SHeaderRow> headerRow = SNew(SHeaderRow)
                                       .Visibility( EVisibility::Visible )
                                       .CanSelectGeneratedColumn( true )
                                       + SHeaderRow::Column( VSTV_OBJECT_VISIBLE )
                                       .DefaultLabel(FText::FromString(VSTV_OBJECT_VISIBLE))
                                       .FixedWidth( 24.0f )
                                       [
                                           SNew(SHorizontalBox)
                                           + SHorizontalBox::Slot()
                                           .AutoWidth()
                                           .HAlign( EHorizontalAlignment::HAlign_Center )
                                           .VAlign( EVerticalAlignment::VAlign_Center )
                                           [
                                               SNew(SImage)
                                               .ToolTipText( FText::FromString(VSTV_OBJECT_VISIBLE))
                                               .Image( FAppStyle::Get().GetBrush("Level.VisibleIcon16x") )
                                           ]
                                       ]
                                       + SHeaderRow::Column( VSTV_OBJECT_HUDCOLOR )
                                       .DefaultLabel(FText::FromString(VSTV_OBJECT_HUDCOLOR))
                                       .FixedWidth( 8.0f )
                                       [
                                           SNullWidget::NullWidget
                                       ]
                                       + SHeaderRow::Column( VSTV_OBJECT_NAME )
                                       .DefaultLabel(FText::FromString(VSTV_OBJECT_NAME))
                                       .ShouldGenerateWidget(true)
                                       .ManualWidth( 144.0f )
                                       + SHeaderRow::Column(VSTV_OBJECT_TAGS)
                                       .DefaultLabel(FText::FromString(VSTV_OBJECT_TAGS));
    Editor = InEditor;

    STreeView<TSharedPtr<FSceneTreeViewItem>>::Construct(
        STreeView<TSharedPtr<FSceneTreeViewItem>>::FArguments()
        // for some reason, SetTreeItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .TreeItemsSource(&ItemsSource)
        .OnGenerateRow( this, &SArianeEditorSceneTreeView::OnGenerateRow )
        .OnGetChildren( this, &SArianeEditorSceneTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SArianeEditorSceneTreeView::OnExpansionChanged )
        .OnSelectionChanged( this, &SArianeEditorSceneTreeView::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SArianeEditorSceneTreeView::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );

    Editor->OnPre3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPre3DPaintingComponentSelectionChanged );
    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPost3DPaintingComponentSelectionChanged );

    // First update. Following updates will be triggered by delegates
    Update();
}

void
SArianeEditorSceneTreeView::OnPre3DPaintingComponentSelectionChanged()
{
    UnbindComponentDelegates();
}

void
SArianeEditorSceneTreeView::OnPost3DPaintingComponentSelectionChanged()
{
    BindComponentDelegates();

    Update();
}

void
SArianeEditorSceneTreeView::RenameSelectedItem()
{
    TArray<TSharedPtr<FSceneTreeViewItem>> LayerSelectedItems = GetSelectedItems();

    // no need to create an undo record or do anything if the selection is empty
    if( LayerSelectedItems.Num() )
    {
        TSharedPtr<ITableRow> tableRow = WidgetFromItem( LayerSelectedItems[0] );
        TSharedPtr<SArianeEditorSceneTreeViewRow> ItemWidget = StaticCastSharedPtr<SArianeEditorSceneTreeViewRow>(tableRow);

        ItemWidget->Rename();
    }
}

TSharedPtr<SWidget>
SArianeEditorSceneTreeView::OnContextMenuOpening()
{
    return SArianeEditorSceneTreeViewContextMenu::CreateWidget( this );
}

FReply
SArianeEditorSceneTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (CommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<TSharedPtr<FSceneTreeViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

TSharedPtr<FSceneTreeViewItem>
SArianeEditorSceneTreeView::GetRootItem()
{
    return RootItem;
}

FArianeEditor*
SArianeEditorSceneTreeView::GetEditor() const
{
    return Editor;
}

void
SArianeEditorSceneTreeView::BuildTree( const TSharedPtr<FSceneTreeViewItem> InItem )
{
    const TArray<FArianeObjectID>& Children = InItem.Get()->GetObject()->GetChildren();

    InItem.Get()->Children.Empty();

    for( const FArianeObjectID& ChildID : Children )
    {
        FArianeObject* Child = const_cast<FArianeObjectID&>(ChildID).GetObject();
        TSharedPtr<FSceneTreeViewItem> ChildItem = MakeShareable(new FSceneTreeViewItem(Child, true ));

        //InItem.Get()->mChildren.Add( childItem );
        // reverse order in order to get the most forward objet on top of the hierarchy
        InItem.Get()->Children.Insert( ChildItem, 0 );

        BuildTree( ChildItem );
    }
}

void
SArianeEditorSceneTreeView::ExpandTree( const TSharedPtr<FSceneTreeViewItem> InItem )
{
    FArianeObject* ItemObject = InItem.Get()->GetObject();

    if( ItemObject->IsExpanded() )
    {
        SetItemExpansion( InItem, true );

        for( int i = 0; i < InItem.Get()->Children.Num(); i++ )
        {
            ExpandTree( InItem.Get()->Children[i] );
        }
    }

    if( ItemObject->IsSelected() )
    {
        SelectedItems.Add( InItem );
    }
}

FArianeGroup*
SArianeEditorSceneTreeView::GetRootGroup()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

        if( DrawingLayer )
        {
            return DrawingLayer->GetRootGroup();
        }
    }

    return nullptr;
}

void
SArianeEditorSceneTreeView::Update()
{
    ItemsSource.Empty();
    ClearSelection();
    RequestTreeRefresh();

    FArianeGroup* RootGroup = GetRootGroup();

    if( RootGroup )
    {
        RootItem = MakeShareable(new FSceneTreeViewItem(RootGroup, true ));

        BuildTree( RootItem );

        ItemsSource.Add( RootItem );

        // Expand items if need
        ExpandTree( RootItem );
    }
}

void
SArianeEditorSceneTreeView::OnGetChildren( TSharedPtr<FSceneTreeViewItem> Parent
                                         , TArray<TSharedPtr<FSceneTreeViewItem>>& OutChildren) const
{
    OutChildren = Parent.Get()->Children;
}

TSharedRef<ITableRow>
SArianeEditorSceneTreeView::OnGenerateRow( TSharedPtr<FSceneTreeViewItem> InItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SArianeEditorSceneTreeViewRow, iOwnerTable, InItem );
}

void
SArianeEditorSceneTreeView::OnExpansionChanged( TSharedPtr<FSceneTreeViewItem> InItem, bool mExpanded )
{
    FArianeObject* ExpandedObject = InItem.Get()->GetObject();

    ExpandedObject->SetExpanded( mExpanded );

    // Reselect
    //SelectedItems.Empty();
    //SelectTree( RootItem );
}

void
SArianeEditorSceneTreeView::OnSelectionChanged( TSharedPtr<FSceneTreeViewItem> InItem
                                              , ESelectInfo::Type SelectInfo )
{
    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    UnbindComponentDelegates();

    // RootItem can be null if the tree is empty
    if( RootItem )
    {
        FArianeGroup* RootGroup = static_cast<FArianeGroup*>(RootItem.Get()->GetObject());

        // only if the action is interactive (user action via GUI)
        if ( SelectInfo != ESelectInfo::Type::Direct )
        {
            // needed for valid GUndo pointer
            ////GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));

            TArray<TSharedPtr<FSceneTreeViewItem>> selectedItems = GetSelectedItems();

            RootGroup->GetDrawingLayer()->ClearObjectSelection();

            for( TSharedPtr<FSceneTreeViewItem> selectedItem : selectedItems )
            {
                FArianeObject* SelectedObject = selectedItem.Get()->GetObject();

                if( SelectedObject->IsSelected() == false )
                {
                    SelectedObject->GetDrawingLayer()->SelectObject( SelectedObject );
                }
            }
        }

        RootGroup->GetPainting3DComponent()->Update( false );
    }

    // Re-register this widget after we are done
    BindComponentDelegates();
}

void
SArianeEditorSceneTreeView::SelectAll()
{
    if( RootItem )
    {
        FArianeGroup* RootGroup = static_cast<FArianeGroup*>(RootItem.Get()->GetObject());

        //FArianeEditor::SelectAllObjects( Editor, RootGroup );
    }
}

void
SArianeEditorSceneTreeView::DeleteObjects()
{
    if( RootItem )
    {
        FArianeGroup* RootGroup = static_cast<FArianeGroup*>(RootItem.Get()->GetObject());

        //FArianeEditor::DeleteObjects( Editor, RootGroup );
    }
}

void
SArianeEditorSceneTreeView::CutObjects()
{
    SArianeEditorSceneTreeView::CopyObjects();
    SArianeEditorSceneTreeView::DeleteObjects();
}

void
SArianeEditorSceneTreeView::CopyObjects()
{
    if( RootItem )
    {
        FArianeGroup* RootGroup = static_cast<FArianeGroup*>(RootItem.Get()->GetObject());

        //FArianeEditor::CopyObjects( RootGroup  );
    }
}

void
SArianeEditorSceneTreeView::PasteObjects()
{
    if( RootItem )
    {
        FArianeGroup* RootGroup = static_cast<FArianeGroup*>(RootItem.Get()->GetObject());

        //FArianeEditor::PasteObjects( Editor, RootGroup );
    }
}

void
SArianeEditorSceneTreeView::MapActionsToCommandList()
{
    CommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw( this, &SArianeEditorSceneTreeView::SelectAll )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SArianeEditorSceneTreeView::DeleteObjects )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw( this, &SArianeEditorSceneTreeView::CutObjects )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw( this, &SArianeEditorSceneTreeView::CopyObjects )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw( this, &SArianeEditorSceneTreeView::PasteObjects )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw( this, &SArianeEditorSceneTreeView::RenameSelectedItem )
    );
}

void
SArianeEditorSceneTreeView::OnPrePainting3DComponentUpdate( bool bInteractive )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();
    UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Painting3DComponent->GetLayerStack()->GetCurrentLayer());

    // Note: Painting3DComponent cannot be null since it is supposed to exist at that step. Do not check for its validity.

    // the bInteractive is voluntarily ignored. During a MouseDown, the flag is set but we still need to mark the widget
    // as needing an update
    if( DrawingLayer )
    {
        FArianeGroup* RootGroup = DrawingLayer->GetRootGroup();

        FArianeObject::Traverse ( RootGroup
                                , [this] ( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
            {
                if( Object->GetInvalidationFlags().Hierarchy
                 || Object->GetInvalidationFlags().Name
                 || Object->GetInvalidationFlags().Tags )
                {
                    bDoUpdate = true;
                }

                return bDoUpdate ? FArianeObject::ETraversalReturnValue::Stop
                                    : FArianeObject::ETraversalReturnValue::Continue;
            } );
    }
}

void
SArianeEditorSceneTreeView::OnPostPainting3DComponentUpdate( bool bInteractive )
{
    if( bInteractive == false )
    {
        if( bDoUpdate )
        {
            Update();

            bDoUpdate = false;
        }
    }
}

void
SArianeEditorSceneTreeView::OnPreLayerStackSelectionChanged()
{
}

void
SArianeEditorSceneTreeView::OnPostLayerStackSelectionChanged()
{
    Update();
}

void
SArianeEditorSceneTreeView::OnPreLayerStackHierarchyChanged()
{
}

void
SArianeEditorSceneTreeView::OnPostLayerStackHierarchyChanged()
{
    Update();
}

void
SArianeEditorSceneTreeView::BindComponentDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->GetLayerStack()->OnPreHierarchyChangedDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPreLayerStackHierarchyChanged );
        Painting3DComponent->GetLayerStack()->OnPostHierarchyChangedDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPostLayerStackHierarchyChanged );

        Painting3DComponent->GetLayerStack()->OnPreSelectionChangedDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPreLayerStackSelectionChanged );
        Painting3DComponent->GetLayerStack()->OnPostSelectionChangedDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPostLayerStackSelectionChanged );

        Painting3DComponent->OnPreUpdateDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPrePainting3DComponentUpdate );
        Painting3DComponent->OnPostUpdateDelegate().AddSP( this, &SArianeEditorSceneTreeView::OnPostPainting3DComponentUpdate );
    }
}

void
SArianeEditorSceneTreeView::UnbindComponentDelegates()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        Painting3DComponent->GetLayerStack()->OnPreHierarchyChangedDelegate().RemoveAll( this );
        Painting3DComponent->GetLayerStack()->OnPostHierarchyChangedDelegate().RemoveAll( this );

        Painting3DComponent->GetLayerStack()->OnPreSelectionChangedDelegate().RemoveAll( this );
        Painting3DComponent->GetLayerStack()->OnPostSelectionChangedDelegate().RemoveAll( this );

        Painting3DComponent->OnPreUpdateDelegate().RemoveAll( this );
        Painting3DComponent->OnPostUpdateDelegate().RemoveAll( this );
    }
}

#undef LOCTEXT_NAMESPACE
