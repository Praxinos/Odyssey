// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposNamingConvention.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "EditorAssetLibrary.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "IMovieScenePlayer.h"
#include "MovieSceneSequence.h"
#include "PlaneActor.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "NamingConvention"

//---

//static
FString
NamingConvention::GetRootPath( const UMovieSceneSequence* iRootSequence )
{
    return iRootSequence->GetPackage()->GetName(); // ie. /Game/MyStoryboard2
}

//static
FString
NamingConvention::GetMasterPath( const UMovieSceneSequence* iRootSequence )
{
    return GetRootPath( iRootSequence ) / TEXT( "Master" ); // ie. /Game/MyStoryboard2/Master
}

//---

//static
FString
NamingConvention::GenerateCameraActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    oPath = FPaths::GetBaseFilename( iRootSequence->GetPathName() );
    oName = TEXT( "Camera_1" );

    return oPath / oName;
}

//static
FString
NamingConvention::GeneratePlaneActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString camera_path;
    FString camera_name;
    GenerateCameraActorPathName( iSequencer, iRootSequence, iSequence, camera_path, camera_name );

    oPath = camera_path;
    oName = TEXT( "Plane_1" );

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateCameraTrackName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, ACineCameraActor* iCamera )
{
    return iCamera->GetActorLabel();
}

//static
FString
NamingConvention::GeneratePlaneTrackName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, APlaneActor* iPlane )
{
    return iPlane->GetActorLabel();
}

//---

//static
FString
NamingConvention::GetMasterMaterialPathName( const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName )
{
    oPath = GetMasterPath( iRootSequence );
    oName = TEXT( "MI_Plane" );

    return oPath / oName;
}

//static
FString
NamingConvention::GetMasterTexturePathName( const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName )
{
    oPath = GetMasterPath( iRootSequence );
    oName = TEXT( "T_Transparent" );

    return oPath / oName;
}

//---

//static
FString
NamingConvention::GenerateNoteAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString current_sequence_name = FPackageName::GetShortName( iSequence->GetPackage() ); // ie. shot_0002_01

    FString note_pathname_base = root_path / TEXT( "Notes" ) / current_sequence_name;
    FString note_suffix = TEXT( "_N_01" );

    FString note_pathname;
    FString note_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( note_pathname_base, note_suffix, note_pathname, note_name );

    oName = note_name;
    oPath = FPackageName::GetLongPackagePath( note_pathname );

    return note_pathname;
}

//static
FString
NamingConvention::GenerateMaterialAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString current_sequence_name = FPackageName::GetShortName( iSequence->GetPackage() ); // ie. shot_0002_01

    FString material_pathname_base = root_path / current_sequence_name;
    FString material_suffix = TEXT( "_MI_01" );

    FString material_pathname;
    FString material_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( material_pathname_base, material_suffix, material_pathname, material_name );

    oName = material_name;
    oPath = FPackageName::GetLongPackagePath( material_pathname );

    return material_pathname;
}

//static
FString
NamingConvention::GenerateTextureAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPath, FString& oName )
{
    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString current_material_name = FPackageName::GetShortName( iMaterial->GetPackage() ); // ie. shot_0002_01_MI_01

    FString texture_pathname_base = root_path / current_material_name;
    FString texture_suffix = TEXT( "_T_01" );

    FString texture_pathname;
    FString texture_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( texture_pathname_base, texture_suffix, texture_pathname, texture_name );

    oName = texture_name;
    oPath = FPackageName::GetLongPackagePath( texture_pathname );

    return texture_pathname;
}

//---

//static
FString
NamingConvention::GenerateSequenceAssetPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, UClass* iType, FString& oPath, FString& oName )
{
    check( iType->IsChildOf<UBoardSequence>() || iType->IsChildOf<UShotSequence>() );

    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString current_sequence_base_name = iType->IsChildOf<UBoardSequence>() ? TEXT( "board" ) : TEXT( "shot" );

    const UMovieSceneToolsProjectSettings* projectSettings = GetDefault<UMovieSceneToolsProjectSettings>();
    uint32 shotNumber = projectSettings->FirstShotNumber;
    uint32 takeNumber = projectSettings->FirstTakeNumber;

    FString sequence_name = MovieSceneToolHelpers::ComposeShotName( current_sequence_base_name, shotNumber, takeNumber );
    FString sequence_pathname = root_path / sequence_name;

    while( UEditorAssetLibrary::DoesAssetExist( sequence_pathname ) )
    {
        shotNumber += projectSettings->ShotIncrement;

        sequence_name = MovieSceneToolHelpers::ComposeShotName( current_sequence_base_name, shotNumber, takeNumber );
        sequence_pathname = root_path / sequence_name;
    }

    //FString sequence_pathname_base = root_path / current_sequence_base_name;
    //FString sequence_suffix = TEXT( "_0010" );

    //FString sequence_pathname;
    //FString sequence_name;
    //FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    //assetToolsModule.Get().CreateUniqueAssetName( sequence_pathname_base, sequence_suffix, sequence_pathname, sequence_name );

    oName = sequence_name;
    oPath = FPackageName::GetLongPackagePath( sequence_pathname );

    return sequence_pathname;
}

#undef LOCTEXT_NAMESPACE
