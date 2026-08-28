// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "SArianeEditorLayerStack.h"
#include "SArianeEditorLayerStackContextMenu.h"
#include "SArianeEditorLayerRow.h"
#include "ArianeEditor.h"
#include "ArianeEditorStyle.h"
// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerStack.h"


// Unreal headers
#include "Framework/Commands/GenericCommands.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

SArianeEditorLayerStack::~SArianeEditorLayerStack()
{
    USelection::SelectionChangedEvent.RemoveAll( this );
}

SArianeEditorLayerStack::SArianeEditorLayerStack()
    : CommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

void
SArianeEditorLayerStack::Construct( const FArguments& InArgs, FArianeEditor* InEditor )
{
    TSharedPtr<SHeaderRow> headerRow = SNew(SHeaderRow)
                                       .Visibility( EVisibility::Visible )
                                       .CanSelectGeneratedColumn( true )
                                       + SHeaderRow::Column( LAYER_NAME )
                                       .DefaultLabel(FText::FromString(LAYER_NAME))
                                       .ShouldGenerateWidget(true)
                                       .FillWidth( 1.0f )
                                       + SHeaderRow::Column( LAYER_TRANSFORM )
                                       .DefaultLabel(FText::FromString(LAYER_TRANSFORM))
                                       .HAlignHeader( HAlign_Right )
                                       .FixedWidth( 24.0f )
                                       [
                                           SNew(SHorizontalBox)
                                           + SHorizontalBox::Slot()
                                           .AutoWidth()
                                           .HAlign( EHorizontalAlignment::HAlign_Center )
                                           .VAlign( EVerticalAlignment::VAlign_Center )
                                           [
                                               SNew(SImage)
                                               .ToolTipText( FText::FromString(LAYER_TRANSFORM))
                                               .Image( FArianeEditorStyle::Get().GetBrush("ArianeEditor.ToolsTab.Transform16") )
                                           ]
                                       ]
                                       + SHeaderRow::Column( LAYER_VISIBLE )
                                       .DefaultLabel(FText::FromString(LAYER_VISIBLE))
                                       .HAlignHeader( HAlign_Right )
                                       .FixedWidth( 24.0f )
                                       [
                                           SNew(SHorizontalBox)
                                           + SHorizontalBox::Slot()
                                           .AutoWidth()
                                           .HAlign( EHorizontalAlignment::HAlign_Center )
                                           .VAlign( EVerticalAlignment::VAlign_Center )
                                           [
                                               SNew(SImage)
                                               .ToolTipText( FText::FromString(LAYER_VISIBLE))
                                               .Image( FArianeEditorStyle::Get().GetBrush("ArianeEditor.LayerStack.Visible16") )
                                           ]
                                       ]
                                       + SHeaderRow::Column( LAYER_LOCKED )
                                       .DefaultLabel(FText::FromString(LAYER_LOCKED))
                                       .HAlignHeader( HAlign_Right )
                                       .FixedWidth( 24.0f )
                                       [
                                           SNew(SHorizontalBox)
                                           + SHorizontalBox::Slot()
                                           .AutoWidth()
                                           .HAlign( EHorizontalAlignment::HAlign_Center )
                                           .VAlign( EVerticalAlignment::VAlign_Center )
                                           [
                                               SNew(SImage)
                                               .ToolTipText( FText::FromString(LAYER_LOCKED))
                                               .Image( FArianeEditorStyle::Get().GetBrush("ArianeEditor.LayerStack.Locked16") )
                                           ]
                                       ];
    Editor = InEditor;
    //Painting3DComponent.Assign(*this, InArgs._Painting3DComponent);

    STreeView<TSharedPtr<FArianeEditorLayerRowItem>>::Construct(
        STreeView<TSharedPtr<FArianeEditorLayerRowItem>>::FArguments()
        // for some reason, SetTreeItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .TreeItemsSource(&ItemsSource)
        .OnGenerateRow( this, &SArianeEditorLayerStack::OnGenerateRow )
        .OnGetChildren( this, &SArianeEditorLayerStack::OnGetChildren )
        .OnExpansionChanged( this, &SArianeEditorLayerStack::OnExpansionChanged )
        .OnSelectionChanged( this, &SArianeEditorLayerStack::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SArianeEditorLayerStack::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );

    Editor->OnPre3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStack::OnPre3DPaintingComponentSelectionChanged );
    Editor->OnPost3DPaintingComponentSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStack::OnPost3DPaintingComponentSelectionChanged );
}

void
SArianeEditorLayerStack::UnbindDelegates()
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        UArianeLayerStack* LayerStack = CurrentPainting3DComponent->GetLayerStack();

        //Painting3DComponent->OnPreLayerStackChangedDelegate().RemoveAll( this );
        LayerStack->OnPostHierarchyChangedDelegate().RemoveAll( this );

        //Painting3DComponent->OnPreCurrentLayerChangedDelegate().RemoveAll( this );
        LayerStack->OnPostSelectionChangedDelegate().RemoveAll( this );
    }
}

void
SArianeEditorLayerStack::BindDelegates()
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        UArianeLayerStack* LayerStack = CurrentPainting3DComponent->GetLayerStack();

        //Painting3DComponent->OnPreLayerStackChangedDelegate().AddSP( this, &SArianeEditorLayerStack::OnPreLayerStackChanged );
        LayerStack->OnPostHierarchyChangedDelegate().AddSP( this, &SArianeEditorLayerStack::OnPostLayerStackHierarchyChanged );

        //Painting3DComponent->OnPreCurrentLayerChangedDelegate().RemoveAll( this );
        LayerStack->OnPostSelectionChangedDelegate().AddSP( this, &SArianeEditorLayerStack::OnPostLayerStackSelectionChanged );
    }
}

void
SArianeEditorLayerStack::OnPre3DPaintingComponentSelectionChanged()
{
    UnbindDelegates();
}

void
SArianeEditorLayerStack::OnPost3DPaintingComponentSelectionChanged()
{
    BindDelegates();

    Update();
}

void
SArianeEditorLayerStack::OnPostLayerStackHierarchyChanged()
{
    Update();
}

void
SArianeEditorLayerStack::OnPostLayerStackSelectionChanged()
{
    Update();
}

void
SArianeEditorLayerStack::RenameSelectedItem()
{
    TArray<TSharedPtr<FArianeEditorLayerRowItem>> selectedItems = GetSelectedItems();

    // no need to create an undo record or do anything if the selection is empty
    if( selectedItems.Num() )
    {
        TSharedPtr<ITableRow> tableRow = WidgetFromItem( selectedItems[0] );
        TSharedPtr<SArianeEditorLayerRow> itemWidget = StaticCastSharedPtr<SArianeEditorLayerRow>(tableRow);

        itemWidget->Rename();
    }
}

void
SArianeEditorLayerStack::DeleteSelectedItem()
{
    UArianePainting3DComponent* CurrentPainting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( CurrentPainting3DComponent )
    {
        UArianeLayerStack* LayerStack = CurrentPainting3DComponent->GetLayerStack();
        TArray<UArianeLayer*> SelectedTrees;
        TArray<UArianeLayer*> ParentFolders;
        uint32 SelectedElderLayerCount = 0;

        LayerStack->GetSelectedTrees( SelectedTrees );

        GEditor->BeginTransaction(LOCTEXT("ariane-layer-stack.delete-layer","Delete Layer"));

        for( UArianeLayer* Layer : SelectedTrees )
        {
            ParentFolders.AddUnique( Layer->GetParentFolder() );
        }

        for( UArianeLayer* Layer : SelectedTrees )
        {
            Layer->Modify();
        }

        for( UArianeLayer* ParentFolder : ParentFolders )
        {
            ParentFolder->Modify();
        }

        for( UArianeLayer* Layer : SelectedTrees )
        {
            Layer->GetParentFolder()->RemoveChildLayer( Layer );
        }

        LayerStack->Modify();
        LayerStack->ClearLayerSelection( true );

        GEditor->EndTransaction();


        Update();
    }
}

TSharedPtr<SWidget>
SArianeEditorLayerStack::OnContextMenuOpening()
{
    return SArianeEditorLayerStackContextMenu::CreateWidget( this );
}

FReply
SArianeEditorLayerStack::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (CommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<TSharedPtr<FArianeEditorLayerRowItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

TSharedPtr<FArianeEditorLayerRowItem>
SArianeEditorLayerStack::GetRootItem()
{
    return RootItem;
}

FArianeEditor*
SArianeEditorLayerStack::GetEditor() const
{
    return Editor;
}

void
SArianeEditorLayerStack::BuildTree( const TSharedPtr<FArianeEditorLayerRowItem> Item )
{
    UArianeLayer* ItemLayer = Item.Get()->GetLayer();

    Item.Get()->Children.Empty();

    if( Cast<UArianeLayerFolder>(ItemLayer) )
    {
        UArianeLayerFolder* ItemLayerFolder = Cast<UArianeLayerFolder>(ItemLayer);
        const TArray<UArianeLayer*>& ChildLayers = ItemLayerFolder->GetChildLayers();

        for( UArianeLayer* ChildLayer : ChildLayers )
        {
            TSharedPtr<FArianeEditorLayerRowItem> ChildItem = MakeShareable( new FArianeEditorLayerRowItem( ChildLayer, true ) );

            //iItem.Get()->mChildren.Add( childItem );
            // reverse order in order to get the most forward objet on top of the hierarchy
            Item.Get()->Children.Insert( ChildItem, 0 );

            BuildTree( ChildItem );
        }
    }
}

void
SArianeEditorLayerStack::ExpandTree( const TSharedPtr<FArianeEditorLayerRowItem> Item )
{
    UArianeLayer* ItemLayer = Item.Get()->GetLayer();

    if( ItemLayer->GetClass() == UArianeLayerFolder::StaticClass() )
    {
        UArianeLayerFolder* ItemLayerFolder = Cast<UArianeLayerFolder>(ItemLayer);

        if( ItemLayerFolder->IsExpanded() )
        {
            SetItemExpansion( Item, true );

            for( int i = 0; i < Item.Get()->Children.Num(); i++ )
            {
                ExpandTree( Item.Get()->Children[i] );
            }
        }
    }
    else
    {
        SetItemExpansion( Item, false );
    }

    if( ItemLayer->IsSelected() )
    {
        SelectedItems.Add( Item );
    }
}

void
SArianeEditorLayerStack::Update()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    ItemsSource.Empty();
    SelectedItems.Empty();
    RequestTreeRefresh();

    if( Painting3DComponent )
    {
        RootItem = MakeShareable(new FArianeEditorLayerRowItem( Painting3DComponent->GetLayerStack()->GetRootFolder(), true ) );

        BuildTree( RootItem );

        // We hide the root item by showing only its child items
        for( TSharedPtr<FArianeEditorLayerRowItem> ChildItem : RootItem->GetChildren() )
        {
            ItemsSource.Add( ChildItem );

            // Expand items if need
            ExpandTree( ChildItem );
        }
    }
}

void
SArianeEditorLayerStack::OnGetChildren( TSharedPtr<FArianeEditorLayerRowItem> Parent
                                      , TArray<TSharedPtr<FArianeEditorLayerRowItem>>& OutChildren) const
{
    OutChildren = Parent.Get()->Children;
}

TSharedRef<ITableRow>
SArianeEditorLayerStack::OnGenerateRow( TSharedPtr<FArianeEditorLayerRowItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    UArianeLayer* Layer = iItem->GetLayer();

    return SNew( SArianeEditorLayerRow, iOwnerTable, iItem );
}

void
SArianeEditorLayerStack::OnExpansionChanged( TSharedPtr<FArianeEditorLayerRowItem> Item, bool bExpanded )
{
    UArianeLayerFolder* ExpandedLayerFolder = Cast<UArianeLayerFolder>(Item.Get()->GetLayer());

    if( ExpandedLayerFolder )
    {
        ExpandedLayerFolder->SetExpanded( bExpanded );
    }

    // Reselect
    //SelectedItems.Empty();
    //SelectTree( mRootItem );
}

void
SArianeEditorLayerStack::ModifyLayerStackAndSelectedLayers( UArianeLayerStack* LayerStack
                                                          , const TArray<UArianeLayer*>& OldSelectedLayers
                                                          , const TArray<UArianeLayer*>& NewSelectedLayers )
{
    for( UArianeLayer* Layer : OldSelectedLayers )
    {
        Layer->Modify();
    }

    for( UArianeLayer* Layer : NewSelectedLayers )
    {
        Layer->Modify();
    }

    LayerStack->Modify();
}

void
SArianeEditorLayerStack::OnSelectionChanged( TSharedPtr<FArianeEditorLayerRowItem> iItem
                                           , ESelectInfo::Type SelectInfo )
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    GEditor->BeginTransaction(LOCTEXT("ariane-editor-layer-stack.selection-changed","Layer Selection Changed"));

    if( Painting3DComponent )
    {
        // mRootItem can be null if the tree is empty
        if( RootItem )
        {
            UArianeLayerFolder* RootLayerFolder = Cast<UArianeLayerFolder>(RootItem.Get()->GetLayer());

            // only if the action is interactive (user action via GUI)
            //if ( SelectInfo != ESelectInfo::Type::Direct )
            {
                TArray<TSharedPtr<FArianeEditorLayerRowItem>> SelectedLayerItems = GetSelectedItems();
                UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();
                //TArray<UArianeLayer*> OldSelectedLayers = LayerStack->GetSelectedLayers();
                TArray<UArianeLayer*> NewSelectedLayers;

                for( TSharedPtr<FArianeEditorLayerRowItem> SelectedLayerItem : SelectedLayerItems )
                {
                    UArianeLayer* SelectedLayer = SelectedLayerItem.Get()->GetLayer();

                    NewSelectedLayers.Add( SelectedLayer );
                }

                // Prepare transatction snapshots for all impacted UObjects
                //ModifyLayerStackAndSelectedLayers( Painting3DComponent->GetLayerStack()
                //                                 , OldSelectedLayers
                //                                 , NewSelectedLayers );

                LayerStack->Modify();

                UnbindDelegates();
                Painting3DComponent->GetLayerStack()->SelectLayers( NewSelectedLayers, true, true );
                BindDelegates();
            }
        }
    }

    GEditor->EndTransaction();
}

void
SArianeEditorLayerStack::SelectAllLayers()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        LayerStack->SelectAllLayers();
    }
}

void
SArianeEditorLayerStack::RemoveSelectedLayers()
{
    UArianePainting3DComponent* Painting3DComponent = Editor->GetCurrentPainting3DComponent();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        GEditor->BeginTransaction(LOCTEXT("ariane-layer-stack.remove-selected-layers","Remove Selected Layers"));

        LayerStack->RemoveSelectedLayers();

        GEditor->EndTransaction();
    }
}

void
SArianeEditorLayerStack::CutSelectedLayers()
{
    SArianeEditorLayerStack::CopySelectedLayers();
    SArianeEditorLayerStack::RemoveSelectedLayers();
}

void
SArianeEditorLayerStack::CopySelectedLayers()
{
    Editor->CopySelectedLayers();
}

void
SArianeEditorLayerStack::PasteLayers()
{
    Editor->PasteLayers();
}

void
SArianeEditorLayerStack::MapActionsToCommandList()
{
    CommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerStack::SelectAllLayers )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerStack::RemoveSelectedLayers )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerStack::CutSelectedLayers )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerStack::CopySelectedLayers )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerStack::PasteLayers )
    );

    CommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerStack::RenameSelectedItem )
    );
}


/*
void
SArianeEditorLayerStack::OnPainting3DComponentChanged()
{
    Update();
}
*/

#undef LOCTEXT_NAMESPACE
