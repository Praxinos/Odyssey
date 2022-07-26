// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

FSceneRenderer::FSceneRenderer( TWeakPtr<ISequencer> iSequencer, const FExportPanel* iPanel, const FIntPoint& iSize )
    : mSequencer( iSequencer )
    , mCurrentPanel( iPanel )
    , mSize( iSize )
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
    sequencer->SetLocalTimeDirectly( mCurrentPanel->GlobalFrame );
    //sequencer->SetLocalTimeDirectly( mCurrentPanel->GlobalFrame * sequencer->GetLocalTime().Rate );
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
    UMovieScene* InnerMovieScene = InnerSequence ? InnerSequence->GetMovieScene() : nullptr;
    if( !InnerMovieScene )
    {
        return nullptr;
    }

    FFrameNumber InnerTime = ( iGlobalTime * SubData->RootToSequenceTransform ).FloorToFrame();
    if( !SubData->PlayRange.Value.Contains( InnerTime ) )
    {
        return nullptr;
    }

    int32 LowestRow = TNumericLimits<int32>::Max();
    int32 HighestOverlap = 0;

    UMovieSceneSingleCameraCutSection* ActiveSection = nullptr;

    if( UMovieSceneSingleCameraCutTrack* CutTrack = Cast<UMovieSceneSingleCameraCutTrack>( InnerMovieScene->GetCameraCutTrack() ) )
    {
        for( UMovieSceneSection* ItSection : CutTrack->GetAllSections() )
        {
            UMovieSceneSingleCameraCutSection* CutSection = Cast<UMovieSceneSingleCameraCutSection>( ItSection );
            if( CutSection && CutSection->GetRange().Contains( InnerTime ) )
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

    if( ActiveSection )
    {
        return ActiveSection->GetFirstCamera( iPlayer, iInnerSequenceID );
    }

    for( FMovieSceneSequenceID Child : Node->Children )
    {
        UCameraComponent* CameraComponent = FindCameraCutComponentRecursive( iGlobalTime, Child, iHierarchy, iPlayer );
        if( CameraComponent )
        {
            return CameraComponent;
        }
    }

    return nullptr;
}

static
const UMovieSceneCinematicBoardSection*
GetCurrentBoardSection( ISequencer* iSequencer )
{
    UMovieSceneCinematicBoardTrack* board_track = BoardSequenceTools::FindCinematicBoardTrack( iSequencer );
    if( !board_track )
        return nullptr;

    UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( board_track->GetAllSections(), iSequencer->GetGlobalTime().Time.GetFrame() );

    return Cast<UMovieSceneCinematicBoardSection>( section );
}

static
const UMovieSceneSingleCameraCutSection*
GetCurrentCameraCutSection( ISequencer* iSequencer )
{
    UMovieSceneSequence* sequence = iSequencer->GetFocusedMovieSceneSequence();
    if( !sequence )
        return nullptr;

    UMovieScene* focusedMovieScene = sequence->GetMovieScene();
    if( !focusedMovieScene )
        return nullptr;

    UMovieSceneSingleCameraCutTrack* cameracutTrack = Cast<UMovieSceneSingleCameraCutTrack>( focusedMovieScene->GetCameraCutTrack() );
    //UMovieSceneSingleCameraCutTrack* cameracutTrack = focusedMovieScene->FindMasterTrack<UMovieSceneSingleCameraCutTrack>();
    if( !cameracutTrack )
        return nullptr;

    UMovieSceneSection* section = MovieSceneHelpers::FindSectionAtTime( cameracutTrack->GetAllSections(), iSequencer->GetGlobalTime().Time.GetFrame() );

    return Cast<UMovieSceneSingleCameraCutSection>( section );
}

void
FSceneRenderer::GetViewCamera()
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if( !sequencer.IsValid() )
        return;

    //---

    const UMovieSceneCinematicBoardSection* board_section = GetCurrentBoardSection( sequencer.Get() );
    if( board_section )
    {
        const FMovieSceneSequenceID             ThisSequenceID = sequencer->GetFocusedTemplateID();
        const FMovieSceneSequenceID             TargetSequenceID = board_section->GetSequenceID();
        const FMovieSceneSequenceHierarchy*     Hierarchy = sequencer->GetEvaluationTemplate().GetCompiledDataManager()->FindHierarchy( sequencer->GetEvaluationTemplate().GetCompiledDataID() );

        if( !Hierarchy )
            return;

        const FMovieSceneSequenceHierarchyNode* ThisSequenceNode = Hierarchy->FindNode( ThisSequenceID );

        check( ThisSequenceNode );

        // Find the TargetSequenceID by comparing deterministic sequence IDs for all children of the current node
        const FMovieSceneSequenceID* InnerSequenceID = Algo::FindByPredicate( ThisSequenceNode->Children,
                                                                              [Hierarchy, TargetSequenceID]( FMovieSceneSequenceID InSequenceID )
                                                                              {
                                                                                  const FMovieSceneSubSequenceData* SubData = Hierarchy->FindSubData( InSequenceID );
                                                                                  return SubData && SubData->DeterministicSequenceID == TargetSequenceID;
                                                                              }
        );

        if( InnerSequenceID )
        {
            mCameraComponent = FindCameraCutComponentRecursive( sequencer->GetGlobalTime().Time.FrameNumber, *InnerSequenceID, *Hierarchy, *sequencer );
            if( mCameraComponent )
                return;
        }
    }

    //---

    const UMovieSceneSingleCameraCutSection* cameracut_section = GetCurrentCameraCutSection( sequencer.Get() );
    if( cameracut_section )
    {
        if( cameracut_section && sequencer.IsValid() )
        {
            mCameraComponent = cameracut_section->GetFirstCamera( *sequencer, sequencer->GetFocusedTemplateID() );
            if( mCameraComponent )
                return;

            //FMovieSceneSpawnable* Spawnable = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene()->FindSpawnable( CameraCutSection->GetCameraBindingID().GetGuid() );
            //if( Spawnable )
            //{
            //    return Cast<AActor>( Spawnable->GetObjectTemplate() );
            //}
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
    GetViewCamera();
    if( !mCameraComponent )
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
    ViewFamily.EngineShowFlags.SetPostProcessing( true );
    ViewFamily.EngineShowFlags.SetVisualizeBuffer( true );
    //ViewFamily.EngineShowFlags.SetTonemapper( false );
    ViewFamily.EngineShowFlags.SetScreenPercentage( false );

    ViewFamily.EngineShowFlags.SetMotionBlur( false );

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
        ReadSurfaceDataFlags.SetLinearToGamma( false );
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
