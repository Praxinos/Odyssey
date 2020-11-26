// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Board/BoardSequence.h"

#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"
#include "Sections/MovieSceneSubSection.h"
#include "MovieSceneTimeHelpers.h"
#include "Tracks/MovieSceneFadeTrack.h"
#include "Tracks/MovieSceneLevelVisibilityTrack.h"
#include "Tracks/MovieSceneAudioTrack.h"

#include "BoardHelpers.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Helpers/SectionsHelpersShift.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h" //TMP

//---

UBoardSequence::UBoardSequence(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , MovieScene(nullptr)
{
    //bParentContextsAreSignificant = true;
}

void UBoardSequence::Initialize()
{
    MovieScene = NewObject<UMovieScene>(this, NAME_None, RF_Transactional);

    MovieScene->SetEvaluationType(EMovieSceneEvaluationType::WithSubFrames);

    FFrameRate TickResolution(24000, 1);
    MovieScene->SetTickResolutionDirectly(TickResolution);

    FFrameRate DisplayRate(24, 1);
    MovieScene->SetDisplayRate(DisplayRate);
}

void UBoardSequence::BindPossessableObject(const FGuid& ObjectId, UObject& PossessedObject, UObject* Context)
{
    MovieScene->RemovePossessable( ObjectId );
}

bool UBoardSequence::CanPossessObject(UObject& Object, UObject* InPlaybackContext) const
{
    return false;
}

void UBoardSequence::LocateBoundObjects(const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects) const
{
}

UMovieScene* UBoardSequence::GetMovieScene() const
{
    return MovieScene;
}

UObject* UBoardSequence::GetParentObject(UObject* Object) const
{
    return nullptr;
}

void UBoardSequence::UnbindPossessableObjects(const FGuid& ObjectId)
{
}

void UBoardSequence::UnbindObjects(const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context)
{
}

void UBoardSequence::UnbindInvalidObjects(const FGuid& ObjectId, UObject* Context)
{
}

#if WITH_EDITOR

ETrackSupport
UBoardSequence::IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const
{
    if( InTrackClass == UMovieSceneCinematicBoardTrack::StaticClass() ||
        InTrackClass == UMovieSceneSingleCameraCutTrack::StaticClass() || //TMP
        InTrackClass == UMovieSceneAudioTrack::StaticClass() ||
        InTrackClass == UMovieSceneFadeTrack::StaticClass() ||
        InTrackClass == UMovieSceneLevelVisibilityTrack::StaticClass() )
    {
        return ETrackSupport::Supported;
    }

    return ETrackSupport::NotSupported;
}

//FText UBoardSequence::GetDisplayName() const
//{
//  return UMovieSceneSequence::GetDisplayName();
//}
//
//void UBoardSequence::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
//{
//  Super::GetAssetRegistryTags(OutTags);
//
//  if (BoundActorClass != nullptr)
//  {
//      FAssetRegistryTag Tag("BoundActorClass", BoundActorClass->GetName(), FAssetRegistryTag::TT_Alphabetical);
//      OutTags.Add(Tag);
//  }
//  else
//  {
//      OutTags.Emplace("BoundActorClass", "(None)", FAssetRegistryTag::TT_Alphabetical);
//  }
//}
//
//void UBoardSequence::GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const
//{
//  Super::GetAssetRegistryTagMetadata(OutMetadata);
//
//  OutMetadata.Add(
//      "BoundActorClass",
//      FAssetRegistryTagMetadata()
//          .SetDisplayName(NSLOCTEXT("TemplateSequence", "BoundActorClass_Label", "Bound Actor Class"))
//          .SetTooltip(NSLOCTEXT("TemplateSequence", "BoundActorClass_Tooltip", "The type of actor bound to this template sequence"))
//      );
//}

#endif

//---

bool
UBoardSequence::IsResizable() const //override
{
    return !GetMovieScene() || GetMovieScene()->GetMasterTracks().Num() == 0 && GetMovieScene()->GetBindings().Num() == 0;
}

void
UBoardSequence::Resize( int32 iNewDuration ) //override
{
    //check( IsResizable() ); // I don't know if SectionResized() should call this function recursively, if not, the line can be uncomment

    auto new_range = TRange<FFrameNumber>( 0, iNewDuration );

    UMovieScene* movie_scene = GetMovieScene();
    if( !movie_scene )
        return;

    movie_scene->SetPlaybackRange( new_range );
}

//---

void
UBoardSequence::SectionAddedOrRemoved( UMovieSceneSection* iSection ) //override
{
    ResizeParentSequenceRecursively();
}

void
UBoardSequence::SectionResized( UMovieSceneSection* iSection ) //override
{
    ResizeChildSequence( iSection );
    ResizeParentSequenceRecursively();
}

void
UBoardSequence::ResizeChildSequence( UMovieSceneSection* iSection )
{
    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( iSection );
    if( !subsection )
        return;

    UEposMovieSceneSequence* subsequence = Cast<UEposMovieSceneSequence>( subsection->GetSequence() );
    if( !subsequence )
        return;

    subsequence->Resize( UE::MovieScene::DiscreteSize( subsection->GetTrueRange() ) );
}

void
UBoardSequence::ResizeParentSequenceRecursively()
{
    TArray< UEposMovieSceneSequence* > parents = BoardHelpers::FindParents( this );
    Algo::Reverse( parents ); // this > childN > ... > child1 > Root

    UEposMovieSceneSequence* child_sequence = 0;
    for( auto parent_sequence : parents )
    {
        if( !child_sequence )
        {
            child_sequence = parent_sequence;
            continue;
        }

        UMovieSceneSection* parent_section = BoardHelpers::FindParentSectionOfSequence( parent_sequence, child_sequence );
        if( !parent_section )
            break;

        int32 child_full_duration = UE::MovieScene::DiscreteSize( parent_section->GetTrueRange() );

        UMovieSceneTrack* child_track = GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
        if( child_track && child_track->GetAllSections().Num() )
        {
            auto child_full_range = TRange<FFrameNumber>( child_track->GetAllSections()[0]->GetInclusiveStartFrame(), child_track->GetAllSections().Last()->GetExclusiveEndFrame() );
            child_full_duration = UE::MovieScene::DiscreteSize( child_full_range );
        }

        auto range = parent_section->GetTrueRange();
        auto new_range = TRange<FFrameNumber>( range.GetLowerBoundValue(), range.GetLowerBoundValue() + child_full_duration );
        parent_section->SetRange( new_range );

        //---

        UMovieSceneTrack* parent_track = parent_section->GetTypedOuter<UMovieSceneTrack>();
        check( parent_track );

        SectionsHelpersShift::OrganizeSections( parent_track->GetAllSections() );

        auto new_full_range = TRange<FFrameNumber>( parent_track->GetAllSections()[0]->GetInclusiveStartFrame(), parent_track->GetAllSections().Last()->GetExclusiveEndFrame() );
        parent_sequence->Resize( UE::MovieScene::DiscreteSize( new_full_range ) );

        //---

        child_sequence = parent_sequence;
    }
}

#ifdef WITH_EDITOR

FLinearColor
UBoardSequence::GetColorTint() const //override
{
    return FLinearColor( 0.5, 0, 0, 0.5 );
}

#endif
