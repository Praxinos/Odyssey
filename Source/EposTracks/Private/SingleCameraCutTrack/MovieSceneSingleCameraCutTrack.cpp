// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"

#include "Tracks/MovieSceneSpawnTrack.h"
#include "MovieScene.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "Evaluation/MovieSceneEvaluationTrack.h"
#include "Compilation/MovieSceneCompilerRules.h"
#include "IMovieSceneTracksModule.h"

#include "EposMovieSceneSequence.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "MovieSceneSingleCameraCutTrack"

/* UMovieSceneCameraCutTrack interface
 *****************************************************************************/
UMovieSceneSingleCameraCutTrack::UMovieSceneSingleCameraCutTrack( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor(120, 120, 120, 65);
#endif

    // By default, don't evaluate camera cuts in pre and postroll
    EvalOptions.bEvaluateInPreroll = EvalOptions.bEvaluateInPostroll = false;

    SupportedBlendTypes.Add(EMovieSceneBlendType::Absolute);
}

UMovieSceneSingleCameraCutSection* UMovieSceneSingleCameraCutTrack::AddNewSingleCameraCut(const FMovieSceneObjectBindingID& CameraBindingID, FFrameNumber StartTime)
{
    Modify();

    FFrameNumber NewSectionEndTime = FindEndTimeForCameraCut(StartTime);
    check( StartTime <= NewSectionEndTime );

    // If there's an existing section, just swap the camera guid
    UMovieSceneSingleCameraCutSection* ExistingSection = nullptr;
    for (auto Section : Sections)
    {
        if (Section->HasStartFrame() && Section->HasEndFrame() && Section->GetInclusiveStartFrame() == StartTime && Section->GetExclusiveEndFrame() == NewSectionEndTime)
        {
            ExistingSection = Cast<UMovieSceneSingleCameraCutSection>(Section);
            break;
        }
    }

    UMovieSceneSingleCameraCutSection* NewSection = ExistingSection;
    if (ExistingSection != nullptr)
    {
        ExistingSection->SetCameraBindingID(CameraBindingID);
    }
    else
    {
        NewSection = NewObject<UMovieSceneSingleCameraCutSection>(this, NAME_None, RF_Transactional);
        NewSection->SetRange(TRange<FFrameNumber>(StartTime, NewSectionEndTime));
        NewSection->SetCameraBindingID(CameraBindingID);

        AddSection(*NewSection);
    }

    // When a new CameraCut is added, sort all CameraCuts to ensure they are in the correct order
    MovieSceneHelpers::SortConsecutiveSections(Sections);

    // Once CameraCuts are sorted fixup the surrounding CameraCuts to fix any gaps
    MovieSceneHelpers::FixupConsecutiveSections(Sections, *NewSection, false);

    UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
    check( outer_sequence );
    outer_sequence->SectionAddedOrRemoved( nullptr );

    return NewSection;
}


/* UMovieSceneTrack interface
 *****************************************************************************/

void UMovieSceneSingleCameraCutTrack::AddSection(UMovieSceneSection& Section)
{
    if (UMovieSceneSingleCameraCutSection* CutSection = Cast<UMovieSceneSingleCameraCutSection>(&Section))
    {
        if( Sections.Num() >= 1 )
            Sections.RemoveAt( 0 );

        check( !Sections.Num() );

        Sections.Add(CutSection);
    }
}

bool UMovieSceneSingleCameraCutTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
    return SectionClass == UMovieSceneSingleCameraCutSection::StaticClass();
}

UMovieSceneSection* UMovieSceneSingleCameraCutTrack::CreateNewSection()
{
    return NewObject<UMovieSceneSingleCameraCutSection>(this, NAME_None, RF_Transactional);
}

bool UMovieSceneSingleCameraCutTrack::SupportsMultipleRows() const
{
    return false;
}

EMovieSceneTrackEasingSupportFlags UMovieSceneSingleCameraCutTrack::SupportsEasing(FMovieSceneSupportsEasingParams& Params) const
{
    return EMovieSceneTrackEasingSupportFlags::None;
}

const TArray<UMovieSceneSection*>& UMovieSceneSingleCameraCutTrack::GetAllSections() const
{
    return Sections;
}

bool UMovieSceneSingleCameraCutTrack::HasSection( const UMovieSceneSection& Section ) const
{
    return Sections.Contains( &Section );
}


bool UMovieSceneSingleCameraCutTrack::IsEmpty() const
{
    return Sections.Num() == 0;
}

void UMovieSceneSingleCameraCutTrack::RemoveSection(UMovieSceneSection& Section)
{
    Sections.Remove(&Section);

    MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, true);

    UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
    check( outer_sequence );
    outer_sequence->SectionAddedOrRemoved( nullptr );

    // @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?
}

void UMovieSceneSingleCameraCutTrack::RemoveSectionAt(int32 SectionIndex)
{
    UMovieSceneSection* SectionToDelete = Sections[SectionIndex];
    MovieSceneHelpers::FixupConsecutiveSections(Sections, *SectionToDelete, true);

    Sections.RemoveAt(SectionIndex);
    MovieSceneHelpers::SortConsecutiveSections(Sections);

    UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
    check( outer_sequence );
    outer_sequence->SectionAddedOrRemoved( nullptr );
}

void UMovieSceneSingleCameraCutTrack::RemoveAllAnimationData()
{
    Sections.Empty();
}

#if WITH_EDITORONLY_DATA
FText UMovieSceneSingleCameraCutTrack::GetDefaultDisplayName() const
{
    return LOCTEXT("TrackName", "Single Camera Cut");
}
#endif


#if WITH_EDITOR
EMovieSceneSectionMovedResult UMovieSceneSingleCameraCutTrack::OnSectionMoved(UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams)
{
    UMovieSceneSingleCameraCutSection* cut_section = Cast<UMovieSceneSingleCameraCutSection>( &ioSection );
    if( !cut_section )
        return EMovieSceneSectionMovedResult::None;

    cut_section->SetStartFrameAuto();

    //MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, false);

    // Test for locking playback out on section out
    //UMovieScene* OwnerScene = GetTypedOuter<UMovieScene>();
    //TRange<FFrameNumber> playback_range = OwnerScene->GetPlaybackRange();
    //playback_range.SetUpperBound( TRangeBound<FFrameNumber>::MinUpper( playback_range.GetUpperBound(), Section.GetRange().GetUpperBound() ) );
    //OwnerScene->SetPlaybackRange( playback_range );

    // At this moment, we can only resize the only section
    if( iParams.MoveType == EPropertyChangeType::ValueSet )
    {
        UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
        check( outer_sequence );
        outer_sequence->SectionResized( cut_section );

        return EMovieSceneSectionMovedResult::SectionsChanged;
    }

    return EMovieSceneSectionMovedResult::None;
}
#endif

FFrameNumber UMovieSceneSingleCameraCutTrack::FindEndTimeForCameraCut( FFrameNumber StartTime )
{
    UMovieScene* OwnerScene = GetTypedOuter<UMovieScene>();

    if( !Sections.Num() )
        return UE::MovieScene::DiscreteExclusiveUpper( OwnerScene->GetPlaybackRange() );

    return Sections[0]->GetExclusiveEndFrame();
}

void UMovieSceneSingleCameraCutTrack::PreCompileImpl( FMovieSceneTrackPreCompileResult& OutPreCompileResult )
{
    for (UMovieSceneSection* Section : Sections)
    {
        if (UMovieSceneSingleCameraCutSection* CameraCutSection = CastChecked<UMovieSceneSingleCameraCutSection>(Section))
        {
            CameraCutSection->ComputeInitialCameraCutTransform();
        }
    }
}

#undef LOCTEXT_NAMESPACE
