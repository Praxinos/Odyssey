// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "PathDrawingTool/ArianeEditorPathTracer.h"
// Ariane Headers
#include "ArianeCore.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeSegmentCubic.h"
// Odyssey Headers
#include <OdysseyVector.h>

// Cubic Bezier functions
#include <ULIS>

FArianeEditorPathTracer::~FArianeEditorPathTracer()
{
}

FArianeEditorPathTracer::FArianeEditorPathTracer()
    : DotLimit ( 0.0 ) // 90 deg
    //, mDotLimit ( 0.7071f ) // cos 45deg
    , SampleDistance( 3.0f )
    , TracingWidth( 6.0f )
    , CubicPath(nullptr)
{
    PointBuffer.Reserve(100);
    RecordBuffer.Reserve(100);
    EdgeBuffer.Reserve(100);
}

void
FArianeEditorPathTracer::SetTracingWidth( double InTracingWidth )
{
    TracingWidth = InTracingWidth;
}

void
FArianeEditorPathTracer::SetDotLimit( double InDotLimit )
{
    DotLimit = InDotLimit;
}

void
FArianeEditorPathTracer::Init()
{
    Reset();
}

void
FArianeEditorPathTracer::Reset()
{
    BestBezier.bInited = false;
    PointBuffer.Empty();
    RecordBuffer.Empty();
    EdgeBuffer.Empty();
    CubicPath = nullptr;
}

void
FArianeEditorPathTracer::Flush( FSceneView* View
                              , FArianeVertex* FirstSegmentVertex
                              , FArianeVertex* LastSegmentVertex ) // for loops
{
    if( CubicPath )
    {
        FArianeSegmentCubic* CurrentCubicSegment = static_cast<FArianeSegmentCubic*>(CubicPath->GetSegments().Last().GetSegment());

        // forbid path with a single vertex that has a segment that loops on itself
        if ( ( CubicPath->GetVertices().Num() == 1 )
          && ( CubicPath->GetVertices()[0].GetVertex() == LastSegmentVertex ) )
        {
            Reset();

            return;
        }

        if( EdgeBuffer.Num() )
        {
            CommitBestBezier( CurrentCubicSegment );

            // relocate the last vertex at the last entry
            if( ( LastSegmentVertex == nullptr ) && PointBuffer.Num() )
            {
                const FTransform& CubicPathTransform = CubicPath->GetTransform();
                FVector LocalPoint = PointBuffer.Last().LocalPosition;

                CurrentCubicSegment->GetVertex(1)->SetPosition( LocalPoint.X, LocalPoint.Y, LocalPoint.Z );
            }
        }

        Reset();
    }
}

TArray<FArianeEditorPathTracer::FTracerPoint>&
FArianeEditorPathTracer::GetPointBuffer()
{
    return PointBuffer;
}

TArray<FArianeEditorPathTracer::FTracerRecord>&
FArianeEditorPathTracer::GetRecordBuffer()
{
    return RecordBuffer;
}

TArray<FArianeEditorPathTracer::FTracerEdge>&
FArianeEditorPathTracer::GetEdgeBuffer()
{
    return EdgeBuffer;
}

double
FArianeEditorPathTracer::GetEdgeChainLength( FTracerEdge* FirstEdge, FTracerEdge* LastEdge )
{
    double Length = 0.0f;

    for( int i = 0; i < EdgeBuffer.Num(); i++ )
    {
        FTracerEdge* CurrentEdge = &EdgeBuffer[i];

        // Pointer arithmetic
        if( ( CurrentEdge >= FirstEdge ) || ( CurrentEdge <= LastEdge ) )
        {
            Length += EdgeBuffer[i].LocalLength;
        }
    }

    return Length;
}

/* AdjustBezier()
 *
 * The idea here is to adjust the cubic curve if it drifts away too much from the sample points
 * How do we do that ? We have the samples points :
 *
 *      o o o o o o o
 *   A                 B
 *
 * and we have computed the cubic curve from entry vector and exit vector (first and last sample points).
 * The length of the handles by default is 0.35 the length of the distance between A and B.
 *      _____________
 *     /             \
 *    /               \
 *   A                 B
 *
 * We then pick a point P1 at position 0.33f and P2 at 0.66f on this cubic curve :
 *      _____________
 *     P1           P2
 *    /               \
 *   A                 B
 *
 * we compare this position with an interpolated sample point "s" located at 0.33f and 0.66f as well.
 *      _____________
 *     P1           P2
 *    /   s1     s2   \
 *   A                 B
 *
 * from the angles between vectors [A-B,A-P1] and [A-B,A-s1], we get a ratio that we use to decrease the length of the handles.
 * same goes for A-P2 and A-s2. Not perfect but not bad. Works only when decreasing the handles' length though, not increasing.
 *      _____________
 *    /               \
 *   A                 B
 *
 */

FVector
FArianeEditorPathTracer::GetSamplePointAtParameter( double iEdgeChainLength, double iAt )
{
    double cumulEdgeLength = 0.0f;
    double ti, tf = 0.0f;
    FVector samplePoint;

    for( int i = 0; i < EdgeBuffer.Num(); i++ )
    {
        cumulEdgeLength += EdgeBuffer[i].LocalLength;

        ti = tf;
        tf = cumulEdgeLength / iEdgeChainLength;

        if( ( iAt >= ti ) && ( iAt <= tf ) )
        {
            double t = iAt - ti;

            samplePoint = EdgeBuffer[i].P0.LocalPosition + ( EdgeBuffer[i].P1.LocalPosition - EdgeBuffer[i].P0.LocalPosition ) * t;

            break;
        }
    }

    return samplePoint;
}

void
FArianeEditorPathTracer::AdjustBezier( FVector Bezier[4], double EdgeChainLength )
{
    FVector ExpectedPoint = ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                               , Bezier[1]
                                                               , Bezier[2]
                                                               , Bezier[3]
                                                               , 0.5f );
    FVector SampledPoint = GetSamplePointAtParameter( EdgeChainLength, 0.5f );

    AdjustBezierHandle( Bezier, ExpectedPoint, SampledPoint, 0 );
    AdjustBezierHandle( Bezier, ExpectedPoint, SampledPoint, 3 );
}

void
FArianeEditorPathTracer::AdjustBezierHandle( FVector Bezier[4]
                                           , FVector& ExpectedPoint
                                           , FVector& SampledPoint
                                           , uint32 AtIndex )
{
    uint32 HandleID = ( AtIndex == 0 ) ? 1 : 2;
    FVector& EndPoint = Bezier[AtIndex];
    FVector& HandlePoint = Bezier[HandleID];
    FVector EndPointToExpectedPoint = ExpectedPoint - EndPoint;
    FVector EndPointToSampledPoint = SampledPoint - EndPoint;
    FVector EndPointToHandlePoint = HandlePoint - EndPoint;

    if ( EndPointToExpectedPoint.SquaredLength()
      && EndPointToSampledPoint.SquaredLength()
      && EndPointToHandlePoint.SquaredLength() )
    {
        FVector direction = EndPointToHandlePoint;
        EndPointToExpectedPoint.Normalize();
        EndPointToSampledPoint.Normalize();
        EndPointToHandlePoint.Normalize();

        double dot0 = EndPointToHandlePoint.Dot( EndPointToExpectedPoint );
        double angle0 = acos( ULIS::FMath::Clamp<double>( dot0, -1.0f, 1.0f ) );
        double dot1 = EndPointToHandlePoint.Dot( EndPointToSampledPoint );
        double angle1 = acos( ULIS::FMath::Clamp<double>( dot1, -1.0f, 1.0f ) );

        if ( angle1 > 0.1f )
        {
            double ratio = angle0 / angle1;

            HandlePoint.X = EndPoint.X + ( direction.X * ratio );
            HandlePoint.Y = EndPoint.Y + ( direction.Y * ratio );
            HandlePoint.Z = EndPoint.Z + ( direction.Z * ratio );
        }
    }
}

bool
FArianeEditorPathTracer::TestBezier( FSceneView* View
                                   , FArianeSegment* PreviousSegment
                                   , FArianeVertex* FirstVertex
                                   , FTracerEdge* FirstEdge
                                   , FTracerEdge* LastEdge
                                   , FVector OutBezier[4] )
{
    FVector& FirstRecordLocalPosition = FirstEdge->P0.LocalPosition;
    FVector& LastRecordLocalPosition = LastEdge->P1.LocalPosition;
    double EdgeChainLength = GetEdgeChainLength( FirstEdge, LastEdge );
    FVector SmoothVector = PreviousSegment ? - PreviousSegment->GetVectorLeavingFromVertex( FirstVertex, true )
                                           : FirstEdge->LocalVector;
    FVector FirstEdgeVector = FirstVertex->IsHandleAligned() ? SmoothVector           * EdgeChainLength * 0.33f
                                                             : FirstEdge->LocalVector * EdgeChainLength * 0.33f;
    FVector LastEdgeVector = LastEdge->LocalVector * EdgeChainLength * 0.33f;

    OutBezier[0] = FirstRecordLocalPosition;
    OutBezier[1] = FirstRecordLocalPosition + FirstEdgeVector;
    OutBezier[2] = LastRecordLocalPosition - LastEdgeVector;
    OutBezier[3] = LastRecordLocalPosition;

    AdjustBezier( OutBezier, EdgeChainLength );

    return TestBezierSamples( View, OutBezier, FirstEdge, LastEdge, 5 );
}

bool
FArianeEditorPathTracer::TestBezierSamples( FSceneView* View
                                          , FVector Bezier[4]
                                          , FTracerEdge* FirstEdge
                                          , FTracerEdge* LastEdge
                                          , uint32 Samples )
{
    const FTransform& PathTransform = CubicPath->GetTransform();

    if( Samples )
    {
        float SampleStep = 1.0f / ( Samples + 1 );
        float SampleT = SampleStep;
        double Tolerance = TracingWidth * 0.5f;

        for( uint32 i = 0; i < Samples; i++ )
        {
            // take sample point to check how far it is from the edges in View
            FVector SampleAt = ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                                  , Bezier[1]
                                                                  , Bezier[2]
                                                                  , Bezier[3]
                                                                  , SampleT );
            //FVector SampleAtWorld = CubicPath->GetTransform().TransformPosition( SampleAt );
            double MinDistance = DBL_MAX;
            FVector2D SampleAtViewport;

            View->WorldToPixel( PathTransform.TransformPosition( SampleAt ), SampleAtViewport );

//UE_LOG( LogTemp, Warning, TEXT("EdgeBuffer %d"), EdgeBuffer.Num() );

            for( FTracerEdge& Edge : EdgeBuffer )
            {
                // Pointer arithmetic
                if( ( &Edge >= FirstEdge ) && ( &Edge <= LastEdge ) )
                {
                    double Dist;
                    double T = FArianeCore::DistanceToSegmentConstrained( SampleAtViewport
                                                                        , Edge.P0.ViewportPosition
                                                                        , Edge.P1.ViewportPosition
                                                                        , Dist );
/*
UE_LOG( LogTemp, Warning, TEXT("PT %f %f - P0:%f %f P1:%f %f"), SampleAtViewport.X, SampleAtViewport.Y
                                                              , Edge.P0.ViewportPosition.X, Edge.P0.ViewportPosition.Y
                                                              , Edge.P1.ViewportPosition.X, Edge.P1.ViewportPosition.Y );
*/

                    if( Dist < MinDistance )
                    {
                        MinDistance = Dist;
                    }
                }
            }

            SampleT += SampleStep;

            if( MinDistance > Tolerance )
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

FArianeEditorPathTracer::FTracerBezier&
FArianeEditorPathTracer::GetBestBezier()
{
    return BestBezier;
}

/*FArianeEditorPathTracer::FTracerBezier&
FArianeEditorPathTracer::GetRawBezier()
{
    return RawBezier;
}
*/

FArianeVertex*
FArianeEditorPathTracer::CommitVertex( const FTracerRecord& CommitRecord )
{
    FArianeVertex* NewVertex = CubicPath->AllocVertex( CommitRecord.LocalPosition
                                                     , CommitRecord.LocalNormal
                                                     , CommitRecord.Radius );

    CubicPath->AddVertex( NewVertex );

    return NewVertex;
}

void
FArianeEditorPathTracer::ExportBestBezier( FArianeSegmentCubic* CubicSegment )
{
    FArianeVertex* Vertex0 = CubicSegment->GetVertex(0);
    FArianeVertex* Vertex1 = CubicSegment->GetVertex(1);
    FArianeHandleSegment* Handle0 = CubicSegment->GetHandle( (uint32) 0 );
    FArianeHandleSegment* Handle1 = CubicSegment->GetHandle( (uint32) 1 );

    Vertex0->SetPosition( BestBezier.Points[0] );
    Handle0->SetPosition( BestBezier.Points[1] );
    Handle1->SetPosition( BestBezier.Points[2] );
    Vertex1->SetPosition( BestBezier.Points[3]);
}

void
FArianeEditorPathTracer::CommitBestBezier( FArianeSegmentCubic* CubicSegment )
{
    ExportBestBezier( CubicSegment );

    // very important. there is no best bezier anymore.
    BestBezier.bInited = false;

    CubicSegment->SetAutoFractioned( true );
}

FArianeSegmentCubic*
FArianeEditorPathTracer::CreateCubicSegment( FArianeVertex* StartingVertex, FArianeVertex* EndingVertex )
{
    //FArianeVertex* NewVertex = EndingVertex == nullptr ? CommitVertex( *PreviousRecord ) : EndingVertex;

    FArianeSegmentCubic* NewCubicSegment = CubicPath->AllocCubicSegment( StartingVertex
                                                                       , StartingVertex->GetPosition()
                                                                       , EndingVertex->GetPosition()
                                                                       , EndingVertex );

    //CubicPath->AddVertex( EndingVertex );
    CubicPath->AddSegment( NewCubicSegment );

    NewCubicSegment->SetAutoFractioned( false );

    return NewCubicSegment;
}

FArianeSegment*
FArianeEditorPathTracer::Trace( FSceneView *View
                              , FArianeSegment* PreviousSegment
                              , FArianeVertex* StartingVertex
                              , FArianeSegment* CurrentSegment
                              , const FVector2D& ViewportPosition
                              , const FVector& LocalPosition
                              , const FVector& LocalNormal
                              , double Radius )
{
    uint32 Indexn = PointBuffer.Num();
    FArianeVertex* EndingVertex;

    PointBuffer.Emplace( ViewportPosition, LocalPosition, LocalNormal, Radius );

    if( CurrentSegment == nullptr )
    {
        CurrentSegment = CreateCubicSegment ( StartingVertex, CommitVertex( PointBuffer[0] ) );
    }

    if( Indexn == 0 )
    {
        RecordBuffer.Emplace( ViewportPosition, LocalPosition, LocalNormal, Radius );
    }
    else
    {
        FTracerRecord* PreviousRecord = &RecordBuffer.Last();
        double LineLength = FVector2D( PreviousRecord->ViewportPosition - ViewportPosition ).Length();

        EndingVertex = CurrentSegment->GetVertex( 1 );

        // A record is a point that is separated from the previous Record by at least some distance (in viewport units, i.e pixels)
        if( LineLength > SampleDistance )
        {
            FTracerEdge* PreviousEdge = EdgeBuffer.Num() ? &EdgeBuffer.Last() : nullptr;
            // Note: FirstEdge can be equal to NewEdge
            FTracerEdge* FirstEdge = EdgeBuffer.Num() ? &EdgeBuffer[0] : nullptr;

            if( PreviousEdge && FirstEdge )
            {
                FTracerRecord* FirstRecord = &RecordBuffer[0];
                FVector CandidateBezier[4];
                bool bBezierFits = TestBezier( View
                                             , PreviousSegment
                                             , StartingVertex
                                             , FirstEdge
                                             , PreviousEdge
                                             , CandidateBezier );
                FVector2D NewEdgeViewportVector = ( ViewportPosition - PreviousEdge->P1.ViewportPosition ).GetSafeNormal();

                // the first Bezier fits no matter what
                if( BestBezier.bInited == false )
                {
                    bBezierFits = true;
                }

                if( bBezierFits )
                {
                    BestBezier.bInited = true;

                    BestBezier.FirstRecord = *FirstRecord;
                    BestBezier.LastRecord = *PreviousRecord;

                    memcpy( BestBezier.Points, CandidateBezier, sizeof( CandidateBezier ) );
                }

                // detect if smooth or not
                EndingVertex->SetHandleAligned( ( PreviousEdge->ViewportVector.Dot( NewEdgeViewportVector ) > DotLimit ) ? true
                                                                                                                         : false );

                bool PreviousRecordSmooth = EndingVertex->IsHandleAligned();
                FTracerRecord PreviousRecordCopy = *PreviousRecord;
                // the Emplace() funcs must be put after we are done with all the pointers form EdgeBuffer and RecordBuffer.
                FTracerRecord* NewRecord = &RecordBuffer.Emplace_GetRef( ViewportPosition, LocalPosition, LocalNormal, Radius );
                FTracerEdge* NewEdge = &EdgeBuffer.Emplace_GetRef( *PreviousRecord, *NewRecord );

                // if the angle between the last 2 edges has passed a certain limit, commit the segment no matter what
                if( ( PreviousRecordSmooth == false )
                // if the angle is smooth BUT the bezier does not fit anymore, commit the segment as well
                 || ( bBezierFits == false    ) )
                {
                    FTracerRecord NewRecordCopy = *NewRecord;
                    FTracerEdge NewEdgeCopy = *NewEdge;
                    //FArianeVertex* NewVertex = CommitVertex( BestBezier.LastRecord );
                    // CommitSegment uses the Best Bezier found my MakeBezier

                    CommitBestBezier( static_cast<FArianeSegmentCubic*>(CurrentSegment) );

                    // start a new bezier
                    CurrentSegment = CreateCubicSegment ( EndingVertex
                                                        , CommitVertex( NewRecordCopy ) );

                    PointBuffer.Empty();
                    RecordBuffer.Empty();
                    EdgeBuffer.Empty();

                    // repopulate buffers
                    PointBuffer.Add( PreviousRecordCopy );
                    RecordBuffer.Add( PreviousRecordCopy );

                    PointBuffer.Add( NewRecordCopy );
                    RecordBuffer.Add( NewRecordCopy );

                    EdgeBuffer.Add( NewEdgeCopy );
                }
            }
            else
            {
                FTracerRecord* NewRecord = &RecordBuffer.Emplace_GetRef( ViewportPosition, LocalPosition, LocalNormal, Radius );

                EdgeBuffer.Emplace( *PreviousRecord, *NewRecord );
            }
        }
    }

    if( CurrentSegment->IsAutoFractioned() == false )
    {
        const FTransform& CubicPathTransform = CubicPath->GetTransform();
        TArray<FArianePoint> FractionPoints;
        TArray<float> FractionRadii;

        FractionPoints.Reserve( PointBuffer.Num() );
        FractionRadii.Reserve( PointBuffer.Num() );

        for( const FTracerPoint& Point : PointBuffer )
        {
            FractionPoints.Add( Point.LocalPosition );
            FractionRadii.Add( Point.Radius );
        }

        //ExportBestBezier( static_cast<FArianeSegmentCubic*>(CurrentSegment) );
        CurrentSegment->SetFractions( FractionPoints, FractionRadii );
    }

    return CurrentSegment;
}

void
FArianeEditorPathTracer::AttachPath( FArianePath* InCubicPath )
{
    CubicPath = InCubicPath;
}

FArianePath*
FArianeEditorPathTracer::GetPath()
{
    return CubicPath;
}
