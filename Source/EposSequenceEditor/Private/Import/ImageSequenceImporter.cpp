// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImageSequenceImporter.h"

#include "Internationalization/Regex.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "ImageSequenceImporter"

//---

FImageSequenceImporter::FImageSequenceImporter( const FImageSequenceImportOptions& iOptions, FString& oErrorMessage )
{
    mImageSequencePath = iOptions.ImageSequencePath.Path;
    mImageSequenceFilePattern = iOptions.FilePattern;

    Build( iOptions.PatternKeywords, oErrorMessage );
}

const FImageSequenceStruct&
FImageSequenceImporter::GetImageSequenceStruct() const
{
    return mImageSequenceStruct;
}

//---

struct FPatternStruct
{
    FString mKeyWithBraces;
    FString mRegex;
    int32   mKeyIndex { INDEX_NONE };
    int32   mKeyPosition { 0 };
};

void
FImageSequenceImporter::Build( const TMap<EImageSequencePatternKeyword, FImageSequencePatternKeyword>& iKeywords, FString& oErrorMessage )
{
    if( mImageSequencePath.IsEmpty() )
    {
        oErrorMessage = TEXT( "no folder to list " ) + mImageSequencePath;
        return;
    }

    TArray<FString> files;
    IFileManager::Get().FindFiles( files, *mImageSequencePath );
    //IFileManager::Get().FindFilesRecursive( files, *mImageSequencePath, TEXT( "*" ), true /* iFiles */, false /* iDirectories */ );

    files.StableSort();

    if( files.IsEmpty() )
    {
        oErrorMessage = TEXT( "no file to load in " ) + mImageSequencePath;
        return;
    }

    //---

    TMap<EImageSequencePatternKeyword, FPatternStruct> pattern_map;
    pattern_map.Add( EImageSequencePatternKeyword::BoardId  , { iKeywords[EImageSequencePatternKeyword::BoardId].mKeywordWithBraces , TEXT( "([_0-9a-zA-Z]+)" ) } );
    pattern_map.Add( EImageSequencePatternKeyword::ShotId   , { iKeywords[EImageSequencePatternKeyword::ShotId].mKeywordWithBraces  , TEXT( "([_0-9a-zA-Z]+)" ) } );
    pattern_map.Add( EImageSequencePatternKeyword::FrameId  , { iKeywords[EImageSequencePatternKeyword::FrameId].mKeywordWithBraces , TEXT( "([_0-9a-zA-Z]+)" ) } );
    pattern_map.Add( EImageSequencePatternKeyword::Duration , { iKeywords[EImageSequencePatternKeyword::Duration].mKeywordWithBraces, TEXT( "([0-9]+)" ) } );

    FString file_pattern_regex = mImageSequenceFilePattern;
    for( auto& pair : pattern_map )
    {
        file_pattern_regex = file_pattern_regex.Replace( *pair.Value.mKeyWithBraces, *pair.Value.mRegex );
    }
    FRegexPattern file_pattern( file_pattern_regex );

    //-

    for( auto& pair : pattern_map )
    {
        pair.Value.mKeyIndex = mImageSequenceFilePattern.Find( pair.Value.mKeyWithBraces );
    }

    if( pattern_map[EImageSequencePatternKeyword::ShotId].mKeyIndex == INDEX_NONE
        || pattern_map[EImageSequencePatternKeyword::FrameId].mKeyIndex == INDEX_NONE )
    {
        oErrorMessage = TEXT( "no {shot} or {panel} keys in pattern " ) + mImageSequenceFilePattern;
        return;
    }

    pattern_map.ValueStableSort( []( const FPatternStruct& iA, const FPatternStruct& iB ) { return iA.mKeyIndex < iB.mKeyIndex; } );

    int32 inc = 1;
    for( auto& pair : pattern_map )
    {
        if( pair.Value.mKeyIndex == INDEX_NONE )
            continue;

        pair.Value.mKeyPosition += inc;

        inc++;
    }

    //-

    for( auto file : files )
    {
        FRegexMatcher matcher( file_pattern, file );

        if( !matcher.FindNext() )
            continue;

        //int32 full_begin = matcher.GetMatchBeginning();
        //int32 full_end = matcher.GetMatchEnding();
        //FTextRange full_range( full_begin, full_end );
        //FString full_string = file.Mid( full_range.BeginIndex, full_range.Len() );

        FString id_board_string;
        if( pattern_map[EImageSequencePatternKeyword::BoardId].mKeyPosition != 0 )
            id_board_string = matcher.GetCaptureGroup( pattern_map[EImageSequencePatternKeyword::BoardId].mKeyPosition );

        FString id_shot_string = matcher.GetCaptureGroup( pattern_map[EImageSequencePatternKeyword::ShotId].mKeyPosition );

        FString id_frame_string = matcher.GetCaptureGroup( pattern_map[EImageSequencePatternKeyword::FrameId].mKeyPosition );

        int32 frame_duration = -1;
        if( pattern_map[EImageSequencePatternKeyword::Duration].mKeyPosition != 0 )
        {
            FString duration_string = matcher.GetCaptureGroup( pattern_map[EImageSequencePatternKeyword::Duration].mKeyPosition );
            if( duration_string.IsNumeric() )
                frame_duration = FCString::Atoi( *duration_string );
        }

        //---

        FImageSequenceBoard* board = nullptr;

        if( id_board_string.IsEmpty() )
        {
            if( mImageSequenceStruct.Boards.IsEmpty() )
            {
                FImageSequenceBoard new_board;
                new_board.Id = TEXT( "auto" );

                int32 index = mImageSequenceStruct.Boards.Add( new_board );
            }

            board = &mImageSequenceStruct.Boards[0];
        }
        else
        {
            for( int32 i = 0; i < mImageSequenceStruct.Boards.Num(); i++ )
            {
                if( mImageSequenceStruct.Boards[i].Id == id_board_string )
                {
                    board = &mImageSequenceStruct.Boards[i];
                    break;
                }
            }

            if( !board )
            {
                FImageSequenceBoard new_board;
                new_board.Id = id_board_string;

                int32 index = mImageSequenceStruct.Boards.Add( new_board );
                board = &mImageSequenceStruct.Boards[index];
            }
        }

        check( board );

        //---

        FImageSequenceShot* shot = nullptr;

        for( int32 i = 0; i < board->Shots.Num(); i++ )
        {
            if( board->Shots[i].Id == id_shot_string )
            {
                shot = &board->Shots[i];
                break;
            }
        }

        if( !shot )
        {
            FImageSequenceShot new_shot;
            new_shot.Id = id_shot_string;

            int32 index = board->Shots.Add( new_shot );
            shot = &board->Shots[index];
        }

        check( shot );

        //---

        FImageSequenceFrame* frame = nullptr;

        for( int32 i = 0; i < shot->Frames.Num(); i++ )
        {
            if( shot->Frames[i].Id == id_frame_string )
            {
                frame = &shot->Frames[i];
                break;
            }
        }

        if( !frame )
        {
            FImageSequenceFrame new_frame;
            new_frame.Id = id_frame_string;

            int32 index = shot->Frames.Add( new_frame );
            frame = &shot->Frames[index];
        }

        check( frame );

        //---

        frame->Pathfile.FilePath = mImageSequencePath / file;

        if( frame_duration > 0 )
            frame->Duration = frame_duration;
    }
}

//---

#undef LOCTEXT_NAMESPACE
