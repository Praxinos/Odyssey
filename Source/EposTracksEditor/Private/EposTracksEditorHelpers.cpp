// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposTracksEditorHelpers.h"

#include "AssetRegistryModule.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"

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

FString
EposTracksEditorHelpers::GenerateNewBoardPath( UMovieScene* iSequenceMovieScene, FString& ioNewBoardName )
{
    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    TArray<FAssetData> objectList;
    assetRegistryModule.Get().GetAssetsByClass( UBoardSequence::StaticClass()->GetFName(), objectList );

    UObject* sequenceAsset = iSequenceMovieScene->GetOuter();
    UPackage* sequencePackage = sequenceAsset->GetOutermost();
    FString sequencePackageName = sequencePackage->GetName(); // ie. /Game/cine/max/master
    int32 lastSlashPos = sequencePackageName.Find( TEXT( "/" ), ESearchCase::IgnoreCase, ESearchDir::FromEnd );
    FString sequencePath = sequencePackageName.Left( lastSlashPos );

    FString newShotPrefix;
    uint32 newShotNumber = INDEX_NONE;
    uint32 newTakeNumber = INDEX_NONE;
    MovieSceneToolHelpers::ParseShotName( ioNewBoardName, newShotPrefix, newShotNumber, newTakeNumber );

    FString newShotDirectory = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, INDEX_NONE );
    FString newShotPath = sequencePath;

    FString shotDirectory = projectSettings->ShotDirectory;
    if( !shotDirectory.IsEmpty() )
    {
        newShotPath /= shotDirectory;
    }
    newShotPath /= newShotDirectory; // put this in the shot directory, ie. /Game/cine/max/shots/shot0010

    // Make sure this shot path is unique
    FString newPackageName = newShotPath;
    newPackageName /= ioNewBoardName; // ie. /Game/cine/max/shots/shot0010/shot0010_001
    if( !IsPackageNameUnique( objectList, newPackageName ) )
    {
        while( 1 )
        {
            newShotNumber += projectSettings->ShotIncrement;
            ioNewBoardName = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, newTakeNumber );
            newShotDirectory = MovieSceneToolHelpers::ComposeShotName( newShotPrefix, newShotNumber, INDEX_NONE );
            newShotPath = sequencePath;
            if( !shotDirectory.IsEmpty() )
            {
                newShotPath /= shotDirectory;
            }
            newShotPath /= newShotDirectory;

            newPackageName = newShotPath;
            newPackageName /= ioNewBoardName;
            if( IsPackageNameUnique( objectList, newPackageName ) )
            {
                break;
            }
        }
    }

    return newShotPath;
}


FString
EposTracksEditorHelpers::GenerateNewBoardName( const TArray<UMovieSceneSection*>& iAllSections, FFrameNumber iTime )
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

    // There aren't any shots, let's create the first shot name
    if( beforeShot == nullptr || nextShot == nullptr )
    {
        // Default case
    }
    // This is the last shot
    else if( beforeShot == nextShot )
    {
        FString nextShotPrefix = projectSettings->ShotPrefix;
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
        FString beforeShotPrefix = projectSettings->ShotPrefix;
        uint32 beforeShotNumber = projectSettings->FirstShotNumber;
        uint32 beforeTakeNumber = projectSettings->FirstTakeNumber;

        FString nextShotPrefix = projectSettings->ShotPrefix;
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
    return MovieSceneToolHelpers::ComposeShotName( projectSettings->ShotPrefix, projectSettings->FirstShotNumber, projectSettings->FirstTakeNumber );
}
