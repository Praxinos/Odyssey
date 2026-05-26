// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerRow.h"

#include "Widgets/Input/SCheckBox.h"

#include "SOdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "SOdysseyAnimationTimelineLighttableHeader.h"
#include "SOdysseyAnimationTimelineCellNamesHeader.h"
#include "OdysseyStyle.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//PUBLIC API-----------------------------------------------------------

void
SOdysseyAnimationLayerRow::Construct(
    const FArguments& iArgs,
    const TSharedRef<SOdysseyLayerStackTreeView>& iOwnerTableView,
    UOdysseyAnimationLayer* iLayer
)
{
    mTimelinePosition = iArgs._TimelinePosition;
    mLayer = iLayer;

    SOdysseyLayerRow::Construct(
        SOdysseyLayerRow::FArguments(),
        iOwnerTableView,
        iLayer
    );
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateWidget( const FName& iRow, const FName& iColumn )
{
    if (iRow == "Lighttable")
    {
        if (iColumn == "Header")
        {
            return GenerateLighttableRowHeaderWidget();
        }
    }

    if (iRow == "OutOfPegs")
    {
        if (iColumn == "Header")
        {
            return GenerateOutOfPegsRowHeaderWidget();
        }
    }

    if (iRow == "CellNames")
    {
        if (iColumn == "Header")
        {
            return GenerateCellNamesRowHeaderWidget();
        }
    }

    return SOdysseyLayerRow::GenerateWidget( iRow, iColumn );
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateLighttableRowHeaderWidget()
{
    return SNew(SOdysseyAnimationTimelineLighttableHeader)
        .Layer(mLayer);
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateOutOfPegsRowHeaderWidget()
{
    return SNew(STextBlock)
        .Text(LOCTEXT("lighttable.timeline-header.out-of-pegs.name", "Out Of Pegs"));
}

TSharedRef<SWidget>
SOdysseyAnimationLayerRow::GenerateCellNamesRowHeaderWidget()
{
    return SNew( SOdysseyAnimationTimelineCellNamesHeader )
        .Layer( mLayer );
}

TArray<TSharedPtr<SWidget>>
SOdysseyAnimationLayerRow::GenerateMainRowHeaderOptionWidgets()
{
    TArray<TSharedPtr<SWidget>> widgets;

    if( mLayer->GetCells().Num() )
    {
        const FCheckBoxStyle* cellNamesToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>( "Animation.CellNamesToggle" );

        widgets.Add(
            SNew( SCheckBox )
            .Style( cellNamesToggleStyle )
            .IsFocusable( false )
            .OnCheckStateChanged( this, &SOdysseyAnimationLayerRow::OnCellNamesCheckStateChanged )
            .IsChecked( this, &SOdysseyAnimationLayerRow::GetCellNamesIsChecked )
        );
    }

    if (mLayer->HasLighttable())
    {
        const FCheckBoxStyle* lighttableToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Animation.LighttableToggle");

        widgets.Add(
            SNew(SCheckBox)
            .Style(lighttableToggleStyle)
            .IsFocusable(false)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerRow::OnLighttableCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerRow::GetLighttableIsChecked)
        );
    }

    widgets.Append(SOdysseyLayerRow::GenerateMainRowHeaderOptionWidgets());

    return widgets;
}

ECheckBoxState
SOdysseyAnimationLayerRow::GetLighttableIsChecked() const
{
    return mLayer->GetLighttable().bIsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLayerRow::OnLighttableCheckStateChanged(ECheckBoxState iState)
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
        FOdysseyLighttable lighttable = layer->GetLighttable();
        lighttable.bIsActivated = iState == ECheckBoxState::Checked;
        layer->SetLighttable( lighttable );
    }

    GetTreeView()->RequestTreeRefresh(); //needed to display layers previously hidden
}

ECheckBoxState
SOdysseyAnimationLayerRow::GetCellNamesIsChecked() const
{
    return mLayer->ShouldDisplayCellNames() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLayerRow::OnCellNamesCheckStateChanged(ECheckBoxState iState)
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
        layer->SetDisplayCellNames( iState == ECheckBoxState::Checked );
    }

    GetTreeView()->RequestTreeRefresh(); //needed to display layers previously hidden
}

#undef LOCTEXT_NAMESPACE
