#include "OdysseyVectorPointIntersection.h"

FOdysseyVectorPointIntersection::~FOdysseyVectorPointIntersection()
{
}

FOdysseyVectorPointIntersection::FOdysseyVectorPointIntersection()
    : FOdysseyVectorPoint ( 0.0f, 0.0f )
{

}

uint32
FOdysseyVectorPointIntersection::GetType()
{
    return FOdysseyVectorPoint::POINT_TYPE_INTERSECTION; // default type;
}

double
FOdysseyVectorPointIntersection::GetT( FOdysseyVectorSegment& iSegment )
{
    return mTMap[&iSegment].t;
}

::ULIS::FVec2D
FOdysseyVectorPointIntersection::GetPosition( FOdysseyVectorSegment& iSegment )
{
    return mTMap[&iSegment].position;
}

::ULIS::FVec2D&
FOdysseyVectorPointIntersection::GetCoords()
{
    return mTMap[GetFirstSegment()].position;
}

void
FOdysseyVectorPointIntersection::Draw( ::ULIS::FRectD &iRoi )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        ::ULIS::FVec2D position = GetPosition(*segment);

        blctx.setFillStyle( BLRgba32( 0xFFFF80FF) );
        blctx.fillRect( position.x - 5, position.y - 5, 10, 10 );
    }
}

void
FOdysseyVectorPointIntersection::AddSegment( FOdysseyVectorSegmentCubic* iSegment, double t )
{
    ::ULIS::FVec2D& point0 = iSegment->GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment->GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iSegment->GetControlPoint(0).GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment->GetControlPoint(1).GetCoords();
    ::ULIS::FVec2D intersectAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                    , ctrlPoint0
                                                                                    , ctrlPoint1
                                                                                    , point1
                                                                                    , t );
    FIntersection intersect = { intersectAt, t };
/*printf("%f %f %f\n", intersectAt.x, intersectAt.y, t);*/
    FOdysseyVectorPoint::AddSegment(iSegment);

    mTMap.insert(std::make_pair(iSegment, intersect));
}

FOdysseyVectorSegment*
FOdysseyVectorPointIntersection::GetSegment( FOdysseyVectorPoint& iOtherPoint )
{
    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( iOtherPoint.GetType() == POINT_TYPE_REGULAR )
        {
            if ( ( segment->GetPoint(0) == &iOtherPoint )
              || ( segment->GetPoint(1) == &iOtherPoint ) ) {
                return segment;
            }
        }

        if ( iOtherPoint.GetType() == POINT_TYPE_INTERSECTION )
        {
            if ( segment->HasIntersectionPoint ( static_cast<FOdysseyVectorPointIntersection&>(iOtherPoint) ) )
            {
                return segment;
            }
        }
    }

    return nullptr;
}
