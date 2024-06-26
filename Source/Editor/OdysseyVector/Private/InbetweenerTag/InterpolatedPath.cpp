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

FInterpolatedPath::FInterpolatedPath( FOdysseyVectorPath* iPath
                                    , uint32 iInbetweenCount
                                    , bool iPolyline )
    : mOriginalPath( iPath )
{
    uint32 pointID = 0;
    uint32 segmentID = 0;

                                       // alloc 1 point per vertex
    mInterpolatedPointBuffer.reserve(  iPath->GetVertexList().size()
                                       // alloc 2 handles per segment
                                   + ( iPath->GetSegmentList().size() * 2 ) );

    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
            // Note: we add +1 for the target position
        mInterpolatedPointBuffer.emplace_back( vertex, mInterpolatedPointBuffer.size() );

        vertex->SetID( pointID++ );
    }

    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

        if( iPolyline )
        {
            std::vector<FOdysseyVectorPoint>& fractionPointBuffer = segment->GetFractionPointBuffer();

            for( FOdysseyVectorPoint& point : fractionPointBuffer )
            {
                mInterpolatedPointBuffer.emplace_back( &point, mInterpolatedPointBuffer.size() );
            }

            mInterpolatedPointBuffer.emplace_back( vertex1, mInterpolatedPointBuffer.size() );
        }
        else
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
                FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);

                // Note: we add +1 for the target position
                mInterpolatedPointBuffer.emplace_back( handle0, mInterpolatedPointBuffer.size() );
                handle0->SetID( pointID++ );
                // Note: we add +1 for the target position
                mInterpolatedPointBuffer.emplace_back( handle1, mInterpolatedPointBuffer.size() );
                handle1->SetID( pointID++ );

                mInterpolatedSegmentCubicBuffer.emplace_back( cubicSegment
                                                            , &mInterpolatedPointBuffer[vertex0->GetID()]
                                                            , &mInterpolatedPointBuffer[handle0->GetID()]
                                                            , &mInterpolatedPointBuffer[handle1->GetID()]
                                                            , &mInterpolatedPointBuffer[vertex1->GetID()] );

            }

            segment->SetID( segmentID++ );
        }
    }
}

std::vector<FInterpolatedSegmentCubic>&
FInterpolatedPath::GetInterpolatedSegmentCubicBuffer()
{
    return mInterpolatedSegmentCubicBuffer;
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
