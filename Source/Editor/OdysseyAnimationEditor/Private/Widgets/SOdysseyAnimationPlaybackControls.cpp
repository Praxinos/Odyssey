// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationPlaybackControls.h"
#include "OdysseyStyleSet.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "UObject/OdysseyObjectEditorUtils.h"

void
SOdysseyAnimationPlaybackControls::Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension)
{
    mExtension = iExtension;
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
    if (!mExtension->Player())
        return false;

    return mExtension->Player()->Status == EOdysseyAnimationPlayerStatus::Playing && !mExtension->Player()->IsBackward();
}

bool
SOdysseyAnimationPlaybackControls::IsPlayingBackward() const
{
    if (!mExtension->Player())
        return false;
        
    return mExtension->Player()->Status == EOdysseyAnimationPlayerStatus::Playing && mExtension->Player()->IsBackward();
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
    if (!mExtension->Player())
        return EVisibility::Collapsed;

    return mExtension->Player()->IsLooping ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility
SOdysseyAnimationPlaybackControls::GetNotLoopingButtonVisibility() const
{
    if (!mExtension->Player())
        return EVisibility::Collapsed;

    return mExtension->Player()->IsLooping ? EVisibility::Collapsed : EVisibility::Visible;
}

FReply
SOdysseyAnimationPlaybackControls::OnPlayClicked()
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return FReply::Unhandled();

    UOdysseyLayerStack* layerStack = animation->GetLayerStack();
    if (!layerStack)
        return FReply::Unhandled();

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!layer)
        return FReply::Unhandled();

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        mExtension->Player()->SetFrameRange(TOptional<FInt32Range>());
    }
    else
    {
        TArray<FInt32Range> ranges;
        for (UOdysseyAnimationCell* cell : selectedCells)
        {
            ranges.Add(cell->GetFrameRange());
        }
    
        FInt32Range range = FInt32Range::Hull(ranges);
        mExtension->Player()->SetFrameRange(range);
    }
    mExtension->Player()->Play(false);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPlayBackwardClicked()
{
    UOdysseyAnimation* animation = mExtension->Animation();
    if (!animation)
        return FReply::Unhandled();

    UOdysseyLayerStack* layerStack = animation->GetLayerStack();
    if (!layerStack)
        return FReply::Unhandled();

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!layer)
        return FReply::Unhandled();

    TArray<UOdysseyAnimationCell*> selectedCells = mExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        mExtension->Player()->SetFrameRange(TOptional<FInt32Range>());
    }
    else
    {
        TArray<FInt32Range> ranges;
        for (UOdysseyAnimationCell* cell : selectedCells)
        {
            ranges.Add(cell->GetFrameRange());
        }
    
        FInt32Range range = FInt32Range::Hull(ranges);
        mExtension->Player()->SetFrameRange(range);
    }
    mExtension->Player()->Play(true);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnStopClicked()
{
    mExtension->Player()->Stop();
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnBeginningClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), 0);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnEndClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), mExtension->Animation()->GetFrameRange().GetUpperBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPreviousClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), mExtension->Animation()->CurrentFrame - 1);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnNextClicked()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), mExtension->Animation()->CurrentFrame + 1);
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnPreviousKeyClicked()
{
    UOdysseyAnimation* animation = mExtension->Animation();
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
    range = cell->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), range.GetLowerBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnNextKeyClicked()
{
    UOdysseyAnimation* animation = mExtension->Animation();
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
    range = cell->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), range.GetLowerBoundValue());
    return FReply::Handled();
}

FReply
SOdysseyAnimationPlaybackControls::OnLoopClicked()
{    
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Player(), GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping), !mExtension->Player()->IsLooping);
    return FReply::Handled();
}
