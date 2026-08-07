// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeGraph.h"

FArianeGraph::FSectionLinear::FSectionLinear( FEdgeLinear* InLinearEdge
                                            , FNode* InNode0
                                            , FNode* InNode1
                                            , double InEdgeT0
                                            , double InEdgeT1
                                            , bool bStitchShortSections
                                            , TArray<FSection*>& OutShortSections )
    : FSection ( InLinearEdge, InNode0, InNode1, InEdgeT0, InEdgeT1 )
{
    Length = fabs ( InEdgeT1 - InEdgeT0 ) * InLinearEdge->Length;

    if( Length > 0.0f )
    {
        FVector2D Tangent = ( Nodes[1]->Position - Nodes[0]->Position ).GetSafeNormal();

        Vector[0] =  Tangent; //GetVectorFromNode( 0, false, true );
        Vector[1] = -Tangent; //GetVectorFromNode( 1, false, true );

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
FArianeGraph::FSectionLinear::GetPointAt( double T )
{
    return Nodes[0]->Position + T * ( Nodes[1]->Position - Nodes[0]->Position );
}

FVector2D
FArianeGraph::FSectionLinear::GetTangentAt( double T, bool bNormalize )
{
    FVector2D Tangent = ( Nodes[1]->Position - Nodes[0]->Position );

    if( bNormalize )
    {
        Tangent.Normalize();
    }

    return Tangent;
}

/*
FVector2D
FOdysseyVectorSection::GetVector( uint32 iVertexIndex )
{
    return mVector[iVertexIndex];
}

FVector2D
FOdysseyVectorSection::GetVectorFromNode( uint32 NodeIndex
                                        , bool bStraight
                                        , bool bNormalize )
{
    ::ULIS::FVec2D tangent = { 0.0f, 0.0f };

    if( bStraight == true )
    {
        tangent =  ( iVertexIndex == 0 ) ? mBezier[3] - mBezier[0]
                                         : mBezier[0] - mBezier[3];
    }
    else
    {
        if( mSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
             tangent = ( iVertexIndex == 0 ) ?  GetTangentAt( 0.0f, false )
                                             : -GetTangentAt( 1.0f, false );
        }

        if( mSegment->HasBaseClass( FOdysseyVectorSegmentExtended::StaticClass() ) )
        {
            tangent =  ( iVertexIndex == 0 ) ? mBezier[3] - mBezier[0]
                                             : mBezier[0] - mBezier[3];
        }
    }

    if( iNormalize )
    {
        if( tangent.DistanceSquared() )
        {
            tangent.Normalize();
        }
    }

    return tangent;
}
*/
