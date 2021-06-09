// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/CinematicBoardTrackHelpers.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Settings/EposTracksSettings.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "CinematicBoardTrackHelpers"

// Same functions as in MovieSceneToolHelpers.cpp
// Convert ULevelSequence/UMovieSceneCinematicShotSection to UBoardSequence/UMovieSceneCinematicBoardSection

//---

namespace
{
static bool IsPackageNameUnique( const TArray<FAssetData>& iObjectList, const FString& iNewPackageName )
{
    for( auto AssetObject : iObjectList )
    {
        if( AssetObject.PackageName.ToString() == iNewPackageName )
        {
            return false;
        }
    }
    return true;
}

static
TArray<FString>
FindSubsequencePaths( TArray<UMovieSceneSection*> iSections )
{
    TArray<FString> section_paths;
    for( auto section : iSections )
    {
        UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );

        FString sequencePath = FPaths::GetPath( board_section->GetSequence()->GetPathName() );
        if( sequencePath.IsEmpty() )
            continue;

        section_paths.Add( sequencePath );
    }

    return section_paths;
}

static
FString
FindMostReleventSubsequencePath( TArray<UMovieSceneSection*> iSections )
{
    TArray<FString> section_paths = FindSubsequencePaths( iSections );
    if( !section_paths.Num() )
        return FString();

    return section_paths[0]; //TODO: improve by selecting the most relevent
}
}

//static
FString
CinematicBoardTrackHelpers::GenerateNewSequencePath( UMovieScene* iRootMovieScene, UMovieScene* iFocusedMovieScene, FString& ioNewSequenceName )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    UObject* sequenceAsset = iRootMovieScene->GetOuter();
    //UObject* sequenceAsset = iFocusedMovieScene->GetOuter();
    UPackage* sequencePackage = sequenceAsset->GetPackage();
    FString sequencePackageName = sequencePackage->GetName(); // ie. /Game/cine/max/master
    //int32 lastSlashPos = sequencePackageName.Find( TEXT( "/" ), ESearchCase::IgnoreCase, ESearchDir::FromEnd );
    //FString sequencePath = sequencePackageName.Left( lastSlashPos );

    UMovieSceneCinematicBoardTrack* track = iFocusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( track )
    {
        FString most_relevent_path = FindMostReleventSubsequencePath( track->GetAllSections() );
        if( !most_relevent_path.IsEmpty() )
        {
            sequencePackageName = most_relevent_path;
        }
    }

    FString newShotPrefix;
    uint32 newShotNumber = INDEX_NONE;
    uint32 newTakeNumber = INDEX_NONE;
    MovieSceneToolHelpers::ParseShotName( ioNewSequenceName, newShotPrefix, newShotNumber, newTakeNumber );

    FString newShotPath = sequencePackageName;
    //FString newShotDirectory = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, INDEX_NONE );
    //FString newShotPath = sequencePath;

    //FString shotDirectory = projectSettings->ShotDirectory;
    //if( !shotDirectory.IsEmpty() )
    //{
    //    newShotPath /= shotDirectory;
    //}
    //newShotPath /= newShotDirectory; // put this in the shot directory, ie. /Game/cine/max/shots/shot0010

    // Make sure this shot path is unique
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );
    TArray<FAssetData> objectList;
    assetRegistryModule.Get().GetAssetsByClass( UBoardSequence::StaticClass()->GetFName(), objectList );
    assetRegistryModule.Get().GetAssetsByClass( UShotSequence::StaticClass()->GetFName(), objectList );

    FString newPackageName = newShotPath;
    newPackageName /= ioNewSequenceName; // ie. /Game/cine/max/shots/shot0010/shot0010_001
    if( !IsPackageNameUnique( objectList, newPackageName ) )
    {
        while( 1 )
        {
            newShotNumber += projectSettings->ShotIncrement;
            ioNewSequenceName = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, newTakeNumber );
            newShotPath = sequencePackageName;
            //newShotDirectory = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, INDEX_NONE );
            //newShotPath = sequencePath;
            //if( !shotDirectory.IsEmpty() )
            //{
            //    newShotPath /= shotDirectory;
            //}
            //newShotPath /= newShotDirectory;

            newPackageName = newShotPath;
            newPackageName /= ioNewSequenceName;
            if( IsPackageNameUnique( objectList, newPackageName ) )
            {
                break;
            }
        }
    }

    return newShotPath;
}

template<typename SequenceClass>
//static
FString
CinematicBoardTrackHelpers::GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    //UMovieSceneCinematicBoardSection* beforeShot = nullptr;
    //UMovieSceneCinematicBoardSection* nextShot = nullptr;

    //FFrameNumber minEndDiff = TNumericLimits<int32>::Max();
    //FFrameNumber minStartDiff = TNumericLimits<int32>::Max();

    //for( auto section : iAllSections )
    //{
    //    if( section->HasEndFrame() && section->GetExclusiveEndFrame() >= iTime )
    //    {
    //        FFrameNumber endDiff = section->GetExclusiveEndFrame() - iTime;
    //        if( minEndDiff > endDiff )
    //        {
    //            minEndDiff = endDiff;
    //            beforeShot = Cast<UMovieSceneCinematicBoardSection>( section );
    //        }
    //    }
    //    if( section->HasStartFrame() && section->GetInclusiveStartFrame() <= iTime )
    //    {
    //        FFrameNumber startDiff = iTime - section->GetInclusiveStartFrame();
    //        if( minStartDiff > startDiff )
    //        {
    //            minStartDiff = startDiff;
    //            nextShot = Cast<UMovieSceneCinematicBoardSection>( section );
    //        }
    //    }
    //}

    int32 beforeShotIndex = INDEX_NONE;
    int32 nextShotIndex = INDEX_NONE;

    for( int i = 0; i < iAllSections.Num(); i++ )
    {
        UMovieSceneSection* current_section = iAllSections[i];
        TRange<FFrameNumber> current_section_range( current_section->GetTrueRange() );

        if( current_section->IsTimeWithinSection( iTime ) )
        {
            TArray<TRange<FFrameNumber>> ranges = current_section->GetTrueRange().Split( ( current_section->GetInclusiveStartFrame().Value + current_section->GetExclusiveEndFrame().Value ) / 2 );
            // Can't split the section
            if( ranges.Num() != 2 )
            {
                beforeShotIndex = i;
                nextShotIndex = ( current_section == iAllSections.Last() ) ? INDEX_NONE : i + 1;
                break;
            }

            if( ranges[0].Contains( iTime ) )
            {
                beforeShotIndex = ( i == 0 ) ? INDEX_NONE : i - 1;
                nextShotIndex = i;
                break;
            }

            beforeShotIndex = i;
            nextShotIndex = ( current_section == iAllSections.Last() ) ? INDEX_NONE : i + 1;
            break;
        }
    }

    UMovieSceneCinematicBoardSection* beforeShot = nullptr;
    if( iAllSections.IsValidIndex( beforeShotIndex ) )
        beforeShot = Cast<UMovieSceneCinematicBoardSection>( iAllSections[beforeShotIndex] );
    UMovieSceneCinematicBoardSection* nextShot = nullptr;
    if( iAllSections.IsValidIndex( nextShotIndex ) )
        nextShot = Cast<UMovieSceneCinematicBoardSection>( iAllSections[nextShotIndex] );

    FString prefix = ( SequenceClass::StaticClass() == UBoardSequence::StaticClass() ) ? TEXT( "board" ) : TEXT( "shot" );

    // There aren't any shots, let's create the first shot name
    if( beforeShot == nullptr && nextShot == nullptr )
    {
        // Default case
    }
    else if( beforeShot == nullptr )
    {
        FString nextShotPrefix = prefix;
        //FString nextShotPrefix = projectSettings->ShotPrefix;
        uint32 nextShotNumber = projectSettings->FirstShotNumber;
        uint32 nextTakeNumber = projectSettings->FirstTakeNumber;

        if( MovieSceneToolHelpers::ParseShotName( nextShot->GetBoardDisplayName(), nextShotPrefix, nextShotNumber, nextTakeNumber ) ) //TODO: use certainly nextShot->GetBoardDisplayName() AND nextShot->SubSequence()->GetDisplayName() as nextShot->GetBoardDisplayName() may be empty !
        {
            uint32 newShotNumber = nextShotNumber - projectSettings->ShotIncrement;
            if( newShotNumber < 0 )
                newShotNumber = nextShotNumber / 2;
            prefix = ( nextShotPrefix == TEXT( "board" ) || nextShotPrefix == TEXT( "shot" ) ) ? prefix : nextShotPrefix;
            return MovieSceneToolHelpers::ComposeShotName( prefix, newShotNumber, projectSettings->FirstTakeNumber );
        }
    }
    else if( nextShot == nullptr )
    {
        FString beforeShotPrefix = prefix;
        //FString beforeShotPrefix = projectSettings->ShotPrefix;
        uint32 beforeShotNumber = projectSettings->FirstShotNumber;
        uint32 beforeTakeNumber = projectSettings->FirstTakeNumber;

        if( MovieSceneToolHelpers::ParseShotName( beforeShot->GetBoardDisplayName(), beforeShotPrefix, beforeShotNumber, beforeTakeNumber ) )
        {
            uint32 newShotNumber = beforeShotNumber + projectSettings->ShotIncrement;
            prefix = ( beforeShotPrefix == TEXT( "board" ) || beforeShotPrefix == TEXT( "shot" ) ) ? prefix : beforeShotPrefix;
            return MovieSceneToolHelpers::ComposeShotName( prefix, newShotNumber, projectSettings->FirstTakeNumber );
        }
    }
    // This is in between two shots
    else
    {
        FString beforeShotPrefix = prefix;
        //FString beforeShotPrefix = projectSettings->ShotPrefix;
        uint32 beforeShotNumber = projectSettings->FirstShotNumber;
        uint32 beforeTakeNumber = projectSettings->FirstTakeNumber;

        FString nextShotPrefix = prefix;
        //FString nextShotPrefix = projectSettings->ShotPrefix;
        uint32 nextShotNumber = projectSettings->FirstShotNumber;
        uint32 nextTakeNumber = projectSettings->FirstTakeNumber;

        if( MovieSceneToolHelpers::ParseShotName( beforeShot->GetBoardDisplayName(), beforeShotPrefix, beforeShotNumber, beforeTakeNumber ) &&
            MovieSceneToolHelpers::ParseShotName( nextShot->GetBoardDisplayName(), nextShotPrefix, nextShotNumber, nextTakeNumber ) )
        {
            if( beforeShotNumber < nextShotNumber )
            {
                uint32 newShotNumber = beforeShotNumber + ( ( nextShotNumber - beforeShotNumber ) / 2 ); // what if we can't find one? or conflicts with another?
                prefix = ( nextShotPrefix == TEXT( "board" ) || nextShotPrefix == TEXT( "shot" ) ) ? prefix : nextShotPrefix;
                return MovieSceneToolHelpers::ComposeShotName( prefix, newShotNumber, projectSettings->FirstTakeNumber );
            }
        }
    }

    // Default case
    return MovieSceneToolHelpers::ComposeShotName( prefix, projectSettings->FirstShotNumber, projectSettings->FirstTakeNumber );
    //return MovieSceneToolHelpers::ComposeShotName( projectSettings->ShotPrefix, projectSettings->FirstShotNumber, projectSettings->FirstTakeNumber );
}

//---

//static
UMovieSceneCinematicBoardTrack*
CinematicBoardTrackHelpers::FindOrCreateCinematicBoardTrack( ISequencer* iSequencer )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return nullptr;

    UMovieScene* focusedMovieScene = sequence->GetMovieScene();
    if( !focusedMovieScene )
        return nullptr;

    if( focusedMovieScene->IsReadOnly() )
    {
        return nullptr;
    }

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( boardTrack != nullptr )
    {
        return boardTrack;
    }

    const FScopedTransaction transaction( LOCTEXT( "AddCinematicBoardTrack_Transaction", "Add Board Track" ) );
    focusedMovieScene->Modify();

    auto newTrack = focusedMovieScene->AddMasterTrack<UMovieSceneCinematicBoardTrack>();
    ensure( newTrack );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );

    return newTrack;
}

template<typename SequenceClass>
//static
UMovieSceneSubSection*
CinematicBoardTrackHelpers::CreateSequenceInternal( ISequencer* iSequencer, FString& ioNewSequenceName, FFrameNumber iNewSectionStartTime, UMovieSceneCinematicBoardSection* iSectionToDuplicate )
{
    UMovieSceneCinematicBoardTrack* boardTrack = CinematicBoardTrackHelpers::FindOrCreateCinematicBoardTrack( iSequencer );

    FString newBoardPath;

    if( iSectionToDuplicate != nullptr )
    {
        // If duplicating a board, use that board's path
        newBoardPath = FPaths::GetPath( iSectionToDuplicate->GetSequence()->GetPathName() );
    }
    else
    {
        newBoardPath = CinematicBoardTrackHelpers::GenerateNewSequencePath( iSequencer->GetRootMovieSceneSequence()->GetMovieScene(), iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene(), ioNewSequenceName );
    }

    // Create a new level sequence asset with the appropriate name
    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

    UObject* newAsset = nullptr;
    for( TObjectIterator<UClass> It; It; ++It )
    {
        UClass* currentClass = *It;
        if( currentClass->IsChildOf( UFactory::StaticClass() ) && !( currentClass->HasAnyClassFlags( CLASS_Abstract ) ) )
        {
            UFactory* factory = Cast<UFactory>( currentClass->GetDefaultObject() );
            if( factory->CanCreateNew() && factory->ImportPriority >= 0 && factory->SupportedClass == SequenceClass::StaticClass() )
            {
                if( iSectionToDuplicate != nullptr )
                {
                    newAsset = assetTools.DuplicateAssetWithDialog( ioNewSequenceName, newBoardPath, iSectionToDuplicate->GetSequence() );
                }
                else
                {
                    if( FindMostReleventSubsequencePath( boardTrack->GetAllSections() ).IsEmpty() )
                        newAsset = assetTools.CreateAssetWithDialog( ioNewSequenceName, newBoardPath, SequenceClass::StaticClass(), factory );
                    else
                        newAsset = assetTools.CreateAsset( ioNewSequenceName, newBoardPath, SequenceClass::StaticClass(), factory );

                }
                break;
            }
        }
    }

    if( newAsset == nullptr )
    {
        return nullptr;
    }

    UMovieSceneSequence* newSequence = Cast<UMovieSceneSequence>( newAsset );

    int32 section_duration;
    if( iSectionToDuplicate )
        section_duration = UE::MovieScene::DiscreteSize( iSectionToDuplicate->GetRange() );
    else if( !boardTrack->GetAllSections().Num() )
        section_duration = UE::MovieScene::DiscreteSize( iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange() );
    else
        section_duration = UE::MovieScene::DiscreteSize( newSequence->GetMovieScene()->GetPlaybackRange() );

    // Create a board section.
    UMovieSceneSubSection* newSection = boardTrack->AddSequence( newSequence, iNewSectionStartTime, section_duration );

    // Set the playback length of the subsequence to match its section length
    newSequence->GetMovieScene()->SetPlaybackRange( 0, UE::MovieScene::DiscreteSize( newSection->GetTrueRange() ) );

    iSequencer->SetLocalTime( newSection->GetInclusiveStartFrame() );

    return newSection;
}

template<typename SequenceClass>
//static
void
CinematicBoardTrackHelpers::InsertSequence( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    const FScopedTransaction transaction( LOCTEXT( "InsertBoard_Transaction", "Insert Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = CinematicBoardTrackHelpers::FindOrCreateCinematicBoardTrack( iSequencer );
    FString newBoardName = CinematicBoardTrackHelpers::GenerateNewSectionName<SequenceClass>( boardTrack->GetAllSections(), iFrameNumber );

    UMovieSceneSubSection* newBoard = CinematicBoardTrackHelpers::CreateSequenceInternal<SequenceClass>( iSequencer, newBoardName, iFrameNumber );
    if( newBoard )
    {
        //newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( newBoard );
    iSequencer->ThrobSectionSelection();
}

//---

//static
void
CinematicBoardTrackHelpers::InsertBoard( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    CinematicBoardTrackHelpers::InsertSequence<UBoardSequence>( iSequencer, iFrameNumber );
}
//static
void
CinematicBoardTrackHelpers::InsertShot( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    CinematicBoardTrackHelpers::InsertSequence<UShotSequence>( iSequencer, iFrameNumber );
}

//static
void
CinematicBoardTrackHelpers::InsertFiller( ISequencer* iSequencer )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    const FScopedTransaction transaction( LOCTEXT( "InsertFiller_Transaction", "Insert Filler" ) );

    FQualifiedFrameTime currentTime = iSequencer->GetLocalTime();

    UMovieSceneCinematicBoardTrack* boardTrack = CinematicBoardTrackHelpers::FindOrCreateCinematicBoardTrack( iSequencer );

    int32 duration = ( projectSettings->DefaultDuration * currentTime.Rate ).FrameNumber.Value;

    UMovieSceneSequence* nullSequence = nullptr;

    UMovieSceneSubSection* newSection = boardTrack->AddSequence( nullSequence, currentTime.Time.FrameNumber, duration );

    UMovieSceneCinematicBoardSection* newBoardSection = Cast<UMovieSceneCinematicBoardSection>( newSection );

    newBoardSection->SetBoardDisplayName( FText( LOCTEXT( "Filler", "Filler" ) ).ToString() );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( newSection );
    iSequencer->ThrobSectionSelection();
}

//static
void
CinematicBoardTrackHelpers::DuplicateSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection )
{
    UMovieSceneSequence* subsequence = iSection->GetSequence();
    if( !subsequence )
        return;

    const FScopedTransaction transaction( LOCTEXT( "DuplicateBoard_Transaction", "Duplicate Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = CinematicBoardTrackHelpers::FindOrCreateCinematicBoardTrack( iSequencer );

    FFrameNumber startTime = iSection->HasStartFrame() ? iSection->GetInclusiveStartFrame() : 0;
    FString newBoardName;
    if( subsequence->IsA<UBoardSequence>() )
        newBoardName = CinematicBoardTrackHelpers::GenerateNewSectionName<UBoardSequence>( boardTrack->GetAllSections(), startTime );
    else
        newBoardName = CinematicBoardTrackHelpers::GenerateNewSectionName<UShotSequence>( boardTrack->GetAllSections(), startTime );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* newBoard;
    if( subsequence->IsA<UBoardSequence>() )
        newBoard = CinematicBoardTrackHelpers::CreateSequenceInternal<UBoardSequence>( iSequencer, newBoardName, startTime, iSection );
    else
        newBoard = CinematicBoardTrackHelpers::CreateSequenceInternal<UShotSequence>( iSequencer, newBoardName, startTime, iSection );

    if( newBoard )
    {
        //newBoard->SetRange( iSection->GetRange() );
        //newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
        newBoard->Parameters.StartFrameOffset = iSection->Parameters.StartFrameOffset;
        newBoard->Parameters.TimeScale = iSection->Parameters.TimeScale;
        newBoard->SetPreRollFrames( iSection->GetPreRollFrames() );

        iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
        iSequencer->EmptySelection();
        iSequencer->SelectSection( newBoard );
        iSequencer->ThrobSectionSelection();
    }
}


#undef LOCTEXT_NAMESPACE
