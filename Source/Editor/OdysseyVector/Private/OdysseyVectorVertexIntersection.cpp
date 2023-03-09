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
FOdysseyVectorVertexIntersection::GetCoords()
{
    if ( auto search = mTMap.find(GetFirstSegment()); search != mTMap.end())
        return search->second.position;
    else
        return mCoords;
}

::ULIS::FVec2D&
FOdysseyVectorVertexIntersection::GetCoordsOnSegment( FOdysseyVectorSegment* iSegment )
{
    return GetCoords();
/*
    if ( auto search = mTMap.find( iSegment ); search != mTMap.end())
        return search->second.position;
    else
        return mCoords;
*/
}

void
FOdysseyVectorVertexIntersection::Draw( UOdysseyVectorPath* iPath, ::ULIS::FRectD &iRoi )
{
    // NOTE: Might not be super fast to call this for each segment
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        ::ULIS::FVec2D position = GetPosition(*segment);

        blctx->setFillStyle( BLRgba32( 0xFFFF80FF) );
        blctx->fillRect( position.x - 5, position.y - 5, 10, 10 );
    }
}

void
FOdysseyVectorVertexIntersection::AddSegment( FOdysseyVectorSegmentCubic* iSegment, double t )
{
    ::ULIS::FVec2D& point0 = iSegment->GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment->GetPoint(1)->GetCoords();
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

FOdysseyVectorSegment*
FOdysseyVectorVertexIntersection::GetSegment( FOdysseyVectorVertex& iOtherVertex )
{
    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( iOtherVertex.GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            if ( ( segment->GetPoint(0) == &iOtherVertex )
              || ( segment->GetPoint(1) == &iOtherVertex ) ) {
                return segment;
            }
        }

        if ( iOtherVertex.GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
        {
            if ( segment->HasIntersectionVertex ( static_cast<FOdysseyVectorVertexIntersection&>(iOtherVertex) ) )
            {
                return segment;
            }
        }
    }

    return nullptr;
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

FOdysseyVectorSection*
FOdysseyVectorVertexIntersection::GetCrossingSection( FOdysseyVectorSegment* iDiscardSegment
                                                    , FOdysseyVectorSection* iDiscardSection )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        if ( section->GetSegment() != iDiscardSegment )
        {
            if( section != iDiscardSection )
            {
                return section;
            }
        }
    }

    return nullptr;
}
