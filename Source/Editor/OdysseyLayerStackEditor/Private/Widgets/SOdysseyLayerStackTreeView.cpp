// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "OdysseyStyleSet.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ToolMenus.h"
#include "Framework/Commands/GenericCommands.h"
#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "Shortcuts/OdysseyLayerStackShortcuts.h"
#include "OdysseyLayerStackFunctionLibrary.h"
#include "OdysseyLayerStack.h"
#include "Widgets/SOdysseyLayerRow.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

static FName contextMenuName = "OdysseyLayerStackContextMenu";

SOdysseyLayerStackTreeView::~SOdysseyLayerStackTreeView()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    UOdysseyLayerStack::OnHierarchyChanged().RemoveAll(this);
	UOdysseyLayer::OnDisplayChildrenChanged().RemoveAll(this);
    UOdysseyLayer::OnDisplayOptionsChanged().RemoveAll(this);
}

SOdysseyLayerStackTreeView::SOdysseyLayerStackTreeView()
    //: mLayerStack(*this, nullptr)
    : mLayerStack(nullptr)
    , mLayerStackShortcuts(nullptr)
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnCurrentLayerChanged);
    UOdysseyLayerStack::OnHierarchyChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerStackHierarchyChanged);
	UOdysseyLayer::OnDisplayChildrenChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerDisplayChildrenChanged);
    UOdysseyLayer::OnDisplayOptionsChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerDisplayOptionsChanged);
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void SOdysseyLayerStackTreeView::Construct(const FArguments& InArgs)
{
    //mLayerStack.Assign(*this, InArgs._LayerStack);
    mLayerStack = InArgs._LayerStack;
    mLayerStackShortcuts = MakeShared<FOdysseyLayerStackShortcuts>(SharedThis(this), mLayerStack);

    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        .SplitterHandleSize(0.f) //Fixes alignment between header row and actual rows
        + SHeaderRow::Column("IsActivated")
            .ToolTipText(LOCTEXT("header-row.is-layer-activated.tooltip", "Toggle Layer Activation"))
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
            .ToolTipText(LOCTEXT("header-row.is-layer-locked.tooltip", "Toggle Layer Locked State"))
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
        + SHeaderRow::Column("DisplayOptions")
            .ToolTipText(LOCTEXT("header-row.display-options.tooltip", "Display / Hide Layer's Options"))
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
            .DefaultLabel(FText())
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

UOdysseyLayerStack*
SOdysseyLayerStackTreeView::GetLayerStack() const
{
    return mLayerStack;
}

void
SOdysseyLayerStackTreeView::SetIsRenamePending(bool iValue)
{
    mIsRenamePending = iValue;
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
	if (mLayerStackShortcuts->GetCommandList()->ProcessCommandBindings(iKeyEvent))
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
        #ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(LOCTEXT("drag-drop.transaction.move-layers", "Move Layers"));
        #endif
        mLayerStack->MoveLayers(layers, nullptr, mLayerStack->GetRootLayers().Num());
    }
    else
    {
        #ifdef WITH_EDITOR
            FScopedTransaction ScopedTransaction(LOCTEXT("drag-drop.transaction.copy-layers", "Copy Layers"));
        #endif
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
SOdysseyLayerStackTreeView::RefreshAllExpansionStates()
{
    if ( !mLayerStack )
        return;

    TArray<UOdysseyLayer*> layers = mLayerStack->GetLayers();
    for(UOdysseyLayer* layer : layers )
    {
        if(!layer )
            continue;

        SetItemExpansion(layer, layer->DisplayChildren);
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

TSharedPtr<SWidget>
SOdysseyLayerStackTreeView::OnContextMenuOpening()
{
    //Create a new command, so that we can add context menu specific entries 
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    commandList->Append(mLayerStackShortcuts->GetCommandList());

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
    
    FToolMenuSection& selectionSection = Menu->AddSection("SelectionSection", LOCTEXT("context-menu.selection-section", "Selection"));
    {
        selectionSection.AddMenuEntry(FGenericCommands::Get().SelectAll);
    }

    FToolMenuSection& commonSection = Menu->AddSection("CommonSection", LOCTEXT("context-menu.common-section", "Common"));
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

    FToolMenuSection& layerSection = Menu->AddSection("LayerSection", LOCTEXT("context-menu.layer-section", "Layer"));
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
SOdysseyLayerStackTreeView::OnLayerDisplayChildrenChanged(UOdysseyLayer* iLayerNode)
{
    if ( !mLayerStack )
        return;
    
    if (iLayerNode->GetLayerStack() != mLayerStack )
        return;
    
    if(IsItemExpanded(Cast<UOdysseyLayer>(iLayerNode)) == iLayerNode->DisplayChildren )
        return;

    SetItemExpansion(Cast<UOdysseyLayer>(iLayerNode), iLayerNode->DisplayChildren);
}

void
SOdysseyLayerStackTreeView::OnLayerDisplayOptionsChanged(UOdysseyLayer* iLayerNode)
{   
    if ( !mLayerStack )
        return;
    
    if (iLayerNode->GetLayerStack() != mLayerStack )
        return;
    
    RequestTreeRefresh();
}

void
SOdysseyLayerStackTreeView::OnExpansionChanged( UOdysseyLayer* iLayerNode, bool iIsExpanded )
{
    FOdysseyObjectEditorUtils::SetPropertyValue(iLayerNode, "DisplayChildren", iIsExpanded);
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

int32
SOdysseyLayerStackTreeView::GetMaxIndentLevel() const
{
    int32 NestingLevel = 0;
    for (const FItemInfo& info : DenseItemInfos)
    {
        NestingLevel = FMath::Max( NestingLevel, info.GetNestingLevel());
    }
    return NestingLevel;
}

#undef LOCTEXT_NAMESPACE
