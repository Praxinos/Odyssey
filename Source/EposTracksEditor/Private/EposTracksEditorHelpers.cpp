// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposTracksEditorHelpers.h"

#include "AssetRegistryModule.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "BoardTrack/MovieSceneBoardSection.h"

// Same functions as in MovieSceneToolHelpers.cpp
// Convert ULevelSequence/UMovieSceneCinematicShotSection to UBoardSequence/UMovieSceneBoardSection

//---

namespace
{
static bool IsPackageNameUnique( const TArray<FAssetData>& ObjectList, FString& NewPackageName )
{
    for( auto AssetObject : ObjectList )
    {
        if( AssetObject.PackageName.ToString() == NewPackageName )
        {
            return false;
        }
    }
    return true;
}
}

FString
EposTracksEditorHelpers::GenerateNewBoardPath( UMovieScene* SequenceMovieScene, FString& NewShotName )
{
    const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    TArray<FAssetData> ObjectList;
    AssetRegistryModule.Get().GetAssetsByClass( UBoardSequence::StaticClass()->GetFName(), ObjectList );

    UObject* SequenceAsset = SequenceMovieScene->GetOuter();
    UPackage* SequencePackage = SequenceAsset->GetOutermost();
    FString SequencePackageName = SequencePackage->GetName(); // ie. /Game/cine/max/master
    int32 LastSlashPos = SequencePackageName.Find( TEXT( "/" ), ESearchCase::IgnoreCase, ESearchDir::FromEnd );
    FString SequencePath = SequencePackageName.Left( LastSlashPos );

    FString NewShotPrefix;
    uint32 NewShotNumber = INDEX_NONE;
    uint32 NewTakeNumber = INDEX_NONE;
    MovieSceneToolHelpers::ParseShotName( NewShotName, NewShotPrefix, NewShotNumber, NewTakeNumber );

    FString NewShotDirectory = MovieSceneToolHelpers::ComposeShotName( NewShotPrefix, NewShotNumber, INDEX_NONE );
    FString NewShotPath = SequencePath;

    FString ShotDirectory = ProjectSettings->ShotDirectory;
    if( !ShotDirectory.IsEmpty() )
    {
        NewShotPath /= ShotDirectory;
    }
    NewShotPath /= NewShotDirectory; // put this in the shot directory, ie. /Game/cine/max/shots/shot0010

    // Make sure this shot path is unique
    FString NewPackageName = NewShotPath;
    NewPackageName /= NewShotName; // ie. /Game/cine/max/shots/shot0010/shot0010_001
    if( !IsPackageNameUnique( ObjectList, NewPackageName ) )
    {
        while( 1 )
        {
            NewShotNumber += ProjectSettings->ShotIncrement;
            NewShotName = MovieSceneToolHelpers::ComposeShotName( NewShotPrefix, NewShotNumber, NewTakeNumber );
            NewShotDirectory = MovieSceneToolHelpers::ComposeShotName( NewShotPrefix, NewShotNumber, INDEX_NONE );
            NewShotPath = SequencePath;
            if( !ShotDirectory.IsEmpty() )
            {
                NewShotPath /= ShotDirectory;
            }
            NewShotPath /= NewShotDirectory;

            NewPackageName = NewShotPath;
            NewPackageName /= NewShotName;
            if( IsPackageNameUnique( ObjectList, NewPackageName ) )
            {
                break;
            }
        }
    }

    return NewShotPath;
}


FString
EposTracksEditorHelpers::GenerateNewBoardName( const TArray<UMovieSceneSection*>& AllSections, FFrameNumber Time )
{
    const UMovieSceneToolsProjectSettings* ProjectSettings = GetDefault<UMovieSceneToolsProjectSettings>();

    UMovieSceneBoardSection* BeforeShot = nullptr;
    UMovieSceneBoardSection* NextShot = nullptr;

    FFrameNumber MinEndDiff = TNumericLimits<int32>::Max();
    FFrameNumber MinStartDiff = TNumericLimits<int32>::Max();

    for( auto Section : AllSections )
    {
        if( Section->HasEndFrame() && Section->GetExclusiveEndFrame() >= Time )
        {
            FFrameNumber EndDiff = Section->GetExclusiveEndFrame() - Time;
            if( MinEndDiff > EndDiff )
            {
                MinEndDiff = EndDiff;
                BeforeShot = Cast<UMovieSceneBoardSection>( Section );
            }
        }
        if( Section->HasStartFrame() && Section->GetInclusiveStartFrame() <= Time )
        {
            FFrameNumber StartDiff = Time - Section->GetInclusiveStartFrame();
            if( MinStartDiff > StartDiff )
            {
                MinStartDiff = StartDiff;
                NextShot = Cast<UMovieSceneBoardSection>( Section );
            }
        }
    }

    // There aren't any shots, let's create the first shot name
    if( BeforeShot == nullptr || NextShot == nullptr )
    {
        // Default case
    }
    // This is the last shot
    else if( BeforeShot == NextShot )
    {
        FString NextShotPrefix = ProjectSettings->ShotPrefix;
        uint32 NextShotNumber = ProjectSettings->FirstShotNumber;
        uint32 NextTakeNumber = ProjectSettings->FirstTakeNumber;

        if( MovieSceneToolHelpers::ParseShotName( NextShot->GetBoardDisplayName(), NextShotPrefix, NextShotNumber, NextTakeNumber ) )
        {
            uint32 NewShotNumber = NextShotNumber + ProjectSettings->ShotIncrement;
            return MovieSceneToolHelpers::ComposeShotName( NextShotPrefix, NewShotNumber, ProjectSettings->FirstTakeNumber );
        }
    }
    // This is in between two shots
    else
    {
        FString BeforeShotPrefix = ProjectSettings->ShotPrefix;
        uint32 BeforeShotNumber = ProjectSettings->FirstShotNumber;
        uint32 BeforeTakeNumber = ProjectSettings->FirstTakeNumber;

        FString NextShotPrefix = ProjectSettings->ShotPrefix;
        uint32 NextShotNumber = ProjectSettings->FirstShotNumber;
        uint32 NextTakeNumber = ProjectSettings->FirstTakeNumber;

        if( MovieSceneToolHelpers::ParseShotName( BeforeShot->GetBoardDisplayName(), BeforeShotPrefix, BeforeShotNumber, BeforeTakeNumber ) &&
            MovieSceneToolHelpers::ParseShotName( NextShot->GetBoardDisplayName(), NextShotPrefix, NextShotNumber, NextTakeNumber ) )
        {
            if( BeforeShotNumber < NextShotNumber )
            {
                uint32 NewShotNumber = BeforeShotNumber + ( ( NextShotNumber - BeforeShotNumber ) / 2 ); // what if we can't find one? or conflicts with another?
                return MovieSceneToolHelpers::ComposeShotName( BeforeShotPrefix, NewShotNumber, ProjectSettings->FirstTakeNumber );
            }
        }
    }

    // Default case
    return MovieSceneToolHelpers::ComposeShotName( ProjectSettings->ShotPrefix, ProjectSettings->FirstShotNumber, ProjectSettings->FirstTakeNumber );
}
