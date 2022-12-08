#include "OdysseyVectorPath.h"

void UOdysseyVectorPath::Init( std::string iName )
{
    SetName( iName );
}

std::list<FOdysseyVectorPoint*>&
UOdysseyVectorPath::GetSelectedPointList()
{
    return mSelectedPointList;
}

FOdysseyVectorSegment*
UOdysseyVectorPath::AppendPoint( FOdysseyVectorPoint* iPoint, FOdysseyVectorPoint* iPreviousPoint )
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
UOdysseyVectorPath::AddLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.push_back( iLoop );

    iLoop->Attach();

    iLoop->SetParent ( this );

    printf("%s: Adding loop\n", __func__ );
}

void
UOdysseyVectorPath::RemoveLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.remove( iLoop );

    iLoop->Detach();

    iLoop->SetParent ( nullptr );

    printf("%s: Removing loop\n", __func__ );
}

UOdysseyVectorObject*
UOdysseyVectorPath::CopyShape()
{
    return nullptr;
}

void
UOdysseyVectorPath::UpdateBBox()
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
UOdysseyVectorPath::UpdateShape()
{
    // update segments
    for ( std::list<FOdysseyVectorSegment*>::iterator it = mInvalidatedSegmentList.begin(); it != mInvalidatedSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        segment->Update();
    }

    mInvalidatedSegmentList.clear();

    // then update Loops

    for ( std::list<UOdysseyVectorLoop*>::iterator it = mInvalidatedLoopList.begin(); it != mInvalidatedLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        loop->Update();
    }

    mInvalidatedLoopList.clear();

    UpdateBBox();
}

void
UOdysseyVectorPath::InvalidateSegment( FOdysseyVectorSegment* iSegment )
{
    mInvalidatedSegmentList.push_back( iSegment );

    Invalidate();
}

void
UOdysseyVectorPath::InvalidateLoop( UOdysseyVectorLoop* iLoop )
{
    mInvalidatedLoopList.push_back ( iLoop );

    Invalidate();
}

void
UOdysseyVectorPath::DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        /*if ( loop->IsFilled() == true )
        {*/
            loop->DrawShape( iRoi, iFlags );
        /*}*/
    }
}

UOdysseyVectorObject*
UOdysseyVectorPath::PickLoops( double iX, double iY, double iRadius )
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        if ( loop->PickShape( iX, iY, iRadius ) )
        {
            return loop;
        }
    }

    return nullptr;
}

UOdysseyVectorLoop*
UOdysseyVectorPath::GetLoopByID( uint64 iID )
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        if ( iID == loop->GetID() )
        {
            return loop;
        }
    }

    return nullptr;
}

void
UOdysseyVectorPath::AddPoint( FOdysseyVectorPoint* iPoint )
{
    mPointList.push_back( iPoint );
}

void
UOdysseyVectorPath::AddSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );

    iSegment->GetPoint(0)->AddSegment( iSegment );
    iSegment->GetPoint(1)->AddSegment( iSegment );
}

void
UOdysseyVectorPath::Clear()
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
UOdysseyVectorPath::RemoveSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );

    iSegment->GetPoint(0)->RemoveSegment( iSegment );
    iSegment->GetPoint(1)->RemoveSegment( iSegment );
}

std::list<FOdysseyVectorSegment*>&
UOdysseyVectorPath::GetSegmentList()
{
    return mSegmentList;
}

FOdysseyVectorSegment*
UOdysseyVectorPath::GetLastSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.back();
}

FOdysseyVectorSegment*
UOdysseyVectorPath::GetFirstSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.front();
}

FOdysseyVectorPoint*
UOdysseyVectorPath::GetLastPoint()
{
    if( mPointList.size() == 0 ) return nullptr;

    return mPointList.back();
}

bool
UOdysseyVectorPath::IsLoop()
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
UOdysseyVectorPath::GetFirstPoint()
{
    if( mPointList.size() == 0 ) return nullptr;

    return mPointList.front();
}

void
UOdysseyVectorPath::DrawStructure( ::ULIS::FRectD& iRoi )
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
UOdysseyVectorPath::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->Draw( iRoi );
    }

    DrawLoops( iRoi, iFlags );
}
