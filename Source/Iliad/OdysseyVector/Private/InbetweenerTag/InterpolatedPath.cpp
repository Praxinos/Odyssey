// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InterpolatedPath.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorHandleSegment.h"

FInterpolatedPath::~FInterpolatedPath()
{
}

void
FInterpolatedPath::Alloc( bool iPolyline )
{
    uint32 pointCount = mOriginalPath->GetVertexList().size();

    if( iPolyline )
    {
        for( FOdysseyVectorSegment* segment : mOriginalPath->GetSegmentList() )
        {
            pointCount += segment->GetFractionPointBuffer().size();
        }
    }
    else
    {
        for( FOdysseyVectorSegment* segment : mOriginalPath->GetSegmentList() )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                pointCount += 2;
            }
        }
    }

    mInterpolatedPointBuffer.reserve( pointCount );
}

FInterpolatedPath::FInterpolatedPath( FOdysseyVectorPath* iPath
                                    , uint32 iInbetweenCount
                                    , bool iPolyline )
    : mOriginalPath( iPath )
{
    uint32 pointID = 0;
    uint32 segmentID = 0;

    Alloc( iPolyline );

    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
            // Note: we add +1 for the target position
        mInterpolatedPointBuffer.emplace_back( vertex, vertex->GetRadius(), mInterpolatedPointBuffer.size() );

        vertex->SetID( pointID++ );
    }

    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
        double radius0 = vertex0->GetRadius();
        double radius1 = vertex1->GetRadius();
        double diffrad = radius1 - radius0;

        if( iPolyline )
        {
            // when the segment is considered as a polyline, we map the intermediate points
            // the bezier is made of internally. They are of type FOdysseyVectorPoint.
            // We also map the end vertices which also derive from type FOdysseyVectorPoint
            std::vector<FInterpolatedPoint*> polylinePointArray;
            std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
            uint32 fractionCount = segment->GetFractionCount();

            polylinePointArray.reserve( 2 + fractionCount - 1 );
            polylinePointArray.push_back( &mInterpolatedPointBuffer[vertex0->GetID()] );

            for( uint32 i = 1; i < fractionCount; i++ )
            {
                FOdysseyVectorFraction& fraction = fractionCache[i];
                FInterpolatedPoint& interpolatedPoint = mInterpolatedPointBuffer.emplace_back( fraction.point[0]
                                                                                             , radius0 + ( diffrad * fraction.fromT )
                                                                                             , mInterpolatedPointBuffer.size() );

                polylinePointArray.push_back( &interpolatedPoint );
            }

            polylinePointArray.push_back( &mInterpolatedPointBuffer[vertex1->GetID()] );

            mInterpolatedSegmentBuffer.emplace_back( segment, polylinePointArray );
        }
        else
        {
            // when the segment is NOT considered as a polyline, we map the end vertices
            // (which derive from type FOdysseyVectorPoint).
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
                FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);
                std::vector<FInterpolatedPoint*> cubicSegmentPointArray;

                // Note: we add +1 for the target position
                mInterpolatedPointBuffer.emplace_back( handle0, 0.0f, mInterpolatedPointBuffer.size() );
                handle0->SetID( pointID++ );
                // Note: we add +1 for the target position
                mInterpolatedPointBuffer.emplace_back( handle1, 0.0f, mInterpolatedPointBuffer.size() );
                handle1->SetID( pointID++ );

                cubicSegmentPointArray.resize( 4 );
                cubicSegmentPointArray[0] = &mInterpolatedPointBuffer[vertex0->GetID()];
                cubicSegmentPointArray[1] = &mInterpolatedPointBuffer[handle0->GetID()];
                cubicSegmentPointArray[2] = &mInterpolatedPointBuffer[handle1->GetID()];
                cubicSegmentPointArray[3] = &mInterpolatedPointBuffer[vertex1->GetID()];

                mInterpolatedSegmentBuffer.emplace_back( cubicSegment, cubicSegmentPointArray );
            }

            segment->SetID( segmentID++ );
        }
    }
}

std::vector<FInterpolatedSegment>&
FInterpolatedPath::GetInterpolatedSegmentBuffer()
{
    return mInterpolatedSegmentBuffer;
}

std::vector<::ULIS::FVec2D>&
FInterpolatedPath::GetInterpolatedPointPositionBuffer()
{
    return mInterpolatedPointPositionBuffer;
}

FOdysseyVectorPath*
FInterpolatedPath::GetOriginalPath()
{
    return mOriginalPath;
}

std::vector<FInterpolatedPoint>&
FInterpolatedPath::GetInterpolatedPointBuffer()
{
    return mInterpolatedPointBuffer;
}
