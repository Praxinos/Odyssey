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
    if( mVectorLayer.IsValid() )
    {
        mVectorLayer->OnNotifyDelegate().RemoveAll( this );
    }

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
                                      .Visibility( EVisibility::Collapsed )
                                      + SHeaderRow::Column("Visible")
                                      .FixedWidth( 24.0f )
                                      [
                                          SNew(SBorder)
                                          .Padding(0,0)
                                          .BorderBackgroundColor( FSlateColor( FLinearColor( 0, 0, 0, 0 ) ) )
                                          .Content()
                                          [
                                              SNew(SImage)
                                              .Image( FOdysseyStyle::GetBrush("Level.VisibleIcon16x") )
                                          ]
                                      ]
                                      + SHeaderRow::Column("HUD Color")
                                      .FixedWidth( 3.0f )
                                      [
                                          SNullWidget::NullWidget
                                      ]
                                      + SHeaderRow::Column("Name");
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
}

bool
SOdysseyPainterEditorVectorSceneTreeView::Private_IsItemSelected( const TSharedPtr<FVectorSceneTreeViewItem>& iItem )  const
{
    return iItem.Get()->GetVectorObject()->IsSelected();
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
    if( mRootItem && ( SelectInfo != ESelectInfo::Type::Direct ) )
    {
        FOdysseyVectorGroupPaint* scene = static_cast<FOdysseyVectorGroupPaint*>(mRootItem.Get()->GetVectorObject());

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene-tree-view.transaction.selection-changed","Selection Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( scene->GetLayer()
                                                                         , scene->GetCell()
                                                                         , 0 );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        scene->GetCell()->ClearObjectSelection();

        // iTtem is null when selection is empty
        if( iItem )
        {
            TArray<TSharedPtr<FVectorSceneTreeViewItem>> selectedItems = GetSelectedItems();

            for( int i = 0; i < selectedItems.Num(); i++ )
            {
                FOdysseyVectorObject* selectedObject = selectedItems[i].Get()->GetVectorObject();

                scene->GetCell()->SelectObject( selectedObject );
            }
        }

        scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );
    }
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
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnVectorLayerNotify( FOdysseyVectorLayer* iLayer
                                                             , const FOdysseyVectorObjectInvalidationFlags& iSignalFlags )
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

void
SOdysseyPainterEditorVectorSceneTreeView::ParseVectorNotifications( const FOdysseyVectorObjectInvalidationFlags& iSignalFlags )
{
    if( iSignalFlags.bits[FOdysseyVectorObjectInvalidationFlags::HIERARCHY]
     || iSignalFlags.bits[FOdysseyVectorObjectInvalidationFlags::OBJECT_SELECTION]
     || iSignalFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_LIST]
     || iSignalFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_HIERARCHY]
     || iSignalFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_LIST]
     || iSignalFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_OBJECT_SELECTION] )
    {
        Update();
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnSceneChanged()
{
    //ParseVectorNotifications( FOdysseyVectorEngine::NOTIFY_ALL );
}

void
SOdysseyPainterEditorVectorSceneTreeView::BindLayerDelegates( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyAnimationLayerImageVector* animationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->GetCurrentLayer());
    UOdysseyTextureLayerImageVector* textureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->GetCurrentLayer());

    if( mOldVectorLayer.IsValid() )
    {
        mOldVectorLayer->OnNotifyDelegate().RemoveAll( this );
    }

    if( animationVectorLayer )
    {
        mOldVectorLayer = mVectorLayer;
        mVectorLayer = animationVectorLayer->GetVectorLayer();

        mVectorLayer->OnNotifyDelegate().AddSP( this, &SOdysseyPainterEditorVectorSceneTreeView::OnVectorLayerNotify );
    }

    if( textureVectorLayer )
    {
        mOldVectorLayer = mVectorLayer;
        mVectorLayer = textureVectorLayer->GetVectorLayer();

        mVectorLayer->OnNotifyDelegate().AddSP( this, &SOdysseyPainterEditorVectorSceneTreeView::OnVectorLayerNotify );
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
            BindLayerDelegates( layerStack );
        }
    }
}

void
SOdysseyPainterEditorVectorSceneTreeView::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    BindLayerDelegates( iLayerStack );
}

#undef LOCTEXT_NAMESPACE
