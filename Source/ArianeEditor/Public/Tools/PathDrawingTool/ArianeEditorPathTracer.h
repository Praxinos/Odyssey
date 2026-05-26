// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"

struct FArianeObject;
struct FArianeSegment;
struct FArianePath;
struct FArianeVertex;

class ARIANEEDITOR_API FArianeEditorPathTracer
{
public:
    struct FTracerPoint
    {
        uint32 ID;
        FVector Coords;
        double Radius;
        bool bSmooth;

        FTracerPoint()
            : ID ( 0 )
            , Coords( 0.0f, 0.0f, 0.0f )
            , Radius ( 0.0f )
            , bSmooth ( false )
        {
            Coords.X = Coords.Y = Coords.Z = Radius = 0.0f;
            bSmooth = false;
        }

        FTracerPoint( uint32 InID, double InX, double InY, double InZ, double InRadius )
            : ID ( InID )
            , Coords( InX, InY, InZ )
            , Radius ( InRadius )
            , bSmooth ( false )
        {
        }
    };

    typedef struct FTracerPoint FTracerRecord;

    struct FTracerEdge
    {
        uint32 ID;
        FVector P0;
        FVector P1;
        double Radius0;
        double Radius1;
        FVector Vector;
        double Length;

        FTracerEdge()
        {
        }

        FTracerEdge( uint32 InID
                   , double InX0
                   , double InY0
                   , double InZ0
                   , double InRadius0
                   , double InX1
                   , double InY1
                   , double InZ1
                   , double InRadius1 )
        : ID ( InID )
        , P0 ( InX0, InY0, InZ0 )
        , P1 ( InX1, InY1, InZ1 )
        , Radius0 ( InRadius0 )
        , Radius1 ( InRadius1 )
        , Vector ( P1 - P0 )
        , Length ( Vector.Length() )
        {
            if( Length )
            {
                Vector.Normalize();
            }
        }
    };

    struct FTracerBezier
    {
        bool bInited;
        double FirstRecordRadius;
        double LastRecordRadius;
        uint32 FirstRecordID;
        uint32 LastRecordID;
        uint32 FirstEdgeID;
        uint32 LastEdgeID;
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
    FArianeSegment* Trace( FArianeVertex* StitchedVertex
                         , double InWorldX
                         , double InWorldY
                         , double InWorldZ
                         , double InRadius );
    bool MakeBezier( bool bForce );
    bool TestBezier( FVector Bezier[4] );
    void Init();
    TArray<FTracerPoint>& GetPointBuffer();
    TArray<FTracerRecord>& GetRecordBuffer();
    TArray<FTracerEdge>& GetEdgeBuffer();
    FArianeSegment* Flush( FArianeVertex* PreviousVertex
                         , FArianeVertex* EndVertex );
    FArianeSegment* CommitSegment( FArianeVertex* PreviousVertex
                                 , FArianeVertex* EndVertex );
    FArianeVertex* CommitVertex( bool bHandleAligned );
    void Reset();
    void ClearPointsTo( uint32 PointID );
    void ClearTo( uint32 RecordID, uint32 EdgeID );
    void AdjustBezier( FVector Bezier[4], double EdgeChainLength );
    void AdjustBezierHandle( FVector Bezier[4]
                           , FVector& ExpectedPoint
                           , FVector& SampledPoint
                           , uint32 At );
    FVector GetSamplePointAtParameter( double EdgeChainLength, double At );
    double GetEdgeChainLength();
    void SetDotLimit( double DotLimit );
    void SetTracingWidth( double TracingWidth );
    //::ULIS::FRectD GetRedrawRect();

private:
    double DotLimit;
    uint32 PointID;
    double SampleDistance;
    double TracingWidth;
    TArray<FTracerPoint> PointBuffer;
    TArray<FTracerRecord> RecordBuffer;
    TArray<FTracerEdge> EdgeBuffer;
    FVector SmoothVector;
    FTracerBezier CandidateBezier;
    FTracerBezier BestBezier;
    FTracerBezier RawBezier;
    FArianePath* CubicPath;
    uint32 Width;
    uint32 Height;
};
