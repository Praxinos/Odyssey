#include "OdysseyVectorVertexIntersection.h"

UOdysseyVectorVertexIntersection::~UOdysseyVectorVertexIntersection()
{
}

UOdysseyVectorVertexIntersection::UOdysseyVectorVertexIntersection()
    : UOdysseyVectorVertex ()
{

}

double
UOdysseyVectorVertexIntersection::GetT( UOdysseyVectorSegment& iSegment )
{
    return mTMap[&iSegment].t;
}

::ULIS::FVec2D
UOdysseyVectorVertexIntersection::GetPosition( UOdysseyVectorSegment& iSegment )
{
    return mTMap[&iSegment].position;
}

::ULIS::FVec2D&
UOdysseyVectorVertexIntersection::GetCoords()
{
    return mTMap[GetFirstSegment()].position;
}

void
UOdysseyVectorVertexIntersection::Draw( ::ULIS::FRectD &iRoi )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        ::ULIS::FVec2D position = GetPosition(*segment);

        blctx.setFillStyle( BLRgba32( 0xFFFF80FF) );
        blctx.fillRect( position.x - 5, position.y - 5, 10, 10 );
    }
}

void
UOdysseyVectorVertexIntersection::AddSegment( UOdysseyVectorSegmentCubic* iSegment, double t )
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
    UOdysseyVectorVertex::AddSegment(iSegment);

    mTMap.insert(std::make_pair(iSegment, intersect));
}

UOdysseyVectorSegment*
UOdysseyVectorVertexIntersection::GetSegment( UOdysseyVectorVertex& iOtherVertex )
{
    for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( typeid ( iOtherVertex ) == typeid ( UOdysseyVectorVertex ) )
        {
            if ( ( segment->GetVertex(0) == &iOtherVertex )
              || ( segment->GetVertex(1) == &iOtherVertex ) ) {
                return segment;
            }
        }

        if ( typeid ( iOtherVertex ) == typeid ( UOdysseyVectorVertexIntersection ) )
        {
            if ( segment->HasIntersectionVertex ( static_cast<UOdysseyVectorVertexIntersection&>(iOtherVertex) ) )
            {
                return segment;
            }
        }
    }

    return nullptr;
}
