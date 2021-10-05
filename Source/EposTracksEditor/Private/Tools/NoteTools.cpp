// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/NoteTools.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "MovieSceneSequence.h"

#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "NoteTools"

//---

//static
UStoryNote*
NoteTools::CreateNote( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );

    UPackage* package = iRootSequence->GetPackage();
    FString package_pathname = package->GetName(); // ie. /Game/MyStoryboard2
    FString package_name = FPaths::GetBaseFilename( iSequence->GetPackage()->GetName() ); // ie. shot_0002_01

    FString note_package_name;
    FString note_asset_name;
    assetToolsModule.Get().CreateUniqueAssetName( FPaths::Combine( package_pathname, TEXT( "Notes" ), package_name ), "_N_01", note_package_name, note_asset_name );

    FString package_path = FPackageName::GetLongPackagePath( note_package_name );
    UObject* new_object = assetToolsModule.Get().CreateAsset( note_asset_name, package_path, UStoryNote::StaticClass(), nullptr );
    UStoryNote* new_note = Cast<UStoryNote>( new_object );
    check( new_note );

    return new_note;
}

#undef LOCTEXT_NAMESPACE
