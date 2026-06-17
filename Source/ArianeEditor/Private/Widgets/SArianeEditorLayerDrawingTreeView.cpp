// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#ifdef unused

#include "Widgets/SArianeEditorLayerDrawingTreeView.h"

#include "Editor.h"
#include "Framework/Commands/GenericCommands.h"

#include "ArianeEditor.h"
#include "ArianeGroup.h"
#include "UArianeLayer.h"
#include "ArianeStyle.h"
#include "Widgets/Tab/SArianeEditorLayerDrawingTreeViewRow.h"
#include "Widgets/Tab/SArianeEditorLayerDrawingTreeViewContextMenu.h"
#include "ArianeLayerStack.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

SLATE_IMPLEMENT_WIDGET(SArianeEditorLayerDrawingTreeView)
void
SArianeEditorLayerDrawingTreeView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mScene, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SArianeEditorLayerDrawingTreeView&>(Widget).OnSceneChanged();
        }
    ));
}

SArianeEditorLayerDrawingTreeView::~SArianeEditorLayerDrawingTreeView()
{
    UnbindLayerDelegates();

    // Keep commented-out until we convert mEditor to a sharedptr
    //mEditor->OnSourceChanged().RemoveAll( this );

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

SArianeEditorLayerDrawingTreeView::SArianeEditorLayerDrawingTreeView()
    : mScene(*this, nullptr)
    , mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();

    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SArianeEditorLayerDrawingTreeView::OnCurrentLayerChanged);
}

void
SArianeEditorLayerDrawingTreeView::Construct( const FArguments& InArgs )
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
/*
                                       + SHeaderRow::Column( VSTV_OBJECT_TRANSFORMED )
                                       .DefaultLabel(FText::FromString(VSTV_OBJECT_TRANSFORMED))
                                       .FixedWidth( 24.0f )
                                       [
                                           SNew(SHorizontalBox)
                                           + SHorizontalBox::Slot()
                                           .AutoWidth()
                                           .HAlign( EHorizontalAlignment::HAlign_Center )
                                           .VAlign( EVerticalAlignment::VAlign_Center )
                                           [
                                               SNew(SImage)
                                               .ToolTipText( FText::FromString(VSTV_OBJECT_TRANSFORMED))
                                               .Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform16") )
                                           ]
                                       ]
*/
                                       + SHeaderRow::Column(VSTV_OBJECT_TAGS)
                                       .DefaultLabel(FText::FromString(VSTV_OBJECT_TAGS));
    Editor = InArgs._Editor;
    Scene.Assign(*this, InArgs._Scene);

    STreeView<TSharedPtr<FLayerDrawingTreeViewItem>>::Construct(
        STreeView<TSharedPtr<FLayerDrawingTreeViewItem>>::FArguments()
        // for some reason, SetTreeItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .TreeItemsSource(&ItemsSource)
        .OnGenerateRow( this, &SArianeEditorLayerDrawingTreeView::OnGenerateRow )
        .OnGetChildren( this, &SArianeEditorLayerDrawingTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SArianeEditorLayerDrawingTreeView::OnExpansionChanged )
        .OnSelectionChanged( this, &SArianeEditorLayerDrawingTreeView::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SArianeEditorLayerDrawingTreeView::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );

    Editor->OnSourceChanged().AddSP( this, &SArianeEditorLayerDrawingTreeView::OnSourceChanged );
}

void
SArianeEditorLayerDrawingTreeView::RenameSelectedItem()
{
    TArray<TSharedPtr<FLayerDrawingTreeViewItem>> selectedItems = GetSelectedItems();

    // no need to create an undo record or do anything if the selection is empty
    if( selectedItems.Num() )
    {
        TSharedPtr<ITableRow> tableRow = WidgetFromItem( selectedItems[0] );
        TSharedPtr<SArianeEditorLayerDrawingTreeViewRow> itemWidget = StaticCastSharedPtr<SArianeEditorLayerDrawingTreeViewRow>(tableRow);

        itemWidget->Rename();
    }
}

TSharedPtr<SWidget>
SArianeEditorLayerDrawingTreeView::OnContextMenuOpening()
{
    return SArianeEditorLayerDrawingTreeViewContextMenu::CreateWidget( this );
}

FReply
SArianeEditorLayerDrawingTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (CommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<TSharedPtr<FLayerDrawingTreeViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

TSharedPtr<FLayerDrawingTreeViewItem>
SArianeEditorLayerDrawingTreeView::GetRootItem()
{
    return RootItem;
}

FArianeEditor*
SArianeEditorLayerDrawingTreeView::GetEditor() const
{
    return Editor;
}

void
SArianeEditorLayerDrawingTreeView::BuildTree( const TSharedPtr<FLayerDrawingTreeViewItem> InItem )
{
    TArray<FArianeObjectID>& Children = InItem.Get()->GetObject()->GetChildren();

    InItem.Get()->Children.Empty();

    for( FArianeObject* Child : Children )
    {
        TSharedPtr<FLayerDrawingTreeViewItem> ChildItem = MakeShareable(new FLayerDrawingTreeViewItem(Child, true ));

        //InItem.Get()->mChildren.Add( childItem );
        // reverse order in order to get the most forward objet on top of the hierarchy
        InItem.Get()->Children.Insert( ChildItem, 0 );

        BuildTree( ChildItem );
    }
}

void
SArianeEditorLayerDrawingTreeView::ExpandTree( const TSharedPtr<FLayerDrawingTreeViewItem> InItem )
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

void
SArianeEditorLayerDrawingTreeView::Update()
{
    ItemsSource.Empty();
    SelectedItems.Empty();
    RequestTreeRefresh();

    FArianeObject* RootObject = Root.Get();

    if (!RootObject)
        return;

    RootItem = MakeShareable(new FLayerDrawingTreeViewItem(Scene, true ));

    BuildTree( RootItem );

    ItemsSource.Add( RootItem );

    // Expand items if need
    ExpandTree( RootItem );
}

void
SArianeEditorLayerDrawingTreeView::OnGetChildren( TSharedPtr<FLayerDrawingTreeViewItem> Parent
                                                , TArray<TSharedPtr<FLayerDrawingTreeViewItem>>& OutChildren) const
{
    OutChildren = Parent.Get()->Children;
}

TSharedRef<ITableRow>
SArianeEditorLayerDrawingTreeView::OnGenerateRow( TSharedPtr<FLayerDrawingTreeViewItem> InItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SArianeEditorLayerDrawingTreeViewRow, iOwnerTable, InItem );
}

void
SArianeEditorLayerDrawingTreeView::OnExpansionChanged( TSharedPtr<FLayerDrawingTreeViewItem> InItem, bool mExpanded )
{
    FArianeObject* ExpandedObject = InItem.Get()->GetObject();

    ExpandedObject->SetExpanded( mExpanded );

    // Reselect
    //SelectedItems.Empty();
    //SelectTree( mRootItem );
}

void
SArianeEditorLayerDrawingTreeView::OnSelectionChanged( TSharedPtr<FLayerDrawingTreeViewItem> InItem
                                                     , ESelectInfo::Type SelectInfo )
{
    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    UnbindLayerDelegates();

    // mRootItem can be null if the tree is empty
    if( RootItem )
    {
        FArianeGroup* scene = static_cast<FArianeGroup*>(mRootItem.Get()->GetObject());

        // only if the action is interactive (user action via GUI)
        if ( SelectInfo != ESelectInfo::Type::Direct )
        {
            // needed for valid GUndo pointer
            ////GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));

            TArray<TSharedPtr<FLayerDrawingTreeViewItem>> selectedItems = GetSelectedItems();

            scene->GetCell()->ClearObjectSelection();

            for( TSharedPtr<FLayerDrawingTreeViewItem> selectedItem : selectedItems )
            {
                FArianeObject* selectedObject = selectedItem.Get()->GetObject();

                if( selectedObject->IsSelected() == false )
                {
                    selectedObject->GetCell()->SelectObject( selectedObject );
                }
            }
        }

        scene->GetLayer()->Update( FArianeObject::UPDATE_PAINTGROUPS );
        scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );
    }

    // Re-register this widget after we are done
    BindLayerDelegates();
}

void
SArianeEditorLayerDrawingTreeView::SelectAll()
{
    if( RootItem )
    {
        FArianeGroup* scene = static_cast<FArianeGroup*>(mRootItem.Get()->GetObject());

        FArianeEditor::SelectAllObjects( mEditor, scene );
    }
}

void
SArianeEditorLayerDrawingTreeView::DeleteObjects()
{
    if( mRootItem )
    {
        FArianeGroup* scene = static_cast<FArianeGroup*>(mRootItem.Get()->GetObject());

        FArianeEditor::DeleteObjects( mEditor, scene );
    }
}

void
SArianeEditorLayerDrawingTreeView::CutObjects()
{
    SArianeEditorLayerDrawingTreeView::CopyObjects();
    SArianeEditorLayerDrawingTreeView::DeleteObjects();
}

void
SArianeEditorLayerDrawingTreeView::CopyObjects()
{
    if( mRootItem )
    {
        FArianeGroup* scene = static_cast<FArianeGroup*>(mRootItem.Get()->GetObject());

        FArianeEditor::CopyObjects( scene  );
    }
}

void
SArianeEditorLayerDrawingTreeView::PasteObjects()
{
    if( mRootItem )
    {
        FArianeGroup* scene = static_cast<FArianeGroup*>(mRootItem.Get()->GetObject());

        FArianeEditor::PasteObjects( mEditor, scene );
    }
}

void
SArianeEditorLayerDrawingTreeView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerDrawingTreeView::SelectAll )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerDrawingTreeView::DeleteObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerDrawingTreeView::CutObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerDrawingTreeView::CopyObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerDrawingTreeView::PasteObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw( this, &SArianeEditorLayerDrawingTreeView::RenameSelectedItem )
    );
}

void
SArianeEditorLayerDrawingTreeView::OnVectorLayerNotify( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags
                                                             , uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FArianeObject::UPDATE_INTERACTIVE ) == 0 )
    {
        FArianeGroup* currentScene = mScene.Get();

        // update the cached Value by calling the getter.
        mScene.UpdateNow( *this );

        // if the attributes value changes, it will automatically trigger Update().
        // Otherwise, like here, we force the update of the tree
        if( currentScene == mScene.Get() )
        {
            ParseVectorNotifications( iSignalFlags );
        }
    }
}

void
SArianeEditorLayerDrawingTreeView::ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags )
{
    if( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::DEFAULT]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::HIERARCHY]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::OBJECT_SELECTION]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::PARAM]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_LIST]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_DEFAULT]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_HIERARCHY]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_OBJECT_SELECTION]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_PARAM]
     || iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_LIST] )
    {
        Update();
    }
}

void
SArianeEditorLayerDrawingTreeView::OnSceneChanged()
{
    // Parse by simulating a scene invalidation
    ParseVectorNotifications( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::DEFAULT) );
}

void
SArianeEditorLayerDrawingTreeView::UnbindLayerDelegates()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnUpdateDelegate().RemoveAll( this );
    }
}

void
SArianeEditorLayerDrawingTreeView::UpdateCurrentLayer( UOdysseyLayerStack* iLayerStack )
{
    //warning. The CurrentLayerStack delegate is STATIC. The iLayerStack received might not belong to this editor
    if( mEditor->LayerStack() == iLayerStack )
    {
        UOdysseyAnimationLayerImageVector* animationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->GetCurrentLayer());
        UOdysseyTextureLayerImageVector* textureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->GetCurrentLayer());

        if( animationVectorLayer )
        {
            mVectorLayer = animationVectorLayer->GetVectorLayer();
        }

        if( textureVectorLayer )
        {
            mVectorLayer = textureVectorLayer->GetVectorLayer();
        }
    }
}

void
SArianeEditorLayerDrawingTreeView::BindLayerDelegates()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnUpdateDelegate().AddSP( this, &SArianeEditorLayerDrawingTreeView::OnVectorLayerNotify );
    }
}

void
SArianeEditorLayerDrawingTreeView::OnSourceChanged()
{
    if( mEditor->GetSource() )
    {
        UOdysseyLayerStack* layerStack = mEditor->GetSource()->GetLayerStack();

        if( layerStack )
        {
            UnbindLayerDelegates();
            UpdateCurrentLayer( layerStack );
            BindLayerDelegates();
        }
    }
}

void
SArianeEditorLayerDrawingTreeView::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UnbindLayerDelegates();
    UpdateCurrentLayer( iLayerStack );
    BindLayerDelegates();
}

#undef LOCTEXT_NAMESPACE

#endif
