// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "SingleCameraCutTrack/TrackEditorKeyThumbnail.h"

#include "Modules/ModuleManager.h"
#include "RHI.h"
#include "RenderingThread.h"
#include "ShowFlags.h"
#include "RHIStaticStates.h"
#include "RendererInterface.h"
#include "Shader.h"
#include "StaticBoundShaderState.h"
#include "EditorViewportClient.h"
#include "LevelEditorViewport.h"
#include "Slate/SceneViewport.h"
#include "GlobalShader.h"
#include "ScreenRendering.h"
#include "TrackEditorThumbnail/TrackEditorThumbnailPool.h"
#include "PipelineStateCache.h"
#include "CommonRenderResources.h"
#include "LegacyScreenPercentageDriver.h"
#include "CanvasTypes.h"
#include "EngineModule.h"
#include "MovieSceneTimeHelpers.h"


FTrackEditorKeyThumbnailCache::FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& InThumbnailPool, IViewportThumbnailClient* InViewportThumbnailClient )
    : FTrackEditorThumbnailCache( InThumbnailPool, InViewportThumbnailClient )
    //: ViewportThumbnailClient( InViewportThumbnailClient )
    //, CustomThumbnailClient( nullptr )
    //, ThumbnailPool( InThumbnailPool )
{
    //check( ViewportThumbnailClient );

    //LastComputationTime = 0;
    //bForceRedraw = false;
    //bNeedsNewThumbnails = false;
}


FTrackEditorKeyThumbnailCache::FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& InThumbnailPool, ICustomThumbnailClient* InCustomThumbnailClient )
    : FTrackEditorThumbnailCache( InThumbnailPool, InCustomThumbnailClient )
    //: ViewportThumbnailClient( nullptr )
    //, CustomThumbnailClient( InCustomThumbnailClient )
    //, ThumbnailPool( InThumbnailPool )
{
    //check( CustomThumbnailClient );

    //LastComputationTime = 0;
    //bForceRedraw = false;
    //bNeedsNewThumbnails = false;
}


FTrackEditorKeyThumbnailCache::~FTrackEditorKeyThumbnailCache()
{
    //TSharedPtr<FTrackEditorThumbnailPool> PinnedPool = ThumbnailPool.Pin();
    //if( PinnedPool.IsValid() )
    //{
    //    PinnedPool->RemoveThumbnailsNeedingRedraw( Thumbnails );
    //}
}


//void FTrackEditorKeyThumbnailCache::SetSingleReferenceFrame( TOptional<double> InReferenceFrame )
//{
//    CurrentCache.SingleReferenceFrame = InReferenceFrame;
//}

void FTrackEditorKeyThumbnailCache::Update( const TRange<double>& NewRange, const TRange<double>& VisibleRange, TArray<double> Keys, const FIntPoint& AllottedSize, const FIntPoint& InDesiredSize, EThumbnailQuality Quality, double InCurrentTime )
{
    PreviousCache.TimeRange = CurrentCache.TimeRange;
    PreviousCache.VisibleRange = CurrentCache.VisibleRange;
    PreviousCache.AllottedSize = CurrentCache.AllottedSize;
    PreviousCache.DesiredSize = CurrentCache.DesiredSize;
    PreviousCache.Quality = CurrentCache.Quality;

    PreviousCacheKey.Keys= CurrentCacheKey.Keys;

    CurrentCache.TimeRange = NewRange;
    CurrentCache.VisibleRange = VisibleRange;
    CurrentCache.AllottedSize = AllottedSize;
    CurrentCache.DesiredSize = InDesiredSize;
    CurrentCache.Quality = Quality;

    CurrentCacheKey.Keys = Keys;

    Revalidate( InCurrentTime );

    // Only update the single reference frame value once we've updated, since that can get set at any time, but Update() may be throttled
    PreviousCache.SingleReferenceFrame = CurrentCache.SingleReferenceFrame;
}


//FIntPoint FTrackEditorKeyThumbnailCache::CalculateTextureSize( const FMinimalViewInfo& ViewInfo ) const
//{
//    float DesiredRatio = ViewInfo.AspectRatio;
//
//    if( CurrentCache.DesiredSize.X <= 0 || CurrentCache.DesiredSize.Y <= 0 )
//    {
//        return FIntPoint( 0, 0 );
//    }
//
//    float SizeRatio = float( CurrentCache.DesiredSize.X ) / CurrentCache.DesiredSize.Y;
//
//    float X = CurrentCache.DesiredSize.X;
//    float Y = CurrentCache.DesiredSize.Y;
//
//    if( SizeRatio > DesiredRatio )
//    {
//        // Take width
//        Y = CurrentCache.DesiredSize.X / DesiredRatio;
//    }
//    else if( SizeRatio < DesiredRatio )
//    {
//        // Take height
//        X = CurrentCache.DesiredSize.Y * DesiredRatio;
//    }
//
//    float Scale;
//    switch( CurrentCache.Quality )
//    {
//        case EThumbnailQuality::Draft:    Scale = 0.5f;   break;
//        case EThumbnailQuality::Best:     Scale = 2.f;    break;
//        default:                          Scale = 1.f;    break;
//    }
//
//    return FIntPoint(
//        FMath::RoundToInt( X * Scale ),
//        FMath::RoundToInt( Y * Scale )
//    );
//}

bool FTrackEditorKeyThumbnailCache::ShouldRegenerateEverything() const
{
    //if( bForceRedraw )
    //{
    //    return true;
    //}

    //const float PreviousScale = PreviousCache.TimeRange.Size<float>() / PreviousCache.AllottedSize.X;
    //const float CurrentScale = CurrentCache.TimeRange.Size<float>() / CurrentCache.AllottedSize.X;
    //const float Threshold = PreviousScale * 0.01f;

    //return PreviousCache.DesiredSize != CurrentCache.DesiredSize || !FMath::IsNearlyEqual( PreviousScale, CurrentScale, Threshold );

    return FTrackEditorThumbnailCache::ShouldRegenerateEverything() || CurrentCacheKey != PreviousCacheKey;
}

//void FTrackEditorKeyThumbnailCache::DrawThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail )
//{
//    if( CustomThumbnailClient )
//    {
//        CustomThumbnailClient->Draw( TrackEditorThumbnail );
//    }
//    else if( ViewportThumbnailClient )
//    {
//        ViewportThumbnailClient->PreDraw( TrackEditorThumbnail );
//
//        DrawViewportThumbnail( TrackEditorThumbnail );
//
//        ViewportThumbnailClient->PostDraw( TrackEditorThumbnail );
//    }
//
//    FSlateTextureRenderTarget2DResource* pSlateResource = TrackEditorThumbnail.GetRenderTarget();
//    if( pSlateResource != nullptr )
//    {
//        FThreadSafeBool* bHasFinishedDrawingPtr = &TrackEditorThumbnail.bHasFinishedDrawing;
//        ENQUEUE_RENDER_COMMAND( SetFinishedDrawing )(
//            [bHasFinishedDrawingPtr, pSlateResource]( FRHICommandList& RHICmdList )
//        {
//            FRHITexture* InTexture = pSlateResource->GetTextureRHI();
//            RHICmdList.Transition( FRHITransitionInfo( InTexture, ERHIAccess::RTV, ERHIAccess::SRVMask ) );
//            *bHasFinishedDrawingPtr = true;
//        }
//        );
//    }
//}
//void FTrackEditorKeyThumbnailCache::DrawViewportThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail )
//{
//    check( ViewportThumbnailClient );
//
//    UCameraComponent* PreviewCameraComponent = ViewportThumbnailClient->GetViewCamera();
//    if( !PreviewCameraComponent )
//    {
//        return;
//    }
//
//    FMinimalViewInfo ViewInfo;
//    PreviewCameraComponent->GetCameraView( FApp::GetDeltaTime(), ViewInfo );
//
//    FIntPoint RTSize = CalculateTextureSize( ViewInfo );
//    if( RTSize.X <= 0 || RTSize.Y <= 0 )
//    {
//        return;
//    }
//
//    TrackEditorThumbnail.bIgnoreAlpha = true;
//    TrackEditorThumbnail.ResizeRenderTarget( RTSize );
//
//    UWorld* World = PreviewCameraComponent->GetWorld();
//
//    FSceneViewFamilyContext ViewFamily( FSceneViewFamily::ConstructionValues( TrackEditorThumbnail.GetRenderTarget(), World->Scene, FEngineShowFlags( ESFIM_Game ) )
//                                        .SetWorldTimes( FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime )
//                                        .SetResolveScene( true ) );
//
//    // Screen percentage is not supported in thumbnail.
//    ViewFamily.EngineShowFlags.ScreenPercentage = false;
//
//    switch( CurrentCache.Quality )
//    {
//        case EThumbnailQuality::Draft:
//            ViewFamily.EngineShowFlags.DisableAdvancedFeatures();
//            ViewFamily.EngineShowFlags.SetPostProcessing( false );
//            break;
//
//        case EThumbnailQuality::Normal:
//        case EThumbnailQuality::Best:
//            ViewFamily.EngineShowFlags.SetMotionBlur( false );
//            break;
//    }
//
//    FSceneViewInitOptions ViewInitOptions;
//
//    ViewInitOptions.BackgroundColor = FLinearColor::Black;
//    ViewInitOptions.SetViewRectangle( FIntRect( FIntPoint::ZeroValue, RTSize ) );
//    ViewInitOptions.ViewFamily = &ViewFamily;
//
//    ViewInitOptions.ViewOrigin = ViewInfo.Location;
//    ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix( ViewInfo.Rotation ) * FMatrix(
//        FPlane( 0, 0, 1, 0 ),
//        FPlane( 1, 0, 0, 0 ),
//        FPlane( 0, 1, 0, 0 ),
//        FPlane( 0, 0, 0, 1 ) );
//
//    ViewInitOptions.ProjectionMatrix = ViewInfo.CalculateProjectionMatrix();
//
//    FSceneView* NewView = new FSceneView( ViewInitOptions );
//    ViewFamily.Views.Add( NewView );
//
//    const float GlobalResolutionFraction = 1.f;
//    const bool  AllowPostProcessSettingsScreenPercentage = false;
//    ViewFamily.SetScreenPercentageInterface( new FLegacyScreenPercentageDriver( ViewFamily, GlobalResolutionFraction, AllowPostProcessSettingsScreenPercentage ) );
//
//    FCanvas Canvas( TrackEditorThumbnail.GetRenderTarget(), nullptr, FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime, World->Scene->GetFeatureLevel() );
//    Canvas.Clear( FLinearColor::Transparent );
//
//    GetRendererModule().BeginRenderingViewFamily( &Canvas, &ViewFamily );
//}
//

void FTrackEditorKeyThumbnailCache::Revalidate( double InCurrentTime )
{
    if( CurrentCache == PreviousCache && CurrentCacheKey == PreviousCacheKey && !bForceRedraw && !bNeedsNewThumbnails )
    {
        return;
    }

    if( FMath::IsNearlyZero( CurrentCache.TimeRange.Size<float>() ) || CurrentCache.TimeRange.IsEmpty() )
    {
        // Can't generate thumbnails for this
        ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Thumbnails );
        ThumbnailsNeedingRedraw.Reset();
        Thumbnails.Reset();
        bNeedsNewThumbnails = false;
        return;
    }

    bNeedsNewThumbnails = true;

    if( ShouldRegenerateEverything() )
    {
        ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Thumbnails );
        Thumbnails.Reset();
    }

    if( InCurrentTime - LastComputationTime > 0.25f )
    {
        ComputeNewThumbnails();
        LastComputationTime = InCurrentTime;
    }
}

void FTrackEditorKeyThumbnailCache::ComputeNewThumbnails()
{
    ThumbnailsNeedingRedraw.Reset();

    if( CurrentCache.SingleReferenceFrame.IsSet() )
    {
        if( !Thumbnails.Num() || bForceRedraw || CurrentCache.SingleReferenceFrame != PreviousCache.SingleReferenceFrame )
        {
            UpdateSingleThumbnail();
        }
    }
    else
    {
        UpdateFilledThumbnails();
    }

    if( ThumbnailsNeedingRedraw.Num() )
    {
        ThumbnailPool.Pin()->AddThumbnailsNeedingRedraw( ThumbnailsNeedingRedraw );
    }
    if( Thumbnails.Num() )
    {
        Setup();
    }

    bForceRedraw = false;
    bNeedsNewThumbnails = false;
}


void FTrackEditorKeyThumbnailCache::UpdateSingleThumbnail()
{
    Thumbnails.Reset();

    const double TimePerPx = CurrentCache.TimeRange.Size<double>() / CurrentCache.AllottedSize.X;
    const double HalfRange = CurrentCache.DesiredSize.X*TimePerPx*.5;
    const double EvalPosition = CurrentCache.SingleReferenceFrame.GetValue();

    TSharedPtr<FTrackEditorThumbnail> NewThumbnail = MakeShareable( new FTrackEditorThumbnail(
        FOnThumbnailDraw::CreateRaw( this, &FTrackEditorKeyThumbnailCache::DrawThumbnail ),
        CurrentCache.DesiredSize,
        TRange<double>( EvalPosition - HalfRange, EvalPosition + HalfRange ),
        EvalPosition
    ) );

    Thumbnails.Add( NewThumbnail );
    ThumbnailsNeedingRedraw.Add( NewThumbnail );
}


void FTrackEditorKeyThumbnailCache::UpdateFilledThumbnails()
{
    // Always remove the last one, it it's the only one which may be placed with a shift position
    if( Thumbnails.Num() )
    {
        TArray<TSharedPtr<FTrackEditorThumbnail>> Remove;
        Remove.Add( Thumbnails.Last() );
        ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Remove );

        Thumbnails.RemoveAt( Thumbnails.Num() - 1, 1, false );
    }

    // Remove any thumbnails from the front of the array that aren't in the actual time range of this section (we keep stuff around outside of the visible range)
    {
        int32 Index = 0;
        for( ; Index < Thumbnails.Num(); ++Index )
        {
            if( Thumbnails[Index]->GetTimeRange().Overlaps( CurrentCache.TimeRange ) )
            {
                break;
            }
        }
        if( Index )
        {
            TArray<TSharedPtr<FTrackEditorThumbnail>> Remove;
            Remove.Append( &Thumbnails[0], Index );
            ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Remove );

            Thumbnails.RemoveAt( 0, Index, false );
        }
    }

    // Remove any thumbnails from the back of the array that aren't in the *actual* time range of this section (we keep stuff around outside of the visible range)
    {
        int32 NumToRemove = 0;
        for( int32 Index = Thumbnails.Num() - 1; Index >= 0; --Index )
        {
            if( !Thumbnails[Index]->GetTimeRange().Overlaps( CurrentCache.TimeRange ) )
            {
                ++NumToRemove;
            }
            else
            {
                break;
            }
        }

        if( NumToRemove )
        {
            TArray<TSharedPtr<FTrackEditorThumbnail>> Remove;
            Remove.Append( &Thumbnails[Thumbnails.Num() - NumToRemove], NumToRemove );
            ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Remove );

            Thumbnails.RemoveAt( Thumbnails.Num() - NumToRemove, NumToRemove, false );
        }
    }

    TRange<double> MaxRange( CurrentCache.VisibleRange.GetLowerBoundValue() - CurrentCache.VisibleRange.Size<double>(), CurrentCache.VisibleRange.GetUpperBoundValue() + CurrentCache.VisibleRange.Size<double>() );
    TRange<double> Boundary = TRange<double>::Intersection( CurrentCache.TimeRange, MaxRange );

    if( !Boundary.IsEmpty() )
    {
        GenerateFront( Boundary );
        GenerateBack( Boundary );
    }

    if( Thumbnails.Num() )
    {
        for( const TSharedPtr<FTrackEditorThumbnail>& Thumbnail : Thumbnails )
        {
            Thumbnail->SortOrder = Thumbnail->GetTimeRange().Overlaps( CurrentCache.VisibleRange ) ? 1 : 10;
        }
    }
}


void FTrackEditorKeyThumbnailCache::GenerateFront( const TRange<double>& Boundary )
{
    if( !Thumbnails.Num() )
        return;

    const double TimePerPx = CurrentCache.TimeRange.Size<double>() / CurrentCache.AllottedSize.X;
    double EndTime = Thumbnails[0]->GetTimeRange().GetLowerBoundValue();
    TRange<double> effective_boundary( Boundary.GetLowerBoundValue(), EndTime );

    for( auto key : CurrentCacheKey.Keys )
    {
        if( !effective_boundary.Contains( key ) && !FMath::IsNearlyEqual( effective_boundary.GetLowerBoundValue(), key ) && !FMath::IsNearlyEqual( effective_boundary.GetUpperBoundValue(), key ) )
            continue;

        FIntPoint TextureSize = CurrentCache.DesiredSize;

        TRange<double> TimeRange( key, key + TextureSize.X * TimePerPx );

        TSharedPtr<FTrackEditorThumbnail> NewThumbnail = MakeShareable( new FTrackEditorThumbnail(
            FOnThumbnailDraw::CreateRaw( this, &FTrackEditorKeyThumbnailCache::DrawThumbnail ),
            TextureSize,
            TimeRange,
            key
            //EvalPosition
        ) );

        Thumbnails.Insert( NewThumbnail, 0 );
        ThumbnailsNeedingRedraw.Add( NewThumbnail );
    }
}


void FTrackEditorKeyThumbnailCache::GenerateBack( const TRange<double>& Boundary )
{
    const double TimePerPx = CurrentCache.TimeRange.Size<double>() / CurrentCache.AllottedSize.X;
    double StartTime = Thumbnails.Num() ? Thumbnails.Last()->GetTimeRange().GetUpperBoundValue() : Boundary.GetLowerBoundValue();
    TRange<double> effective_boundary( StartTime, Boundary.GetUpperBoundValue() );

    for( auto key : CurrentCacheKey.Keys )
    {
        if( !effective_boundary.Contains( key ) && !FMath::IsNearlyEqual( effective_boundary.GetLowerBoundValue(), key ) && !FMath::IsNearlyEqual( effective_boundary.GetUpperBoundValue(), key ) )
            continue;

        if( FMath::IsNearlyEqual( effective_boundary.GetUpperBoundValue(), key ) )
            key -= DELTA;

        FIntPoint TextureSize = CurrentCache.DesiredSize;

        TRange<double> TimeRange( key, key + TextureSize.X * TimePerPx );
        // If already thumbnails, the last one is shifted
        if( Thumbnails.Num() > 0 && !CurrentCache.TimeRange.Contains( TimeRange ) )
        {
            TimeRange = UE::MovieScene::TranslateRange( TimeRange, -( TimeRange.GetUpperBoundValue() - CurrentCache.TimeRange.GetUpperBoundValue() ) );
        }
        else
        {
            // Move the thumbnail to the center of the space if we're the only thumbnail, and we don't fit on
            double Overflow = TextureSize.X*TimePerPx - CurrentCache.TimeRange.Size<double>();
            if( Thumbnails.Num() == 0 && Overflow > 0 )
            {
                TimeRange = UE::MovieScene::TranslateRange( TimeRange, -( Overflow * .5f ) );
            }
        }

        TSharedPtr<FTrackEditorThumbnail> NewThumbnail = MakeShareable( new FTrackEditorThumbnail(
            FOnThumbnailDraw::CreateRaw( this, &FTrackEditorKeyThumbnailCache::DrawThumbnail ),
            TextureSize,
            TimeRange,
            key
            //EvalPosition
        ) );

        NewThumbnail->SortOrder = TimeRange.Overlaps( CurrentCache.VisibleRange ) ? 1 : 10;

        Thumbnails.Add( NewThumbnail );
        ThumbnailsNeedingRedraw.Add( NewThumbnail );
    }
}


//void FTrackEditorKeyThumbnailCache::Setup()
//{
//    if( CustomThumbnailClient )
//    {
//        CustomThumbnailClient->Setup();
//    }
//}
