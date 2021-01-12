// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "BoardHelpers.h"

#include "AssetRegistryModule.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "MovieSceneTimeHelpers.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "Helpers/SectionsHelpersShift.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"

//---

//static
TArray< UEposMovieSceneSequence* >
BoardHelpers::FindParents( UEposMovieSceneSequence* iSequence )
{
    TArray< UEposMovieSceneSequence* > parents;

    UEposMovieSceneSequence* child = iSequence;
    UEposMovieSceneSequence* parent = FindParent( child );

    parents.Add( child );

    while( parent )
    {
        child = parent;
        parent = BoardHelpers::FindParent( child );

        parents.Add( child );
    }

    Algo::Reverse( parents ); // Root > child1 > child2 > ... > childN > iSequence

    return parents;
}

//static
UEposMovieSceneSequence*
BoardHelpers::FindParent( UEposMovieSceneSequence* iSequence )
{
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    TArray<FAssetData> objectList;
    assetRegistryModule.Get().GetAssetsByClass( UBoardSequence::StaticClass()->GetFName(), objectList );

    for( auto asset_data : objectList )
    {
        //UBoardSequence* sequence = asset_data.GetFastAsset();
        UObject* object = asset_data.GetAsset();
        UBoardSequence* sequence = Cast<UBoardSequence>( object );
        if( !sequence )
            continue;

        UMovieSceneTrack* track = sequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
        if( !track )
            continue;

        for( auto section : track->GetAllSections() )
        {
            UMovieSceneSubSection* board_section = Cast<UMovieSceneSubSection>( section );
            UMovieSceneSequence* sub_sequence = board_section->GetSequence();
            if( !sub_sequence )
                continue;

            if( sub_sequence->GetFullName() == iSequence->GetFullName() )
                return sequence;
        }
    }

    return nullptr;
}

//---

//static
UMovieSceneSection*
BoardHelpers::FindParentSectionOfSequence( UMovieSceneSequence* iParentSequence, UMovieSceneSequence* iChildSequence )
{
    UMovieSceneTrack* track = iParentSequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !track )
        return nullptr;

    for( auto section : track->GetAllSections() )
    {
        UMovieSceneSubSection* current_subsection = Cast<UMovieSceneSubSection>( section );
        UMovieSceneSequence* sub_sequence = current_subsection->GetSequence();
        if( !sub_sequence )
            continue;

        if( sub_sequence->GetFullName() == iChildSequence->GetFullName() )
            return current_subsection;
    }

    return nullptr;
}

//---

//static
void
BoardHelpers::ResizeChildSequence( UMovieSceneSection* iSection )
{
    UEposMovieSceneSequence* sequence = iSection->GetTypedOuter<UEposMovieSceneSequence>();
    check( sequence );

    UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( iSection );
    if( !subsection )
        return;

    UEposMovieSceneSequence* subsequence = Cast<UEposMovieSceneSequence>( subsection->GetSequence() );
    if( !subsequence )
        return;

    FFrameTime new_size = FFrameRate::TransformTime( UE::MovieScene::DiscreteSize( subsection->GetTrueRange() ), sequence->GetMovieScene()->GetTickResolution(), subsequence->GetMovieScene()->GetTickResolution() );

    subsequence->Resize( new_size.FloorToFrame().Value );
}

//static
void
BoardHelpers::ResizeParentSequenceRecursively( UEposMovieSceneSequence* iSequence )
{
    TArray< UEposMovieSceneSequence* > parents = BoardHelpers::FindParents( iSequence );
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

        UMovieSceneTrack* child_track = child_sequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
        if( !child_track )
            child_track = child_sequence->GetMovieScene()->GetCameraCutTrack();

        if( child_track && child_track->GetAllSections().Num() )
        {
            auto child_full_range = TRange<FFrameNumber>( child_track->GetAllSections()[0]->GetInclusiveStartFrame(), child_track->GetAllSections().Last()->GetExclusiveEndFrame() );
            child_full_duration = UE::MovieScene::DiscreteSize( child_full_range );

            FFrameTime convert_duration = FFrameRate::TransformTime( child_full_duration, child_sequence->GetMovieScene()->GetTickResolution(), parent_sequence->GetMovieScene()->GetTickResolution() );
            child_full_duration = convert_duration.FloorToFrame().Value;
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
