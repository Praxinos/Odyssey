// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Render/EposSequencePipelineRenderer.h"

#include "IMovieRenderPipelineEditorModule.h"
#include "LevelSequence.h"
#include "MoviePipelineEditorBlueprintLibrary.h"
#include "MoviePipelineExecutor.h"
#include "MoviePipelineQueueSubsystem.h"
#include "MovieRenderPipelineCoreModule.h"
#include "MovieRenderPipelineSettings.h"

#include "EposMovieSceneSequence.h"
#include "Render/EposSequenceRenderHelpers.h"
#include "Render/SRenderOptions.h"

#define LOCTEXT_NAMESPACE "EposSequencePipelineRenderer"

//---

// IMovieRenderPipelineEditorModule is not _API exported, so duplicate values here

//FName IMovieRenderPipelineEditorModule::MoviePipelineQueueTabName = "MoviePipelineQueue";
FName FEposSequencePipelineRenderer::MoviePipelineQueueTabName = "MoviePipelineQueue";
//FText IMovieRenderPipelineEditorModule::MoviePipelineQueueTabLabel = LOCTEXT( "MovieRenderQueueTab_Label", "Movie Render Queue" );
//static FText sgMoviePipelineQueueTabLabel = LOCTEXT( "MovieRenderQueueTab_Label", "Movie Render Queue" );

FText FEposSequencePipelineRenderer::MoviePipelineQueueTabLabel = LOCTEXT( "StoryboardRenderQueueTab_Label", "Storyboard Render Queue" );

void
FEposSequencePipelineRenderer::RenderMovie( UMovieSceneSequence* iSequence, const TArray<UMovieSceneCinematicShotSection*>& iSections ) //override
{
    if( !mMasterConfig )
    {
        TSharedPtr<SRenderOptions> render_options_widget;

        TSharedRef<SWindow> window = SNew( SWindow )
            .Title( LOCTEXT( "storyboard-render-options-title", "Storyboard Render Options" ) )
            .SizingRule( ESizingRule::Autosized );

        window->SetContent
        (
            SAssignNew( render_options_widget, SRenderOptions )
            .ParentWindow( window )
            .Sequence( iSequence )
        );

        GEditor->EditorAddModalWindow( window );

        if( render_options_widget->IsCanceled() )
            return;

        mMasterConfig = render_options_widget->GetMasterConfig();
    }

    //---

    UEposMovieSceneSequence* epos_sequence = Cast<UEposMovieSceneSequence>( iSequence );
    if( epos_sequence )
    {
        TArray<UEposMovieSceneSequence*> sequences;
        sequences.Add( epos_sequence );
        iSequence = EposSequenceRenderHelpers::CreateLevelSequenceTransient( sequences );
        if( !iSequence )
            return;
    }

    //---

    UMoviePipelineQueue* ActiveQueue = GEditor->GetEditorSubsystem<UMoviePipelineQueueSubsystem>()->GetQueue();
    check( ActiveQueue );

    UMoviePipelineExecutorJob* ActiveJob = nullptr;
    for( UMoviePipelineExecutorJob* Job : ActiveQueue->GetJobs() )
    {
        if( Job->Sequence == FSoftObjectPath( iSequence ) )
        {
            ActiveQueue->DeleteJob( Job );
            //ActiveJob = Job;
            break;
        }
    }

    if( !ActiveJob )
    {
        GEditor->BeginTransaction( FText::Format( LOCTEXT( "CreateJob_Transaction", "Add {0}|plural(one=Job, other=Jobs)" ), 1 ) );

        ActiveJob = UMoviePipelineEditorBlueprintLibrary::CreateJobFromSequence( ActiveQueue, Cast<ULevelSequence>( iSequence ) );

        //// The job configuration is already set up with an empty configuration, but we'll try and use their last used preset
        //// (or a engine supplied default) for better user experience.
        //const UMovieRenderPipelineProjectSettings* ProjectSettings = GetDefault<UMovieRenderPipelineProjectSettings>();
        //if( ProjectSettings->LastPresetOrigin.IsValid() )
        //{
        //    ActiveJob->SetPresetOrigin( ProjectSettings->LastPresetOrigin.Get() );
        //}

        ActiveJob->SetPresetOrigin( mMasterConfig );
        // or ActiveJob->SetConfiguration( mMasterConfig ); ??

        mMasterConfig = nullptr;
    }

    UMoviePipelineEditorBlueprintLibrary::EnsureJobHasDefaultSettings( ActiveJob );

    //TArray<FString> ShotNames;
    //for( UMovieSceneCinematicShotSection* ShotSection : iSections )
    //{
    //    ShotNames.Add( ShotSection->GetShotDisplayName() );
    //}

    if( !GEditor->IsTransactionActive() )
    {
        GEditor->BeginTransaction( LOCTEXT( "ModifyJob_Transaction", "Modifying shots in existing job" ) );
    }

    ActiveJob->Modify();

    for( UMoviePipelineExecutorShot* Shot : ActiveJob->ShotInfo )
    {
        // If no specified shots, enabled them all
        if( Shot )
        {
            Shot->bEnabled = true;
            //Shot->bEnabled = ShotNames.Num() == 0 ? true : ShotNames.Contains( Shot->OuterName );
        }
    }

    if( GEditor->IsTransactionActive() )
    {
        GEditor->EndTransaction();
    }

    FGlobalTabmanager::Get()->TryInvokeTab( MoviePipelineQueueTabName );
};

FString
FEposSequencePipelineRenderer::GetDisplayName() const //override
{
    return MoviePipelineQueueTabLabel.ToString();
}

//---

#undef LOCTEXT_NAMESPACE
