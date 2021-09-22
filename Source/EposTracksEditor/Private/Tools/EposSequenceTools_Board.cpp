// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/EposSequenceTools.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Channels/MovieSceneObjectPathChannel.h"
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
#include "PlaneActor.h"
#include "Settings/EposTracksSettings.h"
#include "Shot/ShotSequence.h"
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
CinematicBoardTrackTools::GenerateNewSequencePath( UMovieScene* iRootMovieScene, UMovieScene* iFocusedMovieScene, FString& ioNewSequenceName )
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
CinematicBoardTrackTools::GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime )
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

template<typename SequenceClass>
//static
UMovieSceneSubSection*
CinematicBoardTrackTools::CreateSequenceInternal( ISequencer* iSequencer, FString& ioNewSequenceName, FFrameNumber iNewSectionStartTime, TOptional<int32> iDuration, UMovieSceneCinematicBoardSection* iSectionToDuplicate )
{
    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( iSequencer );

    FString newBoardPath;

    //if( iSectionToDuplicate != nullptr )
    //{
    //    // If duplicating a board, use that board's path
    //    newBoardPath = FPaths::GetPath( iSectionToDuplicate->GetSequence()->GetPathName() );
    //}
    //else
    // Should be ok to generate the cloned path the same way as new shot/board
    {
        newBoardPath = GenerateNewSequencePath( iSequencer->GetRootMovieSceneSequence()->GetMovieScene(), iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene(), ioNewSequenceName );
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

template<typename SequenceClass>
//static
UMovieSceneSubSection*
CinematicBoardTrackTools::InsertSequence( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration )
{
    const FScopedTransaction transaction( LOCTEXT( "InsertBoard_Transaction", "Insert Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( iSequencer );
    FString newBoardName = GenerateNewSectionName<SequenceClass>( boardTrack->GetAllSections(), iFrameNumber );

    UMovieSceneSubSection* newBoard = CreateSequenceInternal<SequenceClass>( iSequencer, newBoardName, iFrameNumber, iDuration );
    if( newBoard )
    {
        //newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    BoardSequenceTools::UpdateViewRange( iSequencer, newBoard ? newBoard->GetTrueRange() : TRange<FFrameNumber>::Empty() );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( newBoard );
    iSequencer->ThrobSectionSelection();

    return newBoard;
}

//---

//static
UMovieSceneSubSection*
CinematicBoardTrackTools::InsertBoard( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration )
{
    return InsertSequence<UBoardSequence>( iSequencer, iFrameNumber, iDuration );
}
//static
UMovieSceneSubSection*
CinematicBoardTrackTools::InsertShot( ISequencer* iSequencer, FFrameNumber iFrameNumber, TOptional<int32> iDuration )
{
    return InsertSequence<UShotSequence>( iSequencer, iFrameNumber, iDuration );
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
CinematicBoardTrackTools::CloneSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection, FFrameNumber iFrameNumber )
{
    UMovieSceneSequence* subsequence = iSection->GetSequence();
    if( !subsequence )
        return;

    const FScopedTransaction transaction( LOCTEXT( "CloneBoard_Transaction", "Clone Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( iSequencer );

    FString newBoardName;
    if( subsequence->IsA<UBoardSequence>() )
        newBoardName = GenerateNewSectionName<UBoardSequence>( boardTrack->GetAllSections(), iFrameNumber );
    else
        newBoardName = GenerateNewSectionName<UShotSequence>( boardTrack->GetAllSections(), iFrameNumber );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* newBoard;
    if( subsequence->IsA<UBoardSequence>() )
        newBoard = CreateSequenceInternal<UBoardSequence>( iSequencer, newBoardName, iFrameNumber, TOptional<int32>(), iSection );
    else
        newBoard = CreateSequenceInternal<UShotSequence>( iSequencer, newBoardName, iFrameNumber, TOptional<int32>(), iSection );

    if( !newBoard )
        return;

    //newBoard->SetRange( iSection->GetRange() );
    //newBoard->SetRowIndex( MovieSceneToolHelpers::FindAvailableRowIndex( boardTrack, newBoard ) );
    newBoard->Parameters.StartFrameOffset = iSection->Parameters.StartFrameOffset;
    newBoard->Parameters.TimeScale = iSection->Parameters.TimeScale;
    newBoard->SetPreRollFrames( iSection->GetPreRollFrames() );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
    BoardSequenceTools::UpdateViewRange( iSequencer, newBoard ? newBoard->GetTrueRange() : TRange<FFrameNumber>::Empty() );
    iSequencer->EmptySelection();
    iSequencer->SelectSection( newBoard );
    iSequencer->ThrobSectionSelection();

    if( !newBoard->GetSequence() )
        return;

    if( newBoard->GetSequence()->IsA<UShotSequence>() )
        ShotSequenceTools::CloneInnerContent( iSequencer, newBoard );
}

//static
void
ShotSequenceTools::CloneInnerContent( ISequencer* iSequencer, UMovieSceneSubSection* iSection )
{
    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *iSequencer, *iSection, iSequencer->GetFocusedTemplateID() );

    result.mInnerSequence->Modify();
    result.mInnerMovieScene->Modify();

    //---

    FGuid camera_guid;
    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, &camera_guid );
    if( !camera )
        return;

    FActorSpawnParameters cameraSpawnParams;
    cameraSpawnParams.Template = camera;
    ACineCameraActor* cloned_camera = camera->GetWorld()->SpawnActor<ACineCameraActor>( cameraSpawnParams );
    if( !cloned_camera )
        return;

    cloned_camera->SetFolderPath( *FPaths::GetBaseFilename( iSequencer->GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::SetActorLabelUnique( cloned_camera, TEXT( "Camera_1" ) ); // The shot name is displayed in another column in the world outliner

    //-

    result.mInnerSequence->UnbindPossessableObjects( camera_guid );
    result.mInnerSequence->BindPossessableObject( camera_guid, *cloned_camera, iSequencer->GetPlaybackContext() );
    result.mInnerMovieScene->FindPossessable( camera_guid )->SetName( cloned_camera->GetActorLabel() );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );

    //---

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;
    int32 plane_count = ShotSequenceHelpers::GetAllPlanes( *iSequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAll, &planes, &plane_bindings );

    for( int i = 0; i < plane_count; i++ )
    {
        bool attachPlaneToCamera = false;
        USceneComponent* RootComp = planes[i]->GetRootComponent();
        if( RootComp && RootComp->GetAttachParent() )
        {
            AActor* ParentActor = RootComp->GetAttachParent()->GetOwner();
            attachPlaneToCamera = ( ParentActor == camera );
        }

        CloneInnerPlane( iSequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerMovieScene, planes[i], plane_bindings[i], cloned_camera, attachPlaneToCamera );
    }
}

//static
void
ShotSequenceTools::CloneInnerPlane( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieScene* iMovieScene, APlaneActor* iPlaneToClone, FGuid iPlaneBinding, ACineCameraActor* iClonedCamera, bool iAttachPlaneToCamera )
{
    FActorSpawnParameters planeSpawnParams;
    planeSpawnParams.Template = iPlaneToClone;
    APlaneActor* cloned_plane = iPlaneToClone->GetWorld()->SpawnActor<APlaneActor>( planeSpawnParams );
    if( !cloned_plane )
        return;

    cloned_plane->SetFolderPath( *FPaths::GetBaseFilename( iSequencer->GetRootMovieSceneSequence()->GetPathName() ) );
    FActorLabelUtilities::SetActorLabelUnique( cloned_plane, TEXT( "Plane_1" ) ); // The shot name is displayed in another column in the world outliner

    cloned_plane->SetActorTransform( iPlaneToClone->GetTransform() );

    //-

    if( iAttachPlaneToCamera )
        GEditor->ParentActors( iClonedCamera, cloned_plane, NAME_None );

    //-

    iSequence->UnbindPossessableObjects( iPlaneBinding );
    iSequence->BindPossessableObject( iPlaneBinding, *cloned_plane, iSequencer->GetPlaybackContext() );
    iMovieScene->FindPossessable( iPlaneBinding )->SetName( cloned_plane->GetActorLabel() );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );

    //---

    TArray<ShotSequenceHelpers::FDrawingData> drawings;
    int32 drawing_count = ShotSequenceHelpers::GetAllDrawings( *iSequencer, iSequence, iSequenceID, iPlaneBinding, &drawings );

    for( auto& drawing : drawings )
    {
        drawing.mSection->Modify();

        FKeyHandle key_handle = drawing.mChannel->GetData().GetHandle( drawing.mKeyIndex );

        FMovieSceneObjectPathChannelKeyValue value;
        UE::MovieScene::GetKeyValue( drawing.mChannel, key_handle, value );
        UMaterialInstance* material = Cast<UMaterialInstance>( value.Get() );
        if( !material )
            continue;

        UMaterialInstanceConstant* new_material = ProjectAssetTools::CloneMaterialAndTexture( iSequence, material, iSequencer->GetRootMovieSceneSequence() );
        if( !new_material )
            continue;

        FMovieSceneObjectPathChannelKeyValue new_value( new_material );
        UE::MovieScene::AssignValue( drawing.mChannel, key_handle, new_value );
    }

    if( drawing_count )
        UE::MovieScene::SetChannelDefault( drawings[0].mChannel, nullptr );

    //-

    if( drawing_count )
    {
        FKeyHandle key_handle = drawings[0].mChannel->GetData().GetHandle( drawings[0].mKeyIndex );

        FMovieSceneObjectPathChannelKeyValue value;
        UE::MovieScene::GetKeyValue( drawings[0].mChannel, key_handle, value );
        UMaterialInstance* material = Cast<UMaterialInstance>( value.Get() );

        cloned_plane->GetStaticMeshComponent()->SetMaterial( 0, material );
    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

#undef LOCTEXT_NAMESPACE
