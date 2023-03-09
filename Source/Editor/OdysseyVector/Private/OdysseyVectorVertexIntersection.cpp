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
FOdysseyVectorVertexIntersection::GetT( FOdysseyVectorSegment& iSegment )
{
    if ( auto search = mTMap.find(&iSegment); search != mTMap.end())
        return search->second.t;
    else
        return 0.0f;
}

::ULIS::FVec2D
FOdysseyVectorVertexIntersection::GetPosition( FOdysseyVectorSegment& iSegment )
{
    if ( auto search = mTMap.find(&iSegment); search != mTMap.end())
        return search->second.position;
    else
        return { 0.0f, 0.0f };
}

::ULIS::FVec2D&
FOdysseyVectorVertexIntersection::GetCoords( FOdysseyVectorSegment* iSegment )
{
    if ( auto search = mTMap.find(GetFirstSegment()); search != mTMap.end())
        return search->second.position;
    else
        return mCoords;
}

void
FOdysseyVectorVertexIntersection::AddSegment( FOdysseyVectorSegmentCubic* iSegment, double t )
{
    ::ULIS::FVec2D& point0 = iSegment->GetVertex(0)->GetCoords( nullptr );
    ::ULIS::FVec2D& point1 = iSegment->GetVertex(1)->GetCoords( nullptr );
    ::ULIS::FVec2D& ctrlPoint0 = iSegment->GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment->GetHandle(1)->GetCoords();
    ::ULIS::FVec2D intersectAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                    , ctrlPoint0
                                                                                    , ctrlPoint1
                                                                                    , point1
                                                                                    , t );

    FIntersection intersect;

    intersect.position = intersectAt;
    intersect.t        = t;
/*printf("%f %f %f\n", intersectAt.x, intersectAt.y, t);*/
    FOdysseyVectorVertex::AddSegment( iSegment );
//UE_LOG(LogTemp, Warning, TEXT("FOdysseyVectorVertexIntersection::AddSegment %d - size:%d"), iSegment, mTMap.size() );
    mTMap.insert( std::make_pair( iSegment, intersect ) );
}

void
FOdysseyVectorVertexIntersection::AttachLoop( FOdysseyVectorCycle* iLoop )
{
    mLoop = iLoop;
}

FOdysseyVectorCycle*
FOdysseyVectorVertexIntersection::GetLoop()
{
    return mLoop;
}
