// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "NamingConvention.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "EditorAssetLibrary.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "IMovieScenePlayer.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "MovieSceneSequence.h"
#include "MovieSceneToolHelpers.h"
#include "MovieSceneToolsProjectSettings.h"
#include "Templates/Greater.h"


#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "EposMovieSceneSequence.h"
#include "EposSequenceHelpers.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "NoteTrack/MovieSceneNoteTrack.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
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
    UMovieSceneCinematicBoardTrack* boardTrack = iParentSequence->GetMovieScene()->FindTrack<UMovieSceneCinematicBoardTrack>();
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

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetSharedPlaybackState()->GetHierarchy();
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            map_sequences.Add( pair.Key, pair.Value.GetSequence() );
    }

    TMap<FMovieSceneSequenceID, UMovieSceneSequence*> map_epos_sequences;
    for( auto pair : map_sequences )
    {
        if( pair.Value->IsA<UEposMovieSceneSequence>() )
            map_epos_sequences.Add( pair );
    }

    //---

    FRelevantPathMap map_path_to_count;

    for( auto pair : map_epos_sequences )
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

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetSharedPlaybackState()->GetHierarchy();
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            sequences.Add( pair.Value.GetSequence() );
    }

    TArray<UMovieSceneSequence*> epos_sequences;
    for( auto sequence : sequences )
    {
        if( sequence->IsA<UEposMovieSceneSequence>() )
            epos_sequences.Add( sequence );
    }

    //---

    FRelevantPathMap map_path_to_count;

    for( auto sequence : epos_sequences )
    {
        auto tracks = sequence->GetMovieScene()->GetTracks();
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
FindAllAnimationPaths( const IMovieScenePlayer& iPlayer, FRelevantPathMap& oParentPaths )
{
    IMovieScenePlayer& player = *const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    TMap<FMovieSceneSequenceID, UMovieSceneSequence*> map_sequences;
    map_sequences.Add( MovieSceneSequenceID::Root, player.GetEvaluationTemplate().GetSequence( MovieSceneSequenceID::Root ) );

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetSharedPlaybackState()->GetHierarchy();
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            map_sequences.Add( pair.Key, pair.Value.GetSequence() );
    }

    TMap<FMovieSceneSequenceID, UMovieSceneSequence*> map_epos_sequences;
    for( auto pair : map_sequences )
    {
        if( pair.Value->IsA<UEposMovieSceneSequence>() )
            map_epos_sequences.Add( pair );
    }

    //---

    FRelevantPathMap map_path_to_count;
    oParentPaths.Empty();

    for( auto pair : map_epos_sequences )
    {
        UMovieSceneSequence* sequence = pair.Value;
        FMovieSceneSequenceID sequence_id = pair.Key;

        if( !sequence->IsA<UShotSequence>() )
            continue;

        TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( player, sequence, sequence_id );
        for( FGuid animation_binding : animation_bindings )
        {
            TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawnedOrTemplate( player, sequence, sequence_id, animation_binding );
            for( AOdysseyAnimationActor* animation_actor : animation_actors )
            {
                const UOdysseyAnimation* animation = animation_actor->GetAnimationComponent() ? animation_actor->GetAnimationComponent()->GetAnimation() : nullptr;
                if( !animation )
                    continue;

                //-

                FString animation_pathname = animation->GetPackage()->GetName();
                FString animation_path = FPackageName::GetLongPackagePath( animation_pathname );

                int32* count = map_path_to_count.Find( animation_path );
                if( count )
                    *count = *count + 1;
                else
                    map_path_to_count.Add( animation_path, 1 );
            }
        }
    }

    map_path_to_count.ValueSort( TGreater<int32>() );
    oParentPaths.ValueSort( TGreater<int32>() );

    return map_path_to_count;
}

static
FRelevantPathMap
FindAllAnimationPaths( const IMovieScenePlayer& iPlayer )
{
    FRelevantPathMap map_parent_animation_paths;
    return FindAllAnimationPaths( iPlayer, map_parent_animation_paths );
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
        FRelevantPathMap map_material_paths = FindAllAnimationPaths( iPlayer );
        if( map_material_paths.Num() )
        {
            TArray<FString> keys;
            map_material_paths.GetKeys( keys );

            root_path = keys[0];
        }
    }

    if( root_path.IsEmpty() )
    {
        if( iRootSequence )
        {
            // Default epos root path
            root_path = iRootSequence->GetPackage()->GetName() + TEXT( "_Private" ); // ie. /Game/MyStoryboard2_Private
        }
    }

    if( root_path.IsEmpty() )
    {
        IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate()
        UMovieSceneSequence* root_sequence = player->GetEvaluationTemplate().GetRootSequence();
        if( root_sequence )
        {
            // Default root path
            root_path = root_sequence->GetPackage()->GetName() + TEXT( "_Private" ); // ie. /Game/MyLevelSequence_Private
        }
    }

    if( root_path.IsEmpty() )
    {
        root_path = TEXT( "/Game" );
    }

    return root_path;
}

//---
//---
//---

//static
FString
NamingConvention::GenerateCameraActorPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iCreateSubpath, FString& oPath, FString& oName )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iSequenceID );
    check( epos_root_sequence );

    FString root_sequence_name = FPackageName::GetShortName( epos_root_sequence->GetPackage()->GetName() );
    FString current_sequence_name = FPackageName::GetShortName( iSequence.GetDisplayName().ToString() );
    //FString current_sequence_name = FPackageName::GetShortName( iSequence->GetPackage()->GetName() );

    FString camera_path = root_sequence_name;
    if( iCreateSubpath )
        camera_path = ( epos_root_sequence != &iSequence ) ? root_sequence_name / current_sequence_name : root_sequence_name;

    //--- Find all camera track names

    TArray<FString> camera_names;

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetSharedPlaybackState()->GetHierarchy();
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& map = hierarchy->AllSubSequenceData();
        for( auto pair : map )
        {
            UMovieSceneSequence* sequence = pair.Value.GetSequence();
            FMovieSceneSequenceID sequence_id = pair.Key;

            if( !sequence->IsA<UShotSequence>() )
                continue;

            FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *player, sequence, sequence_id );
            ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawnedOrTemplate( *player, sequence, sequence_id, camera_binding );
            if( !camera )
                continue;

            camera_names.AddUnique( camera->GetActorLabel() );
        }
    }

    //--- Find all camera indexes inside their names (through a regex)

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionCamera camera_settings = settings->CameraNaming;

    //TOCHECK: if it's no possible to use "named group", maybe try to order the replacement of {...} so we maybe have the group index ?
    const FString camera_pattern_regex = camera_settings.Pattern.Replace( *camera_settings.mPatternKeywordLists.GetKeyword( ENamingConventionCameraPatternKeyword::CameraIndex ).mKeywordWithBraces, TEXT( "([0-9]+)" ) );
    const FString camera_pattern_display = camera_settings.Pattern.Replace( *camera_settings.mPatternKeywordLists.GetKeyword( ENamingConventionCameraPatternKeyword::CameraIndex ).mKeywordWithBraces, TEXT( "{camera_index_formated}" ) );
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

    //--- Try to find the new camera name depending of the max existing index

    int32 max_camera_index = list_of_camera_index.Num() ? list_of_camera_index[0] : camera_settings.IndexFormat.StartNumber;

    FStringFormatNamedArguments args;
    args.Add( TEXT( "camera_index_formated" ), FString::Printf( TEXT( "%0*d" ), camera_settings.IndexFormat.NumDigits, max_camera_index ) );
    FString camera_name = FString::Format( *camera_pattern_display, args );

    while( camera_names.Contains( camera_name ) )
    {
        max_camera_index += camera_settings.IndexFormat.Increment;

        args.FindChecked( TEXT( "camera_index_formated" ) ) = FString::Printf( TEXT( "%0*d" ), camera_settings.IndexFormat.NumDigits, max_camera_index );
        camera_name = FString::Format( *camera_pattern_display, args );
    }

    //---

    oPath = camera_path;
    oName = camera_name;

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateAnimationActorPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iCreateSubpath, FString& oPath, FString& oName )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllAnimations()/GetAllDrawings() will use it to find cache

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iSequenceID );
    check( epos_root_sequence );

    UEposMovieSceneSequence* current_sequence = const_cast<UEposMovieSceneSequence*>( &iSequence ); //PATCH: Because there is no 'const' parameter version of ShotSequenceHelpers::GetCamera()

    //--- Find the camera actor

    FString animation_path;
    FString camera_name;

    FGuid camera_binding = ShotSequenceHelpers::GetCameraBinding( *player, current_sequence, iSequenceID );
    ACineCameraActor* camera = ShotSequenceHelpers::GetCameraSpawnedOrTemplate( *player, current_sequence, iSequenceID, camera_binding );
    if( camera )
    {
        animation_path = camera->GetFolderPath().ToString();
        camera_name = camera->GetActorLabel();
    }

    if( animation_path.IsEmpty() )
    {
        FString camera_path;
        GenerateCameraActorPathName( iPlayer, iSequence, iSequenceID, iCreateSubpath, camera_path, camera_name );

        animation_path = camera_path;
    }

    //--- Find all animation track names

    const UMovieScene* moviescene = iSequence.GetMovieScene();
    const TArray<FMovieSceneBinding>& bindings = moviescene->GetBindings();
    TArray<FString> binding_names;
    for( auto binding : bindings )
        binding_names.AddUnique( moviescene->GetObjectDisplayName( binding.GetObjectGuid() ).ToString() );
        // Don't use binding.GetName() because (for example) it is not updated when the name is changed directly in the shot track label (instead of the board section view)
        // binding.GetName() is still keeping the old "track" name

    //--- Find all animation indexes inside their names (through a regex)

    const UNamingConventionSettings* settings = GetDefault<UNamingConventionSettings>();
    FNamingConventionAnimation animation_settings = settings->AnimationNaming;

    //TOCHECK: if it's no possible to use "named group", maybe try to order the replacement of {...} so we maybe have the group index ?
    const FString animation_pattern_regex = animation_settings.Pattern.Replace( *animation_settings.mPatternKeywordLists.GetKeyword( ENamingConventionAnimationPatternKeyword::AnimationIndex ).mKeywordWithBraces, TEXT( "([0-9]+)" ) );
    const FString animation_pattern_display = animation_settings.Pattern.Replace( *animation_settings.mPatternKeywordLists.GetKeyword( ENamingConventionAnimationPatternKeyword::AnimationIndex ).mKeywordWithBraces, TEXT( "{animation_index_formated}" ) );
    FRegexPattern animation_pattern( animation_pattern_regex );

    // https://stackoverflow.com/questions/3075130/what-is-the-difference-between-and-regular-expressions
    // https://www.regular-expressions.info/refadv.html
    // https://www.regular-expressions.info/atomic.html

    TArray<int32> list_of_animation_index;

    for( auto binding_name : binding_names )
    {
        FRegexMatcher matcher( animation_pattern, binding_name );

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

            list_of_animation_index.Add( index );
        }
    }

    list_of_animation_index.Sort( TGreater<int32>() );

    //--- Try to find the new animation name depending of the max existing index

    int32 max_animation_index = list_of_animation_index.Num() ? list_of_animation_index[0] : animation_settings.IndexFormat.StartNumber;

    FStringFormatNamedArguments args;
    args.Add( TEXT( "animation_index_formated" ), FString::Printf( TEXT( "%0*d" ), animation_settings.IndexFormat.NumDigits, max_animation_index ) );
    FString animation_name = FString::Format( *animation_pattern_display, args );

    while( binding_names.Contains( animation_name ) )
    {
        max_animation_index += animation_settings.IndexFormat.Increment;

        args.FindChecked( TEXT( "animation_index_formated" ) ) = FString::Printf( TEXT( "%0*d" ), animation_settings.IndexFormat.NumDigits, max_animation_index );
        animation_name = FString::Format( *animation_pattern_display, args );
    }

    //---

    oPath = animation_path;
    oName = animation_name;

    return oPath / oName;
}

//static
FString
NamingConvention::GenerateCameraTrackName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, ACineCameraActor* iCamera )
{
    // See comment in GenerateAnimationTrackName()
    return iCamera->GetActorLabel();
}

//static
FString
NamingConvention::GenerateAnimationTrackName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, AOdysseyAnimationActor* iAnimation )
{
    // For the moment, it's ok, but it will change, double-check (for example) the clone animation function...
    // Or maybe name this function GenerateAnimationTrackNameFROMACTOR() et add another one which will really compute a new track name from the existing ones ?
    return iAnimation->GetActorLabel();
}

//---
//---
//---

//static
FString
NamingConvention::GenerateNoteAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPath, FString& oName )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetRootEposSequence()

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iSequenceID );

    //---

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
        FString root_path = GetRootPath( iPlayer, epos_root_sequence ); // ie. /Game/MyStoryboard2
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
NamingConvention::GenerateAnimationAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPath, FString& oName )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iSequenceID );

    //---

    FString animation_path;

    // Try to find the better path from all existing animations
    FRelevantPathMap map_animation_paths = FindAllAnimationPaths( iPlayer );
    if( map_animation_paths.Num() )
    {
        TArray<FString> keys;
        map_animation_paths.GetKeys( keys );

        animation_path = keys[0];
    }

    if( animation_path.IsEmpty() )
    {
        // Try to find the better path from all existing subsequences
        FRelevantPathMap map_sequence_paths = FindAllSequencePaths( iPlayer );
        if( map_sequence_paths.Num() )
        {
            TArray<FString> keys;
            map_sequence_paths.GetKeys( keys );

            animation_path = keys[0];
        }
    }

    if( animation_path.IsEmpty() )
    {
        // Default path of the new animation
        FString root_path = GetRootPath( iPlayer, epos_root_sequence ); // ie. /Game/MyStoryboard2
        animation_path = root_path;

    }

    //---

    FString animation_name = TEXT( "A_" ) + FGuid::NewGuid().ToString();

    //---

    oName = animation_name;
    oPath = animation_path;

    return animation_path / animation_name;
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
    UMovieSceneCinematicBoardTrack* boardTrack = iParentSequence->GetMovieScene()->FindTrack<UMovieSceneCinematicBoardTrack>();
    if( !boardTrack )
        return TOptional<FSequenceNameElements>();

    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();
    UMovieSceneSection* section = EposSequenceHelpers::FindSectionAtTime( sections, iFrameNumber );
    if( !section )
        section = EposSequenceHelpers::FindNearestSectionAtTime( sections, iFrameNumber );
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

    const FMovieSceneSequenceHierarchy* hierarchy = player.GetSharedPlaybackState()->GetHierarchy();
    if( hierarchy )
    {
        const TMap<FMovieSceneSequenceID, FMovieSceneSubSequenceData>& subsequences = hierarchy->AllSubSequenceData();
        for( auto pair : subsequences )
            map_sequences.Add( pair.Key, pair.Value.GetSequence() );
    }

    TMap<FMovieSceneSequenceID, UMovieSceneSequence*> map_epos_sequences;
    for( auto pair : map_sequences )
    {
        if( pair.Value->IsA<UEposMovieSceneSequence>() )
            map_epos_sequences.Add( pair );
    }

    //---

    for( auto pair : map_epos_sequences )
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
NamingConvention::GenerateBoardAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iParentSequence, FMovieSceneSequenceIDRef iParentSequenceID, FFrameNumber iFrameNumber, FString& oPath, FString& oName, FBoardNameElements& oElements )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iParentSequenceID );

    //---

    FString sequence_path;

    // Try to find the better path from existing sibling sequences
    FRelevantPathMap map_sequence_paths = FindSiblingSequencePaths( iPlayer, &iParentSequence );
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
        FString root_path = GetRootPath( iPlayer, epos_root_sequence ); // ie. /Game/MyStoryboard2
        sequence_path = root_path;
    }

    //--- Find all boards

    TArray<UBoardSequence*> board_sequences;

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetSharedPlaybackState()->GetHierarchy();
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

    TOptional<FSequenceNameElements> source_elements = FindNameElements( iPlayer, epos_root_sequence, &iParentSequence, iFrameNumber );

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
NamingConvention::GenerateShotAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iParentSequence, FMovieSceneSequenceIDRef iParentSequenceID, FFrameNumber iFrameNumber, FString& oPath, FString& oName, FShotNameElements& oElements )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iParentSequenceID );

    //---

    FString sequence_path;

    // Try to find the better path from existing sibling sequences
    FRelevantPathMap map_sequence_paths = FindSiblingSequencePaths( iPlayer, &iParentSequence );
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
        FString root_path = GetRootPath( iPlayer, epos_root_sequence ); // ie. /Game/MyStoryboard2
        sequence_path = root_path;
    }

    //--- Find all shots

    TArray<UShotSequence*> shot_sequences;

    const FMovieSceneSequenceHierarchy* hierarchy = player->GetEvaluationTemplate().GetHierarchy();
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

    TOptional<FSequenceNameElements> source_elements = FindNameElements( iPlayer, epos_root_sequence, &iParentSequence, iFrameNumber );

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
NamingConvention::GenerateTakeAssetPathName( const IMovieScenePlayer& iPlayer, const UEposMovieSceneSequence& iParentSequence, FMovieSceneSequenceIDRef iParentSequenceID, UMovieSceneSubSection* iSubSection, FString& oPath, FString& oName, FShotNameElements& oElements )
{
    IMovieScenePlayer* player = const_cast<IMovieScenePlayer*>( &iPlayer ); //PATCH: Because there is no 'const' version of GetEvaluationTemplate() and GetAllPlanes()/GetAllDrawings() will use it to find cache

    const UEposMovieSceneSequence* epos_root_sequence = EposSequenceHelpers::GetRootEposSequence( *player, iParentSequenceID );

    //---

    UShotSequence* subsequence = Cast<UShotSequence>( iSubSection->GetSequence() );
    //check( subsequence );

    FString sequence_pathname = subsequence ? subsequence->GetPackage()->GetName() : FString();
    FString sequence_path = FPackageName::GetLongPackagePath( sequence_pathname );

    // Try to find the better path from existing sibling sequences
    if( sequence_path.IsEmpty() )
    {
        FRelevantPathMap map_sequence_paths = FindSiblingSequencePaths( iPlayer, &iParentSequence );
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
        FString root_path = GetRootPath( iPlayer, epos_root_sequence ); // ie. /Game/MyStoryboard2
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

    TOptional<FSequenceNameElements> source_elements = subsequence ? subsequence->NameElements : FindNameElements( iPlayer, epos_root_sequence, &iParentSequence, iSubSection->GetRange().GetLowerBoundValue() );

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
