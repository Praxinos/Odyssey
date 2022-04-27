// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NamingConvention.h"

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
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "EposSequenceHelpers.h"
#include "IMovieScenePlayer.h"
#include "MovieSceneSequence.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "PlaneActor.h"
#include "Settings/NamingConventionSettings.h"
#include "Shot/ShotSequence.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "NamingConvention"

//---

typedef TMap<FString, int32> FRelevantPathMap;

static
FRelevantPathMap
FindSiblingSequencePaths( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iParentSequence )
{
    UMovieSceneCinematicBoardTrack* boardTrack = iParentSequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !boardTrack )
        return FRelevantPathMap();

    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();

    FRelevantPathMap map_path_to_count;

    for( auto section : sections )
    {
        UMovieSceneSubSection* subsection = CastChecked<UMovieSceneSubSection>( section );
        UMovieSceneSequence* subsequence = subsection->GetSequence();
        if( !subsequence )
            continue;

        FString sequence_pathname = subsequence->GetPackage()->GetName();
        FString sequence_path = FPackageName::GetLongPackagePath( sequence_pathname );

        int32* count = map_path_to_count.Find( sequence_path );
        if( count )
            *count = *count + 1;
        else
            map_path_to_count.Add( sequence_path, 1 );
    }

    map_path_to_count.ValueSort( TGreater<int32>() );

    return map_path_to_count;
}

static
FRelevantPathMap
FindAllSequencePaths( const IMovieScenePlayer& iPlayer )
{
    IMovieScenePlayer& player = *const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    TMap<FMovieSceneSequenceID, UMovieSceneSequence*> map_sequences;
    // Don't get the root sequence because it is the only one which must not be inside the root folder
    //map_sequences.Add( MovieSceneSequenceID::Root, player.GetEvaluationTemplate().GetSequence( MovieSceneSequenceID::Root ) );

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player.GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            map_sequences.Add( pair.Key, pair.Value.GetSequence() );
    }

    //---

    FRelevantPathMap map_path_to_count;

    for( auto pair : map_sequences )
    {
        UMovieSceneSequence* sequence = pair.Value;
        //FMovieSceneSequenceID sequence_id = pair.Key;

        FString sequence_pathname = sequence->GetPackage()->GetName();
        FString sequence_path = FPackageName::GetLongPackagePath( sequence_pathname );

        int32* count = map_path_to_count.Find( sequence_path );
        if( count )
            *count = *count + 1;
        else
            map_path_to_count.Add( sequence_path, 1 );
    }

    map_path_to_count.ValueSort( TGreater<int32>() );

    return map_path_to_count;
}

static
FRelevantPathMap
FindAllNotePaths( const IMovieScenePlayer& iPlayer )
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

    //---

    FRelevantPathMap map_path_to_count;

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

    //---

    if( map_path_to_count.Num() )
        return map_path_to_count;

    map_path_to_count = FindAllSequencePaths( player );
    FRelevantPathMap map_notepath_to_count;
    for( auto pair : map_path_to_count )
    {
        FString path = pair.Key;
        int32 count = pair.Value;

        // Now this line commented, it's maybe no more necessary to keep this last part of code (this loop with FindAllSequencePaths())
        // But this must be double check, because before there was a reason to keep this part here inside this function
        // AND call this part again inside GenerateNoteAssetPathName()
        //path /= TEXT( "Notes" );

        map_notepath_to_count.Add( path, count );
    }

    return map_notepath_to_count;
}

static
FRelevantPathMap
FindAllMaterialPaths( const IMovieScenePlayer& iPlayer, FRelevantPathMap& oParentPaths )
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

    //---

    FRelevantPathMap map_path_to_count;
    oParentPaths.Empty();

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

                const UMaterialInterface* material_interface = drawing.GetMaterial();
                const UMaterialInstance* material = Cast<UMaterialInstance>( material_interface );
                if( !material )
                    continue;

                //-

                FString material_pathname = material->GetPackage()->GetName();
                FString material_path = FPackageName::GetLongPackagePath( material_pathname );

                int32* count = map_path_to_count.Find( material_path );
                if( count )
                    *count = *count + 1;
                else
                    map_path_to_count.Add( material_path, 1 );

                //-

                UMaterialInterface* parent_material = material->Parent;
                check( parent_material );

                FString parent_material_pathname = parent_material->GetPackage()->GetName();
                FString parent_material_path = FPackageName::GetLongPackagePath( parent_material_pathname );


                count = oParentPaths.Find( parent_material_path );
                if( count )
                    *count = *count + 1;
                else
                    oParentPaths.Add( parent_material_path, 1 );
            }
        }
    }

    map_path_to_count.ValueSort( TGreater<int32>() );
    oParentPaths.ValueSort( TGreater<int32>() );

    return map_path_to_count;
}

static
FRelevantPathMap
FindAllMaterialPaths( const IMovieScenePlayer& iPlayer )
{
    FRelevantPathMap map_parent_material_paths;
    return FindAllMaterialPaths( iPlayer, map_parent_material_paths );
}

//---

//static
FString
NamingConvention::GetRootPath( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence )
{
    FString root_path;

    // Try to find the better path from all existing subsequences
    FRelevantPathMap map_sequence_paths = FindAllSequencePaths( iPlayer );
    if( map_sequence_paths.Num() )
    {
        TArray<FString> keys;
        map_sequence_paths.GetKeys( keys );

        root_path = keys[0];
    }

    if( root_path.IsEmpty() )
    {
        // Try to find the better path from all existing materials
        FRelevantPathMap map_material_paths = FindAllMaterialPaths( iPlayer );
        if( map_material_paths.Num() )
        {
            TArray<FString> keys;
            map_material_paths.GetKeys( keys );

            root_path = keys[0];
        }
    }

    if( root_path.IsEmpty() )
    {
        // Default root path
        root_path = iRootSequence->GetPackage()->GetName() + TEXT( "_Private" ); // ie. /Game/MyStoryboard2
    }

    return root_path;
}

//static
FString
NamingConvention::GetMasterPath( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence )
{
    FString master_path;

    // Try to find the better path from all existing materials
    FRelevantPathMap map_parent_material_paths;
    FRelevantPathMap map_material_paths = FindAllMaterialPaths( iPlayer, map_parent_material_paths );
    if( map_parent_material_paths.Num() )
    {
        TArray<FString> keys;
        map_parent_material_paths.GetKeys( keys );

        master_path = keys[0];
    }

    if( master_path.IsEmpty() )
    {
        if( map_material_paths.Num() )
        {
            TArray<FString> keys;
            map_material_paths.GetKeys( keys );

            master_path = keys[0];

            master_path /= TEXT( "Master" );
        }
    }

    if( master_path.IsEmpty() )
    {
        // Default master path
        FString root_path = GetRootPath( iPlayer, iRootSequence ) / TEXT( "Master" ); // ie. /Game/MyStoryboard2/Master
        master_path = root_path;
    }

    return master_path;
}

//---
//---
//---

//static
FString
NamingConvention::GenerateCameraActorPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache
    UMovieSceneSequence* current_sequence = const_cast<UMovieSceneSequence*>( iSequence ); //PATCH: Because there is no 'const' parameter version of ShotSequenceHelpers::GetCamera()

    FString root_sequence_name = FPackageName::GetShortName( iRootSequence->GetPackage()->GetName() );
    FString current_sequence_name = FPackageName::GetShortName( iSequence->GetDisplayName().ToString() );
    //FString current_sequence_name = FPackageName::GetShortName( iSequence->GetPackage()->GetName() );

    FString camera_path = ( iRootSequence != iSequence ) ? root_sequence_name / current_sequence_name : root_sequence_name;

    //--- Find all camera track names

    TArray<FString> camera_names;

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player->GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& map = hierarchy->AllSubSequenceData();
        for( auto pair : map )
        {
            UMovieSceneSequence* sequence = pair.Value.GetSequence();
            FMovieSceneSequenceID sequence_id = pair.Key;

            ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *player, sequence, sequence_id );
            if( !camera )
                continue;

            camera_names.AddUnique( camera->GetActorLabel() );
        }
    }

    //--- Find all camera indexes inside their names (through a regex)

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionCamera camera_settings = settings->CameraNaming;

    //TOCHECK: if it's no possible to use "named group", maybe try to order the replacement of {...} so we maybe have the group index ?
    const FString camera_pattern_regex = camera_settings.Pattern.Replace( TEXT( "{camera-index}" ), TEXT( "([0-9]+)" ) );
    const FString camera_pattern_display = camera_settings.Pattern.Replace( TEXT( "{camera-index}" ), TEXT( "{camera_index_formated}" ) );
    FRegexPattern camera_pattern( camera_pattern_regex );

    TArray<int32> list_of_camera_index;

    for( auto camera_name : camera_names )
    {
        FRegexMatcher matcher( camera_pattern, camera_name );

        if( matcher.FindNext() )
        {
            int32 full_begin = matcher.GetMatchBeginning();
            int32 full_end = matcher.GetMatchEnding();
            FTextRange full_range( full_begin, full_end );
            FString full_string = camera_name.Mid( full_range.BeginIndex, full_range.Len() );

            int32 index_begin = matcher.GetCaptureGroupBeginning( 1 );
            int32 index_end = matcher.GetCaptureGroupEnding( 1 );
            FTextRange index_range( index_begin, index_end );
            FString index_string = camera_name.Mid( index_range.BeginIndex, index_range.Len() );

            int32 index = FCString::Atoi( *index_string );

            list_of_camera_index.Add( index );
        }
    }

    list_of_camera_index.Sort( TGreater<int32>() );

    //--- Try to find the new plane name depending of the max existing index

    int32 max_plane_index = list_of_camera_index.Num() ? list_of_camera_index[0] : camera_settings.IndexFormat.StartNumber;

    FStringFormatNamedArguments args;
    args.Add( TEXT( "camera_index_formated" ), FString::Printf( TEXT( "%0*d" ), camera_settings.IndexFormat.NumDigits, max_plane_index ) );
    FString camera_name = FString::Format( *camera_pattern_display, args );

    while( camera_names.Contains( camera_name ) )
    {
        max_plane_index += camera_settings.IndexFormat.Increment;

        args.FindChecked( TEXT( "camera_index_formated" ) ) = FString::Printf( TEXT( "%0*d" ), camera_settings.IndexFormat.NumDigits, max_plane_index );
        camera_name = FString::Format( *camera_pattern_display, args );
    }

    //---

    oPath = camera_path;
    oName = camera_name;

    return oPath / oName;
}

//static
FString
NamingConvention::GeneratePlaneActorPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache
    UMovieSceneSequence* current_sequence = const_cast<UMovieSceneSequence*>( iSequence ); //PATCH: Because there is no 'const' parameter version of ShotSequenceHelpers::GetCamera()

    //--- Find the current sequence id from the sequence

    FMovieSceneSequenceID current_sequence_id; // invalid

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player->GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& map = hierarchy->AllSubSequenceData();
        for( auto pair : map )
        {
            if( pair.Value.GetSequence() == iSequence )
            {
                current_sequence_id = pair.Key;
                break;
            }
        }
    }
    else
    {
        current_sequence_id = MovieSceneSequenceID::Root;
        check( iRootSequence == iSequence );
    }

    //--- Find the camera actor

    FString plane_path;
    FString camera_name;

    ACineCameraActor* camera = ShotSequenceHelpers::GetCamera( *player, current_sequence, current_sequence_id );
    if( camera )
    {
        plane_path = camera->GetFolderPath().ToString();
        camera_name = camera->GetActorLabel();
    }

    if( plane_path.IsEmpty() )
    {
        FString camera_path;
        GenerateCameraActorPathName( iPlayer, iRootSequence, iSequence, camera_path, camera_name );

        plane_path = camera_path;
    }

    //--- Find all plane track names

    const TArray<FMovieSceneBinding>& bindings = current_sequence->GetMovieScene()->GetBindings();
    TArray<FString> binding_names;
    for( auto binding : bindings )
        binding_names.AddUnique( current_sequence->GetMovieScene()->GetObjectDisplayName( binding.GetObjectGuid() ).ToString() );
        // Don't use binding.GetName() because (for example) it is not updated when the name is changed directly in the shot track label (instead of the board section view)
        // binding.GetName() is still keeping the old "track" name

    //--- Find all plane indexes inside their names (through a regex)

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionPlane plane_settings = settings->PlaneNaming;

    //TOCHECK: if it's no possible to use "named group", maybe try to order the replacement of {...} so we maybe have the group index ?
    const FString plane_pattern_regex = plane_settings.Pattern.Replace( TEXT( "{plane-index}" ), TEXT( "([0-9]+)" ) );
    const FString plane_pattern_display = plane_settings.Pattern.Replace( TEXT( "{plane-index}" ), TEXT( "{plane_index_formated}" ) );
    FRegexPattern plane_pattern( plane_pattern_regex );

    // https://stackoverflow.com/questions/3075130/what-is-the-difference-between-and-regular-expressions
    // https://www.regular-expressions.info/refadv.html
    // https://www.regular-expressions.info/atomic.html

    TArray<int32> list_of_plane_index;

    for( auto binding_name : binding_names )
    {
        FRegexMatcher matcher( plane_pattern, binding_name );

        if( matcher.FindNext() )
        {
            int32 full_begin = matcher.GetMatchBeginning();
            int32 full_end = matcher.GetMatchEnding();
            FTextRange full_range( full_begin, full_end );
            FString full_string = binding_name.Mid( full_range.BeginIndex, full_range.Len() );

            int32 index_begin = matcher.GetCaptureGroupBeginning( 1 );
            int32 index_end = matcher.GetCaptureGroupEnding( 1 );
            FTextRange index_range( index_begin, index_end );
            FString index_string = binding_name.Mid( index_range.BeginIndex, index_range.Len() );

            int32 index = FCString::Atoi( *index_string );

            list_of_plane_index.Add( index );
        }
    }

    list_of_plane_index.Sort( TGreater<int32>() );

    //--- Try to find the new plane name depending of the max existing index

    int32 max_plane_index = list_of_plane_index.Num() ? list_of_plane_index[0] : plane_settings.IndexFormat.StartNumber;

    FStringFormatNamedArguments args;
    args.Add( TEXT( "plane_index_formated" ), FString::Printf( TEXT( "%0*d" ), plane_settings.IndexFormat.NumDigits, max_plane_index ) );
    FString plane_name = FString::Format( *plane_pattern_display, args );

    while( binding_names.Contains( plane_name ) )
    {
        max_plane_index += plane_settings.IndexFormat.Increment;

        args.FindChecked( TEXT( "plane_index_formated" ) ) = FString::Printf( TEXT( "%0*d" ), plane_settings.IndexFormat.NumDigits, max_plane_index );
        plane_name = FString::Format( *plane_pattern_display, args );
    }

    //---

    oPath = plane_path;
    oName = plane_name;

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateCameraTrackName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, ACineCameraActor* iCamera )
{
    // See comment in GeneratePlaneTrackName()
    return iCamera->GetActorLabel();
}

//static
FString
NamingConvention::GeneratePlaneTrackName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, APlaneActor* iPlane )
{
    // For the moment, it's ok, but it will change, double-check (for example) the clone plane function...
    // Or maybe name this function GeneratePlaneTrackNameFROMACTOR() et add another one which will really compute a new track name from the existing ones ?
    return iPlane->GetActorLabel();
}

//---
//---
//---

//static
FString
NamingConvention::GetMasterMaterialPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName )
{
    oPath = GetMasterPath( iPlayer, iRootSequence );
    oName = TEXT( "MI_Plane" );

    return oPath / oName;
}

//static
FString
NamingConvention::GetMasterTexturePathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, FString& oPath, FString& oName )
{
    oPath = GetMasterPath( iPlayer, iRootSequence );
    oName = TEXT( "T_Transparent" );

    return oPath / oName;
}

//---

//static
FString
NamingConvention::GenerateNoteAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString note_path;

    // Try to find the better path from all existing notes
    FRelevantPathMap map_note_paths = FindAllNotePaths( iPlayer );
    if( map_note_paths.Num() )
    {
        TArray<FString> keys;
        map_note_paths.GetKeys( keys );

        note_path = keys[0];
    }

    if( note_path.IsEmpty() )
    {
        // Try to find the better path from all existing subsequences
        FRelevantPathMap map_sequence_paths = FindAllSequencePaths( iPlayer );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            note_path = keys[0];

            //note_path /= TEXT( "Notes" );
        }
    }

    if( note_path.IsEmpty() )
    {
        // Default path name of the new note
        FString root_path = GetRootPath( iPlayer, iRootSequence ); // ie. /Game/MyStoryboard2
        note_path = root_path;// / TEXT( "Notes" );
    }

    //---

    FString note_name = TEXT( "N_" ) + FGuid::NewGuid().ToString();

    //---

    oName = note_name;
    oPath = note_path;

    return note_path / note_name;
}

//static
FString
NamingConvention::GenerateMaterialAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, FString& oPath, FString& oName )
{
    FString material_path;

    // Try to find the better path from all existing materials
    FRelevantPathMap map_material_paths = FindAllMaterialPaths( iPlayer );
    if( map_material_paths.Num() )
    {
        TArray<FString> keys;
        map_material_paths.GetKeys( keys );

        material_path = keys[0];
    }

    if( material_path.IsEmpty() )
    {
        // Try to find the better path from all existing subsequences
        FRelevantPathMap map_sequence_paths = FindAllSequencePaths( iPlayer );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            material_path = keys[0];
        }
    }

    if( material_path.IsEmpty() )
    {
        // Default path of the new material
        FString root_path = GetRootPath( iPlayer, iRootSequence ); // ie. /Game/MyStoryboard2
        material_path = root_path;

    }

    //---

    FString material_name = TEXT( "M_" ) + FGuid::NewGuid().ToString();

    //---

    oName = material_name;
    oPath = material_path;

    return material_path / material_name;
}

//static
FString
NamingConvention::GenerateTextureAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FString& oPath, FString& oName )
{
    //FString root_path = GetRootPath( iPlayer, iRootSequence ); // ie. /Game/MyStoryboard2
    //FString texture_path = root_path;
    FString material_path = FPackageName::GetLongPackagePath( iMaterial->GetPackage()->GetName() );
    FString texture_path = material_path;

    //---

    FString texture_name = TEXT( "T_" ) + FGuid::NewGuid().ToString();

    //---

    oName = texture_name;
    oPath = texture_path;

    return texture_path / texture_name;
}

//---

namespace
{
static
void
FillNameElements( const FNamingConventionGlobal& iGlobalSettings, FSequenceNameElements& oDstElements )
{
    // Copy all 'global' members from settings global to shot elements
    for( TFieldIterator<FProperty> source_property_iterator( FNamingConventionGlobal::StaticStruct() ); source_property_iterator; ++source_property_iterator )
    {
        FProperty* source_property = *source_property_iterator;

        FProperty* destination_property = FShotNameElements::StaticStruct()->FindPropertyByName( source_property->GetFName() );
        if( source_property->GetName().EndsWith( TEXT( "NumDigits" ) ) )
            continue;

        check( destination_property );

        // It doesn't work if the 2 structs are not synchro with the same name of members
        // and I don't know the difference with the (good) outside ContainerPtrToValuePtr<> form below
        //settings_global_property->CopyCompleteValue_InContainer( &board_sequence->NameElements, &mNamingConventionSettings->GlobalNaming );

        const uint8* SourceAddr = source_property->ContainerPtrToValuePtr<uint8>( &iGlobalSettings );
        uint8* DestinationAddr = destination_property->ContainerPtrToValuePtr<uint8>( &oDstElements );

        source_property->CopyCompleteValue( DestinationAddr, SourceAddr );
    }
}
static
void
FillNameElements( const FNamingConventionUser& iUserSettings, FSequenceNameElements& oDstElements )
{
    // Copy all 'user' members from settings user to shot elements
    for( TFieldIterator<FProperty> source_property_iterator( FNamingConventionUser::StaticStruct() ); source_property_iterator; ++source_property_iterator )
    {
        FProperty* source_property = *source_property_iterator;

        FProperty* destination_property = FShotNameElements::StaticStruct()->FindPropertyByName( source_property->GetFName() );
        check( destination_property );

        const uint8* SourceAddr = source_property->ContainerPtrToValuePtr<uint8>( &iUserSettings );
        uint8* DestinationAddr = destination_property->ContainerPtrToValuePtr<uint8>( &oDstElements );

        source_property->CopyCompleteValue( DestinationAddr, SourceAddr );
    }
}

static
TOptional<FSequenceNameElements>
FindSiblingNameElements( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iParentSequence, FFrameNumber iFrameNumber )
{
    UMovieSceneCinematicBoardTrack* boardTrack = iParentSequence->GetMovieScene()->FindMasterTrack<UMovieSceneCinematicBoardTrack>();
    if( !boardTrack )
        return TOptional<FSequenceNameElements>();

    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();
    UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( sections, iFrameNumber );
    if( !section )
        section = MovieSceneHelpers::FindNearestSectionAtTime( sections, iFrameNumber );
    if( !section )
        return TOptional<FSequenceNameElements>();

    UMovieSceneSubSection* subsection = CastChecked<UMovieSceneSubSection>( section );
    if( !subsection->GetSequence() )
        return TOptional<FSequenceNameElements>();

    UBoardSequence* board_sequence = Cast<UBoardSequence>( subsection->GetSequence() );
    if( board_sequence )
        return board_sequence->NameElements;

    UShotSequence* shot_sequence = Cast<UShotSequence>( subsection->GetSequence() );
    if( shot_sequence )
        return shot_sequence->NameElements;

    checkNoEntry();

    return TOptional<FSequenceNameElements>();
}
static
TOptional<FSequenceNameElements>
FindOneNameElements( const IMovieScenePlayer& iPlayer, FFrameNumber iFrameNumber )
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

    //---

    for( auto pair : map_sequences )
    {
        UMovieSceneSequence* sequence = pair.Value;

        UBoardSequence* board_sequence = Cast<UBoardSequence>( sequence );
        if( board_sequence && !board_sequence->NameElements.StudioName.IsEmpty() )
            return board_sequence->NameElements;

        UShotSequence* shot_sequence = Cast<UShotSequence>( sequence );
        if( shot_sequence && !shot_sequence->NameElements.StudioName.IsEmpty() )
            return shot_sequence->NameElements;
    }

    return TOptional<FSequenceNameElements>();
}

static
TOptional<FSequenceNameElements>
FindNameElements( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iParentSequence, FFrameNumber iFrameNumber )
{
    TOptional<FSequenceNameElements> source_elements;

    // Try to get the most relevant name elements
    if( !source_elements )
    {
        TOptional<FSequenceNameElements> reference_elements = FindSiblingNameElements( iPlayer, iParentSequence, iFrameNumber );

        if( reference_elements && !reference_elements.GetValue().StudioName.IsEmpty() )
            source_elements = reference_elements;
    }

    // Try to get the parent name elements
    if( !source_elements )
    {
        const UBoardSequence* parent_board_sequence = CastChecked<UBoardSequence>( iParentSequence ); // To parent is always a board ... to check ...
        FSequenceNameElements reference_elements = parent_board_sequence->NameElements;

        if( !reference_elements.StudioName.IsEmpty() )
            source_elements = reference_elements;
    }

    // Try to get the root name elements
    if( !source_elements )
    {
        const UBoardSequence* root_board_sequence = CastChecked<UBoardSequence>( iRootSequence ); // If we go here, the root is "necessarily" a board ... to check ...
        FSequenceNameElements reference_elements = root_board_sequence->NameElements;

        if( !reference_elements.StudioName.IsEmpty() )
            source_elements = reference_elements;
    }

    // Try to get one valid name elements in the whole hierarchy
    if( !source_elements )
    {
        TOptional<FSequenceNameElements> reference_elements = FindOneNameElements( iPlayer, iFrameNumber );

        if( reference_elements && !reference_elements.GetValue().StudioName.IsEmpty() )
            source_elements = reference_elements;
    }

    return source_elements;
}
};

//static
FString
NamingConvention::GenerateBoardAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iParentSequence, FFrameNumber iFrameNumber, FString& oPath, FString& oName, FBoardNameElements& oElements )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    //---

    FString sequence_path;

    // Try to find the better path from existing sibling sequences
    FRelevantPathMap map_sequence_paths = FindSiblingSequencePaths( iPlayer, iParentSequence );
    if( map_sequence_paths.Num() )
    {
        TArray<FString> keys;
        map_sequence_paths.GetKeys( keys );

        sequence_path = keys[0];
    }

    // Try to find the better path from all existing sequences
    if( sequence_path.IsEmpty() )
    {
        map_sequence_paths = FindAllSequencePaths( iPlayer );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            sequence_path = keys[0];
        }
    }

    if( sequence_path.IsEmpty() )
    {
        FString root_path = GetRootPath( iPlayer, iRootSequence ); // ie. /Game/MyStoryboard2
        sequence_path = root_path;
    }

    //--- Find all boards

    TArray<UBoardSequence*> board_sequences;

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player->GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& map = hierarchy->AllSubSequenceData();
        for( auto pair : map )
        {
            UMovieSceneSequence* sequence = pair.Value.GetSequence();
            FMovieSceneSequenceID sequence_id = pair.Key;

            UBoardSequence* board_sequence = Cast<UBoardSequence>( sequence );
            if( !board_sequence )
                continue;

            board_sequences.AddUnique( board_sequence );
        }
    }

    //--- Compute the next valid board index

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    const FNamingConventionGlobal& global_settings = settings->GlobalNaming;
    const FNamingConventionUser& user_settings = settings->UserNaming;
    const FNamingConventionBoard& board_settings = settings->BoardNaming;

    int32 max_board_index = INDEX_NONE;

    for( auto board_sequence : board_sequences )
    {
        const FBoardNameElements& elements = board_sequence->NameElements;
        if( !elements.IsValid() )
            continue;

        if( elements.Index > max_board_index )
            max_board_index = elements.Index;
    }

    if( max_board_index != INDEX_NONE )
        max_board_index += board_settings.IndexFormat.Increment;
    else
        max_board_index = board_settings.IndexFormat.StartNumber;

    //---

    oElements.Index = max_board_index;

    TOptional<FSequenceNameElements> source_elements = FindNameElements( iPlayer, iRootSequence, iParentSequence, iFrameNumber );

    // Get the source elements values
    if( source_elements )
    {
        FSequenceNameElements* destination_elements = &oElements;
        *destination_elements = source_elements.GetValue();
    }
    // Otherwise, get the settings values
    else
    {
        FillNameElements( global_settings, oElements );
    }

    // User settings always override new sequence elements
    FillNameElements( user_settings, oElements );

    //---

    oName = TEXT( "BS_" ) + FGuid::NewGuid().ToString();
    oPath = sequence_path;

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateShotAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iParentSequence, FFrameNumber iFrameNumber, FString& oPath, FString& oName, FShotNameElements& oElements )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    //---

    FString sequence_path;

    // Try to find the better path from existing sibling sequences
    FRelevantPathMap map_sequence_paths = FindSiblingSequencePaths( iPlayer, iParentSequence );
    if( map_sequence_paths.Num() )
    {
        TArray<FString> keys;
        map_sequence_paths.GetKeys( keys );

        sequence_path = keys[0];
    }

    // Try to find the better path from all existing sequences
    if( sequence_path.IsEmpty() )
    {
        map_sequence_paths = FindAllSequencePaths( iPlayer );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            sequence_path = keys[0];
        }
    }

    if( sequence_path.IsEmpty() )
    {
        FString root_path = GetRootPath( iPlayer, iRootSequence ); // ie. /Game/MyStoryboard2
        sequence_path = root_path;
    }

    //--- Find all shots

    TArray<UShotSequence*> shot_sequences;

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( player->GetEvaluationTemplate().GetCompiledDataID() );
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& map = hierarchy->AllSubSequenceData();
        for( auto pair : map )
        {
            UMovieSceneSequence* sequence = pair.Value.GetSequence();
            FMovieSceneSequenceID sequence_id = pair.Key;

            UShotSequence* shot_sequence = Cast<UShotSequence>( sequence );
            if( !shot_sequence )
                continue;

            shot_sequences.AddUnique( shot_sequence );
        }
    }

    //--- Compute the next valid shot index

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    const FNamingConventionGlobal& global_settings = settings->GlobalNaming;
    const FNamingConventionUser& user_settings = settings->UserNaming;
    const FNamingConventionShot& shot_settings = settings->ShotNaming;

    int32 max_shot_index = INDEX_NONE;

    for( auto shot_sequence : shot_sequences )
    {
        const FShotNameElements& elements = shot_sequence->NameElements;
        if( !elements.IsValid() )
            continue;

        if( elements.Index > max_shot_index )
            max_shot_index = elements.Index;
    }

    if( max_shot_index != INDEX_NONE )
        max_shot_index += shot_settings.IndexFormat.Increment;
    else
        max_shot_index = shot_settings.IndexFormat.StartNumber;

    //---

    oElements.Index = max_shot_index;
    oElements.TakeIndex = shot_settings.TakeFormat.StartNumber;

    TOptional<FSequenceNameElements> source_elements = FindNameElements( iPlayer, iRootSequence, iParentSequence, iFrameNumber );

    // Get the source elements values
    if( source_elements )
    {
        FSequenceNameElements* destination_elements = &oElements;
        *destination_elements = source_elements.GetValue();
    }
    // Otherwise, get the settings values
    else
    {
        FillNameElements( global_settings, oElements );
    }

    // User settings always override new sequence elements
    FillNameElements( user_settings, oElements );

    //---

    oName = TEXT( "SS_" ) + FGuid::NewGuid().ToString();
    oPath = sequence_path;

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateTakeAssetPathName( const IMovieScenePlayer& iPlayer, const UMovieSceneSequence* iRootSequence, const UMovieSceneSequence* iParentSequence, UMovieSceneSubSection* iSubSection, FString& oPath, FString& oName, FShotNameElements& oElements )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    //---

    UShotSequence* subsequence = Cast<UShotSequence>( iSubSection->GetSequence() );
    //check( subsequence );

    FString sequence_pathname = subsequence ? subsequence->GetPackage()->GetName() : FString();
    FString sequence_path = FPackageName::GetLongPackagePath( sequence_pathname );

    // Try to find the better path from existing sibling sequences
    if( sequence_path.IsEmpty() )
    {
        FRelevantPathMap map_sequence_paths = FindSiblingSequencePaths( iPlayer, iParentSequence );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            sequence_path = keys[0];
        }
    }

    // Try to find the better path from all existing sequences
    if( sequence_path.IsEmpty() )
    {
        FRelevantPathMap map_sequence_paths = FindAllSequencePaths( iPlayer );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            sequence_path = keys[0];
        }
    }

    if( sequence_path.IsEmpty() )
    {
        FString root_path = GetRootPath( iPlayer, iRootSequence ); // ie. /Game/MyStoryboard2
        sequence_path = root_path;
    }

    //--- Find all takes

    TArray<UShotSequence*> take_sequences;
    if( subsequence )
    {
        UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( iSubSection );
        for( auto take : board_section->GetTakes() )
        {
            UShotSequence* shot_sequence = Cast<UShotSequence>( take.GetSequence() );
            if( shot_sequence )
                take_sequences.Add( shot_sequence );
        }
    }

    //--- Compute the next valid shot index

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    const FNamingConventionGlobal& global_settings = settings->GlobalNaming;
    const FNamingConventionUser& user_settings = settings->UserNaming;
    const FNamingConventionShot& shot_settings = settings->ShotNaming;

    int32 max_take_index = INDEX_NONE;

    for( auto take_sequence : take_sequences )
    {
        const FShotNameElements& elements = take_sequence->NameElements;
        if( !elements.IsValid() )
            continue;

        if( elements.Index > max_take_index )
            max_take_index = elements.TakeIndex;
    }

    if( max_take_index != INDEX_NONE )
        max_take_index += shot_settings.TakeFormat.Increment;
    else
        max_take_index = shot_settings.TakeFormat.StartNumber;

    //---

    oElements.Index = subsequence ? subsequence->NameElements.Index : shot_settings.IndexFormat.StartNumber;
    oElements.TakeIndex = max_take_index;

    TOptional<FSequenceNameElements> source_elements = subsequence ? subsequence->NameElements : FindNameElements( iPlayer, iRootSequence, iParentSequence, iSubSection->GetRange().GetLowerBoundValue() );

    // Get the source elements values
    if( source_elements )
    {
        FSequenceNameElements* destination_elements = &oElements;
        *destination_elements = source_elements.GetValue();
    }
    // Otherwise, get the settings values
    else
    {
        FillNameElements( global_settings, oElements );
    }

    // User settings always override new sequence elements
    FillNameElements( user_settings, oElements );

    //---

    oName = TEXT( "SS_" ) + FGuid::NewGuid().ToString();
    oPath = sequence_path;

    return oPath / oName;
}

#undef LOCTEXT_NAMESPACE
