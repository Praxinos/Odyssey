// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PathDrawingTool/ArianeEditorPathTracer.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeSegmentCubic.h"

// Cubix Bezier functions
#include <ULIS>

#ifdef unused

FArianeEditorPathTracer::~FArianeEditorPathTracer()
{
}

FArianeEditorPathTracer::FArianeEditorPathTracer()
    : DotLimit ( 0.0 ) // 90 deg
    //, mDotLimit ( 0.7071f ) // cos 45deg
    , PointID( 0 )
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
    PointID = 0;
}

FArianeSegment*
FArianeEditorPathTracer::Flush( FArianeVertex* PreviousVertex, FArianeVertex* EndVertex )
{
    FArianeSegment* NewSegment = nullptr;

    // forbid path with a signle vertex that has a segment that loops on itself
    if ( ( CubicPath->GetVertices().Num() == 1 )
      && ( CubicPath->GetVertices()[0].GetVertex() == EndVertex ) )
    {
        Reset();

        return nullptr;
    }

    if( EdgeBuffer.Num() )
    {
        MakeBezier( true );
        // NewSegment will be nullptr if iEndVertex == mPreviousVertex
        NewSegment = CommitSegment( PreviousVertex
                                  , EndVertex ? EndVertex : CommitVertex( false ) );

        // relocate the last vertex at the last entry
        if( ( EndVertex == nullptr ) && PointBuffer.Num() )
        {
            const FTransform& CubicPathTransform = CubicPath->GetTransform();
            FVector LastPointCoords = PointBuffer.Last().Coords;
            FVector LocalPoint = { CubicPathTransform.InverseTransformPosition( LastPointCoords ) };

            NewSegment->GetVertex(1)->SetPosition( LocalPoint.X, LocalPoint.Y, LocalPoint.Z );
        }
    }

    Reset();

    return NewSegment;
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
FArianeEditorPathTracer::GetEdgeChainLength()
{
    double Length = 0.0f;

    for( int i = 0; i < EdgeBuffer.Num(); i++ )
    {
        Length += EdgeBuffer[i].Length;
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
        cumulEdgeLength += EdgeBuffer[i].Length;

        ti = tf;
        tf = cumulEdgeLength / iEdgeChainLength;

        if( ( iAt >= ti ) && ( iAt <= tf ) )
        {
            double t = iAt - ti;

            samplePoint = EdgeBuffer[i].P0 + ( EdgeBuffer[i].P1 - EdgeBuffer[i].P0 ) * t;

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
                                           , uint32 At )
{
    uint32 PointID = At;
    uint32 HandleID = ( At == 0 ) ? 1 : 2;
    FVector& EndPoint = Bezier[PointID];
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
FArianeEditorPathTracer::TestBezier( FVector Bezier[4] )
{
    // take 9 sample points are check how far they are from the edges
    FVector Samples[9] = { ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.10f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.20f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.30f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.40f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.50f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.60f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.70f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.80f )
                         , ::ULIS::CubicBezierPointAtParameter( Bezier[0]
                                                              , Bezier[1]
                                                              , Bezier[2]
                                                              , Bezier[3]
                                                              , 0.90f ) };
    double Tolerance = TracingWidth * 0.5f;

    for( uint32 i = 0; i < 9; i++ )
    {
        double MinDistance = DBL_MAX;

        for( FTracerEdge& Edge : EdgeBuffer )
        {
            double Dist;
            double t = FOdysseyVector::DistanceToSegmentConstrained( Samples[i], Edge.P0, Edge.P1, Dist );

            if( Dist < MinDistance )
            {
                MinDistance = Dist;
            }
        }

        if( MinDistance > Tolerance )
        {
            return false;
        }
    }

    return true;
}

FArianeEditorPathTracer::FTracerBezier&
FArianeEditorPathTracer::GetBestBezier()
{
    return BestBezier;
}

FArianeEditorPathTracer::FTracerBezier&
FArianeEditorPathTracer::GetRawBezier()
{
    return RawBezier;
}

// is there a bezier that matches ? return true if yes, false otherwise.
bool
FArianeEditorPathTracer::MakeBezier( bool iForce )
{
    FTracerRecord* FirstRecord = &RecordBuffer[0];
    FTracerRecord* LastRecord = &RecordBuffer.Last();
    FVector& FirstRecordCoords = FirstRecord->Coords;
    FVector& LastRecordCoords = LastRecord->Coords;
    FTracerEdge* FirstEdge = &EdgeBuffer[0];
    FTracerEdge* LastEdge = &EdgeBuffer.Last();
    double EdgeChainLength = GetEdgeChainLength();
    FVector FirstEdgeVector = FirstRecord->bSmooth ? SmoothVector * EdgeChainLength * 0.33f
                                                   : FirstEdge->Vector * EdgeChainLength * 0.33f;
    FVector LastEdgeVector = LastEdge->Vector * EdgeChainLength * 0.33f;

    //UE_LOG(LogTemp,Warning,TEXT("mRecordBuffer:%d mEdgeBuffer:%d %f"),mRecordBuffer.size(),mEdgeBuffer.size(),edgeChainLength);

    CandidateBezier.bInited = true;
    CandidateBezier.FirstRecordRadius = FirstRecord->Radius;
    CandidateBezier.LastRecordRadius = LastRecord->Radius;

    CandidateBezier.FirstRecordID = FirstRecord->ID;
    CandidateBezier.LastRecordID = LastRecord->ID;
    CandidateBezier.FirstEdgeID = FirstEdge->ID;
    CandidateBezier.LastEdgeID = LastEdge->ID;

    CandidateBezier.Points[0] = FirstRecordCoords;
    CandidateBezier.Points[1] = FirstRecordCoords + FirstEdgeVector;
    CandidateBezier.Points[2] = LastRecordCoords - LastEdgeVector;
    CandidateBezier.Points[3] = LastRecordCoords;

    // raw bezier is the bezier before adjustement. For debugging purposes only
    RawBezier = CandidateBezier;

    AdjustBezier( CandidateBezier.Points, EdgeChainLength );

    if( 1 ) // the else statement is disabled for now
    {
        if( ( iForce == true ) || ( TestBezier( CandidateBezier.Points ) == true ) )
        {
            BestBezier = CandidateBezier;

            return true;
        }
    }
    else // disabled for now. Testing some "perfect mode" thats generates 1 bezier everytime the direction changes
         // or everytime we can't find a better bezier
    {
        BestBezier = CandidateBezier;

        if( EdgeBuffer.Num() > 1 )
        {
            FTracerEdge* AnteEdge = &EdgeBuffer[EdgeBuffer.Num()-2];
            static double Limit = 0.99939082701f; // cos( 2deg );

            if( ( AnteEdge->Vector.Dot( LastEdge->Vector ) < Limit ) || ( TestBezier( CandidateBezier.Points ) == false ) )
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }

    // if we never found any best bezier, then we use the last candidate
    if( BestBezier.bInited == false )
    {
        BestBezier = CandidateBezier;
    }

    return false;
}

void
FArianeEditorPathTracer::ClearPointsTo( uint32 iPointID )
{
    TArray<FTracerPoint> NewPointArray;
    int PointRank = 0;

    // clear points until the one passed as parameter (but keep it)
    NewPointArray.Reserve( PointBuffer.Num() );

    while( PointBuffer[PointRank++].ID != iPointID );

    for( int i = --PointRank, j = 0; i < PointBuffer.Num(); i++, j++ )
    {
        NewPointArray.Add( PointBuffer[i] );
    }

    PointBuffer = NewPointArray;
}

void
FArianeEditorPathTracer::ClearTo( uint32 RecordID, uint32 EdgeID )
{
    TArray<FTracerRecord> NewRecordArray;
    TArray<FTracerEdge> NewEdgeArray;
    int recordRank = 0;
    int edgeRank = 0;

    // clear records until the one passed as parameter (but keep it)
    NewRecordArray.Reserve( RecordBuffer.Num() );

    while( RecordBuffer[recordRank++].ID != RecordID );

    for( int i = --recordRank, j = 0; i < RecordBuffer.Num(); i++, j++ )
    {
        NewRecordArray.Add( RecordBuffer[i] );
    }

    RecordBuffer = NewRecordArray;



    NewEdgeArray.Reserve( EdgeBuffer.Num() );

    while( EdgeBuffer[edgeRank++].ID != EdgeID );

    for( int i = edgeRank, j = 0; i < EdgeBuffer.Num(); i++, j++ )
    {
        NewEdgeArray.Add( EdgeBuffer[i] );
    }

    EdgeBuffer = NewEdgeArray;
}

FArianeVertex*
FArianeEditorPathTracer::CommitVertex( bool bHandleAligned )
{
    const FTransform& CubicPathTransform = CubicPath->GetTransform();
    FVector LocalPoint = { CubicPathTransform.InverseTransformPosition( BestBezier.Points[3] ) };
    FVector LocalVector = CubicPathTransform.InverseTransformVector( FVector( BestBezier.LastRecordRadius * 0.7071f
                                                                            , BestBezier.LastRecordRadius * 0.7071f
                                                                            , BestBezier.LastRecordRadius * 0.7071f ) );
    double LocalRadius = FVector( LocalVector.X, LocalVector.Y, LocalVector.Z ).Length();
    FArianeVertex* NewVertex = CubicPath->AllocVertex( LocalPoint,
                                                , LocalRadius );

    CubicPath->AddVertex( NewVertex );

    NewVertex->SetHandleAligned( bHandleAligned );

    return NewVertex;
}

FArianeSegment*
FArianeEditorPathTracer::CommitSegment( FArianeVertex* PreviousVertex
                                      , FArianeVertex* EndVertex )
{
    if( EndVertex != PreviousVertex )
    {
        const FTransform& CubicPathTransform = CubicPath->GetTransform();
        FVector LocalHandlePoint[2] = { CubicPathTransform.InverseTransformPosition( BestBezier.Points[1] )
                                      , CubicPathTransform.InverseTransformPosition( BestBezier.Points[2] ) };
        FArianeSegmentCubic* NewCubicSegment = CubicPath->AllocCubicSegment( PreviousVertex
                                                                           , LocalHandlePoint[0].X
                                                                           , LocalHandlePoint[0].Y
                                                                           , LocalHandlePoint[0].Z
                                                                           , LocalHandlePoint[1].X
                                                                           , LocalHandlePoint[1].Y
                                                                           , LocalHandlePoint[1].Z
                                                                           , EndVertex
                                                                           /*, true*/ );
        TArray<double> pressureProfile;

        CubicPath->AddSegment( NewCubicSegment );

        //NewCubicSegment->Update( /*0*/ );

        ClearTo( BestBezier.LastRecordID, BestBezier.LastEdgeID );

        // must be done after segments are added to the path
        // so that the topology exists
        if( PreviousVertex->IsHandleAligned() )
        {
            PreviousVertex->SetHandleAligned( true );
        }

        SmoothVector = BestBezier.Points[3] - BestBezier.Points[2];

        if( SmoothVector.Length() )
        {
            SmoothVector.Normalize();
        }

        // very important. there is no best bezier anymore.
        BestBezier.bInited = false;

        return NewCubicSegment;
    }

    return  nullptr;
}

FArianeSegment*
FArianeEditorPathTracer::Trace( FArianeVertex* PreviousVertex
                              , double WorldX
                              , double WorldY
                              , double WorldZ
                              , double Radius )
{
    uint32 Indexn = PointBuffer.Num();
    FArianeSegment* NewSegment = nullptr;

    PointBuffer.Emplace( PointID, WorldX, WorldY, WorldZ, Radius );

    if( Indexn == 0 )
    {
        RecordBuffer.Emplace( PointID, WorldX, WorldY, WorldZ, Radius );
    }
    else
    {
        FTracerRecord* LastRecord = RecordBuffer.Num() ? &RecordBuffer.Last() : nullptr;
        uint32 indexi = Indexn - 1;

        if( FVector( LastRecord->Coords.X - WorldX
                   , LastRecord->Coords.Y - WorldY
                   , LastRecord->Coords.Z - WorldZ ).Length() > SampleDistance )
        {
            uint32 EdgeCount = EdgeBuffer.Num();
            FTracerEdge* LastEdge = EdgeCount ? &EdgeBuffer.Last() : nullptr;
            FTracerRecord NewRecord = FTracerRecord( PointID, WorldX, WorldY, WorldZ, Radius );
            FTracerEdge NewEdge = FTracerEdge( LastRecord->ID
                                             , LastRecord->Coords.X
                                             , LastRecord->Coords.Y
                                             , LastRecord->Coords.Z
                                             , LastRecord->Radius
                                             , WorldX
                                             , WorldY
                                             , WorldZ
                                             , Radius );

            ClearPointsTo( PointID );

            // detect if smooth or not
            if( LastEdge )
            {
                if ( LastEdge->Vector.Dot( NewEdge.Vector ) > DotLimit )
                {
                    LastRecord->bSmooth = true;
                }
            }

            if( ( LastRecord->bSmooth == false ) && ( LastEdge != nullptr ) )
            {
                FArianeVertex* NewVertex;

                MakeBezier( true );

                NewVertex = CommitVertex( LastRecord->bSmooth );
                NewSegment = CommitSegment( PreviousVertex, NewVertex );

                RecordBuffer.Add( NewRecord );
                EdgeBuffer.Add( NewEdge );
            }
            else
            {
                RecordBuffer.Add( NewRecord );
                EdgeBuffer.Add( NewEdge );

                if( MakeBezier( false ) == false )
                {
                    FArianeVertex* NewVertex;

                    NewVertex = CommitVertex( LastRecord->bSmooth );
                    NewSegment = CommitSegment( PreviousVertex, NewVertex );
                }
            }
        }
    }

    PointID++;

    return NewSegment;
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

#endif
