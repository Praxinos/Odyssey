#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorVertexIntersection::~FOdysseyVectorVertexIntersection()
{
}

FOdysseyVectorVertexIntersection::FOdysseyVectorVertexIntersection()
    : FOdysseyVectorVertex ()
{
}

double
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSegment* iSegment )
{
    if ( auto search = mTMap.find(iSegment); search != mTMap.end())
        return search->second.t;
    else
        return 0.0f;
}

::ULIS::FVec2D
FOdysseyVectorVertexIntersection::GetPosition( FOdysseyVectorSegment* iSegment )
{
    if ( auto search = mTMap.find(iSegment); search != mTMap.end())
        return search->second.position;
    else
        return { 0.0f, 0.0f };
}

::ULIS::FVec2D&
FOdysseyVectorVertexIntersection::GetCoords( FOdysseyVectorSegment* iSegment )
{
    if ( auto search = mTMap.find(iSegment); search != mTMap.end())
        return search->second.position;
    else
        return mCoords;
}

void
FOdysseyVectorVertexIntersection::AddSegment( FOdysseyVectorSegment* iSegment, double t )
{
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
        ::ULIS::FVec2D& point0 = cubicSegment->GetVertex(0)->GetCoords( nullptr );
        ::ULIS::FVec2D& point1 = cubicSegment->GetVertex(1)->GetCoords( nullptr );
        ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
        ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
        ::ULIS::FVec2D intersectAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                        , ctrlPoint0
                                                                                        , ctrlPoint1
                                                                                        , point1
                                                                                        , t );

        FIntersection intersect;

        intersect.position = intersectAt;
        intersect.t        = t;

        FOdysseyVectorVertex::AddSegment( cubicSegment, t );

        mTMap.insert( std::make_pair( cubicSegment, intersect ) );
    }
}
