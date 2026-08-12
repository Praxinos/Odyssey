// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianeGraph.h"
#include "ArianeCore.h"
// ULIS Headers
#include <ULIS.h>

FArianeGraph::FSectionCubic::~FSectionCubic()
{
}

FArianeGraph::FSectionCubic::FSectionCubic( FEdgeCubic* InCubicEdge
                                          , FNode* InNode0
                                          , FNode* InNode1
                                          , double InEdgeT0
                                          , double InEdgeT1
                                          , bool bStitchShortSections
                                          , TArray<FSection*>& OutShortSections )
    : FSection ( InCubicEdge, InNode0, InNode1, InEdgeT0, InEdgeT1 )
{
    FEdgeCubic* CubicEdge = static_cast<FEdgeCubic*>(Edge);

    if( fabs( InEdgeT0 - InEdgeT1 ) < 1.0f )
    {

        FVector2D SubBezier[4];

        FArianeCore::BezierExtract( CubicEdge->Bezier[0]
                                  , CubicEdge->Bezier[1]
                                  , CubicEdge->Bezier[2]
                                  , CubicEdge->Bezier[3]
                                  , InEdgeT0
                                  , InEdgeT1
                                  , SubBezier[0]
                                  , SubBezier[1]
                                  , SubBezier[2]
                                  , SubBezier[3] );
        // here we take the node coords and not the one we could retrieve from the
        // subBezier because it might be inconsistent due to the value at T found from
        // performing linear intersection and not from a bezier-bezier intersection.
        // for this reason T might no be reliable to find the endpoints of our bezier.
        // we only use it for the handles.
        Bezier[0] = Nodes[0]->Position;
        Bezier[3] = Nodes[1]->Position;

        Bezier[1] = Bezier[0] + ( SubBezier[1] - SubBezier[0] );
        Bezier[2] = Bezier[3] + ( SubBezier[2] - SubBezier[3] );
    }
    else
    {
        Bezier[0] = CubicEdge->Bezier[0];
        Bezier[1] = CubicEdge->Bezier[1];
        Bezier[2] = CubicEdge->Bezier[2];
        Bezier[3] = CubicEdge->Bezier[3];
    }

    Length = fabs ( InEdgeT1 - InEdgeT0 ) * CubicEdge->Length;

/*
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubicGap::StaticClass() )
    {
        FOdysseyVectorSegmentCubicGap* gapCubicSegment = static_cast<FOdysseyVectorSegmentCubicGap*>(iSegment);
        ::ULIS::FVec2D* segmentBezier = gapCubicSegment->GetBezier();

        // Gap segments already are in paintgroup coordinates. Just copy the whole thing
        memcpy( mBezier, segmentBezier, sizeof( mBezier ) );

        mLength = ::ULIS::FVec2D( mBezier[0] - mBezier[3] ).Distance();
    }
*/
/*
    if( iSegment->GetClass() == FOdysseyVectorSegmentExtended::StaticClass() )
    {
        FOdysseyVectorSegmentExtended* extendedSegment = static_cast<FOdysseyVectorSegmentExtended*>(iSegment);
        ::ULIS::FVec2D vertex0WorldCoords = iVertex0->GetWorldCoords();
        ::ULIS::FVec2D vertex1WorldCoords = iVertex1->GetWorldCoords();
        BLPoint convertedPoint[2];

        // convert to desired space (the paintgroup, normally)
        convertedPoint[0] = ownerInverseWorldMatrix.map_point( vertex0WorldCoords.x, vertex0WorldCoords.y );
        convertedPoint[1] = ownerInverseWorldMatrix.map_point( vertex1WorldCoords.x, vertex1WorldCoords.y );

        mBezier[0].x = convertedPoint[0].x;
        mBezier[0].y = convertedPoint[0].y;

        mBezier[1].x = convertedPoint[0].x;
        mBezier[1].y = convertedPoint[0].y;

        mBezier[2].x = convertedPoint[1].x;
        mBezier[2].y = convertedPoint[1].y;

        mBezier[3].x = convertedPoint[1].x;
        mBezier[3].y = convertedPoint[1].y;

        mLength = ::ULIS::FVec2D( mBezier[0] - mBezier[3] ).Distance();
    }
*/

    if( Length > 0.0f )
    {
        Vector[0] = ( Bezier[1] - Bezier[0] ).GetSafeNormal(); //GetVectorFromNode( 0, false, true );
        Vector[1] = ( Bezier[2] - Bezier[3] ).GetSafeNormal(); //GetVectorFromNode( 1, false, true );

        // check bezier validity. It can happen at very very small values
        // of T that the bezier has the same values at all controllers. We get rid of those
        // sections in FOdysseyVectorGroupPaint::SimplifyGraph()
        if( /*( mVector[0] == zeroVector ) || ( mVector[1] == zeroVector )*/
            Length < 0.0000000001f
            /*( mBezier[0] == mBezier[1] )
         && ( mBezier[0] == mBezier[2] )
         && ( mBezier[0] == mBezier[3] )*/ )
        {
            Length = 0.0f;
        }
    }

    if( bStitchShortSections )
    {
        if( Length == 0.0f )
        {
            OutShortSections.Add( this );
        }
    }

    Link();
}

FVector2D
FArianeGraph::FSectionCubic::GetPointAt( double T )
{
    FVector2D PointAt = ::ULIS::CubicBezierPointAtParameter<FVector2D>( Bezier[0]
                                                                      , Bezier[1]
                                                                      , Bezier[2]
                                                                      , Bezier[3]
                                                                      , T );

    return PointAt;
}

FVector2D
FArianeGraph::FSectionCubic::GetTangentAt( double T, bool bNormalize )
{
    FVector2D Tangent;

    // Special case when control point are located at endpoint (tangentequals 0 then).

    if( ( T > 0.0f ) && ( T < 1.0f ) )
    {
        Tangent = ::ULIS::CubicBezierTangentAtParameter<FVector2D>( Bezier[0]
                                                                  , Bezier[1]
                                                                  , Bezier[2]
                                                                  , Bezier[3]
                                                                  , T );
    }

    if( T == 0.0f )
    {
        Tangent = Bezier[1] - Bezier[0];

        if( Tangent.SquaredLength() == 0.0f )
        {
            Tangent = Bezier[2] - Bezier[0];
        }
    }

    if( T == 1.0f )
    {
        Tangent =  Bezier[3] - Bezier[2];

        if( Tangent.SquaredLength() == 0.0f )
        {
            Tangent = Bezier[3] - Bezier[1];
        }
    }

    if( bNormalize )
    {
        if( Tangent.SquaredLength() )
        {
            Tangent.Normalize();
        }
    }

    return Tangent;
}

FVector2D*
FArianeGraph::FSectionCubic::GetBezier()
{
    return Bezier;
}
