// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/SOdysseyAnimationPlaybackControls.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "LayerStack/Cells/OdysseyAnimationCellSelection.h"

void
SOdysseyAnimationPlaybackControls::Construct(const FArguments& InArgs)
{
    mAnimation = InArgs._Animation;
    mPlayer = InArgs._Player;
    mPlaybackFramesPerSecond = InArgs._PlaybackFramesPerSecond;

    ChildSlot
    [
        SNew(SHorizontalBox)
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Beginning"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnBeginningClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PreviousKey"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPreviousKeyClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Previous"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPreviousClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.PlayBackward"))
            .Visibility(this, &SOdysseyAnimationPlaybackControls::GetPlayBackwardButtonVisibility)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPlayBackwardClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Stop"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnStopClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Play"))
            .Visibility(this, &SOdysseyAnimationPlaybackControls::GetPlayButtonVisibility)
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnPlayClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Next"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnNextClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.NextKey"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnNextKeyClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.End"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnEndClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Visibility(this, &SOdysseyAnimationPlaybackControls::GetLoopingButtonVisibility)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.Looping"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnLoopClicked)
        ]
        +SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SButton)
            .Visibility(this, &SOdysseyAnimationPlaybackControls::GetNotLoopingButtonVisibility)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("PlaybackControls.NotLooping"))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            .OnClicked(this, &SOdysseyAnimationPlaybackControls::OnLoopClicked)
        ]
    ];
}

bool
SOdysseyAnimationPlaybackControls::IsPlayingForward() const
{
    if (!mPlayer)
        return false;

    return mPlayer->Status == EOdysseyAnimationPlayerStatus::Playing && !mPlayer->IsBackward();
}

bool
SOdysseyAnimationPlaybackControls::IsPlayingBackward() const
{
    if (!mPlayer)
        return false;

    return mPlayer->Status == EOdysseyAnimationPlayerStatus::Playing && mPlayer->IsBackward();
}


EVisibility
SOdysseyAnimationPlaybackControls::GetPlayButtonVisibility() const
{
    return IsPlayingForward() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetPlayBackwardButtonVisibility() const
{
    return IsPlayingBackward() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetLoopingButtonVisibility() const
{
    if (!mPlayer)
        return EVisibility::Collapsed;

    return mPlayer->IsLooping ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetNotLoopingButtonVisibility() const
{
    if (!mPlayer)
        return EVisibility::Collapsed;

    return mPlayer->IsLooping ? EVisibility::Collapsed : EVisibility::Visible;
}

FReply
SOdysseyAnimationPlaybackControls::OnPlayClicked()
{
    TArray<UOdysseyAnimationCell*> selectedCells = mAnimation->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        mPlayer->SetFrameRange(TOptional<FInt32Range>());
    }
    else
    {
        TArray<FInt32Range> ranges;
        for (UOdysseyAnimationCell* cell : selectedCells)
        {
            ranges.Add(cell->GetFrameRange());
        }

        FInt32Range range = FInt32Range::Hull(ranges);
        mPlayer->SetFrameRange(range);
    }
    mPlayer->Play(false);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPlayBackwardClicked()
{
    TArray<UOdysseyAnimationCell*> selectedCells = mAnimation->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        mPlayer->SetFrameRange(TOptional<FInt32Range>());
    }
    else
    {
        TArray<FInt32Range> ranges;
        for (UOdysseyAnimationCell* cell : selectedCells)
        {
            ranges.Add(cell->GetFrameRange());
        }

        FInt32Range range = FInt32Range::Hull(ranges);
        mPlayer->SetFrameRange(range);
    }
    mPlayer->Play(true);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnStopClicked()
{
    mPlayer->Stop();
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnBeginningClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), 0);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnEndClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), mAnimation->GetFrameRange().GetUpperBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPreviousClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), mAnimation->CurrentFrame - 1);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnNextClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), mAnimation->CurrentFrame + 1);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPreviousKeyClicked()
{
    UOdysseyAnimation* animation = mAnimation;
    if (!animation)
        return FReply::Handled();

    UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer);

    if (layer->GetCells().IsEmpty())
        return FReply::Handled();

    FInt32Range range = layer->GetFrameRange();
    int index = INDEX_NONE;
    if (animation->CurrentFrame > range.GetUpperBoundValue())
    {
        index = layer->GetCells().Num() - 1;
    }
    else if (animation->CurrentFrame < range.GetLowerBoundValue())
    {
        index = 0;
    }
    else
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return FReply::Handled();

        index = cell->IndexInLayer - 1;
        if (index < 0)
            return FReply::Handled();
    }

    UOdysseyAnimationCell* cell = layer->GetCells()[index];
    if (!cell)
        return FReply::Handled();

    range = cell->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), range.GetLowerBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnNextKeyClicked()
{
    UOdysseyAnimation* animation = mAnimation;
    if (!animation)
        return FReply::Handled();

    UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer);

    if (layer->GetCells().IsEmpty())
        return FReply::Handled();

    int index = INDEX_NONE;
    FInt32Range range = layer->GetFrameRange();
    if (animation->CurrentFrame > range.GetUpperBoundValue())
    {
        index = layer->GetCells().Num() - 1;
    }
    else if (animation->CurrentFrame < range.GetLowerBoundValue())
    {
        index = 0;
    }
    else
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return FReply::Handled();

        index = cell->IndexInLayer + 1;
        if (index == layer->GetCells().Num())
            return FReply::Handled();
    }

    UOdysseyAnimationCell* cell = layer->GetCells()[index];
    if (!cell)
        return FReply::Handled();

    range = cell->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), range.GetLowerBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnLoopClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mPlayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping), !mPlayer->IsLooping);
    return FReply::Handled();
}
