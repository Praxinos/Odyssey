// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SOdysseyLayerStackTreeView.h"

#include "Editor.h"
#include "Framework/Commands/GenericCommands.h"
#include "Math/UnitConversion.h"
#include "ScopedTransaction.h"
#include "Selection.h"
#include "ToolMenus.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyCommandList.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackFunctionLibrary.h"
#include "OdysseyLayerStackSelection.h"
#include "OdysseyStyle.h"
#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"
#include "Shortcuts/OdysseyLayerStackShortcuts.h"
#include "UObject/OdysseyObjectEditorUtils.h"
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
    , mLayerStackGlobalShortcuts(nullptr)
    , mCommandList(nullptr)
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnCurrentLayerChanged);
    UOdysseyLayerStack::OnHierarchyChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerStackHierarchyChanged);
    UOdysseyLayer::OnDisplayChildrenChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerDisplayChildrenChanged);
    UOdysseyLayer::OnDisplayOptionsChanged().AddRaw(this, &SOdysseyLayerStackTreeView::OnLayerDisplayOptionsChanged);
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void SOdysseyLayerStackTreeView::Construct(const FArguments& InArgs)
{
    mLayerStack = InArgs._LayerStack;
    mLayerStackShortcuts = MakeShared<FOdysseyLayerStackShortcuts>(SharedThis(this), mLayerStack);
    mLayerStackGlobalShortcuts = MakeShared<FOdysseyLayerStackGlobalShortcuts>( mLayerStack );
    mCommandList = MakeShared<FOdysseyCommandList>();
    mCommandList->Append( mLayerStackShortcuts->GetCommandList() );
    mLayerStackGlobalShortcuts->MapActionsToCommandList( mCommandList.ToSharedRef() );

    USelection::SelectionChangedEvent.AddSP(this, &SOdysseyLayerStackTreeView::OnExternalSelectionChanged);

    //---

    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        .SplitterHandleSize(0.f); //Fixes alignment between header row and actual rows

    if (InArgs._Columns.IsSet())
    {
        TArray<SHeaderRow::FColumn::FArguments> columns = InArgs._Columns.GetValue();
        for( SHeaderRow::FColumn::FArguments columnArguments : columns)
        {
            headerRow->AddColumn(columnArguments);
        }
    }
    else
    {
        headerRow->AddColumn(
            SHeaderRow::Column("IsActivated")
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            .HeaderContentPadding(FMargin(0))
            [
                SNew( SComboButton )
                .ContentPadding( 2.f )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .ToolTipText( LOCTEXT( "header-row.is-layer-activated.tooltip", "Toggle Layer Activation" ) )
                .ComboButtonStyle( FAppStyle::Get(), TEXT( "SimpleComboButtonWithIcon" ) )
                .HasDownArrow( false )
                .OnGetMenuContent_Lambda( [this]() -> TSharedRef<SWidget>
                                            {
                                                FMenuBuilder builder( true, mCommandList );

                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().ActivateAllLayers );
                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().InactivateAllLayers );

                                                builder.AddSeparator();

                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().DisplayOnlyCurrentLayer );
                                                builder.AddWidget( SNew( SSpinBox<float> )
                                                                   .IsEnabled_Lambda( [this]() -> bool
                                                                                      {
                                                                                          return mLayerStack->GetDisplayOnlyCurrentLayer();
                                                                                      } )
                                                                   .MinValue( 0.f )
                                                                   .MaxValue( 100.f )
                                                                   .MinSliderValue( 0.f )
                                                                   .MaxSliderValue( 100.f )
                                                                   .MinFractionalDigits( 0 )
                                                                   .MaxFractionalDigits( 2 )
                                                                   .TypeInterface( MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Percentage ) ) )
                                                                   .LinearDeltaSensitivity( 25 )
                                                                   .Delta( 1 )
                                                                   .MinDesiredWidth( 55.f )
                                                                   .Value_Lambda( [this]() -> float
                                                                                  {
                                                                                      return mLayerStack->GetOtherLayersOpacity();
                                                                                  } )
                                                                   .OnValueChanged_Lambda( [this]( float iNewValue )
                                                                                           {
                                                                                               return mLayerStack->SetOtherLayersOpacity( iNewValue );
                                                                                           } )
                                                                   .OnValueCommitted_Lambda( [this]( float iNewValue, ETextCommit::Type iType )
                                                                                             {
                                                                                                 return mLayerStack->SetOtherLayersOpacity( iNewValue );
                                                                                             } )
                                                                   , LOCTEXT( "header-row-entry.other-layers-opacity.label", "Other layers opacity" )
                                                );

                                                return builder.MakeWidget();
                                            } )
                .ButtonContent()
                [
                    SNew(SImage)
                    .ColorAndOpacity(FSlateColor::UseForeground())
                    .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.Visible16"))
                ]
            ]
        );
        headerRow->AddColumn(
            SHeaderRow::Column("IsLocked")
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            .HeaderContentPadding(FMargin(0))
            [
                SNew(SComboButton)
                .ContentPadding( 2.f )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .ToolTipText(LOCTEXT("header-row.is-layer-locked.tooltip", "Toggle Layer Locked State"))
                .ComboButtonStyle( FAppStyle::Get(), TEXT( "SimpleComboButtonWithIcon" ) )
                .HasDownArrow( false )
                .OnGetMenuContent_Lambda( [this]() -> TSharedRef<SWidget>
                                          {
                                              FMenuBuilder builder( true, mCommandList );

                                              builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().LockAllLayers );
                                              builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().UnlockAllLayers );

                                              return builder.MakeWidget();
                                          } )
                .ButtonContent()
                [
                    SNew( SImage )
                    .ColorAndOpacity( FSlateColor::UseForeground() )
                    .Image( FOdysseyStyle::GetBrush( "OdysseyLayerStack.Locked16" ) )
                ]
            ]
        );

        headerRow->AddColumn(
            SHeaderRow::Column("InheritsAlpha")
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            .HeaderContentPadding(FMargin(0))
            [
                SNew(SImage)
                .ColorAndOpacity(FSlateColor::UseForeground())
                .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.ColumnHeader.InheritsAlpha"))
            ]
        );

        headerRow->AddColumn(
            SHeaderRow::Column("DisplayOptions")
            .FixedWidth(24.f)
            .HAlignHeader(HAlign_Center)
            .VAlignHeader(VAlign_Center)
            .HAlignCell(HAlign_Center)
            .VAlignCell(VAlign_Top)
            .HeaderContentPadding(FMargin(0))
            [
                SNew( SComboButton )
                .ContentPadding( 2.f )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                .ToolTipText( LOCTEXT( "header-row.display-options.tooltip", "Display / Hide Layer's Options" ) )
                .ComboButtonStyle( FAppStyle::Get(), TEXT( "SimpleComboButtonWithIcon" ) )
                .HasDownArrow( false )
                .OnGetMenuContent_Lambda( [this]() -> TSharedRef<SWidget>
                                            {
                                                FMenuBuilder builder( true, mCommandList );

                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().CollapseAllLayers );
                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().UncollapseAllLayers );

                                                builder.AddSeparator();

                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().OpenAllFolderLayers );
                                                builder.AddMenuEntry( FOdysseyLayerStackEditorCommands::Get().CloseAllFolderLayers );

                                                return builder.MakeWidget();
                                            } )
                .ButtonContent()
                [
                    SNew(SImage)
                    .ColorAndOpacity(FSlateColor::UseForeground())
                    .Image(FOdysseyStyle::GetBrush("OdysseyLayerStack.OptionsHeader16"))
                ]
            ]
        );

        headerRow->AddColumn(
            SHeaderRow::Column("Header")
            .DefaultLabel(FText())
            .VAlignCell(VAlign_Top)
            .HAlignHeader(HAlign_Fill)
            .VAlignHeader(VAlign_Center)
            .FillWidth(1.0f)
            .HeaderContentPadding(FMargin(0))
            [
                SNew(SBox)
                .HeightOverride(25.f)
                .VAlign(VAlign_Center)
                [
                    InArgs._HeaderContent.Widget
                ]
            ]
        );
    }

    const TArray<UOdysseyLayer*>* rootLayers = mLayerStack ? &mLayerStack->GetRootLayers() : nullptr;

    STreeView<UOdysseyLayer*>::Construct(
        STreeView<UOdysseyLayer*>::FArguments()
        .TreeItemsSource(rootLayers)
        .OnGenerateRow( InArgs._OnGenerateRow )
        .OnGetChildren( this, &SOdysseyLayerStackTreeView::OnGetChildren )
        .OnExpansionChanged( this, &SOdysseyLayerStackTreeView::OnExpansionChanged )
        .OnItemScrolledIntoView(this, &SOdysseyLayerStackTreeView::OnItemScrolledIntoView)
        .OnContextMenuOpening( this, &SOdysseyLayerStackTreeView::OnContextMenuOpening )
        .SelectionMode( ESelectionMode::Multi )
        .HeaderRow(headerRow)
        .ExternalScrollbar(InArgs._ExternalScrollbar)
        .OnTreeViewScrolled(InArgs._OnTreeViewScrolled)
    );

    RefreshAllExpansionStates();

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
            FSlateLayoutTransform RotatedTransform(Pivot - RotatedLocalSize * 0.5f);    // Make the box centered to the alloted geometry, so that it can be rotated around the center.

            FSlateDrawElement::MakeRotatedBox(
                OutDrawElements,
                layerId++,
                geometry.ToPaintGeometry(RotatedLocalSize, RotatedTransform),
                DropIndicatorBrush,
                ESlateDrawEffect::None,
                -HALF_PI,    // 90 deg CCW
                RotatedLocalSize * 0.5f,    // Relative center to the flipped
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

    //If Odyssey encountered a shortcut that could not be executed
    //then don't let Unreal have a chance to execute a shorcut of its own.
    if (mCommandList->HasActionForKeyEvent(iKeyEvent))
    {
        return FReply::Handled();
    }

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
#if WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("drag-drop.transaction.move-layers", "Move Layers"));
#endif
        mLayerStack->MoveLayers(layers, nullptr, mLayerStack->GetRootLayers().Num());
    }
    else
    {
#if WITH_EDITOR
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

        SetItemExpansion(layer, layer->ShouldDisplayChildren());
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
SOdysseyLayerStackTreeView::SynchronizeCurrentAndSelectorItem()
{
    if ( !mLayerStack )
        return;

    if ( mLayerStack->GetLayers().IsEmpty() )
        return;

    UOdysseyLayer* currentLayer = mLayerStack->GetCurrentLayer();

    //Manage SelectorItem / CurrentLayer synchronization
    //CurrentLayer should always be displayed as selected
    //If CurrentLayer is already selected, just make SelectorItem = CurrentLayer
    //If CurrentLayer is not selected, ensure SelectorItem is selected make CurrentLayer = SelectorItem
    if ( currentLayer && Private_IsItemSelected(currentLayer) )
    {
        //Ensures that Selector Item is the Current Layer
        //when the current layer is already selected
        Private_SetItemSelection(currentLayer, true, true);
    }
    else
    {
        //Having SelectorItem always selected, ensures a layer cannot be deselected
        //if it is the current layer
        if ( !Private_IsItemSelected(SelectorItem) )
            Private_SetItemSelection(SelectorItem, true, true);

        //Then we update current layer to match SelectorItem
        //When currentlayer is not in the selection
        if (!Private_IsItemSelected(currentLayer))
        {
            if (!SelectorItem)
            {
                mLayerStack->SetCurrentLayer(mLayerStack->GetRootLayers()[0]);
                return;
            }

            UOdysseyLayerStack* selectorLayerStack = SelectorItem->GetLayerStack();
            if (selectorLayerStack != mLayerStack )
                return;

            if (SelectorItem == mLayerStack->GetCurrentLayer())
                return;

            mLayerStack->SetCurrentLayer(SelectorItem);
        }
    }
}

void
SOdysseyLayerStackTreeView::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
    bIsSelectionChanging = true;
    if ( !mLayerStack )
    {
        STreeView< UOdysseyLayer* >::Private_SignalSelectionChanged(SelectInfo);
        bIsSelectionChanging = false;
        return;
    }

    UOdysseyLayer* currentLayer = mLayerStack->GetCurrentLayer();

    // Update the OdysseyLayerStackSelection
    TArray<UOdysseyLayer*> selectedItems = GetSelectedItems();
    USelection* objectSelection = GEditor->GetSelectedSet( UObject::StaticClass() );
    OdysseyLayerStackSelection::Get()->BeginBatchSelectOperation();
    OdysseyLayerStackSelection::Get()->DeselectAll();

    for( int i = 0; i < selectedItems.Num(); i++ )
    {
        UOdysseyLayer* layer = selectedItems[i];
        OdysseyLayerStackSelection::Get()->Select(layer);
    }
    OdysseyLayerStackSelection::Get()->EndBatchSelectOperation();

    SynchronizeCurrentAndSelectorItem();

    STreeView< UOdysseyLayer* >::Private_SignalSelectionChanged(SelectInfo);
    bIsSelectionChanging = false;
}

void
SOdysseyLayerStackTreeView::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if ( iLayerStack != mLayerStack )
        return;

    if(bIsSelectionChanging)
        return;

    UpdateSelectedItems();
    SynchronizeCurrentAndSelectorItem();
}

void
SOdysseyLayerStackTreeView::UpdateSelectedItems()
{
    if ( !mLayerStack )
        return;

    UOdysseyLayer* CurrentLayer = mLayerStack->GetCurrentLayer();
    if (!CurrentLayer)
        return;

    TArray<UOdysseyLayer*> SelectedLayers = OdysseyLayerStackSelection::GetSelectedLayers(mLayerStack, false);

    Private_ClearSelection();
    for (UOdysseyLayer* SelectedLayer : SelectedLayers)
    {
        Private_SetItemSelection(SelectedLayer, true, /*bWasUserDirected*/false);
    }

    /*
        If the Selector Item is among the Selected Layers
        We consider it as valid.
        Otherwise, we set SelectorItem to be the Current Layer
        by selecting the current layer and setting bWasUserDirected == true
    */
    bool bIsSelectorItemSelected = Private_IsItemSelected(SelectorItem);
    Private_SetItemSelection(CurrentLayer, true, !bIsSelectorItemSelected);
}

// ContextMenu
TSharedPtr<FOdysseyCommandList>
SOdysseyLayerStackTreeView::GetCommandList() const
{
    return mCommandList;
}

TSharedPtr<SWidget>
SOdysseyLayerStackTreeView::OnContextMenuOpening()
{
    //Allows us to extend the menu context by inserting entries everywhere we want
    //Overriding CreateContextMenu does not allow that
    TArray<TSharedPtr<FExtender>> extenders = ExtendContextMenu();
    TSharedPtr<FExtender> extender = FExtender::Combine(extenders);

    //Build menu
    FToolMenuContext menuContext(mCommandList, extender);
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

    if(IsItemExpanded(Cast<UOdysseyLayer>(iLayerNode)) == iLayerNode->ShouldDisplayChildren() )
        return;

    SetItemExpansion(Cast<UOdysseyLayer>(iLayerNode), iLayerNode->ShouldDisplayChildren());
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
    UOdysseyLayerStack* layerStack = iLayerNode->GetLayerStack();
    UOdysseyLayer* focusedLayer = iLayerNode;

    TArray<UOdysseyLayer*> SelectedLayers = OdysseyLayerStackSelection::GetSelectedLayers(layerStack, true);

    /**
     * If the FocuseLayer is not selected, we consider shortcuts should only
     * modify the Focused layer and not the selected layers
     */
    if (!SelectedLayers.Contains( focusedLayer ))
    {
        SelectedLayers.Empty();
        SelectedLayers.Add( focusedLayer );
    }

    for( UOdysseyLayer* layer : SelectedLayers )
    {
        layer->SetDisplayChildren( iIsExpanded );
    }

    OdysseyLayerStackSelection::RegisterUndo(SelectedLayers, SelectedLayers);
}


void
SOdysseyLayerStackTreeView::OnExternalSelectionChanged(UObject* InSelection)
{
    if (InSelection != OdysseyLayerStackSelection::Get())
        return;

    if (bIsSelectionChanging)
        return;

    UpdateSelectedItems();
    SynchronizeCurrentAndSelectorItem();
    /*TArray<UOdysseyLayer*> SelectedLayers = OdysseyLayerStackSelection::GetSelectedLayers(mLayerStack, false);
    Private_ClearSelection();
    for (UOdysseyLayer* Layer : SelectedLayers)
        Private_SetItemSelection(Layer, true);*/
}

void
SOdysseyLayerStackTreeView::OnItemScrolledIntoView(UOdysseyLayer* iLayer, const TSharedPtr<ITableRow>& iRow)
{
    if ( !mLayerStack )
        return;

    if (!iLayer || !iRow)
        return;

    if (mIsRenamePending && iLayer == mLayerStack->GetCurrentLayer())
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
