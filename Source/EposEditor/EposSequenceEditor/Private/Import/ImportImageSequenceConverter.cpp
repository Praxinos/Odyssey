// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Import/ImportImageSequenceConverter.h"

#include "AssetToolsModule.h"
#include "Factories/TextureFactory.h"
#include "ImageUtils.h"
#include "ISequencer.h"
#include "Sections/MovieSceneSubSection.h"

#include "Animation/OdysseyPainterEditorAnimationImport.h"
#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "NamingConvention.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/ResourceAssetTools.h"

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

        CreateAnimation( shot.Panels, subsection );
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
FImportImageSequenceConverter::CreateAnimation( const TArray<FImportImageSequencePanel>& iPanels, UMovieSceneSubSection* iSubSection )
{
    check( iPanels.Num() );

    ISequencer* sequencer = mSequencer.Pin().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *iSubSection, sequencer->GetFocusedTemplateID() );
    check( result.mInnerSequence )

    //---

    TArray<UTexture2D*> textures;
    TArray<int32> durations;
    for( int i = 0; i < iPanels.Num(); i++ )
    {
        FImage image;
        FImageUtils::LoadImage( *iPanels[i].Pathfile.FilePath, image );
        UTexture2D* texture = Cast<UTexture2D>( FImageUtils::CreateTexture( ETextureClass::TwoD, image, GetTransientPackage(), FGuid::NewGuid().ToString() ) );

        textures.Add( texture );

        durations.Add( iPanels[i].Duration );
    }

    check( textures.Num() == durations.Num() );

    //---

    UOdysseyAnimation* new_animation = nullptr;
    if( textures.Num() )
    {
        const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
        TOptional<FLinearColor> background_layer_color;
        FIntPoint texture_size( textures[0]->Source.GetSizeX(), textures[0]->Source.GetSizeY() );

        new_animation = ProjectAssetTools::CreateAnimation( *sequencer, result.mInnerSequence, result.mInnerSequenceId, texture_size, settings->AnimationSettings.Format, settings->AnimationSettings.FrameRate, UOdysseyAnimationLayerImageRaster::StaticClass(), background_layer_color );

        if( new_animation )
        {
            // Remove all existing layers
            UOdysseyLayerStack* layer_stack = new_animation->GetLayerStack();
            layer_stack->RemoveLayers( layer_stack->GetLayers() );

            //---

            // Import the textures (create layer, cells, ...)
            FOdysseyPainterEditorAnimationImport import_sequence;
            UOdysseyAnimationLayerImageRaster* animation_layer = import_sequence.ImportTextureSequence( new_animation, textures, nullptr, 0 );

            //---

            TArray<UOdysseyLayerCell*> cells = animation_layer->GetCells();
            // To have at least durations as long as cells
            if( !ensure( durations.Num() >= cells.Num() ) )
            {
                int32 diff = cells.Num() - durations.Num();
                TArray<int32> padding;
                while( padding.Num() != diff )
                    padding.Add( 48 ); // Arbitrary
                durations.Append( padding );

                check( durations.Num() == cells.Num() );
            }

            // Set the exposure of all cells
            for( int i = 0; i < cells.Num(); i++ )
            {
                //int32 duration_in_tick = ConvertFromDisplayRateToTickResolution( iPanels[0].Duration );

                cells[i]->SetExposure( durations[i] );
            }
        }
    }

    //---

    {
        FCameraArgs camera_args;
        FAnimationArgs animation_args;
        animation_args.mMargin = 0.f;
        animation_args.mAnimation = new_animation;
        BoardSequenceTools::CreateCameraWithAnimation( sequencer, iSubSection->GetTrueRange().GetLowerBoundValue(), camera_args, animation_args );
    }

    //sequencer->ForceEvaluate();
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
