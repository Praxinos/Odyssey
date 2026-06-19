// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/EposSequenceTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CineCameraActor.h"
#include "Components/StaticMeshComponent.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Factories/Factory.h"
#include "Framework/Notifications/NotificationManager.h"
#include "IAssetTools.h"
#include "LevelEditorSubsystem.h"
#include "LevelUtils.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneTimeHelpers.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"
#include "SequencerUtilities.h"
#include "Subsystems/EditorActorSubsystem.h"
#include "UObject/UObjectIterator.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineSection.h"
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

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindTrack<UMovieSceneCinematicBoardTrack>();
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

    UMovieSceneCinematicBoardTrack* boardTrack = focusedMovieScene->FindTrack<UMovieSceneCinematicBoardTrack>();
    if( boardTrack != nullptr )
    {
        return boardTrack;
    }

    const FScopedTransaction transaction( LOCTEXT( "AddCinematicBoardTrack_Transaction", "Add Board Track" ) );
    focusedMovieScene->Modify();

    auto newTrack = focusedMovieScene->AddTrack<UMovieSceneCinematicBoardTrack>();
    ensure( newTrack );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );

    return newTrack;
}

//---

//static
void
ShotSequenceTools::StepToNextShot( ISequencer* iSequencer )
{
    if( !Cast<UShotSequence>( iSequencer->GetFocusedMovieSceneSequence() ) )
        return;

    StepToNextShot( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() );
}

//static
void
ShotSequenceTools::StepToPreviousShot( ISequencer* iSequencer )
{
    if( !Cast<UShotSequence>( iSequencer->GetFocusedMovieSceneSequence() ) )
        return;

    StepToPreviousShot( *iSequencer, iSequencer->GetFocusedMovieSceneSequence(), iSequencer->GetFocusedTemplateID() );
}

//static
void
ShotSequenceTools::StepToNextShot( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    if( iSequenceID == MovieSceneSequenceID::Root )
        return;

    UMovieSceneSubSection* subsection = iSequencer.FindSubSection( iSequenceID );
    if( !subsection )
        return;

    UMovieSceneSequence* parent_sequence = subsection->GetTypedOuter<UMovieSceneSequence>();
    check( parent_sequence );

    FMovieSceneInverseSequenceTransform localToRootTransform = subsection->OuterToInnerTransform().Inverse();
    TOptional<FFrameTime> time_in_parent = localToRootTransform.TryTransformTime( iSequence->GetMovieScene()->GetPlaybackRange().GetLowerBoundValue() );
    if( !time_in_parent )
        return;

    UMovieSceneSubSection* next_subsection = FindNextOrPreviousShot( parent_sequence, time_in_parent->FloorToFrame(), true /* iNextShot */ );
    if( !next_subsection )
        return;

    iSequencer.FocusSequenceInstance( *next_subsection );
    iSequencer.SetLocalTime( iSequencer.GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange().GetLowerBoundValue(), ESnapTimeMode::STM_None );
}

//static
void
ShotSequenceTools::StepToPreviousShot( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID )
{
    if( iSequenceID == MovieSceneSequenceID::Root )
        return;

    UMovieSceneSubSection* subsection = iSequencer.FindSubSection( iSequenceID );
    if( !subsection )
        return;

    UMovieSceneSequence* parent_sequence = subsection->GetTypedOuter<UMovieSceneSequence>();
    check( parent_sequence );

    FMovieSceneInverseSequenceTransform localToRootTransform = subsection->OuterToInnerTransform().Inverse();
    TOptional<FFrameTime> time_in_parent = localToRootTransform.TryTransformTime( iSequence->GetMovieScene()->GetPlaybackRange().GetLowerBoundValue() );
    if( !time_in_parent )
        return;

    UMovieSceneSubSection* previous_subsection = FindNextOrPreviousShot( parent_sequence, time_in_parent->FloorToFrame(), false /* iNextShot */ );
    if( !previous_subsection )
        return;

    iSequencer.FocusSequenceInstance( *previous_subsection );
    iSequencer.SetLocalTime( iSequencer.GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange().GetLowerBoundValue(), ESnapTimeMode::STM_None );
}

// Same as in FSequencer::FindNextOrPreviousShot()#2600
//static
UMovieSceneSubSection*
ShotSequenceTools::FindNextOrPreviousShot( UMovieSceneSequence* iSequence, FFrameNumber iSearchFromTime, bool iNextShot )
{
    UMovieScene* OwnerMovieScene = iSequence->GetMovieScene();

    UMovieSceneTrack* CinematicBoardTrack = OwnerMovieScene->FindTrack( UMovieSceneCinematicBoardTrack::StaticClass() );
    if( !CinematicBoardTrack )
    {
        return nullptr;
    }

    FFrameNumber MinTime = TNumericLimits<FFrameNumber>::Max();

    TMap<FFrameNumber, int32> StartTimeMap;
    for( int32 SectionIndex = 0; SectionIndex < CinematicBoardTrack->GetAllSections().Num(); ++SectionIndex )
    {
        UMovieSceneSection* ShotSection = CinematicBoardTrack->GetAllSections()[SectionIndex];

        if( ShotSection && ShotSection->HasStartFrame() )
        {
            StartTimeMap.Add( ShotSection->GetInclusiveStartFrame(), SectionIndex );
        }
    }

    StartTimeMap.KeySort( TLess<FFrameNumber>() );

    int32 MinShotIndex = -1;
    for( auto StartTimeIt = StartTimeMap.CreateIterator(); StartTimeIt; ++StartTimeIt )
    {
        FFrameNumber StartTime = StartTimeIt->Key;
        if( iNextShot )
        {
            if( StartTime > iSearchFromTime )
            {
                FFrameNumber DiffTime = FMath::Abs( StartTime - iSearchFromTime );
                if( DiffTime < MinTime )
                {
                    MinTime = DiffTime;
                    MinShotIndex = StartTimeIt->Value;
                }
            }
        }
        else
        {
            if( iSearchFromTime >= StartTime )
            {
                FFrameNumber DiffTime = FMath::Abs( StartTime - iSearchFromTime );
                if( DiffTime < MinTime )
                {
                    MinTime = DiffTime;
                    MinShotIndex = StartTimeIt->Value;
                }
            }
        }
    }

    int32 TargetShotIndex = -1;

    if( iNextShot )
    {
        TargetShotIndex = MinShotIndex;
    }
    else
    {
        int32 PreviousShotIndex = -1;
        for( auto StartTimeIt = StartTimeMap.CreateIterator(); StartTimeIt; ++StartTimeIt )
        {
            if( StartTimeIt->Value == MinShotIndex )
            {
                if( PreviousShotIndex != -1 )
                {
                    TargetShotIndex = PreviousShotIndex;
                }
                break;
            }
            PreviousShotIndex = StartTimeIt->Value;
        }
    }

    if( TargetShotIndex == -1 )
    {
        return nullptr;
    }

    return CastChecked<UMovieSceneSubSection>( CinematicBoardTrack->GetAllSections()[TargetShotIndex] );
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

//static
UBoardSequence*
BoardSequenceTools::CreateBoard( const FString& iNewBoardPath, const FString& iNewBoardName )
{
    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    //---

    UObject* NewAsset = nullptr;

    // Attempt to create a new asset
    for( auto factory : AssetTools.GetNewAssetFactories() )
    {
        if( factory->CanCreateNew() && factory->ImportPriority >= 0 && factory->SupportedClass == UBoardSequence::StaticClass() )
        {
            NewAsset = AssetTools.CreateAsset( iNewBoardName, iNewBoardPath, UBoardSequence::StaticClass(), factory );
            break;
        }
    }

    UBoardSequence* board_sequence = Cast<UBoardSequence>( NewAsset );

    if( !board_sequence )
        return nullptr;

    //---

    UNamingConventionSettings* naming_convention_settings = GetMutableDefault<UNamingConventionSettings>();

    board_sequence->NameElements.Index = INDEX_NONE; // To use the real asset name in display

    // Copy all 'global' members from settings global to board elements
    for( TFieldIterator<FProperty> settings_global_property_iterator( FNamingConventionGlobal::StaticStruct() ); settings_global_property_iterator; ++settings_global_property_iterator )
    {
        FProperty* settings_global_property = *settings_global_property_iterator;

        FProperty* board_property = FindFProperty<FProperty>( FBoardNameElements::StaticStruct(), settings_global_property->GetFName() );
        if( settings_global_property->GetName().EndsWith( TEXT( "NumDigits" ) ) )
            continue;

        check( board_property );

        // It doesn't work if the 2 structs are not synchro with the same name of members
        // and I don't know the difference with the (good) outside ContainerPtrToValuePtr<> form below
        //settings_global_property->CopyCompleteValue_InContainer( &board_sequence->NameElements, &mNamingConventionSettings->GlobalNaming );

        const uint8* SourceAddr = settings_global_property->ContainerPtrToValuePtr<uint8>( &naming_convention_settings->GlobalNaming );
        uint8* DestinationAddr = board_property->ContainerPtrToValuePtr<uint8>( &board_sequence->NameElements );

        settings_global_property->CopyCompleteValue( DestinationAddr, SourceAddr );
    }

    // Copy all 'user' members from settings user to board elements
    for( TFieldIterator<FProperty> settings_user_property_iterator( FNamingConventionUser::StaticStruct() ); settings_user_property_iterator; ++settings_user_property_iterator )
    {
        FProperty* settings_user_property = *settings_user_property_iterator;

        FProperty* board_property = FindFProperty<FProperty>( FBoardNameElements::StaticStruct(), settings_user_property->GetFName() );
        check( board_property );

        const uint8* SourceAddr = settings_user_property->ContainerPtrToValuePtr<uint8>( &naming_convention_settings->UserNaming );
        uint8* DestinationAddr = board_property->ContainerPtrToValuePtr<uint8>( &board_sequence->NameElements );

        settings_user_property->CopyCompleteValue( DestinationAddr, SourceAddr );
    }

    return board_sequence;
}

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
    FMovieSceneSequenceID epos_sequence_id = iSequencer->GetFocusedTemplateID();
    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequencer->GetFocusedMovieSceneSequence() );
    if( !epos_sequence )
        return nullptr;

    EjectAnyActor();

    const FScopedTransaction transaction( LOCTEXT( "transaction.insert-board", "Insert Board" ) );

    FString sequence_path;
    FString sequence_name;
    FBoardNameElements board_name_elements;
    FString sequence_pathname = NamingConvention::GenerateBoardAssetPathName( *iSequencer, *epos_sequence, epos_sequence_id, iFrameNumber, sequence_path, sequence_name, board_name_elements );

    UMovieSceneSubSection* new_section = CreateSequenceInternal<UBoardSequence>( iSequencer, sequence_path, sequence_name, iFrameNumber, iDuration );

    if( new_section && new_section->GetSequence() )
    {
        UBoardSequence* board_sequence = CastChecked<UBoardSequence>( new_section->GetSequence() );
        board_sequence->NameElements = board_name_elements;
    }

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
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
    FMovieSceneSequenceID epos_sequence_id = iSequencer->GetFocusedTemplateID();
    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequencer->GetFocusedMovieSceneSequence() );
    if( !epos_sequence )
        return nullptr;

    EjectAnyActor();

    const FScopedTransaction transaction( LOCTEXT( "transaction.insert-shot", "Insert Shot" ) );

    FString sequence_path;
    FString sequence_name;
    FShotNameElements shot_name_elements;
    FString sequence_pathname = NamingConvention::GenerateShotAssetPathName( *iSequencer, *epos_sequence, epos_sequence_id, iFrameNumber, sequence_path, sequence_name, shot_name_elements );

    //---

    UMovieSceneSubSection* new_section = CreateSequenceInternal<UShotSequence>( iSequencer, sequence_path, sequence_name, iFrameNumber, iDuration );

    //---

    if( new_section && new_section->GetSequence() )
    {
        UShotSequence* shot_sequence = CastChecked<UShotSequence>( new_section->GetSequence() );
        shot_sequence->NameElements = shot_name_elements;
    }

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately );
    //iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemAdded );
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

//---

//static
UMovieSceneSubSection*
CinematicBoardTrackTools::CloneSection( ISequencer* iSequencer, UMovieSceneCinematicBoardSection* iSection, FFrameNumber iFrameNumber, bool iEmptyDrawings )
{
    UMovieSceneSequence* subsequence = iSection->GetSequence();
    if( !subsequence )
        return nullptr;
    if( subsequence->IsA<UBoardSequence>() )
        return nullptr;

    FMovieSceneSequenceID epos_sequence_id;
    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( BoardSequenceHelpers::FindSequenceOfSubSection( *iSequencer, *iSection, epos_sequence_id ) );
    if( !epos_sequence )
        return nullptr;

    ULevelEditorSubsystem* levelEditorSubsystem = GEditor->GetEditorSubsystem<ULevelEditorSubsystem>();
    if( FLevelUtils::IsLevelLocked( levelEditorSubsystem->GetCurrentLevel() ) )
    {
        FNotificationInfo Info( LOCTEXT( "cant-clone-in-locked-level", "The requested operation could not be completed because the level is locked." ) );
        Info.ExpireDuration = 5.0f;
        TSharedPtr<SNotificationItem> notification = FSlateNotificationManager::Get().AddNotification( Info );
        if (notification)
            notification->SetCompletionState( SNotificationItem::CS_Fail );
        return nullptr;
    }

    EjectAnyActor();

    //---

    const FScopedTransaction transaction( LOCTEXT( "CloneSection_Transaction", "Clone Section" ) );

    FString sequence_path;
    FString sequence_name;
    FShotNameElements shot_name_elements;
    FString sequence_pathname = NamingConvention::GenerateShotAssetPathName( *iSequencer, *epos_sequence, epos_sequence_id, iFrameNumber, sequence_path, sequence_name, shot_name_elements );

    // Duplicate the board and put it on the next available row
    UMovieSceneSubSection* new_section = CreateSequenceInternal<UShotSequence>( iSequencer, sequence_path, sequence_name, iFrameNumber, TOptional<int32>(), iSection );

    if( !new_section )
        return nullptr;

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

    return new_section;
}

//static
void
ShotSequenceTools::CloneInnerContent( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iEmptyDrawings )
{
    FGuid camera_guid = ShotSequenceHelpers::GetCameraBinding( *iSequencer, iSequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawnedOrTemplate( *iSequencer, iSequence, iSequenceID, camera_guid );
    if( !camera )
        return;

    UObject* object = MovieSceneHelpers::GetObjectTemplate( iSequence, camera_guid, iSequencer->GetSharedPlaybackState(), 0 );
    ACineCameraActor* camera_template = Cast<ACineCameraActor>( object );

    bool bIsSpawnable = MovieSceneHelpers::IsBoundToAnySpawnable( iSequence, camera_guid, iSequencer->GetSharedPlaybackState() );
    check( bIsSpawnable == !!camera_template );

    //---

    check( !FLevelUtils::IsLevelLocked( camera->GetWorld()->GetCurrentLevel() ) );

    iSequence->Modify();
    iSequence->GetMovieScene()->Modify();

    //---

    ACineCameraActor* cloned_camera = nullptr;
    if( bIsSpawnable )
    {
        cloned_camera = camera;
    }
    else
    {
        FActorSpawnParameters cameraSpawnParams;
        cameraSpawnParams.Template = camera;
        cloned_camera = camera->GetWorld()->SpawnActor<ACineCameraActor>( cameraSpawnParams );
        if( !cloned_camera )
            return;
    }

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    check( epos_sequence );

    FString cloned_camera_path;
    FString cloned_camera_name;
    NamingConvention::GenerateCameraActorPathName( *iSequencer, *epos_sequence, iSequenceID, !bIsSpawnable, cloned_camera_path, cloned_camera_name );
    // We don't keep the same name as the original camera (like animation), to be able to increment the (global) index or to use the new shot name

    cloned_camera->SetFolderPath( *cloned_camera_path );
    FActorLabelUtilities::RenameExistingActor( cloned_camera, cloned_camera_name, false ); // The shot name is displayed in another column in the world outliner

    //-

    if( !bIsSpawnable )
    {
        cTemporarySwitchInner switch_to( *iSequencer, iSequenceID );

        FSequencerUtilities::ReplaceBindingWithActors( iSequencer->AsShared(), { cloned_camera }, FMovieSceneBindingProxy{ camera_guid, iSequence } );
    }

    FMovieScenePossessable* possessable = iSequence->GetMovieScene()->FindPossessable( camera_guid );
    if( possessable )
    {
        cloned_camera_name = NamingConvention::GenerateCameraTrackName( *iSequencer, *epos_sequence, iSequenceID, cloned_camera );
        possessable->SetName( cloned_camera_name );
    }

    if( bIsSpawnable )
        MovieSceneHelpers::CopyObjectTemplate( iSequence, camera_guid, cloned_camera, iSequencer->GetSharedPlaybackState(), 0 );

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );

    //---

    //TArray<APlaneActor*> planes;
    //TArray<FGuid> plane_bindings;
    //int32 plane_count = ShotSequenceHelpers::GetAllPlanes( *iSequencer, iSequence, iSequenceID, EGetPlane::kAll, &planes, &plane_bindings );

    //for( int i = 0; i < plane_count; i++ )
    //{
    //    AActor* ParentActor = planes[i]->GetAttachParentActor();
    //    bool attachPlaneToCamera = ( ParentActor && ParentActor == camera );

    //    CloneInnerPlane( iSequencer, iSequence, iSequenceID, iSequence->GetMovieScene(), iEmptyDrawings, planes[i], plane_bindings[i], cloned_camera, attachPlaneToCamera );
    //}

    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *iSequencer, iSequence, iSequenceID );

    for( FGuid animation_binding : animation_bindings )
    {
        TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( *iSequencer, iSequence, iSequenceID, animation_binding );
        if( animation_actors.IsEmpty() )
            continue;

        if( bIsSpawnable )
        {
            CloneInnerAnimation( iSequencer, iSequence, iSequenceID, iSequence->GetMovieScene(), animation_binding, bIsSpawnable, cloned_camera, false /* not used with spawnable */ );
        }
        else
        {
            AActor* parentActor = animation_actors[0]->GetAttachParentActor();
            bool attachAnimationToCamera = ( parentActor && parentActor == camera ); //TODO: do it inside CloneInnerAnimation() ???

            CloneInnerAnimation( iSequencer, iSequence, iSequenceID, iSequence->GetMovieScene(), animation_binding, bIsSpawnable, cloned_camera, attachAnimationToCamera );
        }
    }

    //---

    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections = EposSequenceHelpers::GetNotes( *iSequencer, iSequence, iSequenceID );

    for( auto note_section : note_sections )
    {
        UStoryNote* original_note = note_section->GetNote();

        UStoryNote* duplicate_note = ProjectAssetTools::CloneNote( *iSequencer, iSequence, iSequenceID, original_note );

        note_section->SetNote( duplicate_note );
    }

    //---

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::RefreshAllImmediately ); // Otherwise, some internal stuff may not be up-to-date, and new bindings/moviescenesequenceID/... are not available in cache
}
//
////static
//void
//ShotSequenceTools::CloneInnerPlane( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieScene* iMovieScene, bool iEmptyDrawings, APlaneActor* iPlaneToClone, FGuid iPlaneBinding, ACineCameraActor* iClonedCamera, bool iAttachPlaneToCamera )
//{
//    FActorSpawnParameters planeSpawnParams;
//    planeSpawnParams.Template = iPlaneToClone;
//    APlaneActor* cloned_plane = iPlaneToClone->GetWorld()->SpawnActor<APlaneActor>( planeSpawnParams );
//    if( !cloned_plane )
//        return;
//
//    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
//    check( epos_sequence );
//
//    FString cloned_plane_path;
//    FString cloned_plane_name;
//    NamingConvention::GeneratePlaneActorPathName( *iSequencer, *epos_sequence, iSequenceID, cloned_plane_path, cloned_plane_name );
//    cloned_plane_name = iPlaneToClone->GetActorLabel(); // As the plane actor is cloned, just keep the same name (let see when shot/camera name are a part of the plane name...)
//
//    cloned_plane->SetFolderPath( *cloned_plane_path );
//    FActorLabelUtilities::RenameExistingActor( cloned_plane, cloned_plane_name, false ); // The shot name is displayed in another column in the world outliner
//
//    cloned_plane->SetActorTransform( iPlaneToClone->GetTransform() ); // Should be done, because for attached plane, its new transform are totally weird
//    cloned_plane->SetActorHiddenInGame( true ); // As it was created with the class constructor which set it to true, otherwise the actor to clone is certainly displayed, then the cloned actor will have false by default
//
//    //-
//
//    if( iAttachPlaneToCamera )
//        GEditor->ParentActors( iClonedCamera, cloned_plane, NAME_None );
//
//    //-
//
//    cloned_plane_name = NamingConvention::GeneratePlaneTrackName( *iSequencer, *epos_sequence, iSequenceID, cloned_plane );
//
//    iSequence->UnbindPossessableObjects( iPlaneBinding );
//    iSequence->BindPossessableObject( iPlaneBinding, *cloned_plane, iSequencer->GetPlaybackContext() );
//    iMovieScene->FindPossessable( iPlaneBinding )->SetName( cloned_plane_name );
//
//    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
//
//    //---
//
//    TArray<FDrawing> drawings = ShotSequenceHelpers::GetAllDrawings( *iSequencer, iSequence, iSequenceID, iPlaneBinding );
//
//    for( auto& drawing : drawings )
//    {
//        UMaterialInstance* material = drawing.GetMaterial();
//        if( !material )
//            continue;
//
//        UMaterialInstanceConstant* new_material = iEmptyDrawings
//                                                  ? ProjectAssetTools::CreateMaterialAndTexture( *iSequencer, iSequence, iSequenceID, material )
//                                                  : ProjectAssetTools::CloneMaterialAndTexture( *iSequencer, iSequence, iSequenceID, material );
//        if( !new_material )
//            continue;
//
//        drawing.SetMaterial( new_material );
//    }
//
//    // To have no (hidden) dependency with a material (and as it should always be at least one key)
//    if( drawings.Num() )
//        UE::MovieScene::SetChannelDefault( drawings[0].mChannel, nullptr );
//
//    //-
//
//    // To change the initial material of the actor (which is always overrided by the sequencer when it's opened)
//    if( drawings.Num() )
//    {
//        UMaterialInstance* material = drawings[0].GetMaterial();
//
//        cloned_plane->GetStaticMeshComponent()->SetMaterial( 0, material );
//    }
//
//    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
//}

//static
void
ShotSequenceTools::CloneInnerAnimation( ISequencer* iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMovieScene* iMovieScene, FGuid iAnimationBinding, bool iIsSpawnable, ACineCameraActor* iClonedCamera, bool iAttachAnimationToCamera )
{
    TArray<AOdysseyAnimationActor*> animation_actors_to_clone = ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( *iSequencer, iSequence, iSequenceID, iAnimationBinding );
    if( animation_actors_to_clone.IsEmpty() )
        return;

    //UObject* object = MovieSceneHelpers::GetObjectTemplate( iSequence, iAnimationBinding, iSequencer->GetSharedPlaybackState(), 0 );
    //AOdysseyAnimationActor* animation_template = Cast<AOdysseyAnimationActor>( object );

    AOdysseyAnimationActor* animation_actor_to_clone = animation_actors_to_clone[0];

    //---

    AOdysseyAnimationActor* cloned_animation = nullptr;
    if( iIsSpawnable )
    {
        cloned_animation = animation_actor_to_clone;
    }
    else
    {
        UEditorActorSubsystem* editorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

        TArray<AActor*> actors = editorActorSubsystem->DuplicateActors( { animation_actor_to_clone }, animation_actor_to_clone->GetWorld() );
        cloned_animation = Cast<AOdysseyAnimationActor>( actors.Num() ? actors[0] : nullptr );
        if( !cloned_animation )
            return;
    }

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    check( epos_sequence );

    FString cloned_animation_path;
    FString cloned_animation_name;
    NamingConvention::GenerateAnimationActorPathName( *iSequencer, *epos_sequence, iSequenceID, !iIsSpawnable, cloned_animation_path, cloned_animation_name );
    cloned_animation_name = animation_actor_to_clone->GetActorLabel(); // As the animation actor is cloned, just keep the same name (let see when shot/camera name are a part of the animation name...)

    cloned_animation->SetFolderPath( *cloned_animation_path );
    FActorLabelUtilities::RenameExistingActor( cloned_animation, cloned_animation_name, false ); // The shot name is displayed in another column in the world outliner

    //-

    if( !iIsSpawnable )
    {
        cloned_animation->SetActorTransform( animation_actor_to_clone->GetTransform() ); // Should be done, because for attached animation, its new transform are totally weird
        //cloned_animation->SetActorHiddenInGame( true ); // As it was created with the class constructor which set it to true, otherwise the actor to clone is certainly displayed, then the cloned actor will have false by default

        //-

        if( iAttachAnimationToCamera )
            GEditor->ParentActors( iClonedCamera, cloned_animation, NAME_None );

        //-

        cTemporarySwitchInner switch_to( *iSequencer, iSequenceID );

        FSequencerUtilities::ReplaceBindingWithActors( iSequencer->AsShared(), { cloned_animation }, FMovieSceneBindingProxy{ iAnimationBinding, iSequence } );
    }

    FMovieScenePossessable* possessable = iSequence->GetMovieScene()->FindPossessable( iAnimationBinding );
    if( possessable )
    {
        cloned_animation_name = NamingConvention::GenerateAnimationTrackName( *iSequencer, *epos_sequence, iSequenceID, cloned_animation );
        possessable->SetName( cloned_animation_name );
    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );

    //---

    UOdysseyAnimation* new_animation = ProjectAssetTools::CloneAnimation( *iSequencer, iSequence, iSequenceID, animation_actor_to_clone->GetAnimationComponent()->GetAnimation() );
    if( !new_animation )
        return;

    TMap<UOdysseyAnimation*, UOdysseyAnimation*> original_to_new_animation_map;

    original_to_new_animation_map.Add( cloned_animation->GetAnimationComponent()->GetAnimation(), new_animation );

    FTransform transform = cloned_animation->GetTransform();
    cloned_animation->GetAnimationComponent()->InitializeFromAnimation( new_animation );
    cloned_animation->SetActorTransform( transform );

    if( iIsSpawnable )
        MovieSceneHelpers::CopyObjectTemplate( iSequence, iAnimationBinding, cloned_animation, iSequencer->GetSharedPlaybackState(), 0 );

    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( *iSequencer, epos_sequence, iSequenceID, iAnimationBinding );
    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
    {
        UOdysseyAnimation** animation_exist = original_to_new_animation_map.Find( section->GetAnimation() );
        if( animation_exist )
        {
            section->SetAnimation( *animation_exist );
        }
        else
        {
            UOdysseyAnimation* new_animation_in_section = ProjectAssetTools::CloneAnimation( *iSequencer, iSequence, iSequenceID, section->GetAnimation() );
            if( !new_animation_in_section )
                continue;

            original_to_new_animation_map.Add( cloned_animation->GetAnimationComponent()->GetAnimation(), new_animation_in_section );

            section->SetAnimation( new_animation_in_section );
        }

    }

    iSequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::MovieSceneStructureItemsChanged );
}

#undef LOCTEXT_NAMESPACE
