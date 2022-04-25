// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImageSequenceImporter.h"

#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "ImageSequenceImporter"

//---

FImageSequenceImporter::FImageSequenceImporter( const FString& iPath, FString& oErrorMessage )
{
    mImageSequencePath = iPath;

    Build( oErrorMessage );
}

const FImageSequenceStruct&
FImageSequenceImporter::GetImageSequenceStruct() const
{
    return mImageSequenceStruct;
}

//---

void
FImageSequenceImporter::Build( FString& oErrorMessage )
{
    TArray<FString> files;
    IFileManager::Get().FindFiles( files, *mImageSequencePath, TEXT( "png" ) );
    IFileManager::Get().FindFiles( files, *mImageSequencePath, TEXT( "jpg" ) );
    IFileManager::Get().FindFiles( files, *mImageSequencePath, TEXT( "jpeg" ) );

    if( files.IsEmpty() )
    {
        oErrorMessage = TEXT( "no file to load in " ) + mImageSequencePath;
        return;
    }

    FImageSequenceFrame frame;

    FImageSequenceBoard board0;
    {
        FImageSequenceShot shot0;
        frame.Pathfile.FilePath = mImageSequencePath / files[0];
        shot0.Frames.Add( frame );
        frame.Pathfile.FilePath = mImageSequencePath / files[1];
        shot0.Frames.Add( frame );
        frame.Pathfile.FilePath = mImageSequencePath / files[2];
        shot0.Frames.Add( frame );

        board0.Shots.Add( shot0 );

        FImageSequenceShot shot1;
        frame.Pathfile.FilePath = mImageSequencePath / files[3];
        shot1.Frames.Add( frame );
        frame.Pathfile.FilePath = mImageSequencePath / files[4];
        shot1.Frames.Add( frame );

        board0.Shots.Add( shot1 );
    }

    //---

    FImageSequenceBoard board1;
    {
        FImageSequenceShot shot0;
        frame.Pathfile.FilePath = mImageSequencePath / files[5];
        shot0.Frames.Add( frame );

        board1.Shots.Add( shot0 );

        FImageSequenceShot shot1;
        frame.Pathfile.FilePath = mImageSequencePath / files[6];
        shot1.Frames.Add( frame );

        board1.Shots.Add( shot1 );
    }

    //---

    mImageSequenceStruct.Boards.Add( board0 );
    mImageSequenceStruct.Boards.Add( board1 );

    //---
    //---
    //---

    //FImageSequenceFrame frame;

    //FImageSequenceBoard board0;
    //{
    //    FImageSequenceShot shot0;
    //    frame.Pathfile.FilePath = mImageSequencePath / files[0];
    //    shot0.Frames.Add( frame );
    //    frame.Pathfile.FilePath = mImageSequencePath / files[1];
    //    shot0.Frames.Add( frame );
    //    frame.Pathfile.FilePath = mImageSequencePath / files[2];
    //    shot0.Frames.Add( frame );

    //    board0.Shots.Add( shot0 );

    //    FImageSequenceShot shot1;
    //    frame.Pathfile.FilePath = mImageSequencePath / files[3];
    //    shot1.Frames.Add( frame );
    //    frame.Pathfile.FilePath = mImageSequencePath / files[4];
    //    shot1.Frames.Add( frame );

    //    board0.Shots.Add( shot1 );

    //    FImageSequenceShot shot2;
    //    frame.Pathfile.FilePath = mImageSequencePath / files[5];
    //    shot2.Frames.Add( frame );

    //    board0.Shots.Add( shot2 );

    //    FImageSequenceShot shot3;
    //    frame.Pathfile.FilePath = mImageSequencePath / files[6];
    //    shot3.Frames.Add( frame );

    //    board0.Shots.Add( shot3 );
    //}

    //mImageSequenceStruct.Boards.Add( board0 );

    //---
    //---
    //---

    //FImageSequenceShot shot;

    //for( auto file : files )
    //{
    //    FImageSequenceFrame frame;
    //    frame.Pathfile.FilePath = mImageSequencePath / file;
    //    //frame.Duration = 48;

    //    shot.Frames.Add( frame );
    //}

    //FImageSequenceBoard board;
    //board.Shots.Add( shot );

    //mImageSequenceStruct.Boards.Add( board );
}

//---

#undef LOCTEXT_NAMESPACE
