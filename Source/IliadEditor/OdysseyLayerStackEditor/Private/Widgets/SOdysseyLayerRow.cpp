// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SOdysseyLayerRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyle.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Framework/Commands/GenericCommands.h"
#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackEditorCommands.h"
#include "OdysseyLayerStackShortcuts.h"
#include "Widgets/SOdysseyLayerExpanderArrow.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyLayerStack.h"
#include "Math/UnitConversion.h"
#include "SEnumCombo.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "LayerStackEditor"

SOdysseyLayerRow::SOdysseyLayerRow()
    : mSetOpacityTransactionName(LOCTEXT("layer-image-raster.transaction.set-opacity", "Change Layer Opacity"))
{
}

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyLayerRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView, UOdysseyLayer* iLayer)
{
    ensure(iLayer);

    mLayerStackGlobalShortcuts = MakeShared<FOdysseyLayerStackGlobalShortcuts>( TAttribute<UOdysseyLayerStack*>::CreateSP( this, &SOdysseyLayerRow::GetLayerStack ),
                                                                                TAttribute<UOdysseyLayer*>::CreateSP( this, &SOdysseyLayerRow::GetLayer ) );

    SOdysseyLayerRowBase::Construct(
        InArgs,
        iOwnerTableView,
        iLayer
    );
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateWidget( const FName& iRow, const FName& iColumn )
{
    if (iRow == "Main")
    {
        if (iColumn == "IsActivated")
        {
            return GenerateMainRowIsActivatedWidget();
        }
        else if (iColumn == "IsLocked")
        {
            return GenerateMainRowIsLockedWidget();
        }
        else if (iColumn == "DisplayOptions")
        {
            return GenerateMainRowDisplayOptionsWidget();
        }
        else if (iColumn == "Header")
        {
            return GenerateMainRowHeaderWidget();
        }
    }
    if (iRow == "Blend")
    {
        if (iColumn == "Header")
        {
            return GenerateBlendRowHeaderWidget();
        }
    }
    return SOdysseyLayerRowBase::GenerateWidget(iRow, iColumn);
}

FMargin
SOdysseyLayerRow::GetColumnPadding( FName iColumn ) const
{
    if (iColumn == "Header")
        return FMargin(0.f, 0.f, 2.f, 0.f);

    return SOdysseyLayerRowBase::GetColumnPadding(iColumn);
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowHeaderWidget()
{
    TSharedRef<SHorizontalBox> horizontalBox = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .Padding(FMargin(2.f, 0.f, 0.f, 0.f))
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew( SOdysseyLayerExpanderArrow, SharedThis(this) )
                .ArrowPadding(FMargin(0.f, 2.f, 0.f, 0.f))
                .ExpanderImageOpened(GetLayer()->GetIconExpanded().GetIcon())
                .ExpanderImageClosed(GetLayer()->GetIcon().GetIcon())
                .IndentAmount(16.f)
                .ShouldDrawWires( true )
        ]
        + SHorizontalBox::Slot()
        .Padding(FMargin(2.f, 0.f, 0.f, 0.f))
        .VAlign(VAlign_Center)
        [
            SAssignNew(mNameWidget, SInlineEditableTextBlock)
                .IsEnabled_Lambda([this](){ return GetLayer()->IsEditable();})
                .Text(this, &SOdysseyLayerRow::GetLayerName)
                .Font(this, &SOdysseyLayerRow::GetLayerNameFont)
                .OnTextCommitted(this, &SOdysseyLayerRow::OnLayerNameCommited)
                .IsSelected(this, &SOdysseyLayerRow::IsSelectedExclusively) //Allows edition to work
        ]
        + SHorizontalBox::Slot()
        .Padding(FMargin(2.f, 0.f, 0.f, 0.f))
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SNumericEntryBox<int>)
            .Visibility(this, &SOdysseyLayerRow::GetCollapsedOpacityVisibility)
            .IsEnabled_Lambda([this](){ return GetLayer()->IsEditable();})
            .Value_Lambda([this]() { return (int)(GetLayer()->GetOpacity() * 100.f + 0.5f);})
            .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
            .AllowSpin(true)
            .ShiftMultiplier(10)
            .Delta(1)
            .MinValue(0)
            .MinSliderValue(0)
            .MaxValue(100)
            .MaxSliderValue(100)
            .OnValueChanged(this, &SOdysseyLayerRow::OnOpacityValueChanged)
            .OnValueCommitted(this, &SOdysseyLayerRow::OnOpacityValueCommitted)
            .OnBeginSliderMovement(this, &SOdysseyLayerRow::OnOpacityBeginSliderMovement)
            .OnEndSliderMovement(this, &SOdysseyLayerRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth
        ];

    TArray<TSharedPtr<SWidget>> optionWidgets = GenerateMainRowHeaderOptionWidgets();
    for (TSharedPtr<SWidget> widget : optionWidgets)
    {
        horizontalBox->AddSlot()
        .AutoWidth()
        .Padding(FMargin(2.f, 0.f, 0.f, 0.f))
        .VAlign(VAlign_Center)
        [
            widget.ToSharedRef()
        ];
    }

    return horizontalBox;
}

TArray<TSharedPtr<SWidget>>
SOdysseyLayerRow::GenerateMainRowHeaderOptionWidgets()
{
    return {};
}

TSharedRef<SWidget>
SOdysseyLayerRow::CreateBlendModesMenu()
{
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = StaticCastSharedPtr<SOdysseyLayerStackTreeView>(OwnerTablePtr.Pin());
    //Create a new command, so that we can add context menu specific entries
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();

    // Set the current layer here will deselect all selected layers (delegate bound in SOdysseyLayerStackTreeView)
    // This function should only be called when the current layer is NOT inside the selected layer list
    // and only be called when a layer is changed ouside the selection to set only it current
    // (Take care of every delegates bound to SetCurrentLayer())
    //
    // If this behavior is adopted, it could/should be extended to other options in layer:
    // - FOdysseyLayerStackGlobalShortcuts::Action_OpenFolderLayer()
    // - FOdysseyLayerStackGlobalShortcuts::Action_CloseFolderLayer()
    // - SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged()
    // - SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged()
    // - SOdysseyLayerRow::OnOpacityValueCommitted()
    // - SOdysseyLayerRow::OnOpacityValueChanged()
    // - SOdysseyLayerStackTreeView::OnExpansionChanged()
    // - FOdysseyPainterEditorGlobalLayersShortcuts::Action_ChangeLayerOpacity()
    // - SOdysseyAnimationLayerRow::OnLighttableCheckStateChanged()
    // - SOdysseyAnimationLayerRow::OnCellNamesCheckStateChanged()
    //GetLayer()->GetLayerStack()->SetCurrentLayer( GetLayer() );

    // global shortcuts
    mLayerStackGlobalShortcuts->MapActionsToCommandList( commandList );
    // widget shortcuts
    commandList->Append( treeView.Get()->GetLayerStackShortcuts()->GetCommandList());

    FMenuBuilder menu( true, commandList );

    menu.BeginSection("Context Menu");
    for ( TSharedPtr<FUICommandInfo> commandInfo : FOdysseyLayerStackEditorCommands::Get().SetCurrentLayerBlendMode )
    {
        menu.AddMenuEntry( commandInfo );
    }
    menu.EndSection();

    return menu.MakeWidget();
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateBlendRowHeaderWidget()
{
    return SNew(SHorizontalBox)
          +SHorizontalBox::Slot()
          .Padding(FMargin(0, 0, 1.f, 0))
          [
              SNew(SNumericEntryBox<int>)
              .IsEnabled_Lambda([this](){ return GetLayer()->IsEditable();})
              .Value_Lambda([this]() { return (int)(GetLayer()->GetOpacity() * 100.f + 0.5f);})
              .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<int32>( EUnit::Percentage ) ))
              .AllowSpin(true)
              .ShiftMultiplier(10)
              .Delta(1)
              .MinValue(0)
              .MinSliderValue(0)
              .MaxValue(100)
              .MaxSliderValue(100)
              .OnValueChanged(this, &SOdysseyLayerRow::OnOpacityValueChanged)
              .OnValueCommitted(this, &SOdysseyLayerRow::OnOpacityValueCommitted)
              .OnBeginSliderMovement(this, &SOdysseyLayerRow::OnOpacityBeginSliderMovement)
              .OnEndSliderMovement(this, &SOdysseyLayerRow::OnOpacityEndSliderMovement)
            //.MinDesiredValueWidth
          ]
          +SHorizontalBox::Slot()
          .Padding(FMargin(1.f, 0, 0, 0))
          .VAlign(VAlign_Center)
          [
              //SNew(SEnumComboBox, StaticEnum<EOdysseyBlendingMode>())
              SNew(SComboButton)
              .IsEnabled_Lambda([this](){ return GetLayer()->IsEditable();})
              .OnGetMenuContent( this, &SOdysseyLayerRow::CreateBlendModesMenu )
              .ContentPadding(FMargin(0))
              .ButtonContent()
              [
                  SNew(STextBlock)
                  .Text_Lambda([this](){ return UEnum::GetDisplayValueAsText(GetLayer()->GetBlendMode()); } )
              ]
          ];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowDisplayOptionsWidget()
{
    const FCheckBoxStyle* displayOptionsToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.DisplayOptionsToggle");

    return SNew(SBox)
        .VAlign(VAlign_Center)
        [
            SNew(SCheckBox)
            .Style(displayOptionsToggleStyle)
            .IsFocusable(false)
            .OnCheckStateChanged(this, &SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged)
            .IsChecked(this, &SOdysseyLayerRow::GetDisplayOptionsCheckBoxState)
        ];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowIsActivatedWidget()
{
    const FCheckBoxStyle* isActivatedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsActivatedToggle");

    return SNew(SBox)
        .VAlign(VAlign_Center)
        [
            SNew(SCheckBox)
            .Style(isActivatedToggleStyle)
            .IsEnabled(this, &SOdysseyLayerRow::GetIsActivatedCheckBoxEnabled)
            .IsFocusable(false)
            .OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged)
            .IsChecked(this, &SOdysseyLayerRow::GetIsActivatedCheckBoxState)
        ];
}

TSharedRef<SWidget>
SOdysseyLayerRow::GenerateMainRowIsLockedWidget()
{
    const FCheckBoxStyle* isLockedToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("LayerStack.IsLockedToggle");

    return SNew(SBox)
        .VAlign(VAlign_Center)
        [
            SNew(SCheckBox)
            .IsFocusable(false)
            .IsEnabled(this, &SOdysseyLayerRow::GetIsLockedCheckBoxEnabled)
            .Style(isLockedToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged)
            .IsChecked(this, &SOdysseyLayerRow::GetIsLockedCheckBoxState)
        ];
}

void
SOdysseyLayerRow::OnIsActivatedCheckBoxStateChanged(ECheckBoxState iState)
{
    TSet<UOdysseyLayer*> selected_layers;
    UOdysseyLayerStack* layerStack = GetLayer()->GetLayerStack();
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the focused layer is outside the selection, just change it
    UOdysseyLayer* focusedLayer = GetLayer();
    if( !selected_layers.Contains( focusedLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( focusedLayer );
    }

    FScopedTransaction ScopedTransaction( LOCTEXT( "layer.transaction.set-is-activated", "Change Layer Active" ) );

    for( UOdysseyLayer* layer : selected_layers )
    {
        layer->SetIsActivated( iState == ECheckBoxState::Checked );
    }
}

ECheckBoxState
SOdysseyLayerRow::GetIsActivatedCheckBoxState() const
{
    if( !GetLayer()->GetParent()->IsActivatedRecursively() )
        return ECheckBoxState::Unchecked;
        //return ECheckBoxState::Undetermined;

    return GetLayer()->IsActivated() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SOdysseyLayerRow::GetIsActivatedCheckBoxEnabled() const
{
    return GetLayer()->GetParent()->IsActivatedRecursively();
}

//-

void
SOdysseyLayerRow::OnIsLockedCheckBoxStateChanged(ECheckBoxState iState)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-is-locked", "Change Layer Lock"));
    GetLayer()->SetIsLocked(iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyLayerRow::GetIsLockedCheckBoxState() const
{
    if( !GetLayer()->IsActivatedRecursively() )
        return ECheckBoxState::Undetermined;

    return GetLayer()->IsLocked() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

bool
SOdysseyLayerRow::GetIsLockedCheckBoxEnabled() const
{
    if( !GetLayer()->IsActivatedRecursively() )
        return false;

    return !GetLayer()->GetParent()->IsLockedRecursively();
}

//---

UOdysseyLayerStack*
SOdysseyLayerRow::GetLayerStack() const
{
    return GetLayer()->GetLayerStack();
}

FText
SOdysseyLayerRow::GetLayerName() const
{
    return GetLayer()->GetLayerName();
}

void
SOdysseyLayerRow::OnLayerNameCommited(const FText& iText, ETextCommit::Type iType)
{
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-name", "Change Layer Name"));
    GetLayer()->SetLayerName(iText);
}

FSlateFontInfo
SOdysseyLayerRow::GetLayerNameFont() const
{
    if ( !GetLayer() )
        return FStyleDefaults::GetFontInfo();

    UOdysseyLayerStack* layerStack = GetLayer()->GetLayerStack();

    if (!layerStack || layerStack->GetCurrentLayer() != GetLayer())
        return FStyleDefaults::GetFontInfo();

    return FAppStyle::Get().GetFontStyle("NormalFontBold");
}

void
SOdysseyLayerRow::Rename()
{
    mNameWidget->EnterEditingMode();
}

FReply
SOdysseyLayerRow::OnRowDragDetected(const FGeometry& iGeometry, const FPointerEvent& iEvent, TWeakPtr<SOdysseyLayerStackTreeView> iTreeView)
{
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = iTreeView.Pin();
    if (treeView.IsValid() && iEvent.IsMouseButtonDown( EKeys::LeftMouseButton ))
    {
        TSharedPtr<FOdysseyLayerStackDragDropOperation> operation = treeView->CreateDragDropOperation();

        if (operation.IsValid())
        {
            //PATCH: Unreal does not manage collision between SInlineWidget entering editiong mode
            //         and Drag'n'Drop, which can lead to cases where after dropping an element,
            //       a layer will start editing its name, which is not an expected behaviour
            //       We call OnDragOver on the namewidget to reset the timer responsible for
            //       entering editing mode.
            //       As the handle to the timerdelegate is private, it's the only way to do it
            //       in an almost clean way.
            mNameWidget->OnDragOver(FGeometry(), FDragDropEvent(FPointerEvent(), nullptr));
            //END OF PATCH:

            return FReply::Handled().BeginDragDrop(operation.ToSharedRef());
        }
    }

    return FReply::Unhandled();
}

void
SOdysseyLayerRow::OnDisplayOptionsCheckBoxStateChanged(ECheckBoxState iState)
{
    TSet<UOdysseyLayer*> selected_layers;
    UOdysseyLayerStack* layerStack = GetLayer()->GetLayerStack();
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the focused layer is outside the selection, just change it
    UOdysseyLayer* focusedLayer = GetLayer();
    if( !selected_layers.Contains( focusedLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( focusedLayer );
    }

    for( UOdysseyLayer* layer : selected_layers )
    {
        layer->SetDisplayOptions( iState == ECheckBoxState::Checked );
    }
}

ECheckBoxState
SOdysseyLayerRow::GetDisplayOptionsCheckBoxState() const
{
    return GetLayer()->ShouldDisplayOptions() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyLayerRow::OnBlendModeComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    if ( !GetLayer()->IsEditable() )
        return;

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction(LOCTEXT("layer.transaction.set-blend-mode", "Change Layer BlendMode"));
    GetLayer()->SetBlendMode(EOdysseyBlendingMode(iValue));
}

void
SOdysseyLayerRow::OnOpacityValueCommitted(int iValue, ETextCommit::Type iType)
{
    UOdysseyLayer* focusedLayer = GetLayer();
    if( !focusedLayer->IsEditable() )
        return;

    TSet<UOdysseyLayer*> selected_layers;
    UOdysseyLayerStack* layerStack = focusedLayer->GetLayerStack();
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the focused layer is outside the selection, just change it
    if( !selected_layers.Contains( focusedLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( focusedLayer );
    }

    //---

    //Creating a transaction here manages entering a value using keyboard
    FScopedTransaction ScopedTransaction( mSetOpacityTransactionName );

    // The reference offset from the focused layer
    int referenceOffsetAsInt = iValue - int( focusedLayer->GetOpacity() * 100.f + .5f );

    for( UOdysseyLayer* layer : selected_layers )
    {
        // Always process the focused layer with the given value as-is (to avoid any conversion)
        if( layer == focusedLayer )
        {
            layer->SetOpacity( iValue / 100.f );
        }
        else
        {
            // If CONTROL is down, just set all opacities of all other layers as the focused layer one
            if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
            {
                layer->SetOpacity( iValue / 100.f );
            }
            // Otherwise, apply the offset to all other layers
            else
            {
                // Make all computations as int:
                // - to try to avoid approximation of float
                // - the spin box stores int value
                int opacityAsInt = int( layer->GetOpacity() * 100.f + .5f );
                layer->SetOpacity( ( opacityAsInt + referenceOffsetAsInt ) / 100.f );
            }
        }
    }
}

void
SOdysseyLayerRow::OnOpacityValueChanged(int iValue)
{
    UOdysseyLayer* focusedLayer = GetLayer();
    if( !focusedLayer->IsEditable() )
        return;

    TSet<UOdysseyLayer*> selected_layers;
    UOdysseyLayerStack* layerStack = focusedLayer->GetLayerStack();
    for( UOdysseyLayer* layer : layerStack->GetLayers() )
    {
        if( layerStack->IsLayerSelected( layer ) )
            selected_layers.Add( layer );
    }
    // Generally, the current layer is selected except when the layer stack is created (before any click interactions in layer stack header)
    // But too much interrogations to fix it (as many callbacks can be called.
    // (add a flag in SetCurrentLayer() to deselect all and select only the new current layer or in FOdysseyLayerSelection or ...)
    // So, at least for now, just always add it.
    //check( selected_layers.Contains( layerStack->GetCurrentLayer() ) );
    selected_layers.Add( layerStack->GetCurrentLayer() );

    // If the focused layer is outside the selection, just change it
    if( !selected_layers.Contains( focusedLayer ) )
    {
        selected_layers.Empty();
        selected_layers.Add( focusedLayer );
    }

    //---

    // The reference offset from the focused layer
    int referenceOffsetAsInt = iValue - int( focusedLayer->GetOpacity() * 100.f + .5f );

    for( UOdysseyLayer* layer : selected_layers )
    {
        // Always process the focused layer with the given value as-is (to avoid any conversion)
        if( layer == focusedLayer )
        {
            layer->SetOpacityInteractive( iValue / 100.f );
        }
        else
        {
            // If CONTROL is down, just set all opacities as the focused layer one
            if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
            {
                layer->SetOpacityInteractive( iValue / 100.f );
            }
            // Otherwise, apply the offset to all other layers
            else
            {
                // Make all computations as int:
                // - to try to avoid approximation of float
                // - the spin box stores int value
                int opacityAsInt = int( layer->GetOpacity() * 100.f + .5f );
                layer->SetOpacityInteractive( ( opacityAsInt + referenceOffsetAsInt ) / 100.f );
            }
        }
    }
}

void
SOdysseyLayerRow::OnOpacityBeginSliderMovement()
{
    //Creating a transaction here manages entering a value using slider
    GEditor->BeginTransaction(mSetOpacityTransactionName);
}

void
SOdysseyLayerRow::OnOpacityEndSliderMovement(int iValue)
{
    GEditor->EndTransaction();
}

EVisibility
SOdysseyLayerRow::GetCollapsedOpacityVisibility() const
{
    return GetLayer()->ShouldDisplayOptions() ? EVisibility::Collapsed : EVisibility::Visible;
}

#undef LOCTEXT_NAMESPACE
