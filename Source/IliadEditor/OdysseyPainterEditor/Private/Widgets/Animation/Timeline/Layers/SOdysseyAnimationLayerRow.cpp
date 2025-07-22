// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLighttableHeader.h"
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

TArray<TSharedPtr<SWidget>>
SOdysseyAnimationLayerRow::GenerateMainRowHeaderOptionWidgets()
{
    TArray<TSharedPtr<SWidget>> widgets;

    if (mLayer->HasLighttable())
    {
        const FCheckBoxStyle* lighttableToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Animation.LighttableToggle");

        widgets.Add(
            SNew(SCheckBox)
            .Style(lighttableToggleStyle)
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
    FOdysseyLighttable lighttable = mLayer->GetLighttable();
    lighttable.bIsActivated = iState == ECheckBoxState::Checked;
    mLayer->SetLighttable(lighttable);
    GetTreeView()->RequestTreeRefresh(); //needed to display layers previously hidden
}

#undef LOCTEXT_NAMESPACE
