// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/LighttableTools.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorSupportDelegates.h"
#include "Engine/Texture2D.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"
#include "MovieSceneSequenceVisitor.h"
#include "ISequencer.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "Sections/MovieSceneSubSection.h"

#include "EposMovieSceneSequence.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "LighttableTools"

//---

//static
void
LighttableTools::Activate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    Activate( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
void
LighttableTools::Deactivate( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    Deactivate( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//static
int8
LighttableTools::GetState( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iAnimationBinding )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return -1;

    return GetState( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iAnimationBinding );
}

//---

//static
void
LighttableTools::Deactivate( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    Deactivate( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBinding );
}

//static
void
LighttableTools::Activate( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    Activate( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBinding );
}

//static
int8
LighttableTools::GetState( ISequencer* iSequencer, FGuid iAnimationBinding )
{
    return GetState( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iAnimationBinding );
}

//---

//static
void
LighttableTools::Deactivate( ISequencer* iSequencer )
{
    if( !iSequencer )
        return;

    struct FBindingVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitObjectBinding( const FMovieSceneBinding& iBinding, const UE::MovieScene::FSubSequenceSpace& iLocalSpace ) override
        {
            const FMovieSceneSequenceHierarchy* Hierarchy = mSequencer->GetSharedPlaybackState()->GetHierarchy();
            UMovieSceneSequence* subsequence = Hierarchy->FindSubSequence( iLocalSpace.SequenceID );

            if( iLocalSpace.SequenceID == MovieSceneSequenceID::Root )
                subsequence = mSequencer->GetRootMovieSceneSequence();

            LighttableTools::Deactivate( *mSequencer, subsequence, iLocalSpace.SequenceID, iBinding.GetObjectGuid() );
        }

        ISequencer* mSequencer;
    };

    //---

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;
    params.bVisitObjectBindings = true;

    FBindingVisitor visitor;
    visitor.mSequencer = iSequencer;

    // Visit all tracks
    VisitSequence( iSequencer->GetRootMovieSceneSequence(), params, visitor );
}

//---

//static
void
LighttableTools::Activate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iSequencer, iSequence, iSequenceID, iAnimationBinding );

    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
    {
        UOdysseyAnimation* animation = section->GetAnimation();

        UOdysseyLayerStack* layer_stack = animation->GetLayerStack();
        TArray<UOdysseyLayer*> layers = layer_stack->GetLayers();
        for( UOdysseyLayer* layer : layers )
        {
            FOdysseyLighttable lighttable = layer->GetLighttable();
            lighttable.bIsActivated = true;
            layer->SetLighttable( lighttable );
        }
    }
}

//static
void
LighttableTools::Deactivate( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    if( !epos_sequence )
    {
        checkf( false, TEXT( "iSequence is certainly a LevelSequence, manage it" ) );
        return;
    }

    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iSequencer, iSequence, iSequenceID, iAnimationBinding );

    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
    {
        UOdysseyAnimation* animation = section->GetAnimation();

        UOdysseyLayerStack* layer_stack = animation->GetLayerStack();
        TArray<UOdysseyLayer*> layers = layer_stack->GetLayers();
        for( UOdysseyLayer* layer : layers )
        {
            FOdysseyLighttable lighttable = layer->GetLighttable();
            lighttable.bIsActivated = false;
            layer->SetLighttable( lighttable );
        }
    }
}

//---

//static
int8
LighttableTools::GetState( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iSequencer, iSequence, iSequenceID, iAnimationBinding );

    int32 total_drawing = 0;
    int32 total_drawing_on = 0;
    int32 total_drawing_off = 0;

    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
    {
        UOdysseyAnimation* animation = section->GetAnimation();

        UOdysseyLayerStack* layer_stack = animation->GetLayerStack();
        TArray<UOdysseyLayer*> layers = layer_stack->GetLayers();
        for( UOdysseyLayer* layer : layers )
        {
            FOdysseyLighttable lighttable = layer->GetLighttable();

            total_drawing++;

            if( lighttable.bIsActivated )
                total_drawing_on++;
            else
                total_drawing_off++;
        }
    }

    if( !total_drawing )
        return -1;

    if( total_drawing == total_drawing_on )
        return 1;
    else if( total_drawing == total_drawing_off )
        return 0;
    else
        return -1;
}

//TODO: certainly remove IsOn/IsOff and replace them by GetState in the code

//static
bool
LighttableTools::IsOn( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( iSequencer, iSequence, iSequenceID, iAnimationBinding );

    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
    {
        UOdysseyAnimation* animation = section->GetAnimation();

        UOdysseyLayerStack* layer_stack = animation->GetLayerStack();
        TArray<UOdysseyLayer*> layers = layer_stack->GetLayers();
        for( UOdysseyLayer* layer : layers )
        {
            FOdysseyLighttable lighttable = layer->GetLighttable();

            if( lighttable.bIsActivated )
                return true;
        }
    }

    return false;
}

//static
bool
LighttableTools::IsOff( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iAnimationBinding )
{
    return !IsOn( iSequencer, iSequence, iSequenceID, iAnimationBinding );
}

#undef LOCTEXT_NAMESPACE
