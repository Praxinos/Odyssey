// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "HAL/ThreadSafeBool.h"
#include "Animation/CurveSequence.h"
#include "Rendering/RenderingCommon.h"
#include "MovieSceneToolsUserSettings.h"
#include "RHI.h"
#include "Misc/FrameTime.h"
#include "Slate/SlateTextures.h"
#include "TrackEditorThumbnail/TrackEditorThumbnail.h"

struct FMinimalViewInfo;

class FLevelEditorViewportClient;
class FSceneViewport;
class FSlateShaderResource;
class FSlateTexture2DRHIRef;
class FThumbnailViewportClient;
class FTrackEditorThumbnail;
class FTrackEditorThumbnailPool;
class UCameraComponent;


/** Cache key data */
struct FKeyThumbnailCacheData
{
    FKeyThumbnailCacheData() : Keys()
    {
    }

    bool operator==( const FKeyThumbnailCacheData& RHS ) const
    {
        return Keys == RHS.Keys;
    }

    bool operator!=( const FKeyThumbnailCacheData& RHS ) const
    {
        return Keys != RHS.Keys;
    }

    /** All the keys to display thumbnails */
    TArray<double> Keys;
};

// Duplicate of Source\Editor\MovieSceneTools\Public\TrackEditorThumbnail\TrackEditorThumbnail.h as it was not intended to be inherit (no virtual)
// - 'override' all methods
// - only inherit to not have to rewrite all the members and call Super::...() for methods which don't need to be modified
class FTrackEditorKeyThumbnailCache
    : FTrackEditorThumbnailCache
{
    typedef FTrackEditorThumbnailCache Super;

public:
    FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& ThumbnailPool, IViewportThumbnailClient* InViewportThumbnailClient );
    FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& ThumbnailPool, ICustomThumbnailClient* InCustomThumbnailClient );

    ~FTrackEditorKeyThumbnailCache();

    void ForceRedraw();

    void SetSingleReferenceFrame( TOptional<double> InReferenceFrame );
    TOptional<double> GetSingleReferenceFrame() const;

    const TArray<TSharedPtr<FTrackEditorThumbnail>>& GetThumbnails() const;

    void Update( const TRange<double>& NewRange, const TRange<double>& VisibleRange, TArray<double> Keys, const FIntPoint& AllottedSize, const FIntPoint& InDesiredSize, EThumbnailQuality InQuality, double InCurrentTime );

    void Revalidate( double InCurrentTime );

protected:

    void DrawThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail );
    void DrawViewportThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail );
    FIntPoint CalculateTextureSize( const FMinimalViewInfo& ViewInfo ) const;

    bool ShouldRegenerateEverything() const;

    void ComputeNewThumbnails();

    void UpdateSingleThumbnail();
    void UpdateFilledThumbnails();

    void GenerateFront( const TRange<double>& Boundary );
    void GenerateBack( const TRange<double>& Boundary );

    void Setup();

protected:

    FKeyThumbnailCacheData CurrentCacheKey;
    FKeyThumbnailCacheData PreviousCacheKey;

    float CameraViewRatio;
};
