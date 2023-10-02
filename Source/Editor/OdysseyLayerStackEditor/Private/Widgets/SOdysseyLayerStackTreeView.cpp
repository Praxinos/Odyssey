// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "OdysseyStyleSet.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ToolMenus.h"
#include "Framework/Commands/GenericCommands.h"
#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyLayerStackFunctionLibrary.h"
#include "OdysseyLayerStackClipboard.h"

#define LOCTEXT_NAMESPACE "SOdysseyLayerStackTreeView"

static FName contextMenuName = "OdysseyLayerStackContextMenu";

/*SLATE_IMPLEMENT_WIDGET(SOdysseyLayerStackTreeView)
void
SOdysseyLayerStackTreeView::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
	SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::Layout)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyLayerStackTreeView&>(Widget).LoadLayerStack();
        }
    ));
} */

SOdysseyLayerStackTreeView::~SOdysseyLayerStackTreeView()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    UOdysseyLayerStack::OnHierarchyChanged().RemoveAll(this);
	UOdysseyLayer::OnIsExpandedChanged().RemoveAll(this);
}

SOdysseyLayerStackTreeView::SOdysseyLayerStackTreeView()
    //: mLayerStack(*this, nullptr)
    : mLayerStack(nullptr)
    , mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnCurrentLayerChanged);
    UOdysseyLayerStack::OnHierarchyChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerStackHierarchyChanged);
	UOdysseyLayer::OnIsExpandedChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerIsExpandedChanged);
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void SOdysseyLayerStackTreeView::Construct(const FArguments& InArgs)
{
    //mLayerStack.Assign(*this, InArgs._LayerStack);
    mLayerStack = InArgs._LayerStack;

    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        .SplitterHandleSize(0.f) //Fixes alignment between header row and actual rows
        + SHeaderRow::Column("IsActivated")
            .ToolTipText(LOCTEXT("OdysseyLayerIsActivatedButtonToolTip", "Toggle Layer Activation"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            //.DefaultTooltip(FText::FromName(GetColumnID()))
            //.HeaderContentPadding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
            ]
        + SHeaderRow::Column("IsLocked")
            .ToolTipText(LOCTEXT("OdysseyLayerLockedButtonToolTip", "Toggle Layer Locked State"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            //.DefaultTooltip(FText::FromName(GetColumnID()))
            //.HeaderContentPadding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Locked16"))
            ]
        + SHeaderRow::Column("IsOptionsDisplayed")
            .ToolTipText(LOCTEXT("OdysseyLayerIsOptionsDisplayedButtonToolTip", "Show/Hide inline Layer options"))
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            //.DefaultTooltip(FText::FromName(GetColumnID()))
            //.HeaderContentPadding(FMargin(20.0f, 0.0f, 20.0f, 0.0f))
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsHeader16"))
            ]
        + SHeaderRow::Column("Header")
            .DefaultLabel(LOCTEXT("", ""))
            .VAlignCell(VAlign_Top)
            .FillWidth(InArgs._HeaderFillWidth)
            .FixedWidth(InArgs._HeaderFixedWidth)
            .ManualWidth(InArgs._HeaderManualWidth)
            .FillSized(InArgs._HeaderFillSized);

    for( SHeaderRow::FColumn::FArguments columnArguments : InArgs._AdditionalColumns)
    {
        headerRow->AddColumn(columnArguments);
    }

    const TArray<UOdysseyLayer*>* rootLayers = mLayerStack ? &mLayerStack->GetRootLayers() : nullptr;

    STreeView<UOdysseyLayer*>::Construct(
        STreeView<UOdysseyLayer*>::FArguments()
        .TreeItemsSource(rootLayers)
        .OnGenerateRow( InArgs._OnGenerateRow )
        .OnGetChildren( this, &SOdysseyLayerStackTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SOdysseyLayerStackTreeView::OnExpansionChanged )
        //.OnSelectionChanged( this, &SOdysseyLayerStackTreeView::OnSelectionChanged )
        .OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyLayerStackTreeView::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
    );

    //Menus
    CreateContextMenu();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

int32
SOdysseyLayerStackTreeView::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
    int32 layerId = STreeView<UOdysseyLayer*>::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    if (mDisplayDropZone)
    {
        if ( GetRootItems().Num() == 0 )
            return layerId;

        TSharedPtr<SMultiColumnTableRow<UOdysseyLayer*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyLayer*>>(WidgetFromItem(GetRootItems().Last()));
        if (!rowWidget)
            return layerId;

        const FTableRowStyle& style = FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows");
        const FSlateBrush* DropIndicatorBrush = &style.DropIndicator_Above;

        FGeometry geometry = rowWidget->GetPaintSpaceGeometry();
        const FVector2D& widgetSize = geometry.GetLocalSize();
        FVector2D translate(0.f, widgetSize.Y);
        FVector2D parentAbsolutePosition(geometry.AbsolutePosition.X, geometry.AbsolutePosition.Y);
        geometry = FGeometry(translate, parentAbsolutePosition, widgetSize, 1.0f);

        if (Private_GetOrientation() == Orient_Vertical)
        {
            FSlateDrawElement::MakeBox
            (
                OutDrawElements,
                layerId++,
                geometry.ToPaintGeometry(),
                DropIndicatorBrush,
                ESlateDrawEffect::None,
                DropIndicatorBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
            );
        }
        else
        {
            // Reuse the drop indicator asset for horizontal, by rotating the drawn box 90 degrees.
            const FVector2D LocalSize(geometry.GetLocalSize());
            const FVector2D Pivot(LocalSize * 0.5f);
            const FVector2D RotatedLocalSize(LocalSize.Y, LocalSize.X);
            FSlateLayoutTransform RotatedTransform(Pivot - RotatedLocalSize * 0.5f);	// Make the box centered to the alloted geometry, so that it can be rotated around the center.

            FSlateDrawElement::MakeRotatedBox(
                OutDrawElements,
                layerId++,
                geometry.ToPaintGeometry(RotatedLocalSize, RotatedTransform),
                DropIndicatorBrush,
                ESlateDrawEffect::None,
                -HALF_PI,	// 90 deg CCW
                RotatedLocalSize * 0.5f,	// Relative center to the flipped
                FSlateDrawElement::RelativeToElement,
                DropIndicatorBrush->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
            );
        }
    }

	return layerId;
}

FReply
SOdysseyLayerStackTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mCommandList->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return STreeView<UOdysseyLayer*>::OnKeyDown(iGeometry, iKeyEvent);
}

FReply
SOdysseyLayerStackTreeView::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if ( !mLayerStack )
        return FReply::Unhandled();
    
    TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = DragDropEvent.GetOperationAs<FOdysseyLayerStackDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    UOdysseyLayerStack* operationLayerStack = operation->GetLayerStack();
	if ( !operationLayerStack )
		return FReply::Unhandled();

    if ( GetRootItems().Num() == 0 )
        return FReply::Unhandled();

    TSharedPtr<SMultiColumnTableRow<UOdysseyLayer*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyLayer*>>(WidgetFromItem(GetRootItems().Last()));
    if (!rowWidget)
        return FReply::Unhandled();

    bool isNotSupported = operation->GetLayers().ContainsByPredicate(
        [this](UOdysseyLayer* iLayer)
        {
            return !mLayerStack->SupportsLayerClass(iLayer->GetClass());
        }
    );

    if ( isNotSupported )
        return FReply::Unhandled();

    FGeometry geometry = rowWidget->GetTickSpaceGeometry();
    const FVector2D localPointerPos = geometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    const FVector2D& widgetSize = geometry.GetLocalSize();

    //Don't display the dropzone if we don't drag under the last line
    if (localPointerPos.Y <= widgetSize.Y)
    {
        mDisplayDropZone = false;
        return FReply::Unhandled();
    }

    mDisplayDropZone = true;

	return FReply::Handled();
}

void
SOdysseyLayerStackTreeView::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
    mDisplayDropZone = false;
}

FReply
SOdysseyLayerStackTreeView::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    mDisplayDropZone = false;

    if ( !mLayerStack )
        return FReply::Unhandled();
    
    TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = DragDropEvent.GetOperationAs<FOdysseyLayerStackDragDropOperation>();
    if (!operation)
        return FReply::Unhandled();

    UOdysseyLayerStack* operationLayerStack = operation->GetLayerStack();
	if ( !operationLayerStack )
		return FReply::Unhandled();

    TSharedPtr<SMultiColumnTableRow<UOdysseyLayer*>> rowWidget = StaticCastSharedPtr<SMultiColumnTableRow<UOdysseyLayer*>>(WidgetFromItem(GetRootItems().Last()));
    if (!rowWidget)
        return FReply::Unhandled();

    bool isNotSupported = operation->GetLayers().ContainsByPredicate(
        [this](UOdysseyLayer* iLayer)
        {
            return !mLayerStack->SupportsLayerClass(iLayer->GetClass());
        }
    );

    if ( isNotSupported )
        return FReply::Unhandled();

    FGeometry geometry = rowWidget->GetTickSpaceGeometry();
    const FVector2D localPointerPos = geometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition());
    const FVector2D& widgetSize = geometry.GetLocalSize();

    //Don't drop if we don't drag under the last line
    if (localPointerPos.Y <= widgetSize.Y)
        return FReply::Unhandled();

    //do nothing
    TArray<UOdysseyLayer*> layers = operation->GetLayers();
    if ( operationLayerStack == mLayerStack ) //droped from same layerstack, do a move of topmost dropped layers
    {
        mLayerStack->MoveLayers(layers, nullptr, mLayerStack->GetRootLayers().Num());
    }
    else
    {
        mLayerStack->CopyLayers(layers, nullptr, mLayerStack->GetRootLayers().Num());
    }
	return FReply::Handled();
}

void
SOdysseyLayerStackTreeView::ResetDropZone()
{
    mDisplayDropZone = false;
}

//PRIVATE API-----------------------------------------------------------

void
SOdysseyLayerStackTreeView::OnGetChildren(UOdysseyLayer* iParent, TArray<UOdysseyLayer*>& oChildren) const
{
    if ( !mLayerStack )
        return;
    
    oChildren = iParent->GetChildren();
}

void
SOdysseyLayerStackTreeView::RefreshRootLayersArray()
{
    /* mRootLayers.Empty();

    UOdysseyLayerStack* mLayerStack = mLayerStack.Get();
    if ( !layerstack )
        return;

    mRootLayers = mLayerStack->GetRootLayers();
    RefreshAllExpansionStates(); */
}

void
SOdysseyLayerStackTreeView::RefreshAllExpansionStates()
{
    if ( !mLayerStack )
        return;

    TArray<UOdysseyLayer*> layers = mLayerStack->GetLayers();
    for(UOdysseyLayer* layer : layers )
    {
        if(!layer )
            continue;

        SetItemExpansion(layer, layer->IsExpanded);
    }
}

void
SOdysseyLayerStackTreeView::OnLayerStackHierarchyChanged(UOdysseyLayerStack* iLayerStack)
{
    if ( !mLayerStack )
        return;

	if ( iLayerStack != mLayerStack )
		return;

    RefreshAllExpansionStates();
    RequestTreeRefresh();
}

void
SOdysseyLayerStackTreeView::SetCurrentLayerFromSelectorItem()
{
    if ( !mLayerStack )
        return;

	if ( mLayerStack->GetLayers().Num() == 0)
		return;

    if (!SelectorItem)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(mLayerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(mLayerStack->GetRootLayers()[0]));
        return;
    }

    UOdysseyLayerStack* selectorLayerStack = SelectorItem->GetLayerStack();
    if (selectorLayerStack != mLayerStack )
        return;

    if (SelectorItem == mLayerStack->CurrentLayer)
        return;
        
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(SelectorItem));
}

void
SOdysseyLayerStackTreeView::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
    if ( !mLayerStack )
    {
        STreeView< UOdysseyLayer* >::Private_SignalSelectionChanged(SelectInfo);
        return;
    }

    //Ensure selectorItem = currentLayer if currentLayer is selected
    UOdysseyLayer* currentLayer = mLayerStack->CurrentLayer.Get();
    if ( currentLayer && Private_IsItemSelected(currentLayer) )
    {
        Private_SetItemSelection(currentLayer, true, true);
    }
    else
    {
        if ( !Private_IsItemSelected(SelectorItem) )
            Private_SetItemSelection(SelectorItem, true, true);

        SetCurrentLayerFromSelectorItem();
    }

    STreeView< UOdysseyLayer* >::Private_SignalSelectionChanged(SelectInfo);
}

void
SOdysseyLayerStackTreeView::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if ( !mLayerStack )
        return;

    if ( iLayerStack != mLayerStack )
        return;

    Private_ClearSelection();

    UOdysseyLayer* currentLayer = mLayerStack->CurrentLayer.Get();
    if( currentLayer )
    {
        Private_SetItemSelection(currentLayer, true, true);
        Private_SignalSelectionChanged(ESelectInfo::Direct);
    }
}

// ContextMenu

/*
TArray<SOdysseyLayerStackTreeView::FOnExtendContextMenu>&
SOdysseyLayerStackTreeView::GetOnExtendContextMenuDelegates()
{
    static TArray<FOnExtendContextMenu> onExtendContextMenuDelegates;
    return onExtendContextMenuDelegates;
}*/

TSharedPtr<SWidget>
SOdysseyLayerStackTreeView::OnContextMenuOpening()
{
    //Create a new command, so that we can add context menu specific entries 
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    commandList->Append(mCommandList);

    //Allows us to extend the menu context by inserting entries everywhere we want
    //Overriding CreateContextMenu does not allow that
    TArray<TSharedPtr<FExtender>> extenders = ExtendContextMenu();
    TSharedPtr<FExtender> extender = FExtender::Combine(extenders);

    //Build menu
    FToolMenuContext menuContext(commandList, extender);
    return UToolMenus::Get()->GenerateWidget(contextMenuName, menuContext);
}

void SOdysseyLayerStackTreeView::CreateContextMenu()
{
    UToolMenus* ToolMenus = UToolMenus::Get();
    if (!ensure(ToolMenus))
        return;
    
    if (ToolMenus->IsMenuRegistered(contextMenuName))
        return;

    UToolMenu* Menu = ToolMenus->RegisterMenu(contextMenuName);
    
    FToolMenuSection& selectionSection = Menu->AddSection("Selection", LOCTEXT("LayerStackCommonSection", "Selection"));
    {
        selectionSection.AddMenuEntry(FGenericCommands::Get().SelectAll);
    }

    FToolMenuSection& commonSection = Menu->AddSection("Common", LOCTEXT("LayerStackCommonSection", "Common"));
    {
        commonSection.AddMenuEntry(FGenericCommands::Get().Duplicate);
        commonSection.AddMenuEntry(FGenericCommands::Get().Rename);
        commonSection.AddSeparator("");
        commonSection.AddMenuEntry(FGenericCommands::Get().Cut);
        commonSection.AddMenuEntry(FGenericCommands::Get().Copy);
        commonSection.AddMenuEntry(FGenericCommands::Get().Paste);
        commonSection.AddSeparator("");
        commonSection.AddMenuEntry(FGenericCommands::Get().Delete);
    }

    FToolMenuSection& layerSection = Menu->AddSection("Layer", LOCTEXT("LayerStackLayerSection", "Layer"));
    {
        layerSection.AddMenuEntry(FOdysseyLayerStackEditorCommands::Get().MergeSelectedLayers);
        layerSection.AddMenuEntry(FOdysseyLayerStackEditorCommands::Get().FlattenSelectedLayers);
    }
}

TArray<TSharedPtr<FExtender>>
SOdysseyLayerStackTreeView::ExtendContextMenu()
{
	return TArray< TSharedPtr<FExtender> >();
}

void
SOdysseyLayerStackTreeView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::CopyLayers)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::PasteLayers)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::CutLayers)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::SelectAllLayers)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::DeleteSelectedLayers),
        FCanExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::CanDeleteSelectedLayers)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Duplicate,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::DuplicateSelectedLayers)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::RenameCurrentLayer)
    );

    mCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().MergeSelectedLayers,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::MergeSelectedLayers),
        FCanExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::CanMergeSelectedLayers)
    );

    mCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().FlattenSelectedLayers,
        FExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::FlattenSelectedLayers),
        FCanExecuteAction::CreateRaw(this, &SOdysseyLayerStackTreeView::CanFlattenSelectedLayers)
    );
}

// Commands

void
SOdysseyLayerStackTreeView::CopyLayers()
{
    FOdysseyLayerStackClipboard::Get()->Copy(GetSelectedItems());
}

void
SOdysseyLayerStackTreeView::CutLayers()
{
    FOdysseyLayerStackClipboard::Get()->Copy(GetSelectedItems());
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Cut Layers"));
#endif
    DeleteSelectedLayers();
}

void
SOdysseyLayerStackTreeView::PasteLayers()
{
    //First check if the copied layers can be pasted in this layerstack
    const TArray<UOdysseyLayer*>& originalLayers = FOdysseyLayerStackClipboard::Get()->GetLayers();

    if (originalLayers.Num() == 0)
        return;

    bool notSupported = originalLayers.ContainsByPredicate(
        [this](UOdysseyLayer* iLayer)
        {
            return !mLayerStack->SupportsLayerClass(iLayer->GetClass());
        }
    );

    if (notSupported)
        return;

    UOdysseyLayer* parent = mLayerStack->CurrentLayer.Get()->GetParent();
    int indexInParent = mLayerStack->CurrentLayer.Get()->GetIndexInParent();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Paste Layers"));
#endif

    TArray<UOdysseyLayer*> pastedLayers = mLayerStack->CopyLayers(originalLayers, parent, indexInParent);
    FOdysseyObjectEditorUtils::SetPropertyValue(mLayerStack, "CurrentLayer", TSoftObjectPtr<UOdysseyLayer>(pastedLayers[0]));
    SetItemSelection(pastedLayers, true);
}

void
SOdysseyLayerStackTreeView::SelectAllLayers()
{
    if ( !mLayerStack )
        return;

    //ItemsSource is the ListView::ItemsSource, which contains all displayed items, even deep children
    //It is NOT the same as TreeItemsSource or mRootLayers which only contain root elements
    SetItemSelection(SListView<UOdysseyLayer*>::GetItems(), true);
}

void
SOdysseyLayerStackTreeView::DeleteSelectedLayers()
{
    if ( !mLayerStack )
        return;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    mLayerStack->RemoveLayers(selectedLayers);
}

bool
SOdysseyLayerStackTreeView::CanDeleteSelectedLayers()
{
    if ( !mLayerStack )
        return false;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return false;

    //If one of the root layers is not selected, we can delete selected layers
    const TArray<UOdysseyLayer*>& rootLayers = mLayerStack->GetRootLayers();
    for (UOdysseyLayer* rootLayer : rootLayers)
    {
        if (!selectedLayers.Contains(rootLayer))
            return true;
    }
    
    return false;
}

void
SOdysseyLayerStackTreeView::DuplicateSelectedLayers()
{
    if ( !mLayerStack )
        return;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

	//manage current layer seperately
	TArray<UOdysseyLayer*> duplicatedLayers = mLayerStack->DuplicateLayers(selectedLayers);
}

void
SOdysseyLayerStackTreeView::RenameCurrentLayer()
{
    if ( !mLayerStack )
        return;

    if (!mLayerStack->CurrentLayer)
        return;
    
    mIsRenamePending = true; //has to come before ScrollItemIntoView() in case the item is already into view, which will trigger OnItemScrolledIntoView() immediately
	RequestScrollIntoView(mLayerStack->CurrentLayer.Get());
}

void
SOdysseyLayerStackTreeView::MergeSelectedLayers()
{
    if ( !mLayerStack )
        return;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    mLayerStack->MergeLayers(selectedLayers);
}

bool
SOdysseyLayerStackTreeView::CanMergeSelectedLayers()
{
    if ( !mLayerStack )
        return false;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return false;

    return mLayerStack->CanMergeLayers(selectedLayers);
}

void
SOdysseyLayerStackTreeView::FlattenSelectedLayers()
{
    if ( !mLayerStack )
        return;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    mLayerStack->FlattenLayers(selectedLayers);
}

bool
SOdysseyLayerStackTreeView::CanFlattenSelectedLayers()
{
    if ( !mLayerStack )
        return false;

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return false;

    return mLayerStack->CanFlattenLayers(selectedLayers);
}
    
void
SOdysseyLayerStackTreeView::OnLayerIsExpandedChanged(UOdysseyLayer* iLayerNode)
{
    if ( !mLayerStack )
        return;
    
    if (iLayerNode->GetLayerStack() != mLayerStack )
        return;
    
    if(IsItemExpanded(Cast<UOdysseyLayer>(iLayerNode)) == iLayerNode->IsExpanded )
        return;

    SetItemExpansion(Cast<UOdysseyLayer>(iLayerNode), iLayerNode->IsExpanded);
}

void
SOdysseyLayerStackTreeView::OnExpansionChanged( UOdysseyLayer* iLayerNode, bool iIsExpanded )
{
    FOdysseyObjectEditorUtils::SetPropertyValue(iLayerNode, "IsExpanded", iIsExpanded);
}

void
SOdysseyLayerStackTreeView::OnItemScrolledIntoView(UOdysseyLayer* iLayer, const TSharedPtr<ITableRow>& iRow)
{
    if ( !mLayerStack )
        return;

    if (!iLayer || !iRow)
        return;

    if (mIsRenamePending && iLayer == mLayerStack->CurrentLayer)
    {
        TSharedPtr<SOdysseyLayerRow> layerRow = StaticCastSharedPtr<SOdysseyLayerRow>(iRow);
        layerRow->Rename();
        mIsRenamePending = false;
    }
}
    
TSharedPtr<FOdysseyLayerStackDragDropOperation>
SOdysseyLayerStackTreeView::CreateDragDropOperation() const
{
    if ( !mLayerStack )
        return nullptr;

    TSharedRef<FOdysseyLayerStackDragDropOperation> operation =  MakeShared<FOdysseyLayerStackDragDropOperation>(mLayerStack, GetSelectedItems());
	operation->Construct();
    return operation;
}

#undef LOCTEXT_NAMESPACE
