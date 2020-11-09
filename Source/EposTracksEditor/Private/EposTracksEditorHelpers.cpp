// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposTracksEditorHelpers.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Helpers/SectionsHelpersArrange.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "EposTracksEditorHelpers"

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
}

//static
FString
EposTracksEditorHelpers::GenerateNewSequencePath( UMovieScene* iSequenceMovieScene, FString& ioNewSequenceName )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    UObject* sequenceAsset = iSequenceMovieScene->GetOuter();
    UPackage* sequencePackage = sequenceAsset->GetOutermost();
    FString sequencePackageName = sequencePackage->GetName(); // ie. /Game/cine/max/master
    int32 lastSlashPos = sequencePackageName.Find( TEXT( "/" ), ESearchCase::IgnoreCase, ESearchDir::FromEnd );
    FString sequencePath = sequencePackageName.Left( lastSlashPos );

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
EposTracksEditorHelpers::GenerateNewSectionName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    UMovieSceneCinematicBoardSection* beforeShot = nullptr;
    UMovieSceneCinematicBoardSection* nextShot = nullptr;

    FFrameNumber minEndDiff = TNumericLimits<int32>::Max();
    FFrameNumber minStartDiff = TNumericLimits<int32>::Max();

    for( auto section : iAllSections )
    {
        if( section->HasEndFrame() && section->GetExclusiveEndFrame() >= iTime )
        {
            FFrameNumber endDiff = section->GetExclusiveEndFrame() - iTime;
            if( minEndDiff > endDiff )
            {
                minEndDiff = endDiff;
                beforeShot = Cast<UMovieSceneCinematicBoardSection>( section );
            }
        }
        if( section->HasStartFrame() && section->GetInclusiveStartFrame() <= iTime )
        {
            FFrameNumber startDiff = iTime - section->GetInclusiveStartFrame();
            if( minStartDiff > startDiff )
            {
                minStartDiff = startDiff;
                nextShot = Cast<UMovieSceneCinematicBoardSection>( section );
            }
        }
    }

    FString prefix = ( SequenceClass::StaticClass() == UBoardSequence::StaticClass() ) ? TEXT( "board" ) : TEXT( "shot" );

    // There aren't any shots, let's create the first shot name
    if( beforeShot == nullptr || nextShot == nullptr )
    {
        // Default case
    }
    // This is the last shot
    else if( beforeShot == nextShot )
    {
        FString nextShotPrefix = prefix;
        //FString nextShotPrefix = projectSettings->ShotPrefix;
        uint32 nextShotNumber = projectSettings->FirstShotNumber;
        uint32 nextTakeNumber = projectSettings->FirstTakeNumber;

        if( MovieSceneToolHelpers::ParseShotName( nextShot->GetBoardDisplayName(), nextShotPrefix, nextShotNumber, nextTakeNumber ) )
        {
            uint32 newShotNumber = nextShotNumber + projectSettings->ShotIncrement;
            return MovieSceneToolHelpers::ComposeShotName( nextShotPrefix, newShotNumber, projectSettings->FirstTakeNumber );
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
                return MovieSceneToolHelpers::ComposeShotName( beforeShotPrefix, newShotNumber, projectSettings->FirstTakeNumber );
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
EposTracksEditorHelpers::FindOrCreateCinematicBoardTrack( ISequencer* iSequencer )
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
EposTracksEditorHelpers::CreateSequenceInternal( ISequencer* iSequencer, FString& ioNewSequenceName, FFrameNumber iNewSectionStartTime, UMovieSceneCinematicBoardSection* iSectionToDuplicate )
{
    FString newBoardPath;

    if( iSectionToDuplicate != nullptr )
    {
        // If duplicating a board, use that board's path
        newBoardPath = FPaths::GetPath( iSectionToDuplicate->GetSequence()->GetPathName() );
    }
    else
    {
        newBoardPath = EposTracksEditorHelpers::GenerateNewSequencePath( iSequencer->GetRootMovieSceneSequence()->GetMovieScene(), ioNewSequenceName );
        //newBoardPath = EposTracksEditorHelpers::GenerateNewBoardPath( iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene(), ioNewSequenceName );
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
                    newAsset = assetTools.DuplicateAsset( ioNewSequenceName, newBoardPath, iSectionToDuplicate->GetSequence() );
                    //newAsset = assetTools.DuplicateAssetWithDialog( ioNewSequenceName, newBoardPath, iSectionToDuplicate->GetSequence() );
                }
                else
                {
                    newAsset = assetTools.CreateAsset( ioNewSequenceName, newBoardPath, SequenceClass::StaticClass(), factory );
                    //newAsset = assetTools.CreateAssetWithDialog( ioNewSequenceName, newBoardPath, SequenceClass::StaticClass(), factory );

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

    int32 duration = UE::MovieScene::DiscreteSize( iSectionToDuplicate ? iSectionToDuplicate->GetRange() : newSequence->GetMovieScene()->GetPlaybackRange() );

    UMovieSceneCinematicBoardTrack* boardTrack = EposTracksEditorHelpers::FindOrCreateCinematicBoardTrack( iSequencer );

    // Create a board section.
    UMovieSceneSubSection* newSection = boardTrack->AddSequence( newSequence, iNewSectionStartTime, duration );

    return newSection;
}

template<typename SequenceClass>
//static
void
EposTracksEditorHelpers::InsertSequence( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    const FScopedTransaction transaction( LOCTEXT( "InsertBoard_Transaction", "Insert Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = EposTracksEditorHelpers::FindOrCreateCinematicBoardTrack( iSequencer );
    FString newBoardName = EposTracksEditorHelpers::GenerateNewSectionName<SequenceClass>( boardTrack->GetAllSections(), iFrameNumber );

    UMovieSceneSubSection* newBoard = EposTracksEditorHelpers::CreateSequenceInternal<SequenceClass>( iSequencer, newBoardName, iFrameNumber );
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
EposTracksEditorHelpers::InsertBoard( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    EposTracksEditorHelpers::InsertSequence<UBoardSequence>( iSequencer, iFrameNumber );
}
//static
void
EposTracksEditorHelpers::InsertShot( ISequencer* iSequencer, FFrameNumber iFrameNumber )
{
    EposTracksEditorHelpers::InsertSequence<UShotSequence>( iSequencer, iFrameNumber );
}

//static
void
EposTracksEditorHelpers::InsertFiller( ISequencer* iSequencer )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    const FScopedTransaction transaction( LOCTEXT( "InsertFiller_Transaction", "Insert Filler" ) );

    FQualifiedFrameTime currentTime = iSequencer->GetLocalTime();

    UMovieSceneCinematicBoardTrack* boardTrack = EposTracksEditorHelpers::FindOrCreateCinematicBoardTrack( iSequencer );

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
EposTracksEditorHelpers::DuplicateSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection )
{
    UMovieSceneSequence* subsequence = iSection->GetSequence();
    if( !subsequence )
        return;

    const FScopedTransaction transaction( LOCTEXT( "DuplicateBoard_Transaction", "Duplicate Board" ) );

    UMovieSceneCinematicBoardTrack* boardTrack = EposTracksEditorHelpers::FindOrCreateCinematicBoardTrack( iSequencer );

    FFrameNumber startTime = iSection->HasStartFrame() ? iSection->GetInclusiveStartFrame() : 0;
    FString newBoardName;
    if( subsequence->IsA<UBoardSequence>() )
        newBoardName = EposTracksEditorHelpers::GenerateNewSectionName<UBoardSequence>( boardTrack->GetAllSections(), startTime );
    else
        newBoardName = EposTracksEditorHelpers::GenerateNewSectionName<UShotSequence>( boardTrack->GetAllSections(), startTime );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* newBoard;
    if( subsequence->IsA<UBoardSequence>() )
        newBoard = EposTracksEditorHelpers::CreateSequenceInternal<UBoardSequence>( iSequencer, newBoardName, startTime, iSection );
    else
        newBoard = EposTracksEditorHelpers::CreateSequenceInternal<UShotSequence>( iSequencer, newBoardName, startTime, iSection );

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

//---

//static
void
EposTracksEditorHelpers::ArrangeSections( ISequencer* iSequencer )
{
    auto track = iSequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !track )
        return;

    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();

    SectionsHelpersArrange::Arrange( track, settings->BoardTrackSettings.ArrangeShots );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

//static
void
EposTracksEditorHelpers::SetArrangeSections( ISequencer* iSequencer, EArrangeSections iArrangeSections )
{
    UEposTracksEditorSettings* settings = GetMutableDefault<UEposTracksEditorSettings>();
    settings->BoardTrackSettings.ArrangeShots = iArrangeSections;
    settings->SaveConfig();

    EposTracksEditorHelpers::ArrangeSections( iSequencer );
}



#undef LOCTEXT_NAMESPACE
