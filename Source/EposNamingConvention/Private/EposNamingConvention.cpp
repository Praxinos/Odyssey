// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposNamingConvention.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"

#include "IMovieScenePlayer.h"
#include "MovieSceneSequence.h"
#include "PlaneActor.h"

#define LOCTEXT_NAMESPACE "NamingConvention"

//---

//static
FString
NamingConvention::GenerateCameraActorPathName( const IMovieScenePlayer& iSequencer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    oPath = FPaths::GetBaseFilename( iRootSequence->GetPathName() );
    oName = TEXT( "Camera_1" );

    return FPaths::Combine( oPath, oName );
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

    return FPaths::Combine( oPath, oName );
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
NamingConvention::GenerateNoteAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString root_package_pathname = iRootSequence->GetPackage()->GetName(); // ie. /Game/MyStoryboard2
    FString current_package_name = FPaths::GetBaseFilename( iSequence->GetPackage()->GetName() ); // ie. shot_0002_01

    FString note_pathname;
    FString note_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( FPaths::Combine( root_package_pathname, TEXT( "Notes" ), current_package_name ), "_N_01", note_pathname, note_name );

    oName = note_name;
    oPath = FPackageName::GetLongPackagePath( note_pathname );

    return note_pathname;
}

//static
FString
NamingConvention::GenerateMaterialAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    UPackage* package = iSequence->GetPackage();
    FString current_package_pathname = package->GetName(); // ie. /Game/MyStoryboard2/shot0001_01

    FString material_pathname;
    FString material_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( current_package_pathname, "_MI_01", material_pathname, material_name );

    oName = material_name;
    oPath = FPackageName::GetLongPackagePath( material_pathname );

    return material_pathname;
}

//static
FString
NamingConvention::GenerateTextureAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPath, FString& oName )
{
    UPackage* package = iMaterial->GetPackage();
    FString current_package_pathname = package->GetName(); // ie. /Game/MyStoryboard2/M_Plane_Basic_Inst

    FString texture_pathname;
    FString texture_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( current_package_pathname, "_T_01", texture_pathname, texture_name );

    oName = texture_name;
    oPath = FPackageName::GetLongPackagePath( texture_pathname );

    return texture_pathname;
}

#undef LOCTEXT_NAMESPACE
