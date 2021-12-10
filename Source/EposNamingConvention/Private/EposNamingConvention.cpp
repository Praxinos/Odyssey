// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposNamingConvention.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorAssetLibrary.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"

#include "Board/BoardSequence.h"
#include "EposSequenceHelpers.h"
#include "IMovieScenePlayer.h"
#include "MovieSceneSequence.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "PlaneActor.h"
#include "Shot/ShotSequence.h"
#include "StoryNote.h"

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
NamingConvention::GenerateCameraActorPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    oPath = FPaths::GetBaseFilename( iRootSequence->GetPathName() );
    oName = TEXT( "Camera_1" );

    return oPath / oName;
}

//static
FString
NamingConvention::GeneratePlaneActorPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString camera_path;
    FString camera_name;
    GenerateCameraActorPathName( iPlayer, iRootSequence, iSequence, camera_path, camera_name );

    oPath = camera_path;
    oName = TEXT( "Plane_1" );

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateCameraTrackName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, ACineCameraActor* iCamera )
{
    return iCamera->GetActorLabel();
}

//static
FString
NamingConvention::GeneratePlaneTrackName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, APlaneActor* iPlane )
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

static
TMap<FString, int32>
FindNotePaths( const IMovieScenePlayer& iPlayer )
{
    IMovieScenePlayer& player = *const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate()

    TArray<UMovieSceneSequence*> sequences;
    sequences.Add( player.GetEvaluationTemplate().GetSequence( MovieSceneSequenceID::Root ) );

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player.GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            sequences.Add( pair.Value.GetSequence() );
    }

    //-

    TMap<FString, int32> map_path_to_count;

    for( auto sequence : sequences )
    {
        auto tracks = sequence->GetMovieScene()->GetMasterTracks();
        for( auto track : tracks )
        {
            if( !track->IsA<UMovieSceneNoteTrack>() )
                continue;

            auto sections = track->GetAllSections();
            for( auto section : sections )
            {
                const UMovieSceneNoteSection* note_section = Cast<UMovieSceneNoteSection>( section );
                if( !note_section )
                    continue;

                const UStoryNote* note = note_section->GetNote();
                if( !note )
                    continue;

                FString note_pathname = note->GetPackage()->GetName();
                FString note_path = FPackageName::GetLongPackagePath( note_pathname );

                int32* count = map_path_to_count.Find( note_path );
                if( count )
                    *count = *count + 1;
                else
                    map_path_to_count.Add( note_path, 1 );
            }
        }
    }

    map_path_to_count.ValueSort( TGreater<int32>() );

    return map_path_to_count;
}

//static
FString
NamingConvention::GenerateNoteAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    // Default path name of the new note
    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString note_path = root_path / TEXT( "Notes" );

    // Try to find a better path from all existing notes
    TMap<FString, int32> map_note_paths = FindNotePaths( iPlayer );
    if( map_note_paths.Num() )
    {
        TArray<FString> keys;
        map_note_paths.GetKeys( keys );

        note_path = keys[0];
    }

    //-

    FString current_sequence_name = FPackageName::GetShortName( iSequence->GetPackage() ); // ie. shot_0002_01

    FString note_name_base = current_sequence_name;
    FString note_suffix = TEXT( "_N_01" );

    //-

    FString note_pathname;
    FString note_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( note_path / note_name_base, note_suffix, note_pathname, note_name );

    oName = note_name;
    oPath = FPackageName::GetLongPackagePath( note_pathname );

    return note_pathname;
}

static
TMap<FString, int32>
FindMaterialPaths( const IMovieScenePlayer& iPlayer )
{
    IMovieScenePlayer& player = *const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    TMap<FMovieSceneSequenceID, UMovieSceneSequence*> map_sequences;
    map_sequences.Add( MovieSceneSequenceID::Root, player.GetEvaluationTemplate().GetSequence( MovieSceneSequenceID::Root ) );

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player.GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            map_sequences.Add( pair.Key, pair.Value.GetSequence() );
    }

    //-

    TMap<FString, int32> map_path_to_count;

    for( auto pair : map_sequences )
    {
        UMovieSceneSequence* sequence = pair.Value;
        FMovieSceneSequenceID sequence_id = pair.Key;

        TArray<FGuid> plane_bindings;
        ShotSequenceHelpers::GetAllPlanes( player, sequence, sequence_id, EGetPlane::kAll, nullptr, &plane_bindings );

        for( auto plane_binding : plane_bindings )
        {
            TArray<FDrawing> drawings = ShotSequenceHelpers::GetAllDrawings( player, sequence, sequence_id, plane_binding );

            for( auto drawing : drawings )
            {
                if( !drawing.Exists() )
                    continue;

                const UMaterialInterface* material = drawing.GetMaterial();
                if( !material )
                    continue;

                FString material_pathname = material->GetPackage()->GetName();
                FString material_path = FPackageName::GetLongPackagePath( material_pathname );

                int32* count = map_path_to_count.Find( material_path );
                if( count )
                    *count = *count + 1;
                else
                    map_path_to_count.Add( material_path, 1 );
            }
        }
    }

    map_path_to_count.ValueSort( TGreater<int32>() );

    return map_path_to_count;
}

//static
FString
NamingConvention::GenerateMaterialAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    // Default path of the new material
    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString material_path = root_path;

    // Try to find a better path from all existing materials
    TMap<FString, int32> map_material_paths = FindMaterialPaths( iPlayer );
    if( map_material_paths.Num() )
    {
        TArray<FString> keys;
        map_material_paths.GetKeys( keys );

        material_path = keys[0];
    }

    //-

    FString current_sequence_name = FPackageName::GetShortName( iSequence->GetPackage() ); // ie. shot_0002_01

    FString material_name_base = current_sequence_name;
    FString material_suffix = TEXT( "_MI_01" );

    //-

    FString material_pathname;
    FString material_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( material_path / material_name_base, material_suffix, material_pathname, material_name );

    oName = material_name;
    oPath = FPackageName::GetLongPackagePath( material_pathname );

    return material_pathname;
}

//static
FString
NamingConvention::GenerateTextureAssetPathName( const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPath, FString& oName )
{
    FString root_path = GetRootPath( iRootSequence ); // ie. /Game/MyStoryboard2
    FString texture_path = root_path;

    //-

    FString current_material_name = FPackageName::GetShortName( iMaterial->GetPackage() ); // ie. shot_0002_01_MI_01

    FString texture_name_base = current_material_name;
    FString texture_suffix = TEXT( "_T_01" );

    //-

    FString texture_pathname;
    FString texture_name;
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    assetToolsModule.Get().CreateUniqueAssetName( texture_path / texture_name_base, texture_suffix, texture_pathname, texture_name );

    oName = texture_name;
    oPath = FPackageName::GetLongPackagePath( texture_pathname );

    return texture_pathname;
}

//---

//static
FString
NamingConvention::GenerateSequenceAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, UClass* iType, FString& oPath, FString& oName )
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
