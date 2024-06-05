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
                                    , const ::ULIS::FRectD& iSpaceBBox
                                    , const BLMatrix2D& iSpaceInverseMatrix 
                                    , uint32 iInbetweenCount )
    : mOriginalPath( iPath )
{
    BLMatrix2D conversionMatrix;
    uint32 pointID = 0;
    uint32 segmentID = 0;

    FOdysseyVector::MatrixMultiply( iSpaceInverseMatrix
                                  , iPath->GetWorldMatrix()
                                  , conversionMatrix );

                                       // alloc 1 point per vertex
    mInterpolatedPointBuffer.reserve(  iPath->GetVertexList().size()
                                       // alloc 2 handles per segment
                                   + ( iPath->GetSegmentList().size() * 2 ) );

    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
        BLPoint pt = conversionMatrix.mapPoint( vertex->GetX(), vertex->GetY() );
        double spaceX = pt.x - iSpaceBBox.x;
        double spaceY = pt.y - iSpaceBBox.y;
        //double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
        //double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );
        double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
        double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
        mInterpolatedPointBuffer.emplace_back( vertex, mInterpolatedPointBuffer.size(), u, v );

        vertex->SetID( pointID++ );
    }

    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);
            BLPoint pt0 = conversionMatrix.mapPoint( handle0->GetX(), handle0->GetY() );
            BLPoint pt1 = conversionMatrix.mapPoint( handle1->GetX(), handle1->GetY() );
            // compute UV for first handle
            double space0X = pt0.x - iSpaceBBox.x;
            double space0Y = pt0.y - iSpaceBBox.y;
            double u0 = std::clamp<double>( space0X / iSpaceBBox.w, 0.0f, 1.0f );
            double v0 = std::clamp<double>( space0Y / iSpaceBBox.h, 0.0f, 1.0f );
            // compute UV for second handle
            double space1X = pt1.x - iSpaceBBox.x;
            double space1Y = pt1.y - iSpaceBBox.y;
            double u1 = std::clamp<double>( space1X / iSpaceBBox.w, 0.0f, 1.0f );
            double v1 = std::clamp<double>( space1Y / iSpaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle0, mInterpolatedPointBuffer.size(), u0, v0 );
            handle0->SetID( pointID++ );
            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle1, mInterpolatedPointBuffer.size(), u1, v1 );
            handle1->SetID( pointID++ );

            mInterpolatedSegmentCubicBuffer.emplace_back( cubicSegment
                                                        , &mInterpolatedPointBuffer[vertex0->GetID()]
                                                        , &mInterpolatedPointBuffer[handle0->GetID()]
                                                        , &mInterpolatedPointBuffer[handle1->GetID()]
                                                        , &mInterpolatedPointBuffer[vertex1->GetID()] );

            cubicSegment->SetID( segmentID++ );
        }
    }
}

std::vector<::ULIS::FVec2D>&
FInterpolatedPath::GetInterpolatedPointPositionBuffer()
{
    return mInterpolatedPointPositionBuffer;
}

std::vector<FInterpolatedPoint>&
FInterpolatedPath::GetInterpolatedPointBuffer()
{
    return mInterpolatedPointBuffer;
}
