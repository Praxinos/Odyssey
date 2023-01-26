#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorPath.h"

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
    if ( auto search = mTMap.find(&iSegment); search != mTMap.end())
        return search->second.t;
    else
        return 0.0f;
}

::ULIS::FVec2D
UOdysseyVectorVertexIntersection::GetPosition( UOdysseyVectorSegment& iSegment )
{
    if ( auto search = mTMap.find(&iSegment); search != mTMap.end())
        return search->second.position;
    else
        return { 0.0f, 0.0f };
}

::ULIS::FVec2D&
UOdysseyVectorVertexIntersection::GetCoords()
{
    if ( auto search = mTMap.find(GetFirstSegment()); search != mTMap.end())
        return search->second.position;
    else
        return mCoords;
}

void
UOdysseyVectorVertexIntersection::Draw( UOdysseyVectorPath* iPath, ::ULIS::FRectD &iRoi )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        ::ULIS::FVec2D position = GetPosition(*segment);

        blctx->setFillStyle( BLRgba32( 0xFFFF80FF) );
        blctx->fillRect( position.x - 5, position.y - 5, 10, 10 );
    }
}

void
UOdysseyVectorVertexIntersection::AddSegment( UOdysseyVectorSegmentCubic* iSegment, double t )
{
    ::ULIS::FVec2D& point0 = iSegment->GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment->GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iSegment->GetControlPoint(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment->GetControlPoint(1)->GetCoords();
    ::ULIS::FVec2D intersectAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                    , ctrlPoint0
                                                                                    , ctrlPoint1
                                                                                    , point1
                                                                                    , t );

    FIntersection intersect;

    intersect.position = intersectAt;
    intersect.t        = t;
/*printf("%f %f %f\n", intersectAt.x, intersectAt.y, t);*/
    UOdysseyVectorVertex::AddSegment( iSegment );
//UE_LOG(LogTemp, Warning, TEXT("UOdysseyVectorVertexIntersection::AddSegment %d - size:%d"), iSegment, mTMap.size() );
    mTMap.insert( std::make_pair( iSegment, intersect ) );
}

UOdysseyVectorSegment*
UOdysseyVectorVertexIntersection::GetSegment( UOdysseyVectorVertex& iOtherVertex )
{
    for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( iOtherVertex.GetClass() == UOdysseyVectorVertex::StaticClass() )
        {
            if ( ( segment->GetPoint(0) == &iOtherVertex )
              || ( segment->GetPoint(1) == &iOtherVertex ) ) {
                return segment;
            }
        }

        if ( iOtherVertex.GetClass() == UOdysseyVectorVertexIntersection::StaticClass() )
        {
            if ( segment->HasIntersectionVertex ( static_cast<UOdysseyVectorVertexIntersection&>(iOtherVertex) ) )
            {
                return segment;
            }
        }
    }

    return nullptr;
}

void
UOdysseyVectorVertexIntersection::AttachLoop( FOdysseyVectorLoop* iLoop )
{
    mLoop = iLoop;
}

FOdysseyVectorLoop*
UOdysseyVectorVertexIntersection::GetLoop()
{
    return mLoop;
}
