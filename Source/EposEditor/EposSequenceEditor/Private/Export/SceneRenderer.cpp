// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/SceneRenderer.h"

#include "Camera/CameraComponent.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineModule.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneSequenceHierarchy.h"
#include "ISequencer.h"
#include "LegacyScreenPercentageDriver.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSequence.h"
#include "SceneViewExtension.h"
#include "SceneViewExtensionContext.h"

#include "Export/ExportStruct.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SceneRenderer"

//---

FSceneRenderer::FSceneRenderer( TWeakPtr<ISequencer> iSequencer, const FExportPanel* iPanel, const FIntPoint& iSize, EViewModeIndex iViewMode )
    : mSequencer( iSequencer )
    , mCurrentPanel( iPanel )
    , mSize( iSize )
    , mViewMode( iViewMode )
{
}

bool
FSceneRenderer::RenderPlane( TArray<FColor>& oSamples )
{
    oSamples.Reset();

    if( !mCurrentPanel )
        return false;

    PreDraw();

    GetViewCamera();
    RenderToTexture( oSamples, nullptr );

    PostDraw();

    return true;
}

bool
FSceneRenderer::RenderPlane( FTextureRenderTargetResource* oRenderTarget )
{
    if( !mCurrentPanel )
        return false;

    PreDraw();

    GetViewCamera();
    TArray<FColor> samples;
    RenderToTexture( samples, oRenderTarget );

    PostDraw();

    return true;
}

//---

void
FSceneRenderer::PreDraw()
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if( !sequencer.IsValid() )
        return;

    sequencer->EnterSilentMode();

    //SavedPlaybackStatus = sequencer->GetPlaybackStatus();
    sequencer->SetPlaybackStatus( EMovieScenePlayerStatus::Jumping );
    sequencer->SetGlobalTime( mCurrentPanel->GlobalFrame );
    sequencer->ForceEvaluate();
}

void
FSceneRenderer::PostDraw()
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if( !sequencer.IsValid() )
        return;

    //Thumbnail.SetupFade( sequencer->GetSequencerWidget() );
    sequencer->ExitSilentMode();
}

static
UCameraComponent*
GetFirstCamera( UMovieSceneSequence* iSequence, FMovieSceneSequenceID iSequenceID, FFrameNumber iLocalTime, IMovieScenePlayer& iPlayer )
{
    UMovieScene* InnerMovieScene = iSequence ? iSequence->GetMovieScene() : nullptr;
    if( !InnerMovieScene )
        return nullptr;

    int32 LowestRow = TNumericLimits<int32>::Max();
    int32 HighestOverlap = 0;

    UMovieSceneSingleCameraCutSection* ActiveSection = nullptr;

    if( UMovieSceneSingleCameraCutTrack* CutTrack = Cast<UMovieSceneSingleCameraCutTrack>( InnerMovieScene->GetCameraCutTrack() ) )
    {
        for( UMovieSceneSection* ItSection : CutTrack->GetAllSections() )
        {
            UMovieSceneSingleCameraCutSection* CutSection = Cast<UMovieSceneSingleCameraCutSection>( ItSection );
            if( CutSection && CutSection->GetRange().Contains( iLocalTime ) )
            {
                bool bSectionWins =
                    ( CutSection->GetRowIndex() < LowestRow ) ||
                    ( CutSection->GetRowIndex() == LowestRow && CutSection->GetOverlapPriority() > HighestOverlap );

                if( bSectionWins )
                {
                    HighestOverlap = CutSection->GetOverlapPriority();
                    LowestRow = CutSection->GetRowIndex();
                    ActiveSection = CutSection;
                }
            }
        }
    }

    return ActiveSection ? ActiveSection->GetFirstCamera( iPlayer, iSequenceID ) : nullptr;
}

UCameraComponent*
FindCameraCutComponentRecursive( FFrameNumber iGlobalTime, FMovieSceneSequenceID iInnerSequenceID, const FMovieSceneSequenceHierarchy& iHierarchy, IMovieScenePlayer& iPlayer )
{
    const FMovieSceneSequenceHierarchyNode* Node = iHierarchy.FindNode( iInnerSequenceID );
    const FMovieSceneSubSequenceData* SubData = iHierarchy.FindSubData( iInnerSequenceID );
    if( !ensure( SubData && Node ) )
    {
        return nullptr;
    }

    UMovieSceneSequence* InnerSequence = SubData->GetSequence();

    FFrameNumber InnerTime = ( iGlobalTime * SubData->RootToSequenceTransform ).FloorToFrame();
    if( !SubData->PlayRange.Value.Contains( InnerTime ) )
    {
        return nullptr;
    }

    UCameraComponent* CameraComponent = GetFirstCamera( InnerSequence, iInnerSequenceID, InnerTime, iPlayer );
    if( CameraComponent )
        return CameraComponent;

    for( FMovieSceneSequenceID Child : Node->Children )
    {
        CameraComponent = FindCameraCutComponentRecursive( iGlobalTime, Child, iHierarchy, iPlayer );
        if( CameraComponent )
        {
            return CameraComponent;
        }
    }

    return nullptr;
}

void
FSceneRenderer::GetViewCamera()
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if( !sequencer.IsValid() )
        return;

    //---

    const FMovieSceneSequenceHierarchyNode* root_node = sequencer->GetEvaluationTemplate().GetHierarchy()->FindNode( sequencer->GetRootTemplateID() );

    // Try to find the a camera in the root sequence at the current frame
    UCameraComponent* CameraComponent = GetFirstCamera( sequencer->GetRootMovieSceneSequence(), sequencer->GetRootTemplateID(), sequencer->GetGlobalTime().Time.FrameNumber, *sequencer );
    if( CameraComponent )
    {
        mCameraComponent = CameraComponent;
        return;
    }

    // Otherwise try to recursively find a camera in each child at the current frame
    for( FMovieSceneSequenceID Child : root_node->Children )
    {
        CameraComponent = FindCameraCutComponentRecursive( sequencer->GetGlobalTime().Time.FrameNumber, Child, *sequencer->GetEvaluationTemplate().GetHierarchy(), *sequencer );
        if( CameraComponent )
        {
            mCameraComponent = CameraComponent;
            return;
        }
    }
}

static
FIntPoint
CalculateConstraintSize( const FMinimalViewInfo& iViewInfo, const FIntPoint& iUserSize )
{
    FIntPoint constraint_size;
    float target_ratio = iUserSize.X / float( iUserSize.Y );
    if( FMath::IsNearlyEqual( iViewInfo.AspectRatio, target_ratio ) )
    {
        constraint_size = iUserSize;
    }
    else if( iViewInfo.AspectRatio < target_ratio )
    {
        constraint_size.X = iUserSize.Y * iViewInfo.AspectRatio;
        constraint_size.Y = iUserSize.Y;
    }
    else
    {
        constraint_size.X = iUserSize.X;
        constraint_size.Y = iUserSize.X / iViewInfo.AspectRatio;
    }

    return constraint_size;
}

void
FSceneRenderer::RenderToTexture( TArray<FColor>& oSamples, FTextureRenderTargetResource* oRenderTarget )
{
    if( !ensureMsgf( mCameraComponent, TEXT( "No camera found at frame %d (in tick global time" ), mSequencer.Pin()->GetGlobalTime().Time.GetFrame().Value ) )
        return;

    FMinimalViewInfo ViewInfo;
    mCameraComponent->GetCameraView( FApp::GetDeltaTime(), ViewInfo );

    FIntPoint TargetSize = mSize;
    if( TargetSize.X <= 0 || TargetSize.Y <= 0 )
        return;

    FIntPoint ConstraintSize = CalculateConstraintSize( ViewInfo, TargetSize );

    UWorld* World = mCameraComponent->GetWorld();

    //---

    FTextureRenderTargetResource* RenderTargetResource = nullptr;
    UTextureRenderTarget2D* RenderTargetTexture = nullptr;

    if( oRenderTarget )
    {
        RenderTargetResource = oRenderTarget;
    }
    else
    {
        RenderTargetTexture = NewObject<UTextureRenderTarget2D>();
        check( RenderTargetTexture );
        RenderTargetTexture->AddToRoot();
        RenderTargetTexture->ClearColor = FLinearColor::Transparent;
        //RenderTargetTexture->TargetGamma = TargetGamma;
        RenderTargetTexture->InitCustomFormat( TargetSize.X, TargetSize.Y, PF_FloatRGBA, false );
        RenderTargetResource = RenderTargetTexture->GameThread_GetRenderTargetResource();
    }

    FSceneViewFamilyContext ViewFamily(
        FSceneViewFamily::ConstructionValues( RenderTargetResource, World->Scene, FEngineShowFlags( ESFIM_Game ) )
        .SetTime( FGameTime::GetTimeSinceAppStart() )
        .SetResolveScene( true )
    );

    // To enable visualization mode
    //ViewFamily.EngineShowFlags.SetPostProcessing( true );
    //ViewFamily.EngineShowFlags.SetVisualizeBuffer( false );
    //ViewFamily.EngineShowFlags.SetTonemapper( true );
    //ViewFamily.EngineShowFlags.SetMotionBlur( false );
    ViewFamily.EngineShowFlags.SetScreenPercentage( false );

    ViewFamily.ViewMode = mViewMode;

    EngineShowFlagOverride( ESFIM_Game, ViewFamily.ViewMode, ViewFamily.EngineShowFlags, false );

    FSceneViewStateReference viewState;
    viewState.Allocate( World->Scene->GetFeatureLevel() );

    //---

    FSceneViewInitOptions ViewInitOptions;

    ViewInitOptions.bInCameraCut = true;
    ViewInitOptions.SceneViewStateInterface = viewState.GetReference();

    //ViewInitOptions.BackgroundColor = FLinearColor::Red;
    ViewInitOptions.SetViewRectangle( FIntRect( 0, 0, TargetSize.X, TargetSize.Y ) );
    FIntPoint offset( ( TargetSize.X - ConstraintSize.X ) / 2.f, ( TargetSize.Y - ConstraintSize.Y ) / 2.f );
    ViewInitOptions.SetConstrainedViewRectangle( FIntRect( offset.X + 0, offset.Y + 0, offset.X + ConstraintSize.X, offset.Y + ConstraintSize.Y ) );
    ViewInitOptions.ViewFamily = &ViewFamily;
    ViewInitOptions.ViewOrigin = ViewInfo.Location;
    ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix( ViewInfo.Rotation ) * FMatrix(
        FPlane( 0, 0, 1, 0 ),
        FPlane( 1, 0, 0, 0 ),
        FPlane( 0, 1, 0, 0 ),
        FPlane( 0, 0, 0, 1 ) );
    ViewInitOptions.ProjectionMatrix = ViewInfo.CalculateProjectionMatrix();

    // If no "show only" primitives are provided, we must pass an unset TOptional - otherwise an empty set will mean no primitive should be visible.
    //ViewInitOptions.ShowOnlyPrimitives = !ShowOnlyPrimitives.IsEmpty() ? TOptional<TSet<FPrimitiveComponentId>>( ShowOnlyPrimitives ) : TOptional<TSet<FPrimitiveComponentId>>();
    //ViewInitOptions.HiddenPrimitives = HiddenPrimitives;

    //---

    FSceneView* NewView = new FSceneView( ViewInitOptions );
    //NewView->CurrentBufferVisualizationMode = VisualizationMode;
    ViewFamily.Views.Add( NewView );

    NewView->StartFinalPostprocessSettings( ViewInitOptions.ViewOrigin );
    NewView->EndFinalPostprocessSettings( ViewInitOptions );

    ViewFamily.SetScreenPercentageInterface( new FLegacyScreenPercentageDriver(
        ViewFamily, /* GlobalResolutionFraction = */ 1.0f ) );
    //ViewFamily.SetScreenPercentageInterface( new FLegacyScreenPercentageDriver( ViewFamily, GlobalResolutionFraction ) );

    FCanvas Canvas( RenderTargetResource, nullptr, FGameTime::GetTimeSinceAppStart(), World->Scene->GetFeatureLevel() );
    Canvas.Clear( FLinearColor::Transparent );

    ViewFamily.ViewExtensions = GEngine->ViewExtensions->GatherActiveExtensions( FSceneViewExtensionContext( World->Scene ) );
    for( const FSceneViewExtensionRef& Extension : ViewFamily.ViewExtensions )
    {
        Extension->SetupViewFamily( ViewFamily );
        Extension->SetupView( ViewFamily, *NewView );
    }

    GetRendererModule().BeginRenderingViewFamily( &Canvas, &ViewFamily );
    //PerformSceneRender( Canvas, ViewFamily, bPerformWarmpup );

    //---

    if( !oRenderTarget )
    {
        // Copy the contents of the remote texture to system memory
        oSamples.SetNumUninitialized( TargetSize.X * TargetSize.Y );
        FReadSurfaceDataFlags ReadSurfaceDataFlags;
        ReadSurfaceDataFlags.SetLinearToGamma( true );
        RenderTargetResource->ReadPixels( oSamples, ReadSurfaceDataFlags, FIntRect( 0, 0, TargetSize.X, TargetSize.Y ) );
    }

    if( RenderTargetTexture )
    {
        RenderTargetTexture->RemoveFromRoot();
        RenderTargetTexture = nullptr;
    }
}

//---

#undef LOCTEXT_NAMESPACE
