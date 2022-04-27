// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "KeyThumbnail/TrackEditorKeyThumbnail.h"

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
    //UCameraComponent* PreviewCameraComponent = ViewportThumbnailClient->GetViewCamera();
    //if( !PreviewCameraComponent )
    //{
    //    return;
    //}

    //FMinimalViewInfo ViewInfo;
    //PreviewCameraComponent->GetCameraView( FApp::GetDeltaTime(), ViewInfo );

    //CameraViewRatio = ViewInfo.AspectRatio;
    CameraViewRatio = 1.777777f;

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

    //UE_LOG( LogTemp, Warning, TEXT( "Thumbnails before remove" ) );
    //for( auto thumbnail : Thumbnails )
    //    UE_LOG( LogTemp, Warning, TEXT( "key: %f - timerange: [%f-%f[ - texturesize: %d %d" ), thumbnail->GetEvalPosition(), thumbnail->GetTimeRange().GetLowerBoundValue(), thumbnail->GetTimeRange().GetUpperBoundValue(), thumbnail->GetDesiredSize().X, thumbnail->GetDesiredSize().Y );
    //UE_LOG( LogTemp, Warning, TEXT( "---" ) );

    TRange<double> MaxRange( CurrentCache.VisibleRange.GetLowerBoundValue() - CurrentCache.VisibleRange.Size<double>(), CurrentCache.VisibleRange.GetUpperBoundValue() + CurrentCache.VisibleRange.Size<double>() );
    TRange<double> Boundary = TRange<double>::Intersection( CurrentCache.TimeRange, MaxRange );

    TArray<FThumbnailData> thumbnail_data = GenerateData( Boundary );

    TArray<FThumbnailData> thumbnail_to_create_data;
    for( auto data : thumbnail_data )
    {
        if( data.mRemoved )
        {
            auto key_match = [&]( const TSharedPtr<FTrackEditorThumbnail>& iThumbnail )
            {
                return FMath::IsNearlyEqual( iThumbnail->GetEvalPosition(), data.mPosition );
            };
            TSharedPtr<FTrackEditorThumbnail>* thumbnail_key_match_ptr = Thumbnails.FindByPredicate( key_match );
            TSharedPtr<FTrackEditorThumbnail> thumbnail_key_match = thumbnail_key_match_ptr ? *thumbnail_key_match_ptr : nullptr;

            if( thumbnail_key_match )
            {
                TArray<TSharedPtr<FTrackEditorThumbnail>> Remove;
                Remove.Add( thumbnail_key_match );
                ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Remove );

                Thumbnails.Remove( thumbnail_key_match );
            }

            continue;
        }

        auto exact_match = [&]( const TSharedPtr<FTrackEditorThumbnail>& iThumbnail )
        {
            return FMath::IsNearlyEqual( iThumbnail->GetEvalPosition(), data.mPosition )
                && FMath::IsNearlyEqual( iThumbnail->GetTimeRange().GetLowerBoundValue(), data.mTimeRange.GetLowerBoundValue() )
                //&& FMath::IsNearlyEqual( iThumbnail->GetTimeRange().GetUpperBoundValue(), data.mTimeRange.GetUpperBoundValue() ) // This value fluctuate for same range (due to TimePerPx computation)
                && iThumbnail->GetDesiredSize() == data.mDesiredSize;
        };
        auto key_match = [&]( const TSharedPtr<FTrackEditorThumbnail>& iThumbnail )
        {
            return FMath::IsNearlyEqual( iThumbnail->GetEvalPosition(), data.mPosition )
                && iThumbnail->GetDesiredSize() == data.mDesiredSize;
        };

        TSharedPtr<FTrackEditorThumbnail>* thumbnail_exact_match_ptr = Thumbnails.FindByPredicate( exact_match );
        TSharedPtr<FTrackEditorThumbnail>* thumbnail_key_match_ptr = Thumbnails.FindByPredicate( key_match );

        TSharedPtr<FTrackEditorThumbnail> thumbnail_exact_match = thumbnail_exact_match_ptr ? *thumbnail_exact_match_ptr : nullptr;
        TSharedPtr<FTrackEditorThumbnail> thumbnail_key_match = thumbnail_key_match_ptr ? *thumbnail_key_match_ptr : nullptr;
        if( !thumbnail_exact_match )
        {
            thumbnail_to_create_data.Add( data );

            if( thumbnail_key_match )
            {
                TArray<TSharedPtr<FTrackEditorThumbnail>> Remove;
                Remove.Add( thumbnail_key_match );
                ThumbnailPool.Pin()->RemoveThumbnailsNeedingRedraw( Remove );

                Thumbnails.Remove( thumbnail_key_match );
            }
        }
    }

    //UE_LOG( LogTemp, Warning, TEXT( "Thumbnails after remove" ) );
    //for( auto thumbnail : Thumbnails )
    //    UE_LOG( LogTemp, Warning, TEXT( "key: %f - timerange: [%f-%f[ - texturesize: %d %d" ), thumbnail->GetEvalPosition(), thumbnail->GetTimeRange().GetLowerBoundValue(), thumbnail->GetTimeRange().GetUpperBoundValue(), thumbnail->GetDesiredSize().X, thumbnail->GetDesiredSize().Y );
    //UE_LOG( LogTemp, Warning, TEXT( "---" ) );

    if( !Boundary.IsEmpty() )
    {
        GenerateThumbnails( Boundary, thumbnail_to_create_data );
    }

    if( Thumbnails.Num() )
    {
        for( const TSharedPtr<FTrackEditorThumbnail>& Thumbnail : Thumbnails )
        {
            Thumbnail->SortOrder = Thumbnail->GetTimeRange().Overlaps( CurrentCache.VisibleRange ) ? 1 : 10;
        }
    }
}

TArray<FTrackEditorKeyThumbnailCache::FThumbnailData>
FTrackEditorKeyThumbnailCache::GenerateData( const TRange<double>& Boundary )
{
    TArray<FThumbnailData> data;

    const double TimePerPx = CurrentCache.TimeRange.Size<double>() / CurrentCache.AllottedSize.X;

    //UE_LOG( LogTemp, Warning, TEXT( "GenerateData()" ) );
    //UE_LOG( LogTemp, Warning, TEXT( "timeperpx: %f" ), TimePerPx );

    for( int i = 0; i < CurrentCacheKey.Keys.Num(); i++ )
    {
        double key = CurrentCacheKey.Keys[i];

        //PATCH
        // For the last key (The one at the end of the section)
        // It needs to be shifted to be considered inside the section by the sequencer
        if( CurrentCacheKey.Keys.Num() > 1 && i == CurrentCacheKey.Keys.Num() - 1 )
            key -= DELTA;

        if( !Boundary.Contains( key ) && !FMath::IsNearlyEqual( Boundary.GetLowerBoundValue(), key ) && !FMath::IsNearlyEqual( Boundary.GetUpperBoundValue(), key ) )
        {
            data.Add( { FIntPoint(), TRange<double>(), key, true } );
            continue;
        }

        FIntPoint TextureSize = CurrentCache.DesiredSize;
        TextureSize.X = FMath::Min( TextureSize.X, int( CurrentCache.DesiredSize.Y * CameraViewRatio ) );

        TRange<double> TimeRange( key, key + TextureSize.X * TimePerPx );

        // Move the thumbnail to the center of the space if we're the only thumbnail, and we don't fit on
        double Overflow = TextureSize.X*TimePerPx - CurrentCache.TimeRange.Size<double>();
        if( /*Thumbnails.Num() == 0*/ CurrentCacheKey.Keys.Num() == 1 && Overflow > 0 )
        {
            TimeRange = UE::MovieScene::TranslateRange( TimeRange, -( Overflow * .5f ) );
        }
        // If already thumbnails, the last one is shifted
        else if( /*Thumbnails.Num() > 0 &&*/ !CurrentCache.TimeRange.Contains( TimeRange ) )
        {
            TimeRange = UE::MovieScene::TranslateRange( TimeRange, -( TimeRange.GetUpperBoundValue() - CurrentCache.TimeRange.GetUpperBoundValue() ) );
        }

        //UE_LOG( LogTemp, Warning, TEXT( "key: %f - timerange: [%f-%f[ - texturesize: %d %d" ), key, TimeRange.GetLowerBoundValue(), TimeRange.GetUpperBoundValue(), TextureSize.X, TextureSize.Y );

        data.Add( { TextureSize, TimeRange, key, false } );
    }
    //UE_LOG( LogTemp, Warning, TEXT( "---" ) );

    return data;
}

void FTrackEditorKeyThumbnailCache::GenerateThumbnails( const TRange<double>& Boundary, const TArray<FThumbnailData>& iThumbnailData )
{
    const double TimePerPx = CurrentCache.TimeRange.Size<double>() / CurrentCache.AllottedSize.X;

    for( auto data : iThumbnailData )
    {
        TSharedPtr<FTrackEditorThumbnail> NewThumbnail = MakeShareable( new FTrackEditorThumbnail(
            FOnThumbnailDraw::CreateRaw( this, &FTrackEditorKeyThumbnailCache::DrawThumbnail ),
            data.mDesiredSize,
            data.mTimeRange,
            data.mPosition
        ) );

        NewThumbnail->SortOrder = data.mTimeRange.Overlaps( CurrentCache.VisibleRange ) ? 1 : 10;

        Thumbnails.Add( NewThumbnail );
        ThumbnailsNeedingRedraw.Add( NewThumbnail );
    }
}


void FTrackEditorKeyThumbnailCache::Setup()
{
    Super::Setup();
}
