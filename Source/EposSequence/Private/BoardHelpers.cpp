// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "BoardHelpers.h"

#include "AssetRegistryModule.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"

//---

//static
TArray< UMovieSceneSequence* >
BoardHelpers::FindParents( UMovieSceneSequence* iSequence )
{
    TArray< UMovieSceneSequence* > parents;

    UMovieSceneSequence* child = iSequence;
    UMovieSceneSequence* parent = FindParent( child );

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
UMovieSceneSequence*
BoardHelpers::FindParent( UMovieSceneSequence* iSequence )
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
            UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
            UMovieSceneSequence* sub_sequence = board_section->GetSequence();
            if( !sub_sequence )
                continue;

            if( sub_sequence->GetFullName() == iSequence->GetFullName() )
                return sequence;
        }
    }

    return nullptr;
}
