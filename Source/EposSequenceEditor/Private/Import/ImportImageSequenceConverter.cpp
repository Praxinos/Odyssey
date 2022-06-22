// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/ImportImageSequenceConverter.h"

#include "AssetToolsModule.h"
#include "ISequencer.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ImportImageSequenceConverter"

//---

FImportImageSequenceConverter::FImportImageSequenceConverter( const FImportImageSequenceStruct* iImageSequenceStruct, TWeakPtr<ISequencer> iSequencer, UBoardSequence* ioBoardSequence )
    : mSequencer( iSequencer )
    , mImageSequenceStruct( iImageSequenceStruct )
    , mBoardSequence( ioBoardSequence )
{
    check( iSequencer.Pin()->GetFocusedMovieSceneSequence() == ioBoardSequence );

    Convert();
}

void
FImportImageSequenceConverter::Convert()
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    if( mImageSequenceStruct->Boards.Num() == 1 )
    {
        CreateShotsRecursive( mImageSequenceStruct->Boards[0].Shots, mBoardSequence );
    }
    else
    {
        CreateBoardsRecursive( mImageSequenceStruct->Boards, mBoardSequence );
    }
}

void
FImportImageSequenceConverter::CreateBoardsRecursive( const TArray<FImportImageSequenceBoard>& iBoards, UBoardSequence* ioParentBoardSequence )
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    for( auto& board : iBoards )
    {
        CreateBoard( board, ioParentBoardSequence );
    }

    //---

    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( sequencer );
    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();

    check( iBoards.Num() == sections.Num() );

    for( int i = 0; i < iBoards.Num(); i++ )
    {
        FImportImageSequenceBoard board = iBoards[i];
        UMovieSceneSubSection* subsection = CastChecked<UMovieSceneSubSection>( sections[i] );

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection, sequencer->GetFocusedTemplateID() );
        if( !result.mInnerSequence )
            continue;

        ShotSequenceTools::cTemporarySwitchInner switch_to( *sequencer, result.mInnerSequenceId );

        CreateShotsRecursive( board.Shots, CastChecked<UBoardSequence>( sequencer->GetFocusedMovieSceneSequence() ) );
    }
}

void
FImportImageSequenceConverter::CreateBoard( const FImportImageSequenceBoard& iBoard, UBoardSequence* ioParentBoardSequence )
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    int32 duration = GetDuration( iBoard );

    int32 duration_in_tick = ConvertFromDisplayRateToTickResolution( duration );

    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( sequencer );
    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();
    FFrameNumber end_frame = 0;
    for( auto& section : sections )
    {
        FFrameNumber current_section_end = section->GetTrueRange().GetUpperBoundValue();
        if( current_section_end > end_frame )
            end_frame = current_section_end;
    }

    CinematicBoardTrackTools::InsertBoard( sequencer, end_frame, duration_in_tick );
    sequencer->ForceEvaluate();
}

void
FImportImageSequenceConverter::CreateShotsRecursive( const TArray<FImportImageSequenceShot>& iShots, UBoardSequence* ioParentBoardSequence )
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    for( auto& shot : iShots )
    {
        CreateShot( shot, ioParentBoardSequence );
    }

    //---

    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( sequencer );
    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();

    check( iShots.Num() == sections.Num() );

    for( int i = 0; i < iShots.Num(); i++ )
    {
        FImportImageSequenceShot shot = iShots[i];
        UMovieSceneSubSection* subsection = CastChecked<UMovieSceneSubSection>( sections[i] );

        CreateDrawings( shot.Panels, subsection );
    }
}

void
FImportImageSequenceConverter::CreateShot( const FImportImageSequenceShot& iShot, UBoardSequence* ioParentBoardSequence )
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    int32 duration = GetDuration( iShot );

    int32 duration_in_tick = ConvertFromDisplayRateToTickResolution( duration );

    UMovieSceneCinematicBoardTrack* boardTrack = BoardSequenceTools::FindOrCreateCinematicBoardTrack( sequencer );
    TArray<UMovieSceneSection*> sections = boardTrack->GetAllSections();
    FFrameNumber end_frame = 0;
    for( auto& section : sections )
    {
        FFrameNumber current_section_end = section->GetTrueRange().GetUpperBoundValue();
        if( current_section_end > end_frame )
            end_frame = current_section_end;
    }

    CinematicBoardTrackTools::InsertShot( sequencer, end_frame, duration_in_tick );
    sequencer->ForceEvaluate();
}

void
FImportImageSequenceConverter::CreateDrawings( const TArray<FImportImageSequencePanel>& iPanels, UMovieSceneSubSection* iSubSection )
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    FString path;
    FString name;
    NamingConvention::GenerateTextureAssetPathName( *sequencer, mBoardSequence, iSubSection->GetSequence(), nullptr, path, name );
    FString destination_path = path;

    check( iPanels.Num() );

    FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>( "AssetTools" );

    //TArray<FString> pathfiles { panel.Pathfile.FilePath };
    //TArray<TPair<FString, FString>> FilesAndDestinations;
    //AssetToolsModule.Get().ExpandDirectories( pathfiles, destination_path, FilesAndDestinations );
    //AssetToolsModule.Get().ImportAssets( pathfiles, destination_path, nullptr, true, &FilesAndDestinations, false );

    {
        TArray<FString> pathfiles { iPanels[0].Pathfile.FilePath };
        TArray<UObject*> assets = AssetToolsModule.Get().ImportAssets( pathfiles, destination_path );
        UTexture2D* texture = Cast<UTexture2D>( assets[0] );
        check( texture );

        FCameraArgs camera_args;
        FPlaneArgs plane_args;
        plane_args.mTexture = texture;
        BoardSequenceTools::CreateCamera( sequencer, iSubSection->GetTrueRange().GetLowerBoundValue(), camera_args, plane_args );
    }

    //sequencer->ForceEvaluate();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *iSubSection, sequencer->GetFocusedTemplateID() );
    check( result.mInnerSequence )

    TArray<FGuid> plane_bindings;
    ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAll, nullptr, &plane_bindings );
    check( plane_bindings.Num() );

    FGuid plane_binding = plane_bindings[0];

    //---

    int32 duration_in_tick = ConvertFromDisplayRateToTickResolution( iPanels[0].Duration );
    FFrameNumber next_frame_number = iSubSection->GetTrueRange().GetLowerBoundValue() + duration_in_tick;

    for( int i = 1; i < iPanels.Num(); i++ )
    {
        FImportImageSequencePanel panel = iPanels[i];

        TArray<FString> pathfiles { panel.Pathfile.FilePath };
        TArray<UObject*> assets = AssetToolsModule.Get().ImportAssets( pathfiles, destination_path );
        UTexture2D* texture = Cast<UTexture2D>( assets[0] );
        check( texture );

        //---

        FDrawingArgs drawing_args;
        drawing_args.mTexture = texture;
        BoardSequenceTools::CreateDrawing( sequencer, next_frame_number, plane_binding, drawing_args );

        //---

        duration_in_tick = ConvertFromDisplayRateToTickResolution( panel.Duration );
        next_frame_number += duration_in_tick;
    }
}

//---

int32
FImportImageSequenceConverter::GetDuration( const FImportImageSequenceBoard& iBoard ) const
{
    int32 duration = 0;
    for( auto& shot : iBoard.Shots )
        duration += GetDuration( shot );

    return duration;
}

int32
FImportImageSequenceConverter::GetDuration( const FImportImageSequenceShot& iShot ) const
{
    int32 duration = 0;
    for( auto& panel : iShot.Panels )
        duration += panel.Duration;

    return duration;
}

int32
FImportImageSequenceConverter::ConvertFromDisplayRateToTickResolution( int32 iDuration ) const
{
    FFrameRate tick_resolution = mBoardSequence->GetMovieScene()->GetTickResolution();
    FFrameRate display_rate = mBoardSequence->GetMovieScene()->GetDisplayRate();

    return FFrameRate::TransformTime( iDuration, display_rate, tick_resolution ).GetFrame().Value;
}

//---

#undef LOCTEXT_NAMESPACE
