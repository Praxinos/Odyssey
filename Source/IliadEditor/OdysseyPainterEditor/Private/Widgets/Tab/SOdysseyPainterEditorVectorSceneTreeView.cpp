// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeView.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyStyle.h"
#include "Undo/OdysseyVectorUndo.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewRow.h"
#include "Widgets/Tab/SOdysseyPainterEditorVectorSceneTreeViewContextMenu.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyPainterEditorVectorSceneTreeView)
void
SOdysseyPainterEditorVectorSceneTreeView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mScene, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPainterEditorVectorSceneTreeView&>(Widget).OnSceneChanged();
        }
    ));
}

SOdysseyPainterEditorVectorSceneTreeView::~SOdysseyPainterEditorVectorSceneTreeView()
{
    UnbindLayerDelegates();

    // Keep commented-out until we convert mEditor to a sharedptr
    //mEditor->OnSourceChanged().RemoveAll( this );

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

SOdysseyPainterEditorVectorSceneTreeView::SOdysseyPainterEditorVectorSceneTreeView()
    : mScene(*this, nullptr)
    , mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();

    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SOdysseyPainterEditorVectorSceneTreeView::OnCurrentLayerChanged);
}

void
SOdysseyPainterEditorVectorSceneTreeView::Construct( const FArguments& InArgs )
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
    mEditor = InArgs._Editor;
    mScene.Assign(*this, InArgs._Scene);

    STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::Construct(
        STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::FArguments()
        // for some reason, SetTreeItemsSource does not work, so we have to use an array that we
        // call mItemsSource and that we will updates with the desired items
        .TreeItemsSource(&mItemsSource)
        .OnGenerateRow( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGenerateRow )
        .OnGetChildren( this, &SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SOdysseyPainterEditorVectorSceneTreeView::OnExpansionChanged )
        .OnSelectionChanged( this, &SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged )
        //.OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyPainterEditorVectorSceneTreeView::OnContextMenuOpening )
        //.SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );

    mEditor->OnSourceChanged().AddSP( this, &SOdysseyPainterEditorVectorSceneTreeView::OnSourceChanged );
}

void
SOdysseyPainterEditorVectorSceneTreeView::RenameSelectedItem()
{
    TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();

    // no need to create an undo record or do anything if the selection is empty
    if( selectedItems.Num() )
    {
        TSharedPtr<ITableRow> tableRow = WidgetFromItem( selectedItems[0] );
        TSharedPtr<SOdysseyPainterEditorVectorSceneTreeViewRow> itemWidget = StaticCastSharedPtr<SOdysseyPainterEditorVectorSceneTreeViewRow>(tableRow);

        itemWidget->Rename();
    }
}

TSharedPtr<SWidget>
SOdysseyPainterEditorVectorSceneTreeView::OnContextMenuOpening()
{
    return SOdysseyPainterEditorVectorSceneTreeViewContextMenu::CreateWidget( this );
}

FReply
SOdysseyPainterEditorVectorSceneTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
    if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<TSharedPtr<FVectorSceneTreeViewItem>>::OnKeyDown( iGeometry, iKeyEvent );
}

TSharedPtr<FVectorSceneTreeViewItem>
SOdysseyPainterEditorVectorSceneTreeView::GetRootItem()
{
    return mRootItem;
}

FOdysseyPainterEditor*
SOdysseyPainterEditorVectorSceneTreeView::GetEditor() const
{
    return mEditor;
}

void
SOdysseyPainterEditorVectorSceneTreeView::BuildTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    std::list<FOdysseyVectorObject*>& childrenList = iItem.Get()->GetVectorObject()->GetChildrenList();

    iItem.Get()->mChildren.Empty();

    for( FOdysseyVectorObject* child : childrenList )
    {
        TSharedPtr<FVectorSceneTreeViewItem> childItem = MakeShareable(new FVectorSceneTreeViewItem(child, true ));

        //iItem.Get()->mChildren.Add( childItem );
        // reverse order in order to get the most forward objet on top of the hierarchy
        iItem.Get()->mChildren.Insert( childItem, 0 );

        BuildTree( childItem );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::ExpandTree( const TSharedPtr<FVectorSceneTreeViewItem> iItem )
{
    FOdysseyVectorObject* itemObject = iItem.Get()->GetVectorObject();

    if( itemObject->IsExpanded() )
    {
        SetItemExpansion( iItem, true );

        for( int i = 0; i < iItem.Get()->mChildren.Num(); i++ )
        {
            ExpandTree( iItem.Get()->mChildren[i] );
        }
    }

    if( itemObject->IsSelected() )
    {
        SetSelection( iItem );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::Update()
{
    uint64 hudFlags = mEditor->GetVectorHUDFlags();

    mItemsSource.Empty();
    SelectedItems.Empty();
    RequestTreeRefresh();

    FOdysseyVectorGroupPaint* scene = mScene.Get();
    if (!scene)
        return;

    //if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        int32 sceneCellIndex = scene->GetCell()->GetIndex();
        FOdysseyVectorLayer* sharedEnv = scene->GetLayer();
        if (!sharedEnv)
            return;

        for( FOdysseyVectorTag* tag : sharedEnv->GetSharedTagList() )
        {
            if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                int32 sourceCellIndex = inbetweenerTag->GetSourceCellIndex();
                int32 targetCellIndex = inbetweenerTag->GetTargetCellIndex();

                if ((sceneCellIndex > sourceCellIndex) && ( sceneCellIndex < targetCellIndex ) )
                {
                    if ((sceneCellIndex > sourceCellIndex) && ( sceneCellIndex <= targetCellIndex ) )
                    {
                        FOdysseyVectorObject* owner = inbetweenerTag->GetOwner();
                        mItemsSource.Add( MakeShareable(new FVectorSceneTreeViewItem( owner, false ) ) );
                    }
                }
            }
        }
    }

    mRootItem = MakeShareable(new FVectorSceneTreeViewItem(scene, true ));

    BuildTree( mRootItem );

    mItemsSource.Add( mRootItem );

    // Expand items if need
    ExpandTree( mRootItem );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnGetChildren( TSharedPtr<FVectorSceneTreeViewItem> iParent
                                                       , TArray<TSharedPtr<FVectorSceneTreeViewItem>>& oChildren) const
{
    oChildren = iParent.Get()->mChildren;
}

TSharedRef<ITableRow>
SOdysseyPainterEditorVectorSceneTreeView::OnGenerateRow( TSharedPtr<FVectorSceneTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    return SNew( SOdysseyPainterEditorVectorSceneTreeViewRow, iOwnerTable, iItem );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnExpansionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem, bool mExpanded )
{
    FOdysseyVectorObject* expandedObject = iItem.Get()->GetVectorObject();

    expandedObject->SetExpanded( mExpanded );

    // Reselect
    //SelectedItems.Empty();
    //SelectTree( mRootItem );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnSelectionChanged( TSharedPtr<FVectorSceneTreeViewItem> iItem
                                                            , ESelectInfo::Type SelectInfo )
{
    // Unregister this widget's updates when the vector scene is updated. We don't want this widget to be
    // rebuilt while it's processing stuff
    UnbindLayerDelegates();

    // mRootItem can be null if the tree is empty
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        // only if the action is interactive (user action via GUI)
        if ( SelectInfo != ESelectInfo::Type::Direct )
        {
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( scene->GetLayer()
                                                                             , scene->GetCell() );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();

            scene->GetCell()->ClearObjectSelection();

            for( TSharedPtr<FVectorSceneTreeViewItem> selectedItem : selectedItems )
            {
                FOdysseyVectorObject* selectedObject = selectedItem.Get()->GetVectorObject();

                if( selectedObject->IsSelected() == false )
                {
                    selectedObject->GetCell()->SelectObject( selectedObject );
                }
            }
        }

        scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );
    }

    // Re-register this widget after we are done
    BindLayerDelegates();
}

void
SOdysseyPainterEditorVectorSceneTreeView::SelectAll()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::SelectAllObjects( mEditor, scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::DeleteObjects()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::DeleteObjects( mEditor, scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::CutObjects()
{
    SOdysseyPainterEditorVectorSceneTreeView::CopyObjects();
    SOdysseyPainterEditorVectorSceneTreeView::DeleteObjects();
}

void
SOdysseyPainterEditorVectorSceneTreeView::CopyObjects()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::CopyObjects( scene  );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::PasteObjects()
{
    if( mRootItem )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        FOdysseyPainterEditor::PasteObjects( mEditor, scene );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::SelectAll )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::DeleteObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::CutObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::CopyObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::PasteObjects )
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw( this, &SOdysseyPainterEditorVectorSceneTreeView::RenameSelectedItem )
    );
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnVectorLayerNotify( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags
                                                             , uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_INTERACTIVE ) == 0 )
    {
        FOdysseyVectorGroupPaint* currentScene = mScene.Get();

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
SOdysseyPainterEditorVectorSceneTreeView::ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags )
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
SOdysseyPainterEditorVectorSceneTreeView::OnSceneChanged()
{
    // Parse by simulating a scene invalidation
    ParseVectorNotifications( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::DEFAULT) );
}

void
SOdysseyPainterEditorVectorSceneTreeView::UnbindLayerDelegates()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnUpdateDelegate().RemoveAll( this );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::UpdateCurrentLayer( UOdysseyLayerStack* iLayerStack )
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
SOdysseyPainterEditorVectorSceneTreeView::BindLayerDelegates()
{
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnUpdateDelegate().AddSP( this, &SOdysseyPainterEditorVectorSceneTreeView::OnVectorLayerNotify );
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnSourceChanged()
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
SOdysseyPainterEditorVectorSceneTreeView::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UnbindLayerDelegates();
    UpdateCurrentLayer( iLayerStack );
    BindLayerDelegates();
}

#undef LOCTEXT_NAMESPACE
