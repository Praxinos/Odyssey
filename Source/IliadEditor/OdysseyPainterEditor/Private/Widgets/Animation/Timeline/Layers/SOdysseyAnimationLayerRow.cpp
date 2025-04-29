// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/Layers/SOdysseyAnimationLayerRow.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableHeader.h"
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
    if (iRow == "LightTable")
    {
        if (iColumn == "Header")
        {
            return GenerateLightTableRowHeaderWidget();
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
SOdysseyAnimationLayerRow::GenerateLightTableRowHeaderWidget()
{
    return SNew(SOdysseyAnimationTimelineLightTableHeader)
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
        const FCheckBoxStyle* lightTableToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Animation.LightTableToggle");

        widgets.Add(
            SNew(SCheckBox)
            .Style(lightTableToggleStyle)
            .OnCheckStateChanged(this, &SOdysseyAnimationLayerRow::OnLightTableCheckStateChanged)
            .IsChecked(this, &SOdysseyAnimationLayerRow::GetLightTableIsChecked)
        );
    }

    widgets.Append(SOdysseyLayerRow::GenerateMainRowHeaderOptionWidgets());

    return widgets;
}

ECheckBoxState
SOdysseyAnimationLayerRow::GetLightTableIsChecked() const
{
    return mLayer->GetLighttable().bIsActivated ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyAnimationLayerRow::OnLightTableCheckStateChanged(ECheckBoxState iState)
{
    FOdysseyAnimationLightTable lighttable = mLayer->GetLighttable();
    lighttable.bIsActivated = iState == ECheckBoxState::Checked;
    mLayer->SetLighttable(lighttable);
    GetTreeView()->RequestTreeRefresh(); //needed to display layers previously hidden
}

#undef LOCTEXT_NAMESPACE
