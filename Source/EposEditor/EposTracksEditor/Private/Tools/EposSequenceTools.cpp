// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "AnimatedRange.h"
#include "CineCameraActor.h"
#include "Engine/Selection.h"
#include "ISequencer.h"
#include "LevelEditorViewport.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneSubSection.h"
#include "Tracks/MovieScene3DAttachTrack.h"
#include "Tracks/MovieScene3DTransformTrack.h"

#include "ActorHelpers.h"
#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "OdysseyAnimationActor.h"
#include "ScalingComponent.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools"

//---

bool
IsFocusedSequenceSameAs( ISequencer* iSequencer, const UMovieSceneSequence* iSequence )
{
    return iSequencer->GetFocusedMovieSceneSequence() == iSequence;
}

bool
IsFocusedSequenceSameAs( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection )
{
    return iSequencer->GetFocusedMovieSceneSequence() == iSubSection.GetTypedOuter<UMovieSceneSequence>();
}

//---

void
EjectAnyActor()
{
    if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->IsAnyActorLocked() )
    {
        if( GCurrentLevelEditingViewportClient && GCurrentLevelEditingViewportClient->GetViewMode() != VMI_Unknown && GCurrentLevelEditingViewportClient->AllowsCinematicControl() )
        {
            GCurrentLevelEditingViewportClient->SetCinematicActorLock( nullptr );
            GCurrentLevelEditingViewportClient->SetActorLock( nullptr );
            GCurrentLevelEditingViewportClient->bLockedCameraView = false;
            GCurrentLevelEditingViewportClient->ViewFOV = GCurrentLevelEditingViewportClient->FOVAngle;
            GCurrentLevelEditingViewportClient->RemoveCameraRoll();
            GCurrentLevelEditingViewportClient->UpdateViewForLockedActor();
            GCurrentLevelEditingViewportClient->Invalidate();
        }
    }
}

//---

void BoardSequenceTools::UpdateViewRange( ISequencer* iSequencer, TRange<FFrameNumber> iNewRange )
{
    if( iNewRange.IsEmpty() )
        return;

    TRange<double> working_range = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetEditorData().GetWorkingRange(); // iSequencer->GetClampRange() is only inside FSequencer and not ISequencer
    working_range = UE::MovieScene::DilateRange( working_range, 0., ( iNewRange / iSequencer->GetFocusedTickResolution() ).Size<double>() );
    iSequencer->SetClampRange( working_range );

    TRange<double> view_range = iSequencer->GetViewRange();
    view_range = UE::MovieScene::DilateRange( view_range, 0., ( iNewRange / iSequencer->GetFocusedTickResolution() ).Size<double>() );
    iSequencer->SetViewRange( view_range );
}

//---

ShotSequenceTools::cTemporarySwitchInner::cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID )
    : mSequencer( iSequencer )
    , mOriginalId()
    , mOriginalGlobalTime()
{
    mOriginalId = mSequencer.GetFocusedTemplateID();
    if( iInnerID == mOriginalId )
        return;

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mOriginalGlobalTime = ConvertFrameTime( mSequencer.GetGlobalTime().Time, tick_resolution, display_rate );

    UMovieSceneSubSection* subsection = mSequencer.FindSubSection( iInnerID );
    check( subsection );
    mSequencer.FocusSequenceInstance( *subsection );
}

ShotSequenceTools::cTemporarySwitchInner::~cTemporarySwitchInner()
{
    FMovieSceneSequenceID focused_id = mSequencer.GetFocusedTemplateID();
    if( focused_id == mOriginalId )
        return;

    //mSequencer.PopToSequenceInstance( mOriginalId );

    if( mOriginalId == MovieSceneSequenceID::Root )
    {
        //mSequencer.ResetToNewRootSequence( *mSequencer.GetRootMovieSceneSequence() );
        mSequencer.PopToSequenceInstance( mOriginalId );
    }
    else
    {
        UMovieSceneSubSection* subsection = mSequencer.FindSubSection( mOriginalId );
        check( subsection );
        mSequencer.FocusSequenceInstance( *subsection );
    }

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mSequencer.SetGlobalTime( ConvertFrameTime( mOriginalGlobalTime, display_rate, tick_resolution ) );
}

//---

//static
void
BoardSequenceTools::RenameBinding( ISequencer* iSequencer, const UMovieSceneSubSection& iSubSection, FGuid iBinding, FString iNewLabel )
{
    check( iSequencer->GetFocusedMovieSceneSequence()->IsA<UBoardSequence>() );

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, iSubSection, iSequencer->GetFocusedTemplateID() );
    if( !result.mInnerSequence )
        return;

    if( result.mInnerSequence->IsA<UBoardSequence>() )
        return;

    ShotSequenceTools::RenameBinding( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, iBinding, iNewLabel );
}

//static
void
ShotSequenceTools::RenameBinding( ISequencer* iSequencer, FGuid iBinding, FString iNewLabel )
{
    ShotSequenceTools::RenameBinding( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iBinding, iNewLabel );
}

//static
void
ShotSequenceTools::RenameBinding( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FGuid iBinding, FString iNewLabel )
{
    check( iSequence->IsA<UShotSequence>() );

    UMovieScene* movie_scene = iSequence->GetMovieScene();
    FMovieScenePossessable* possessable = movie_scene ? movie_scene->FindPossessable( iBinding ) : nullptr;
    if( !possessable )
        return;

    auto objects = iSequencer.FindBoundObjects( iBinding, iSequenceID );
    AActor* actor = nullptr;
    if( objects.Num() )
        actor = Cast<AActor>( objects[0] );

    //---

    const FScopedTransaction transaction( LOCTEXT( "SetTrackName", "Set Track Name" ) );

    FMovieScenePossessable new_possessable( *possessable );
    new_possessable.SetName( iNewLabel );
    movie_scene->ReplacePossessable( iBinding, new_possessable );

    //---

    if( actor )
        FActorLabelUtilities::RenameExistingActor( actor, new_possessable.GetName() );

    iSequencer.NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
}

//---

TArray<ShotSequenceTools::FBindingAndActorClass> ShotSequenceTools::mDirectBindingsSelectedHistory;

//static
void
BoardSequenceTools::AddSelectedActorToHistory( ISequencer* iSequencer, AActor* iActor )
{
    ShotSequenceTools::AddSelectedActorToHistory( iSequencer, iActor );
}

//static
void
ShotSequenceTools::AddSelectedActorToHistory( ISequencer* iSequencer, AActor* iActor )
{
    if( !iActor )
        return;

    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequenceId = iSequencer->GetFocusedTemplateID();

    if( sequence->IsA<UBoardSequence>() )
    {
        UMovieScene* moviescene = sequence->GetMovieScene();
        UMovieSceneCinematicBoardTrack* board_track = moviescene ? moviescene->FindTrack<UMovieSceneCinematicBoardTrack>() : nullptr;
        TArray<UMovieSceneSection*> sections = board_track ? board_track->GetAllSections() : TArray<UMovieSceneSection*>();
        for( UMovieSceneSection* section : sections )
        {
            UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
            if( !subsection )
                continue;

            FGuid binding = iSequencer->FindCachedObjectId( *iActor, subsection->GetSequenceID() );
            if( binding.IsValid() )
            {
                FBindingAndActorClass entry = { binding, subsection->GetSequenceID(), iActor->GetClass() };
                mDirectBindingsSelectedHistory.RemoveAll( [entry]( const FBindingAndActorClass& iEntry )
                                                          {
                                                              return iEntry.Guid == entry.Guid && iEntry.SequenceId == entry.SequenceId;
                                                          } );
                mDirectBindingsSelectedHistory.Add( entry );
            }
        }
    }
    else if( sequence->IsA<UShotSequence>() )
    {
        FGuid binding = iSequencer->FindCachedObjectId( *iActor, sequenceId );
        if( binding.IsValid() )
        {
            FBindingAndActorClass entry = { binding, sequenceId, iActor->GetClass() };
            mDirectBindingsSelectedHistory.RemoveAll( [entry]( const FBindingAndActorClass& iEntry )
                                                      {
                                                          return iEntry.Guid == entry.Guid && iEntry.SequenceId == entry.SequenceId;
                                                      } );
            mDirectBindingsSelectedHistory.Add( entry );
        }
    }

}

//static
AActor*
BoardSequenceTools::GuessActorToSelect( ISequencer* iSequencer, const FFrameNumber& iFrameNumber )
{
    // This is no more used as there are some times where this selection is auto-empty by the engine
    // so we can't rely on it to always guess the animation/camera to auto-select
    //
    TArray<AActor*> actor_selected;
    //// Store the currently selected actor to guess the type of actor (camera or animation) to auto-select
    //USelection* selection = GEditor->GetSelectedActors();
    //TArray<AActor*> actor_selected;
    //selection->GetSelectedObjects<AActor>( actor_selected );

    AActor* actor_to_select = ShotSequenceTools::GuessActorToSelect( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, actor_selected );
    // If already selected, nothing to do
    //if( actor_selected.Contains( actor_to_select ) )
    //    return nullptr;

    return actor_to_select;
}

//static
AActor*
ShotSequenceTools::GuessActorToSelect( ISequencer* iSequencer, const FFrameNumber& iFrameNumber )
{
    // This is no more used as there are some times where this selection is auto-empty by the engine
    // so we can't rely on it to always guess the animation/camera to auto-select
    //
    TArray<AActor*> actor_selected;
    //// Store the currently selected actor to guess the type of actor (camera or animation) to auto-select
    //USelection* selection = GEditor->GetSelectedActors();
    //TArray<AActor*> actor_selected;
    //selection->GetSelectedObjects<AActor>( actor_selected );

    AActor* actor_to_select = ShotSequenceTools::GuessActorToSelect( iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID(), iFrameNumber, actor_selected );
    // If already selected, nothing to do
    //if( actor_selected.Contains( actor_to_select ) )
    //    return nullptr;

    return actor_to_select;
}

//static
AActor*
ShotSequenceTools::GuessActorToSelect( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber, const TArray<AActor*>& /*iLastSelectedActors*/ )
{
    // See comment above in GuessActorToSelect() to know why iLastSelectedActors is no more used
    //TArray<AActor*> actors = iLastSelectedActors.FilterByPredicate( []( const AActor* iActor )
    //                                                                {
    //                                                                    return iActor->IsA<ACineCameraActor>();
    //                                                                } );

    // Actor may be destroyed, for example:
    // - drag on some shots (to select some animations)
    // - open another map
    // - actors in the cache are all invalid
    mDirectBindingsSelectedHistory.RemoveAll( []( FBindingAndActorClass iEntry )
                                              {
                                                  return !iEntry.Guid.IsValid() || iEntry.SequenceId == MovieSceneSequenceID::Invalid;
                                              } );

    // Auto-select camera if it's the last actor type directly selected by the user
    if( mDirectBindingsSelectedHistory.Num()
        && mDirectBindingsSelectedHistory.Last().ActorClass == ACineCameraActor::StaticClass() )
    //if( actors.Num() )
    {
        if( iSequence->IsA<UBoardSequence>() )
        {
            UMovieSceneSequence* sequence = nullptr;
            FMovieSceneSequenceID sequenceId = MovieSceneSequenceID::Invalid;
            FGuid camera_binding = BoardSequenceHelpers::GetCameraBindingRecursive( *iSequencer, iSequence, iSequenceId, iFrameNumber, &sequence, &sequenceId );
            ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, sequence, sequenceId, camera_binding );

            return camera;
        }
        else if( iSequence->IsA<UShotSequence>() )
        {
            FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceId );
            ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawned( *iSequencer, iSequence, iSequenceId, camera_binding );

            return camera;
        }
    }

    //---

    // Otherwise *always* return an animation (if there is one at the current frame)

    //actors = iLastSelectedActors.FilterByPredicate( []( const AActor* iActor )
    //                                                {
    //                                                    return iActor->IsA<AOdysseyAnimationActor>();
    //                                                } );
    //if( ShotSequenceTools::mDirectActorsSelectedHistory.Num()
    //    && ShotSequenceTools::mDirectActorsSelectedHistory.Last()->IsA<AOdysseyAnimationActor>() )
    //if( actors.Num() )
    {
        TArray<FGuid> unordered_bindings;
        UMovieSceneSequence* sequence = nullptr;
        FMovieSceneSequenceID sequenceId = MovieSceneSequenceID::Invalid;
        if( iSequence->IsA<UBoardSequence>() )
        {
            unordered_bindings = BoardSequenceHelpers::GetAnimationBindingsRecursive( *iSequencer, iSequence, iSequenceId, iFrameNumber, &sequence, &sequenceId );
        }
        else if( iSequence->IsA<UShotSequence>() )
        {
            sequence = iSequence;
            sequenceId = iSequenceId;

            unordered_bindings = ShotSequenceHelpers::GetAnimationBindings( *iSequencer, iSequence, iSequenceId );
        }

        // Animation not found AND no sequence (shot) found, so nothing can be guess
        if( !sequence )
            return nullptr;

        TArray<FGuid> ordered_bindings;
        ShotSequenceTools::SortBindings( unordered_bindings, sequence->GetMovieScene(), &ordered_bindings );

        if( ordered_bindings.Num() )
        {
            int32 max_preferred_index = INDEX_NONE;
            for( FGuid binding : ordered_bindings )
            {
                int32 current_index = mDirectBindingsSelectedHistory.FindLastByPredicate( [binding, sequenceId]( const FBindingAndActorClass& iEntry )
                                                                                          {
                                                                                              return iEntry.Guid == binding && iEntry.SequenceId == sequenceId;
                                                                                          } );
                if( current_index != INDEX_NONE )
                {
                    if( current_index > max_preferred_index )
                        max_preferred_index = current_index;
                }
            }

            AActor* preferred_animation = nullptr;
            if( mDirectBindingsSelectedHistory.IsValidIndex( max_preferred_index ) )
            {
                TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *iSequencer, nullptr /* TODO: must also be stored in historic ... */ , mDirectBindingsSelectedHistory[max_preferred_index].SequenceId, mDirectBindingsSelectedHistory[max_preferred_index].Guid );
                if( animation_actors.Num() )
                    preferred_animation = animation_actors[0];
            }

            if( !preferred_animation )
            {
                TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *iSequencer, sequence, sequenceId, ordered_bindings[0] );
                if( animation_actors.Num() )
                {
                    preferred_animation = animation_actors[0];

                    if( preferred_animation )
                    {
                        FBindingAndActorClass entry = { ordered_bindings[0], sequenceId, preferred_animation->GetClass() };
                        mDirectBindingsSelectedHistory.Add( entry );
                    }
                }
            }

            return preferred_animation;
        }
    }

    //---

    return nullptr;
}

//---

//static
void
ShotSequenceTools::SortBindings( TArray<FGuid> iBindings, UMovieScene* iMovieScene, TArray<FGuid>* oOrderedBindings )
{
    // Find their corresponding scene binding
    TArray<FMovieSceneBinding*> bindings;
    for( int i = 0; i < iBindings.Num(); i++ )
    {
        bindings.Add( iMovieScene->FindBinding( iBindings[i] ) );
    }

    // Sort scene bindings by their sorting order/name
    // (This should match the native sorting of tracks inside shot)
    Algo::StableSort( bindings, [iMovieScene]( const FMovieSceneBinding* iA, const FMovieSceneBinding* iB )
                      {
                          // If at least one of the binding was not already sorted (by drag'n drop in shot), use the name to sort both
                          if( iA->GetSortingOrder() == -1 || iB->GetSortingOrder() == -1 )
                          {
                              FString nameA = iMovieScene->GetObjectDisplayName( iA->GetObjectGuid() ).ToString();
                              FString nameB = iMovieScene->GetObjectDisplayName( iB->GetObjectGuid() ).ToString();

                              return nameA < nameB;
                          }
                          // Otherwise just use the set sorting order
                          else
                              return iA->GetSortingOrder() < iB->GetSortingOrder();
                      } );

    // Get all animations in the gui order
    for( FMovieSceneBinding* ordered_binding : bindings )
    {
        oOrderedBindings->Add( ordered_binding->GetObjectGuid() );
    }
}

//---
//---
//---

static
float
UnwindChannel( const float& OldValue, float NewValue )
{
    while( NewValue - OldValue > 180.0f )
    {
        NewValue -= 360.0f;
    }
    while( NewValue - OldValue < -180.0f )
    {
        NewValue += 360.0f;
    }
    return NewValue;
}
static
FRotator
UnwindRotator( const FRotator& InOld, const FRotator& InNew )
{
    FRotator Result;
    Result.Pitch = UnwindChannel( InOld.Pitch, InNew.Pitch );
    Result.Yaw = UnwindChannel( InOld.Yaw, InNew.Yaw );
    Result.Roll = UnwindChannel( InOld.Roll, InNew.Roll );
    return Result;
}

// From ...\UE_4.26\Engine\Source\Editor\MovieSceneTools\Private\TrackEditors\TransformTrackEditor.cpp
//static
void
ShotSequenceTools::GetTransformKeys( ISequencer& iSequencer, const TOptional<FTransformData>& LastTransform, const FTransformData& CurrentTransform, EMovieSceneTransformChannel ChannelsToKey, UObject* Object, UMovieSceneSection* Section, FGeneratedTrackKeys& OutGeneratedKeys )
{
    UMovieScene3DTransformSection* TransformSection = Cast<UMovieScene3DTransformSection>( Section );
    EMovieSceneTransformChannel TransformMask = TransformSection->GetMask().GetChannels();

    using namespace UE::MovieScene;

    bool bLastVectorIsValid = LastTransform.IsSet();

    // If key all is enabled, for a key on all the channels
    if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll )
    {
        bLastVectorIsValid = false;
        ChannelsToKey = EMovieSceneTransformChannel::All;
    }

    //FBuiltInComponentTypes* BuiltInComponents = FBuiltInComponentTypes::Get();

    //FTransformData RecomposedTransform = RecomposeTransform( CurrentTransform, Object, Section );

    // Set translation keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::TranslationZ );

        if( bLastVectorIsValid )
        {
            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Translation.X, CurrentTransform.Translation.X );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Translation.Y, CurrentTransform.Translation.Y );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Translation.Z, CurrentTransform.Translation.Z );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::TranslationX ) )
        {
            bKeyX = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::TranslationY ) )
        {
            bKeyY = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::TranslationZ ) )
        {
            bKeyZ = false;
        }

        FVector KeyVector = CurrentTransform.Translation;
        //FVector KeyVector = RecomposedTransform.Translation;

        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 0, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 1, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 2, KeyVector.Z, bKeyZ ) );
    }

    // Set rotation keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::RotationZ );

        FRotator KeyRotator = CurrentTransform.Rotation;
        if( bLastVectorIsValid )
        {
            KeyRotator = UnwindRotator( LastTransform->Rotation, CurrentTransform.Rotation );

            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Rotation.Roll, KeyRotator.Roll );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Rotation.Pitch, KeyRotator.Pitch );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Rotation.Yaw, KeyRotator.Yaw );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::RotationX ) )
        {
            bKeyX = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::RotationY ) )
        {
            bKeyY = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::RotationZ ) )
        {
            bKeyZ = false;
        }

        // Do we need to unwind re-composed rotations?
        //KeyRotator = UnwindRotator( CurrentTransform.Rotation, RecomposedTransform.Rotation );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 3, KeyRotator.Roll, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 4, KeyRotator.Pitch, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 5, KeyRotator.Yaw, bKeyZ ) );

    }

    // Set scale keys/defaults
    {
        bool bKeyX = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleX );
        bool bKeyY = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleY );
        bool bKeyZ = EnumHasAnyFlags( ChannelsToKey, EMovieSceneTransformChannel::ScaleZ );

        if( bLastVectorIsValid )
        {
            bKeyX &= !FMath::IsNearlyEqual( LastTransform->Scale.X, CurrentTransform.Scale.X );
            bKeyY &= !FMath::IsNearlyEqual( LastTransform->Scale.Y, CurrentTransform.Scale.Y );
            bKeyZ &= !FMath::IsNearlyEqual( LastTransform->Scale.Z, CurrentTransform.Scale.Z );
        }

        if( iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup && ( bKeyX || bKeyY || bKeyZ ) )
        {
            bKeyX = bKeyY = bKeyZ = true;
        }

        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::ScaleX ) )
        {
            bKeyX = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::ScaleY ) )
        {
            bKeyY = false;
        }
        if( !EnumHasAnyFlags( TransformMask, EMovieSceneTransformChannel::ScaleZ ) )
        {
            bKeyZ = false;
        }

        FVector KeyVector = CurrentTransform.Scale;
        //FVector KeyVector = RecomposedTransform.Scale;
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 6, KeyVector.X, bKeyX ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 7, KeyVector.Y, bKeyY ) );
        OutGeneratedKeys.Add( FMovieSceneChannelValueSetter::Create<FMovieSceneDoubleChannel>( 8, KeyVector.Z, bKeyZ ) );
    }
}

// From ...\UE_4.26\Engine\Source\Editor\MovieSceneTools\Public\KeyframeTrackEditor.h
//static
bool
ShotSequenceTools::AddKeysToSection( ISequencer& iSequencer, UMovieSceneSection* Section, FFrameNumber KeyTime, const FGeneratedTrackKeys& Keys, ESequencerKeyMode KeyMode, EKeyFrameTrackEditorSetDefault SetDefault )
{
    EAutoChangeMode AutoChangeMode = iSequencer.GetAutoChangeMode();

    FMovieSceneChannelProxy& Proxy = Section->GetChannelProxy();

    const bool bSetDefaults = iSequencer.GetAutoSetTrackDefaults() && ( SetDefault != EKeyFrameTrackEditorSetDefault::DoNotSetDefault );

    // The default value is a value for the channel when there are no keyframes. For example, if you add keys and
    // then delete them all, the default value is the value of the channel. In the implementation of ApplyDefault,
    // all the setters check that the default value is only set when there are NO keyframes. So, ApplyDefault needs
    // to be called here in AddKeysToSection BEFORE any keys are added.
    if( bSetDefaults )
    {
        for( const FMovieSceneChannelValueSetter& GeneratedKey : Keys )
        {
            GeneratedKey->ApplyDefault( Section, Proxy, SetDefault );
        }
    }

    bool key_created = false;

    if( KeyMode != ESequencerKeyMode::AutoKey || AutoChangeMode == EAutoChangeMode::AutoKey || AutoChangeMode == EAutoChangeMode::All )
    {
        EMovieSceneKeyInterpolation InterpolationMode = iSequencer.GetKeyInterpolation();

        const bool bKeyEvenIfUnchanged =
            KeyMode == ESequencerKeyMode::ManualKeyForced ||
            iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll ||
            iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyGroup;

        const bool bKeyEvenIfEmpty =
            ( KeyMode == ESequencerKeyMode::AutoKey && AutoChangeMode == EAutoChangeMode::All ) ||
            KeyMode == ESequencerKeyMode::ManualKeyForced;

        for( const FMovieSceneChannelValueSetter& GeneratedKey : Keys )
        {
            key_created |= GeneratedKey->Apply( Section, Proxy, KeyTime, InterpolationMode, bKeyEvenIfUnchanged, bKeyEvenIfEmpty );
        }
    }

    return key_created;
}

//static
void
ShotSequenceTools::UpdateChannel( TSharedPtr<ISequencer> iSequencer, AActor* iActor, const ACineCameraActor* iCamera, EMovieSceneTransformChannel iChannelsToApply )
{
    if( !iSequencer.IsValid() )
        return;

    ISequencer* sequencer = iSequencer.Get();
    UMovieSceneSequence* sequence = sequencer->GetFocusedMovieSceneSequence();
    FMovieSceneSequenceID sequence_id = sequencer->GetFocusedTemplateID();
    FFrameNumber frame = sequencer->GetLocalTime().Time.GetFrame();

    if( sequence->IsA<UBoardSequence>() )
    {
        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequence, sequence_id, frame );
        sequence = result.mInnerSequence;
        sequence_id = result.mInnerSequenceId;
        frame = result.mInnerTime.GetFrame();
    }

    FGuid binding = sequencer->FindObjectId( *iActor, sequence_id );

    // Update the default transform channel to take care of the attach track
    UMovieScene3DTransformTrack* transformTrack = Cast<UMovieScene3DTransformTrack>( sequence->GetMovieScene()->FindTrack<UMovieScene3DTransformTrack>( binding ) );
    if( transformTrack )
    {
        if( transformTrack->GetAllSections().Num() )
        {
            UMovieScene3DTransformSection* transformSection = Cast<UMovieScene3DTransformSection>( transformTrack->GetAllSections()[0] );

            // Set to EKeyGroupMode::KeyGroup, otherwise it will be overwritten by EMovieSceneTransformChannel::All
            // in GetTransformKeys() as (certainly) iSequencer.GetKeyGroupMode() == EKeyGroupMode::KeyAll
            EKeyGroupMode backup_groupmode = sequencer->GetKeyGroupMode();
            sequencer->SetKeyGroupMode( EKeyGroupMode::KeyGroup );

            FTransformData newTransformData( iActor->GetActorTransform() );

            UMovieScene3DAttachTrack* attachTrack = sequence->GetMovieScene()->FindTrack<UMovieScene3DAttachTrack>( binding );
            //bool hasParentActor = iActor->GetParentActor();
            bool hasParentActor = !!iActor->GetAttachParentActor();
            if( EnumHasAnyFlags( iChannelsToApply, EMovieSceneTransformChannel::Translation )
                && ( attachTrack || hasParentActor ) )
            {
                FTransform world_actor_transform = iActor->GetActorTransform();
                FTransform relative_transform = world_actor_transform.GetRelativeTransform( iCamera->GetActorTransform() );

                newTransformData = relative_transform;
            }

            FGeneratedTrackKeys generated_keys;
            ShotSequenceTools::GetTransformKeys( *sequencer, TOptional<FTransformData>(), newTransformData, iChannelsToApply, iActor, transformSection, generated_keys );

            // For now, just update the default values of channels
            // The problem with creating new keys is:
            // - camera has NO keys for its focal length (so a unique default value for all frames)
            // - on frame X, the focal length changes -> new scale for the animation -> create a new key
            // - on frame X+n, the focal length changes -> new scale for the animation -> create a new key
            // BUT the key on frame X won't be relevant anymore as the focal length of the camera is also changed on frame X
            for( const FMovieSceneChannelValueSetter& generated_key : generated_keys )
            {
                generated_key->ApplyDefault( transformSection, transformSection->GetChannelProxy(), EKeyFrameTrackEditorSetDefault::SetDefaultOnAddKeys );
            }
            //bool key_created = ShotSequenceTools::AddKeysToSection( *sequencer, transformSection, sequencer->GetLocalTime().Time.GetFrame(), generated_keys, ESequencerKeyMode::AutoKey, EKeyFrameTrackEditorSetDefault::SetDefaultOnAddKeys );

            sequencer->SetKeyGroupMode( backup_groupmode );
        }
    }
}

//---

//static
bool
ShotSequenceTools::CanMoveAndScaleActor( const AActor* iActor, const ACineCameraActor* iCamera )
{
    if( !iActor || !iCamera )
        return false;

    // Already normalized
    FVector camera_lookat = iCamera->GetActorForwardVector();
    FVector animation_lookat = iActor->GetActorUpVector();

    // Check if the animation (as an infinit mathematical plane) is orthogonal to the camera
    // but the center of the animation is not necessary facing the camera
    if( !FVector::Parallel( camera_lookat, animation_lookat, FMath::Cos( FMath::DegreesToRadians( 0.1f ) ) /* tolerance is less than 0.1° */ ) )
        return false;

    FVector camera_to_animation( iActor->GetActorLocation() - iCamera->GetActorLocation() );
    camera_to_animation.Normalize();

    // Now check if the center of the animation is facing the camera
    if( !camera_lookat.Equals( camera_to_animation ) && !camera_lookat.Equals( -camera_to_animation ) )
        return false;
    // The tolerance between 2 parallels is the cosinus of angle
    // and by default, 2 parallels are considered parallel if the angle is less than 1°
    // and the angle can't be reduce too much due to precision
    // so, just compare the 2 normalized vectors with the default tolerance
    // (in addition, as it is the same base point, coplanar just check parallelism)
    //if( !FVector::Coplanar( iCamera->GetActorLocation(), camera_lookat, iCamera->GetActorLocation(), camera_to_animation, FMath::Cos( FMath::DegreesToRadians( 0.1f ) ) ) )
    //    return false;

    return true;
};

//static
bool
ShotSequenceTools::MoveAndScaleActor( AActor* ioActor, const ACineCameraActor* iCamera, float iNewDistance, EScaleActor iScaleType, TSharedPtr<ISequencer> iSequencer )
{
    if( !ShotSequenceTools::CanMoveAndScaleActor( ioActor, iCamera ) )
        return false;

    if( FMath::IsNearlyZero( iNewDistance ) )
        return false;

    struct FParameterCache
    {
        float mDistanceFromCamera;
        FVector mActorScale;
        FVector mCameraViewSize;
    };

    FParameterCache old_parameter;
    old_parameter.mActorScale = ioActor->GetActorScale3D();
    old_parameter.mDistanceFromCamera = FVector::Distance( iCamera->GetActorLocation(), ioActor->GetActorLocation() );
    old_parameter.mCameraViewSize = ActorHelpers::ComputeSizeOfCameraView( iCamera, old_parameter.mDistanceFromCamera );

    FVector new_camera_view_size = ActorHelpers::ComputeSizeOfCameraView( iCamera, iNewDistance );

    FVector new_animation_location = iCamera->GetActorLocation() + ( ioActor->GetActorLocation() - iCamera->GetActorLocation() ).GetSafeNormal() * iNewDistance;
    ioActor->SetActorLocation( new_animation_location );

    bool update_channels = false;

    switch( iScaleType )
    {
        // Until EScaleActor::kFitToCamera will be removed
        PRAGMA_DISABLE_DEPRECATION_WARNINGS
        case EScaleActor::kFitToCamera:
        PRAGMA_ENABLE_DEPRECATION_WARNINGS
        case EScaleActor::kRelativeScale:
        {
            FVector ratio = new_camera_view_size / old_parameter.mCameraViewSize;
            FVector new_actor_scale = old_parameter.mActorScale * ratio;

            ioActor->SetActorScale3D( new_actor_scale );

            update_channels = true;
        }
        break;

        case EScaleActor::kNo:
            // nothing to do
            break;

        default: checkNoEntry();
    }

    if( iSequencer )
    {
        UpdateChannel( iSequencer, ioActor, iCamera, EMovieSceneTransformChannel::Translation );
        if( update_channels )
            UpdateChannel( iSequencer, ioActor, iCamera, EMovieSceneTransformChannel::Scale );
    }

    return true;
}

//static
bool
ShotSequenceTools::CanFitActorToCameraView( const AActor* iActor, const ACineCameraActor* iCamera )
{
    return CanMoveAndScaleActor( iActor, iCamera );
};

//static
bool
ShotSequenceTools::FitActorToCameraView( AActor* ioActor, const ACineCameraActor* iCamera, TSharedPtr<ISequencer> iSequencer )
{
    if( !CanFitActorToCameraView( ioActor, iCamera ) )
        return false;

    float distance = FVector::Distance( iCamera->GetActorLocation(), ioActor->GetActorLocation() );
    FVector new_camera_view_size = ActorHelpers::ComputeSizeOfCameraView( iCamera, distance );

    FVector scale = new_camera_view_size;
    UScalingComponent* scaling_component = ioActor->FindComponentByClass<UScalingComponent>();
    if( scaling_component )
        scale = scaling_component->ComputeScaleWithScaleAndMargin( new_camera_view_size );

    ioActor->SetActorScale3D( scale );

    UpdateChannel( iSequencer, ioActor, iCamera, EMovieSceneTransformChannel::Scale );

    return true;
}

#undef LOCTEXT_NAMESPACE
