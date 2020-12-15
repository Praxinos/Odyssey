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

class EPOSTRACKSEDITOR_API FTrackEditorKeyThumbnailCache
    // Be carefull, because there is no virtual inside this base class (specially for the destructor)
    // That's also why intermediate functions (ComputeNewThumbnails()/UpdateSingleThumbnail()/...) must also be inherited (otherwise those functions (which call each other) will call the base class ones)
    // But this avoid to duplicate all the functions
    : public FTrackEditorThumbnailCache
{
public:
    FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& ThumbnailPool, IViewportThumbnailClient* InViewportThumbnailClient );
    FTrackEditorKeyThumbnailCache( const TSharedPtr<FTrackEditorThumbnailPool>& ThumbnailPool, ICustomThumbnailClient* InCustomThumbnailClient );

    ~FTrackEditorKeyThumbnailCache();

    void Update( const TRange<double>& NewRange, const TRange<double>& VisibleRange, TArray<double> Keys, const FIntPoint& AllottedSize, const FIntPoint& InDesiredSize, EThumbnailQuality InQuality, double InCurrentTime );

    void Revalidate( double InCurrentTime );

protected:

    //void DrawThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail );
    //void DrawViewportThumbnail( FTrackEditorThumbnail& TrackEditorThumbnail );
    //FIntPoint CalculateTextureSize( const FMinimalViewInfo& ViewInfo ) const;

    bool ShouldRegenerateEverything() const;

    void ComputeNewThumbnails();

    void UpdateSingleThumbnail();
    void UpdateFilledThumbnails();

    void GenerateFront( const TRange<double>& Boundary );
    void GenerateBack( const TRange<double>& Boundary );

protected:

    FKeyThumbnailCacheData CurrentCacheKey;
    FKeyThumbnailCacheData PreviousCacheKey;
};
