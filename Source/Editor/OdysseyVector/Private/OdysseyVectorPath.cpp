#include "OdysseyVectorPath.h"

FOdysseyVectorPath::~FOdysseyVectorPath()
{
}

FOdysseyVectorPath::FOdysseyVectorPath()
    : FOdysseyVectorObject()
{

}

FOdysseyVectorPath::FOdysseyVectorPath( std::string iName )
    : FOdysseyVectorObject( iName )
{

}

std::list<FOdysseyVectorPoint*>&
FOdysseyVectorPath::GetSelectedPointList()
{
    return mSelectedPointList;
}

FOdysseyVectorSegment*
FOdysseyVectorPath::AppendPoint( FOdysseyVectorPoint* iPoint, FOdysseyVectorPoint* iPreviousPoint )
{
    mPointList.push_back( iPoint );

    if ( iPreviousPoint )
    {
        if ( iPreviousPoint->GetSegmentCount() < 2 )
        {
            AddSegment( new FOdysseyVectorSegment ( *this, iPreviousPoint, iPoint ) );
        }
    }

    return NULL;
}

void
FOdysseyVectorPath::AddLoop( FOdysseyVectorLoop* iLoop )
{
    mLoopList.push_back( iLoop );

    iLoop->Attach();

    iLoop->SetParent ( this );

    printf("%s: Adding loop\n", __func__ );
}

void
FOdysseyVectorPath::RemoveLoop( FOdysseyVectorLoop* iLoop )
{
    mLoopList.remove( iLoop );

    iLoop->Detach();

    iLoop->SetParent ( nullptr );

    printf("%s: Removing loop\n", __func__ );
}

FOdysseyVectorObject*
FOdysseyVectorPath::CopyShape()
{
    return nullptr;
}

void
FOdysseyVectorPath::UpdateBBox()
{
    double x1 = DBL_MAX, y1 = DBL_MAX, x2 = -DBL_MAX, y2 = -DBL_MAX;

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);
        ::ULIS::FRectD& coords = segment->GetBoundingBox();
        double rx1 = coords.x, ry1 = coords.y, rx2 = coords.x + coords.w, ry2 = coords.y + coords.h;

        if ( rx1 < x1 ) x1 = rx1;
        if ( ry1 < y1 ) y1 = ry1;
        if ( rx2 > x2 ) x2 = rx2;
        if ( ry2 > y2 ) y2 = ry2;
    }

    mBBox = ::ULIS::TRectangle<double>::FromMinMax( x1, y1, x2, y2 );
}

void
FOdysseyVectorPath::UpdateShape()
{
    // update segments
    for ( std::list<FOdysseyVectorSegment*>::iterator it = mInvalidatedSegmentList.begin(); it != mInvalidatedSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        segment->Update();
    }

    mInvalidatedSegmentList.clear();

    // then update Loops

    for ( std::list<FOdysseyVectorLoop*>::iterator it = mInvalidatedLoopList.begin(); it != mInvalidatedLoopList.end(); ++it )
    {
        FOdysseyVectorLoop* loop = static_cast<FOdysseyVectorLoop*>(*it);

        loop->Update();
    }

    mInvalidatedLoopList.clear();

    UpdateBBox();
}

void
FOdysseyVectorPath::InvalidateSegment( FOdysseyVectorSegment* iSegment )
{
    mInvalidatedSegmentList.push_back( iSegment );

    Invalidate();
}

void
FOdysseyVectorPath::InvalidateLoop( FOdysseyVectorLoop* iLoop )
{
    mInvalidatedLoopList.push_back ( iLoop );

    Invalidate();
}

void
FOdysseyVectorPath::DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    for( std::list<FOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        FOdysseyVectorLoop* loop = static_cast<FOdysseyVectorLoop*>(*it);

        /*if ( loop->IsFilled() == true )
        {*/
            loop->DrawShape( iRoi, iFlags );
        /*}*/
    }
}

FOdysseyVectorObject*
FOdysseyVectorPath::PickLoops( double iX, double iY, double iRadius )
{
    for( std::list<FOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        FOdysseyVectorLoop* loop = static_cast<FOdysseyVectorLoop*>(*it);

        if ( loop->PickShape( iX, iY, iRadius ) )
        {
            return loop;
        }
    }

    return nullptr;
}

FOdysseyVectorLoop*
FOdysseyVectorPath::GetLoopByID( uint64 iID )
{
    for( std::list<FOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        FOdysseyVectorLoop* loop = static_cast<FOdysseyVectorLoop*>(*it);

        if ( iID == loop->GetID() )
        {
            return loop;
        }
    }

    return nullptr;
}

void
FOdysseyVectorPath::AddPoint( FOdysseyVectorPoint* iPoint )
{
    mPointList.push_back( iPoint );
}

void
FOdysseyVectorPath::AddSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );

    iSegment->GetPoint(0)->AddSegment( iSegment );
    iSegment->GetPoint(1)->AddSegment( iSegment );
}

void
FOdysseyVectorPath::Clear()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != --mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->GetPoint(0)->RemoveSegment(segment);
        segment->GetPoint(1)->RemoveSegment(segment);
        /*RemoveSegment( segment );*/ // this alters the list, hence the loop and leads to a crash
    }

    mSegmentList.clear();
}

void
FOdysseyVectorPath::RemoveSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );

    iSegment->GetPoint(0)->RemoveSegment( iSegment );
    iSegment->GetPoint(1)->RemoveSegment( iSegment );
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorPath::GetSegmentList()
{
    return mSegmentList;
}

FOdysseyVectorSegment*
FOdysseyVectorPath::GetLastSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.back();
}

FOdysseyVectorSegment*
FOdysseyVectorPath::GetFirstSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.front();
}

FOdysseyVectorPoint*
FOdysseyVectorPath::GetLastPoint()
{
    if( mPointList.size() == 0 ) return nullptr;

    return mPointList.back();
}

bool
FOdysseyVectorPath::IsLoop()
{
    if ( mPointList.size() )
    {
        if ( mPointList.size() == mSegmentList.size() )
        {
            return true;
        }
    }

    return false;
}

FOdysseyVectorPoint*
FOdysseyVectorPath::GetFirstPoint()
{
    if( mPointList.size() == 0 ) return nullptr;

    return mPointList.front();
}

void
FOdysseyVectorPath::DrawStructure( ::ULIS::FRectD& iRoi )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    blctx.setStrokeStyle( BLRgba32( 0xFF00FF00 ) );
    blctx.setStrokeWidth(1.0f);

    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->DrawStructure( iRoi );
    }
}

void
FOdysseyVectorPath::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->Draw( iRoi );
    }

    DrawLoops( iRoi, iFlags );
}
