// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"

struct FArianeObject;
struct FArianeSegment;
struct FArianeSegmentCubic;
struct FArianePath;
struct FArianeVertex;

class ARIANEEDITOR_API FArianeEditorPathTracer
{
public:
    struct FTracerPoint
    {
        FVector2D ViewportPosition;
        FVector LocalPosition;
        FVector LocalNormal;
        double Radius;

        FTracerPoint()
            : ViewportPosition( FVector2D::Zero() )
            , LocalNormal( FVector::Zero() )
            , Radius ( 0.0f )
        {
        }

        FTracerPoint( const FVector2D& InViewportPosition
                    , const FVector& InLocalPosition
                    , const FVector& InLocalNormal
                    , double InRadius )
            : ViewportPosition( InViewportPosition )
            , LocalPosition( InLocalPosition )
            , LocalNormal( InLocalNormal )
            , Radius ( InRadius )
        {
        }
    };

    typedef struct FTracerPoint FTracerRecord;

    struct FTracerEdge
    {
        FTracerRecord P0;
        FTracerRecord P1;
        FVector2D ViewportVector;
        FVector LocalVector;
        double LocalLength;

        FTracerEdge()
        {
        }

        FTracerEdge( const FTracerRecord& InP0
                   , const FTracerRecord& InP1 )
        : P0 ( InP0 )
        , P1 ( InP1 )
        , ViewportVector ( P1.ViewportPosition - P0.ViewportPosition )
        , LocalVector ( P1.LocalPosition - P0.LocalPosition )
        , LocalLength ( LocalVector.Length() )
        {
            ViewportVector.Normalize();
            LocalVector.Normalize();
        }
    };

    struct FTracerBezier
    {
        bool bInited;
        FTracerRecord FirstRecord;
        FTracerRecord LastRecord;
        FVector Points[4];

        FTracerBezier()
        {
            bInited = false;
        }
    };

public:
    ~FArianeEditorPathTracer();
    FArianeEditorPathTracer();

    FTracerBezier& GetBestBezier();
    FTracerBezier& GetRawBezier();
    void AttachPath( FArianePath* InCubicPath );
    FArianePath* GetPath();
    FArianeSegment* Trace( FSceneView *View
                         , FArianeSegment* PreviousSegment
                         , FArianeVertex* StartingVertex
                         , FArianeSegment* CurrentSegment
                         , const FVector2D& ViewportPosition
                         , const FVector& LocalPosition
                         , const FVector& LocalNormal
                         , double Radius );
    bool TestBezierSamples( FSceneView* View
                          , FVector InBezier[4]
                          , FTracerEdge* FirstEdge
                          , FTracerEdge* LastEdge
                          , uint32 Samples );
    bool TestBezier( FSceneView* View
                   , FArianeSegment* PreviousSegment
                   , FArianeVertex* FirsVertex
                   , FTracerEdge* FirstEdge
                   , FTracerEdge* LastEdge
                   , FVector OutBezier[4] );
    void Init();
    TArray<FTracerPoint>& GetPointBuffer();
    TArray<FTracerRecord>& GetRecordBuffer();
    TArray<FTracerEdge>& GetEdgeBuffer();
    void Flush( FSceneView* View
              , FArianeVertex* PreviousVertex
              , FArianeVertex* EndVertex );
    FArianeSegmentCubic* CreateCubicSegment( FArianeVertex* StartingVertex, FArianeVertex* EndingVertex );
    void CommitBestBezier( FArianeSegmentCubic* CubicSegment );
    FArianeVertex* CommitVertex( const FTracerRecord& CommitRecord );
    void Reset();
    void AdjustBezier( FVector Bezier[4], double EdgeChainLength );
    void AdjustBezierHandle( FVector Bezier[4]
                           , FVector& ExpectedPoint
                           , FVector& SampledPoint
                           , uint32 At );
    FVector GetSamplePointAtParameter( double EdgeChainLength, double At );
    double GetEdgeChainLength( FTracerEdge* FirstEdge, FTracerEdge* LastEdge );
    void SetDotLimit( double DotLimit );
    void SetTracingWidth( double TracingWidth );
    //::ULIS::FRectD GetRedrawRect();
    void ExportBestBezier( FArianeSegmentCubic* CubicSegment );

private:
    double DotLimit;
    double SampleDistance;
    double TracingWidth;
    TArray<FTracerPoint> PointBuffer;
    TArray<FTracerRecord> RecordBuffer;
    TArray<FTracerEdge> EdgeBuffer;
    FTracerBezier BestBezier;
    FArianePath* CubicPath;
    uint32 Width;
    uint32 Height;
};
