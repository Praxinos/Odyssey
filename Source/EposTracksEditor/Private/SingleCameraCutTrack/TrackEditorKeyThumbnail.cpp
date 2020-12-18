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
    , CameraViewRatio( 1.f )
{
}


FTrackEditorKeyThumbnailCache::FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& InThumbnailPool, ICustomThumbnailClient* InCustomThumbnailClient )
    : FTrackEditorThumbnailCache( InThumbnailPool, InCustomThumbnailClient )
    , CameraViewRatio( 1.f )
{
}


FTrackEditorKeyThumbnailCache::~FTrackEditorKeyThumbnailCache()
{
}

void FTrackEditorKeyThumbnailCache::ForceRedraw()
{
    Super::ForceRedraw();
}

void FTrackEditorKeyThumbnailCache::SetSingleReferenceFrame( TOptional<double> InReferenceFrame )
{
    Super::SetSingleReferenceFrame( InReferenceFrame );
}

TOptional<double> FTrackEditorKeyThumbnailCache::GetSingleReferenceFrame() const
{
    return Super::GetSingleReferenceFrame();
}

const TArray<TSharedPtr<FTrackEditorThumbnail>>& FTrackEditorKeyThumbnailCache::GetThumbnails() const
{
    return Super::GetThumbnails();
}


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


FIntPoint FTrackEditorKeyThumbnailCache::CalculateTextureSize( const FMinimalViewInfo& ViewInfo ) const
{
    return Super::CalculateTextureSize( ViewInfo );
}

void FTrackEditorKeyThumbnailCache::DrawThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail )
{
    Super::DrawThumbnail( TrackEditorThumbnail );
}
void FTrackEditorKeyThumbnailCache::DrawViewportThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail )
{
    Super::DrawViewportThumbnail( TrackEditorThumbnail );
}

bool FTrackEditorKeyThumbnailCache::ShouldRegenerateEverything() const
{
    return Super::ShouldRegenerateEverything() || CurrentCacheKey != PreviousCacheKey;
}


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
    UCameraComponent* PreviewCameraComponent = ViewportThumbnailClient->GetViewCamera();
    if( !PreviewCameraComponent )
    {
        return;
    }

    FMinimalViewInfo ViewInfo;
    PreviewCameraComponent->GetCameraView( FApp::GetDeltaTime(), ViewInfo );

    CameraViewRatio = ViewInfo.AspectRatio;

    //---

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
        Generate( Boundary );
    }

    if( Thumbnails.Num() )
    {
        for( const TSharedPtr<FTrackEditorThumbnail>& Thumbnail : Thumbnails )
        {
            Thumbnail->SortOrder = Thumbnail->GetTimeRange().Overlaps( CurrentCache.VisibleRange ) ? 1 : 10;
        }
    }
}


void FTrackEditorKeyThumbnailCache::Generate( const TRange<double>& Boundary )
{
    const double TimePerPx = CurrentCache.TimeRange.Size<double>() / CurrentCache.AllottedSize.X;

    for( int i = 0; i < CurrentCacheKey.Keys.Num(); i++ )
    {
        double key = CurrentCacheKey.Keys[i];

        if( !Boundary.Contains( key ) && !FMath::IsNearlyEqual( Boundary.GetLowerBoundValue(), key ) && !FMath::IsNearlyEqual( Boundary.GetUpperBoundValue(), key ) )
            continue;

        //PATCH
        // For the last key (The one at the end of the section)
        // It needs to be shifted to be considered inside the section by the sequencer
        if( i == CurrentCacheKey.Keys.Num() - 1 )
            key -= DELTA;

        auto already_in = [&]( const TSharedPtr<FTrackEditorThumbnail>& iThumbnail ) { return FMath::IsNearlyEqual( iThumbnail->GetEvalPosition(), key ); };
        if( Thumbnails.ContainsByPredicate( already_in ) )
            continue;

        FIntPoint TextureSize = CurrentCache.DesiredSize;
        TextureSize.X = FMath::Min( TextureSize.X, int( CurrentCache.DesiredSize.Y * CameraViewRatio ) );

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
        ) );

        NewThumbnail->SortOrder = TimeRange.Overlaps( CurrentCache.VisibleRange ) ? 1 : 10;

        Thumbnails.Add( NewThumbnail );
        ThumbnailsNeedingRedraw.Add( NewThumbnail );
    }
}


void FTrackEditorKeyThumbnailCache::Setup()
{
    Super::Setup();
}
