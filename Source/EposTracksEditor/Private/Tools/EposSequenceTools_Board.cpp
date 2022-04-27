// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/EposSequenceTools.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "IAssetTools.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "PlaneActor.h"
#include "Settings/EposTracksSettings.h"
#include "Settings/NamingConventionSettings.h"
#include "Shot/ShotSequence.h"
#include "StoryNote.h"
#include "Tools/ResourceAssetTools.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools_Board"

// Same functions as in MovieSceneToolHelpers.cpp
// Convert ULevelSequence/UMovieSceneCinematicShotSection to UBoardSequence/UMovieSceneCinematicBoardSection

//static
UMovieSceneCinematicBoardTrack*
BoardSequenceTools::FindCinematicBoardTrack( ISequencer* iSequencer )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return nullptr;

    UMovieScene* focusedMovieScene = sequence->GetMovieScene();
    if( !focusedMovieScene )
        return nullptr;

    if( focusedMovieScene->IsReadOnly() )
        return nullptr;

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !boardTrack )
        return nullptr;

    return boardTrack;
}

//static
UMovieSceneCinematicBoardTrack*
BoardSequenceTools::FindOrCreateCinematicBoardTrack( ISequencer* iSequencer )
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

//---

//namespace
//{
//static bool IsPackageNameUnique( const TArray<FAssetData>& iObjectList, const FString& iNewPackageName )
//{
//    for( auto AssetObject : iObjectList )
//    {
//        if( AssetObject.PackageName.ToString() == iNewPackageName )
//        {
//            return false;
//        }
//    }
//    return true;
//}
//
//static
//TArray<FString>
//FindSubsequencePaths( TArray<UMovieSceneSection*> iSections )
//{
//    TArray<FString> section_paths;
//    for( auto section : iSections )
//    {
//        UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section );
//
//        FString sequencePath = FPaths::GetPath( board_section->GetSequence()->GetPathName() );
//        if( sequencePath.IsEmpty() )
//            continue;
//
//        section_paths.Add( sequencePath );
//    }
//
//    return section_paths;
//}
//
//static
//FString
//FindMostReleventSubsequencePath( TArray<UMovieSceneSection*> iSections )
//{
//    TArray<FString> section_paths = FindSubsequencePaths( iSections );
//    if( !section_paths.Num() )
//        return FString();
//
//    return section_paths[0]; //TODO: improve by selecting the most relevent
//}
//}

////static
//FString
//CinematicBoardTrackTools::GenerateNewSequencePath( UMovieScene* iRootMovieScene, UMovieScene* iFocusedMovieScene, FString& ioNewSequenceName )
//{
//    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
//
//    UObject* sequenceAsset = iRootMovieScene->GetOuter();
//    //UObject* sequenceAsset = iFocusedMovieScene->GetOuter();
//    UPackage* sequencePackage = sequenceAsset->GetPackage();
//    FString sequencePackageName = sequencePackage->GetName(); // ie. /Game/cine/max/master
//    //int32 lastSlashPos = sequencePackageName.Find( TEXT( "/" ), ESearchCase::IgnoreCase, ESearchDir::FromEnd );
//    //FString sequencePath = sequencePackageName.Left( lastSlashPos );
//
//    UMovieSceneCinematicBoardTrack* track = iFocusedMovieScene->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
//    if( track )
//    {
//        FString most_relevent_path = FindMostReleventSubsequencePath( track->GetAllSections() );
//        if( !most_relevent_path.IsEmpty() )
//        {
//            sequencePackageName = most_relevent_path;
//        }
//    }
//
//    FString newShotPrefix;
//    uint32 newShotNumber = INDEX_NONE;
//    uint32 newTakeNumber = INDEX_NONE;
//    MovieSceneToolHelpers::ParseShotName( ioNewSequenceName, newShotPrefix, newShotNumber, newTakeNumber );
//
//    FString newShotPath = sequencePackageName;
//    //FString newShotDirectory = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, INDEX_NONE );
//    //FString newShotPath = sequencePath;
//
//    //FString shotDirectory = projectSettings->ShotDirectory;
//    //if( !shotDirectory.IsEmpty() )
//    //{
//    //    newShotPath /= shotDirectory;
//    //}
//    //newShotPath /= newShotDirectory; // put this in the shot directory, ie. /Game/cine/max/shots/shot0010
//
//    // Make sure this shot path is unique
//    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );
//    TArray<FAssetData> objectList;
//    assetRegistryModule.Get().GetAssetsByClass( UBoardSequence::StaticClass()->GetFName(), objectList );
//    assetRegistryModule.Get().GetAssetsByClass( UShotSequence::StaticClass()->GetFName(), objectList );
//
//    FString newPackageName = newShotPath;
//    newPackageName /= ioNewSequenceName; // ie. /Game/cine/max/shots/shot0010/shot0010_001
//    if( !IsPackageNameUnique( objectList, newPackageName ) )
//    {
//        while( 1 )
//        {
//            newShotNumber += projectSettings->ShotIncrement;
//            ioNewSequenceName = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, newTakeNumber );
//            newShotPath = sequencePackageName;
//            //newShotDirectory = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, INDEX_NONE );
//            //newShotPath = sequencePath;
//            //if( !shotDirectory.IsEmpty() )
//            //{
//            //    newShotPath /= shotDirectory;
//            //}
//            //newShotPath /= newShotDirectory;
//
//            newPackageName = newShotPath;
//            newPackageName /= ioNewSequenceName;
//            if( IsPackageNameUnique( objectList, newPackageName ) )
//            {
//                break;
//            }
//        }
//    }
//
//    return newShotPath;
//}
//
//template<typename SequenceClass>
////static
//FString
//CinematicBoardTrackTools::GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime )
//{
//    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
//
//    //UMovieSceneCinematicBoardSection* beforeShot = nullptr;
//    //UMovieSceneCinematicBoardSection* nextShot = nullptr;
//
//    //FFrameNumber minEndDiff = TNumericLimits<int32>::Max();
//    //FFrameNumber minStartDiff = TNumericLimits<int32>::Max();
//
//    //for( auto section : iAllSections )
//    //{
//    //    if( section->HasEndFrame() && section->GetExclusiveEndFrame() >= iTime )
//    //    {
//    //        FFrameNumber endDiff = section->GetExclusiveEndFrame() - iTime;
//    //        if( minEndDiff > endDiff )
//    //        {
//    //            minEndDiff = endDiff;
//    //            beforeShot = Cast<UMovieSceneCinematicBoardSection>( section );
//    //        }
//    //    }
//    //    if( section->HasStartFrame() && section->GetInclusiveStartFrame() <= iTime )
//    //    {
//    //        FFrameNumber startDiff = iTime - section->GetInclusiveStartFrame();
//    //        if( minStartDiff > startDiff )
//    //        {
//    //            minStartDiff = startDiff;
//    //            nextShot = Cast<UMovieSceneCinematicBoardSection>( section );
//    //        }
//    //    }
//    //}
//
//    int32 beforeShotIndex = INDEX_NONE;
//    int32 nextShotIndex = INDEX_NONE;
//
//    for( int i = 0; i < iAllSections.Num(); i++ )
//    {
//        UMovieSceneSection* current_section = iAllSections[i];
//        TRange<FFrameNumber> current_section_range( current_section->GetTrueRange() );
//
//        if( current_section->IsTimeWithinSection( iTime ) )
//        {
//            TArray<TRange<FFrameNumber>> ranges = current_section->GetTrueRange().Split( ( current_section->GetInclusiveStartFrame().Value + current_section->GetExclusiveEndFrame().Value ) / 2 );
//            // Can't split the section
//            if( ranges.Num() != 2 )
//            {
//                beforeShotIndex = i;
//                nextShotIndex = ( current_section == iAllSections.Last() ) ? INDEX_NONE : i + 1;
//                break;
//            }
//
//            if( ranges[0].Contains( iTime ) )
//            {
//                beforeShotIndex = ( i == 0 ) ? INDEX_NONE : i - 1;
//                nextShotIndex = i;
//                break;
//            }
//
//            beforeShotIndex = i;
//            nextShotIndex = ( current_section == iAllSections.Last() ) ? INDEX_NONE : i + 1;
//            break;
//        }
//    }
//
//    UMovieSceneCinematicBoardSection* beforeShot = nullptr;
//    if( iAllSections.IsValidIndex( beforeShotIndex ) )
//        beforeShot = Cast<UMovieSceneCinematicBoardSection>( iAllSections[beforeShotIndex] );
//    UMovieSceneCinematicBoardSection* nextShot = nullptr;
//    if( iAllSections.IsValidIndex( nextShotIndex ) )
//        nextShot = Cast<UMovieSceneCinematicBoardSection>( iAllSections[nextShotIndex] );
//
//    FString prefix = ( SequenceClass::StaticClass() == UBoardSequence::StaticClass() ) ? TEXT( "board" ) : TEXT( "shot" );
//
//    // There aren't any shots, let's create the first shot name
//    if( beforeShot == nullptr && nextShot == nullptr )
//    {
//        // Default case
//    }
//    else if( beforeShot == nullptr )
//    {
//        FString nextShotPrefix = prefix;
//        //FString nextShotPrefix = projectSettings->ShotPrefix;
//        uint32 nextShotNumber = projectSettings->FirstShotNumber;
//        uint32 nextTakeNumber = projectSettings->FirstTakeNumber;
//
//        if( MovieSceneToolHelpers::ParseShotName( nextShot->GetBoardDisplayName(), nextShotPrefix, nextShotNumber, nextTakeNumber ) ) //TODO: use certainly nextShot->GetBoardDisplayName() AND nextShot->SubSequence()->GetDisplayName() as nextShot->GetBoardDisplayName() may be empty !
//        {
//            uint32 newShotNumber = nextShotNumber - projectSettings->ShotIncrement;
//            if( newShotNumber < 0 )
//                newShotNumber = nextShotNumber / 2;
//            prefix = ( nextShotPrefix == TEXT( "board" ) || nextShotPrefix == TEXT( "shot" ) ) ? prefix : nextShotPrefix;
//            return MovieSceneToolHelpers::ComposeShotName( prefix, newShotNumber, projectSettings->FirstTakeNumber );
//        }
//    }
//    else if( nextShot == nullptr )
//    {
//        FString beforeShotPrefix = prefix;
//        //FString beforeShotPrefix = projectSettings->ShotPrefix;
//        uint32 beforeShotNumber = projectSettings->FirstShotNumber;
//        uint32 beforeTakeNumber = projectSettings->FirstTakeNumber;
//
//        if( MovieSceneToolHelpers::ParseShotName( beforeShot->GetBoardDisplayName(), beforeShotPrefix, beforeShotNumber, beforeTakeNumber ) )
//        {
//            uint32 newShotNumber = beforeShotNumber + projectSettings->ShotIncrement;
//            prefix = ( beforeShotPrefix == TEXT( "board" ) || beforeShotPrefix == TEXT( "shot" ) ) ? prefix : beforeShotPrefix;
//            return MovieSceneToolHelpers::ComposeShotName( prefix, newShotNumber, projectSettings->FirstTakeNumber );
//        }
//    }
//    // This is in between two shots
//    else
//    {
//        FString beforeShotPrefix = prefix;
//        //FString beforeShotPrefix = projectSettings->ShotPrefix;
//        uint32 beforeShotNumber = projectSettings->FirstShotNumber;
//        uint32 beforeTakeNumber = projectSettings->FirstTakeNumber;
//
//        FString nextShotPrefix = prefix;
//        //FString nextShotPrefix = projectSettings->ShotPrefix;
//        uint32 nextShotNumber = projectSettings->FirstShotNumber;
//        uint32 nextTakeNumber = projectSettings->FirstTakeNumber;
//
//        if( MovieSceneToolHelpers::ParseShotName( beforeShot->GetBoardDisplayName(), beforeShotPrefix, beforeShotNumber, beforeTakeNumber ) &&
//            MovieSceneToolHelpers::ParseShotName( nextShot->GetBoardDisplayName(), nextShotPrefix, nextShotNumber, nextTakeNumber ) )
//        {
//            if( beforeShotNumber < nextShotNumber )
//            {
//                uint32 newShotNumber = beforeShotNumber + ( ( nextShotNumber - beforeShotNumber ) / 2 ); // what if we can't find one? or conflicts with another?
//                prefix = ( nextShotPrefix == TEXT( "board" ) || nextShotPrefix == TEXT( "shot" ) ) ? prefix : nextShotPrefix;
//                return MovieSceneToolHelpers::ComposeShotName( prefix, newShotNumber, projectSettings->FirstTakeNumber );
//            }
//        }
//    }
//
//    // Default case
//    return MovieSceneToolHelpers::ComposeShotName( prefix, projectSettings->FirstShotNumber, projectSettings->FirstTakeNumber );
//    //return MovieSceneToolHelpers::ComposeShotName( projectSettings->ShotPrefix, projectSettings->FirstShotNumber, projectSettings->FirstTakeNumber );
//}

//---

template<typename SequenceClass>
//static
UMovieSceneSubSection*
CinematicBoardTrackTools::CreateSequenceInternal( ISequencer* iSequencer, const FString& iNewSequencePath, const FString& iNewSequenceName, FFrameNumber iNewSectionStartTime, TOptional<int32> iDuration, UMovieSceneCinematicBoardSection* iSectionToDuplicate )
{
    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( iSequencer );

    //if( iSectionToDuplicate != nullptr )
    //{
    //    // If duplicating a board, use that board's path
    //    newBoardPath = FPaths::GetPath( iSectionToDuplicate->GetSequence()->GetPathName() );
    //}
    //else
    // Should be ok to generate the cloned path the same way as new shot/board
    //{
    //    newBoardPath = GenerateNewSequencePath( iSequencer->GetRootMovieSceneSequence()->GetMovieScene(), iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene(), ioNewSequenceName );
    //}

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
                    //if( FindMostReleventSubsequencePath( boardTrack->GetAllSections() ).IsEmpty() )
                    //    newAsset = assetTools.DuplicateAssetWithDialog( iNewSequenceName, iNewSequencePath, iSectionToDuplicate->GetSequence() );
                    //else
                        newAsset = assetTools.DuplicateAsset( iNewSequenceName, iNewSequencePath, iSectionToDuplicate->GetSequence() );
                }
                else
                {
                    //if( FindMostReleventSubsequencePath( boardTrack->GetAllSections() ).IsEmpty() )
                    //    newAsset = assetTools.CreateAssetWithDialog( iNewSequenceName, iNewSequencePath, SequenceClass::StaticClass(), factory );
                    //else
                        newAsset = assetTools.CreateAsset( iNewSequenceName, iNewSequencePath, SequenceClass::StaticClass(), factory );
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
    else if( iDuration.IsSet() )
        section_duration = iDuration.GetValue();
    else if( !boardTrack->GetAllSections().Num() )
        section_duration = UE::MovieScene::DiscreteSize( iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange() );
    else
        section_duration = -1; // To use the duration of the current section
        // section_duration = UE::MovieScene::DiscreteSize( newSequence->GetMovieScene()->GetPlaybackRange() );

    // Create a board section.
    UMovieSceneSubSection* newSection = boardTrack->AddSequence( newSequence, iNewSectionStartTime, section_duration );

    // Set the playback length of the subsequence to match its section length
    newSequence->GetMovieScene()->SetPlaybackRange( 0, UE::MovieScene::DiscreteSize( newSection->GetTrueRange() ) );

    iSequencer->SetLocalTime( newSection->GetInclusiveStartFrame() );

    return newSection;
}

//---

//static
UMovieSceneSubSection*
CinematicBoardTrackTools::InsertBoard( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration )
{
    const FScopedTransaction transaction( LOCTEXT( "transaction.insert-board", "Insert Board" ) );

    FString sequence_path;
    FString sequence_name;
    FBoardNameElements board_name_elements;
    FString sequence_pathname = NamingConvention::GenerateBoardAssetPathName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequencer->GetFocusedMovieSceneSequence(), iFrameNumber, sequence_path, sequence_name, board_name_elements );

    UMovieSceneSubSection* new_section = CreateSequenceInternal<UBoardSequence>( iSequencer, sequence_path, sequence_name, iFrameNumber, iDuration );

    if( new_section && new_section->GetSequence() )
    {
        UBoardSequence* board_sequence = CastChecked<UBoardSequence>( new_section->GetSequence() );
        board_sequence->NameElements = board_name_elements;
    }

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    BoardSequenceTools::UpdateViewRange( iSequencer, new_section ? new_section->GetTrueRange() : TRange<FFrameNumber>::Empty() );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( new_section );
    iSequencer->ThrobSectionSelection();

    return new_section;
}
//static
UMovieSceneSubSection*
CinematicBoardTrackTools::InsertShot( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration )
{
    const FScopedTransaction transaction( LOCTEXT( "transaction.insert-shot", "Insert Shot" ) );

    FString sequence_path;
    FString sequence_name;
    FShotNameElements shot_name_elements;
    FString sequence_pathname = NamingConvention::GenerateShotAssetPathName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequencer->GetFocusedMovieSceneSequence(), iFrameNumber, sequence_path, sequence_name, shot_name_elements );

    //---

    UMovieSceneSubSection* new_section = CreateSequenceInternal<UShotSequence>( iSequencer, sequence_path, sequence_name, iFrameNumber, iDuration );

    //---

    if( new_section && new_section->GetSequence() )
    {
        UShotSequence* shot_sequence = CastChecked<UShotSequence>( new_section->GetSequence() );
        shot_sequence->NameElements = shot_name_elements;
    }

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    BoardSequenceTools::UpdateViewRange( iSequencer, new_section ? new_section->GetTrueRange() : TRange<FFrameNumber>::Empty() );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( new_section );
    iSequencer->ThrobSectionSelection();

    return new_section;
}

////static
//void
//CinematicBoardTrackTools::InsertFiller( ISequencer* iSequencer )
//{
//    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
//
//    const FScopedTransaction transaction( LOCTEXT( "InsertFiller_Transaction", "Insert Filler" ) );
//
//    FQualifiedFrameTime currentTime = iSequencer->GetLocalTime();
//
//    UMovieSceneCinematicBoardTrack* boardTrack = FindOrCreateCinematicBoardTrack( iSequencer );
//
//    int32 duration = ( projectSettings->DefaultDuration * currentTime.Rate ).FrameNumber.Value;
//
//    UMovieSceneSequence* nullSequence = nullptr;
//
//    UMovieSceneSubSection* newSection = boardTrack->AddSequence( nullSequence, currentTime.Time.FrameNumber, duration );
//
//    UMovieSceneCinematicBoardSection* newBoardSection = Cast<UMovieSceneCinematicBoardSection>( newSection );
//
//    newBoardSection->SetBoardDisplayName( FText( LOCTEXT( "Filler", "Filler" ) ).ToString() );
//
//    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
//    iSequencer->EmptySelection();
//    iSequencer->SelectSection( newSection );
//    iSequencer->ThrobSectionSelection();
//}

////static
//void
//CinematicBoardTrackTools::DuplicateSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection )
//{
//    UMovieSceneSequence* subsequence = iSection->GetSequence();
//    if( !subsequence )
//        return;
//
//    const FScopedTransaction transaction( LOCTEXT( "DuplicateBoard_Transaction", "Duplicate Board" ) );
//
//    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( iSequencer );
//
//    FFrameNumber startTime = iSection->HasStartFrame() ? iSection->GetInclusiveStartFrame() : 0;
//    FString newBoardName;
//    if( subsequence->IsA<UBoardSequence>() )
//        newBoardName = GenerateNewSectionName<UBoardSequence>( boardTrack->GetAllSections(), startTime );
//    else
//        newBoardName = GenerateNewSectionName<UShotSequence>( boardTrack->GetAllSections(), startTime );
//
//    // Duplicate the board and put it on the next available row
//    UMovieSceneSubSection* newBoard;
//    if( subsequence->IsA<UBoardSequence>() )
//        newBoard = CreateSequenceInternal<UBoardSequence>( iSequencer, newBoardName, startTime, TOptional<int32>(), iSection );
//    else
//        newBoard = CreateSequenceInternal<UShotSequence>( iSequencer, newBoardName, startTime, TOptional<int32>(), iSection );
//
//    if( !newBoard )
//        return;
//
//    //newBoard->SetRange( iSection->GetRange() );
//    //newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
//    newBoard->Parameters.StartFrameOffset = iSection->Parameters.StartFrameOffset;
//    newBoard->Parameters.TimeScale = iSection->Parameters.TimeScale;
//    newBoard->SetPreRollFrames( iSection->GetPreRollFrames() );
//
//    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
//    BoardSequenceTools::UpdateViewRange( iSequencer, newBoard ? newBoard->GetTrueRange() : TRange<FFrameNumber>::Empty() );
//    iSequencer->EmptySelection();
//    iSequencer->SelectSection( newBoard );
//    iSequencer->ThrobSectionSelection();
//}

//static
void
CinematicBoardTrackTools::CloneSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection, FFrameNumber iFrameNumber, bool iEmptyDrawings )
{
    UMovieSceneSequence* subsequence = iSection->GetSequence();
    if( !subsequence )
        return;
    if( subsequence->IsA<UBoardSequence>() )
        return;

    const FScopedTransaction transaction( LOCTEXT( "CloneSection_Transaction", "Clone Section" ) );

    FString sequence_path;
    FString sequence_name;
    FShotNameElements shot_name_elements;
    FString sequence_pathname = NamingConvention::GenerateShotAssetPathName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequencer->GetFocusedMovieSceneSequence(), iFrameNumber, sequence_path, sequence_name, shot_name_elements );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* new_section = CreateSequenceInternal<UShotSequence>( iSequencer, sequence_path, sequence_name, iFrameNumber, TOptional<int32>(), iSection );

    if( !new_section )
        return;

    if( new_section->GetSequence() )
    {
        UShotSequence* shot_sequence = CastChecked<UShotSequence>( new_section->GetSequence() );

        shot_sequence->NameElements = shot_name_elements;
    }

    //---

    //new_section->SetRange( iSection->GetRange() );
    //new_section->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, new_section ) );
    new_section->Parameters.StartFrameOffset = iSection->Parameters.StartFrameOffset;
    new_section->Parameters.TimeScale = iSection->Parameters.TimeScale;
    new_section->SetPreRollFrames( iSection->GetPreRollFrames() );

    //iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
    BoardSequenceTools::UpdateViewRange( iSequencer, new_section ? new_section->GetTrueRange() : TRange<FFrameNumber>::Empty() );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( new_section );
    iSequencer->ThrobSectionSelection();

    //---

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, *new_section, iSequencer->GetFocusedTemplateID() );

    ShotSequenceTools::CloneInnerContent( iSequencer, result.mInnerSequence, result.mInnerSequenceId, iEmptyDrawings );
}

//static
void
ShotSequenceTools::CloneInnerContent( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iEmptyDrawings )
{
    iSequence->Modify();
    iSequence->GetMovieScene()->Modify();

    //---

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, iSequence, iSequenceID, &camera_guid );
    if( !camera )
        return;

    FActorSpawnParameters cameraSpawnParams;
    cameraSpawnParams.Template = camera;
    ACineCameraActor* cloned_camera = camera->GetWorld()->SpawnActor<ACineCameraActor>( cameraSpawnParams );
    if( !cloned_camera )
        return;

    FString cloned_camera_path;
    FString cloned_camera_name;
    NamingConvention::GenerateCameraActorPathName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, cloned_camera_path, cloned_camera_name );
    // We don't keep the same name as the original camera (like plane), to be able to increment the (global) index or to use the new shot name

    cloned_camera->SetFolderPath( *cloned_camera_path );
    FActorLabelUtilities::RenameExistingActor( cloned_camera, cloned_camera_name, false ); // The shot name is displayed in another column in the world outliner

    //-

    cloned_camera_name = NamingConvention::GenerateCameraTrackName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, cloned_camera );

    iSequence->UnbindPossessableObjects( camera_guid );
    iSequence->BindPossessableObject( camera_guid, *cloned_camera, iSequencer->GetPlaybackContext() );
    iSequence->GetMovieScene()->FindPossessable( camera_guid )->SetName( cloned_camera_name );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );

    //---

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;
    int32 plane_count = ShotSequenceHelpers::GetAllPlanes( *iSequencer, iSequence, iSequenceID, EGetPlane::kAll, &planes, &plane_bindings );

    for( int i = 0; i < plane_count; i++ )
    {
        bool attachPlaneToCamera = false;
        USceneComponent* RootComp = planes[i]->GetRootComponent();
        if( RootComp && RootComp->GetAttachParent() )
        {
            AActor* ParentActor = RootComp->GetAttachParent()->GetOwner();
            attachPlaneToCamera = ( ParentActor == camera );
        }

        CloneInnerPlane( iSequencer, iSequence, iSequenceID, iSequence->GetMovieScene(), iEmptyDrawings, planes[i], plane_bindings[i], cloned_camera, attachPlaneToCamera );
    }

    //---

    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections = EposSequenceHelpers::GetNotes( *iSequencer, iSequence, iSequenceID );

    for( auto note_section : note_sections )
    {
        UStoryNote* original_note = note_section->GetNote();

        UStoryNote* duplicate_note = ProjectAssetTools::CloneNote( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, original_note );

        note_section->SetNote( duplicate_note );
    }

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately ); // Otherwise, some internal stuff may not be up-to-date, and new bindings/moviescenesequenceID/... are not available in cache
}

//static
void
ShotSequenceTools::CloneInnerPlane( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieScene* iMovieScene, bool iEmptyDrawings, APlaneActor* iPlaneToClone, FGuid iPlaneBinding, ACineCameraActor* iClonedCamera, bool iAttachPlaneToCamera )
{
    FActorSpawnParameters planeSpawnParams;
    planeSpawnParams.Template = iPlaneToClone;
    APlaneActor* cloned_plane = iPlaneToClone->GetWorld()->SpawnActor<APlaneActor>( planeSpawnParams );
    if( !cloned_plane )
        return;

    FString cloned_plane_path;
    FString cloned_plane_name;
    NamingConvention::GeneratePlaneActorPathName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, cloned_plane_path, cloned_plane_name );
    cloned_plane_name = iPlaneToClone->GetActorLabel(); // As the plane actor is cloned, just keep the same name (let see when shot/camera name are a part of the plane name...)

    cloned_plane->SetFolderPath( *cloned_plane_path );
    FActorLabelUtilities::RenameExistingActor( cloned_plane, cloned_plane_name, false ); // The shot name is displayed in another column in the world outliner

    cloned_plane->SetActorTransform( iPlaneToClone->GetTransform() ); // Should be done, because for attached plane, its new transform are totally weird
    cloned_plane->SetActorHiddenInGame( true ); // As it was created with the class constructor which set it to true, otherwise the actor to clone is certainly displayed, then the cloned actor will have false by default

    //-

    if( iAttachPlaneToCamera )
        GEditor->ParentActors( iClonedCamera, cloned_plane, NAME_None );

    //-

    cloned_plane_name = NamingConvention::GeneratePlaneTrackName( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, cloned_plane );

    iSequence->UnbindPossessableObjects( iPlaneBinding );
    iSequence->BindPossessableObject( iPlaneBinding, *cloned_plane, iSequencer->GetPlaybackContext() );
    iMovieScene->FindPossessable( iPlaneBinding )->SetName( cloned_plane_name );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );

    //---

    TArray<FDrawing> drawings = ShotSequenceHelpers::GetAllDrawings( *iSequencer, iSequence, iSequenceID, iPlaneBinding );

    for( auto& drawing : drawings )
    {
        UMaterialInstance* material = drawing.GetMaterial();
        if( !material )
            continue;

        UMaterialInstanceConstant* new_material = iEmptyDrawings ? ProjectAssetTools::CreateMaterialAndTexture( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, material ) : ProjectAssetTools::CloneMaterialAndTexture( *iSequencer, iSequencer->GetRootMovieSceneSequence(), iSequence, material );
        if( !new_material )
            continue;

        drawing.SetMaterial( new_material );
    }

    // To have no (hidden) dependency with a material (and as it should always be at least one key)
    if( drawings.Num() )
        UE::MovieScene::SetChannelDefault( drawings[0].mChannel, nullptr );

    //-

    // To change the initial material of the actor (which is always overrided by the sequencer when it's opened)
    if( drawings.Num() )
    {
        UMaterialInstance* material = drawings[0].GetMaterial();

        cloned_plane->GetStaticMeshComponent()->SetMaterial( 0, material );
    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

#undef LOCTEXT_NAMESPACE
