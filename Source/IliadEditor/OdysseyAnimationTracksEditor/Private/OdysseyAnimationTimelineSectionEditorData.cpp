// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTimelineSectionEditorData.h"

#include "MovieScene.h"
#include "MovieSceneSequence.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyRenderingAbility.h"
#include "OdysseyAnimationLayerStackUserData.h"

UOdysseyAnimationTimelineSectionEditorData* UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(UOdysseyAnimationTimelineSection& Section)
{
    UOdysseyAnimationTimelineSectionEditorData* Data = Cast<UOdysseyAnimationTimelineSectionEditorData>(Section.GetEditorData());
    if (!IsValid(Data) || Data->GetOuter() != &Section)
    {
        Data = NewObject<UOdysseyAnimationTimelineSectionEditorData>(&Section, NAME_None, RF_Transient);
        Section.SetEditorData(Data);
        Data->Initialize();
    }
    return Data;
}

void UOdysseyAnimationTimelineSectionEditorData::Initialize()
{
    UOdysseyAnimationTimelineSection* Section = CastChecked<UOdysseyAnimationTimelineSection>(GetOuter());
    AnimationCutChannel.SetParentSection(Section);
    Section->OnSignatureChanged().AddUObject(this, &UOdysseyAnimationTimelineSectionEditorData::OnSectionChanged);
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationTimelineSectionEditorData::OnAnimationChanged);
    RebuildAnimationCutChannel();
}

void UOdysseyAnimationTimelineSectionEditorData::BeginDestroy()
{
    UOdysseyAnimationTimelineSection* Section = Cast<UOdysseyAnimationTimelineSection>(GetOuter());
    if (Section) { Section->OnSignatureChanged().RemoveAll(this); }
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    Super::BeginDestroy();
}

FOdysseyAnimationCutChannel& UOdysseyAnimationTimelineSectionEditorData::GetAnimationCutChannel()
{
    return AnimationCutChannel;
}

const FOdysseyAnimationCutChannel& UOdysseyAnimationTimelineSectionEditorData::GetAnimationCutChannel() const
{
    return AnimationCutChannel;
}

FSimpleMulticastDelegate& UOdysseyAnimationTimelineSectionEditorData::OnAnimationCutChannelChanged()
{
    return CutChannelChanged;
}

void UOdysseyAnimationTimelineSectionEditorData::OnSectionChanged()
{
    RebuildAnimationCutChannel();
}

void UOdysseyAnimationTimelineSectionEditorData::OnAnimationChanged(const FOdysseyRenderingChangedEvent& Event)
{
    if (bLockChannelRebuild || Event.IsInteractive() || Event.GetType() != FOdysseyRenderingChangedEvent::kCompositionChange) { return; }
    UOdysseyAnimationTimelineSection* Section = CastChecked<UOdysseyAnimationTimelineSection>(GetOuter());
    UOdysseyAnimation* Animation = Section->GetAnimation();
    if (!Animation) { return; }
    const FGuid Id = Event.GetId();
    UOdysseyLayerStack* Stack = UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation);
    bool bRelevant = Id == Animation->GetRenderingId() || (Stack && Id == Stack->GetRenderingId())
        || (Stack && Stack->GetLayerRoot() && Id == Stack->GetLayerRoot()->GetRenderingId());
    if (Stack && !bRelevant)
    {
        for (UOdysseyLayer* Layer : Stack->GetLayers())
        {
            if (Id == Layer->GetRenderingId()) { bRelevant = true; break; }
            UOdysseyAnimationLayer* AnimationLayer = Cast<UOdysseyAnimationLayer>(Layer);
            if (!AnimationLayer) { continue; }
            for (UOdysseyAnimationCell* Cell : AnimationLayer->GetCells())
            {
                if (Cell && Id == Cell->GetRenderingId()) { bRelevant = true; break; }
            }
            if (bRelevant) { break; }
        }
    }
    if (bRelevant) { RebuildAnimationCutChannel(); }
}

void UOdysseyAnimationTimelineSectionEditorData::UpdateAnimationCutChannel(const TArray<FKeyHandle>& KeyHandles, EPropertyChangeType::Type ChangeType)
{
    if (KeyHandles.IsEmpty()) { return; }
    TGuardValue<bool> Updating(bLockChannelRebuild, true);
    AnimationCutChannel.Update(KeyHandles, ChangeType);
}

void UOdysseyAnimationTimelineSectionEditorData::RebuildAnimationCutChannel()
{
    if (bLockChannelRebuild) { return; }
    TGuardValue<bool> Updating(bLockChannelRebuild, true);
    UOdysseyAnimationTimelineSection* Section = CastChecked<UOdysseyAnimationTimelineSection>(GetOuter());

    AnimationCutChannel.Reset();

    UMovieSceneSequence* sequence = Section->GetTypedOuter<UMovieSceneSequence>();
    if( !sequence )
    {
        CutChannelChanged.Broadcast();
        return;
    }

    UOdysseyLayerStack* layer_stack = Section->GetAnimation() ? UOdysseyAnimationLayerStackUserData::GetLayerStack(Section->GetAnimation()) : nullptr;
    TArray<UOdysseyLayer*> layers = layer_stack ? layer_stack->GetLayers() : TArray<UOdysseyLayer*>();
    TSet<UOdysseyAnimationLayer*> animation_layers;
    for( UOdysseyLayer* layer : layers )
        animation_layers.Add( Cast<UOdysseyAnimationLayer>( layer ) );
    animation_layers.Remove( nullptr );

    for( UOdysseyAnimationLayer* layer : animation_layers )
    {
        UOdysseyAnimationCell* previous_cell = nullptr;
        TArray<UOdysseyAnimationCell*> cells_unsafe = layer->GetCells();
        TArray<UOdysseyAnimationCell*> cells = cells_unsafe.FilterByPredicate( []( const UOdysseyAnimationCell* iCell )
                                                                           {
                                                                               return !!iCell;
                                                                           } );
        for( UOdysseyAnimationCell* cell : cells )
        {
            {
                FAnimationCutEntry animationcutentry( previous_cell, cell );
                FFrameNumber frame_in_timeline = animationcutentry.GetFrameReference();
                FFrameNumber frame_in_sequence = Section->ConvertFrameFromTimelineToSequence( frame_in_timeline );

                int32 index = AnimationCutChannel.GetData().FindKey( frame_in_sequence );
                if( index != INDEX_NONE )
                {
                    AnimationCutChannel.GetData().GetValues()[index].Value.AddNewEntry( animationcutentry );
                }
                else
                {
                    FAnimationCut new_animationcut;
                    new_animationcut.AddNewEntry( animationcutentry );

                    AnimationCutChannel.GetData().AddKey( frame_in_sequence, FOdysseyAnimationCutValue{ new_animationcut } );
                }
            }

            previous_cell = cell;

            if( cell == cells.Last() )
            {
                FAnimationCutEntry animationcutentry( previous_cell, nullptr );
                FFrameNumber frame_in_timeline = animationcutentry.GetFrameReference();
                FFrameNumber frame_in_sequence = Section->ConvertFrameFromTimelineToSequence( frame_in_timeline );

                int32 index = AnimationCutChannel.GetData().FindKey( frame_in_sequence );
                if( index != INDEX_NONE )
                {
                    AnimationCutChannel.GetData().GetValues()[index].Value.AddNewEntry( animationcutentry );
                }
                else
                {
                    FAnimationCut new_animationcut;
                    new_animationcut.AddNewEntry( animationcutentry );

                    AnimationCutChannel.GetData().AddKey( frame_in_sequence, FOdysseyAnimationCutValue{ new_animationcut } );
                }
            }
        }
    }

    CutChannelChanged.Broadcast();
}
