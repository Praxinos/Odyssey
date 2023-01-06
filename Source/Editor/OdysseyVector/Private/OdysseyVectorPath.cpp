#include "OdysseyVectorPath.h"

void UOdysseyVectorPath::Init( std::string iName )
{
    SetName( iName );
}

std::list<UOdysseyVectorPoint*>&
UOdysseyVectorPath::GetSelectedPointList()
{
    return mSelectedPointList;
}

UOdysseyVectorSegment*
UOdysseyVectorPath::AppendVertex( UOdysseyVectorVertex* iPoint, UOdysseyVectorVertex* iPreviousPoint )
{
    mVertexList.push_back( iPoint );

    if ( iPreviousPoint )
    {
        if ( iPreviousPoint->GetSegmentCount() < 2 )
        {
            AddSegment( UOdysseyVectorSegment::New( this, iPreviousPoint, iPoint ) );
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

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);
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
    for ( std::list<UOdysseyVectorSegment*>::iterator it = mInvalidatedSegmentList.begin(); it != mInvalidatedSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

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
UOdysseyVectorPath::InvalidateSegment( UOdysseyVectorSegment* iSegment )
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
UOdysseyVectorPath::AddVertex( UOdysseyVectorVertex* iVertex )
{
    mVertexList.push_back( iVertex );
}

void
UOdysseyVectorPath::AddSegment( UOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );

    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(0))->AddSegment( iSegment );
    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(1))->AddSegment( iSegment );
}

void
UOdysseyVectorPath::Clear()
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != --mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment *segment = (*it);

        Cast<UOdysseyVectorVertex>(segment->GetPoint(0))->RemoveSegment(segment);
        Cast<UOdysseyVectorVertex>(segment->GetPoint(1))->RemoveSegment(segment);
        /*RemoveSegment( segment );*/ // this alters the list, hence the loop and leads to a crash
    }

    mSegmentList.clear();
}

void
UOdysseyVectorPath::RemoveSegment( UOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );

    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(0))->RemoveSegment( iSegment );
    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(1))->RemoveSegment( iSegment );
}

std::list<UOdysseyVectorSegment*>&
UOdysseyVectorPath::GetSegmentList()
{
    return mSegmentList;
}

std::list<UOdysseyVectorVertex*>&
UOdysseyVectorPath::GetVertexList()
{
    return mVertexList;
}

UOdysseyVectorSegment*
UOdysseyVectorPath::GetLastSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.back();
}

UOdysseyVectorSegment*
UOdysseyVectorPath::GetFirstSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.front();
}

UOdysseyVectorVertex*
UOdysseyVectorPath::GetLastVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.back();
}

bool
UOdysseyVectorPath::IsLoop()
{
    if ( mVertexList.size() )
    {
        if ( mVertexList.size() == mSegmentList.size() )
        {
            return true;
        }
    }

    return false;
}

UOdysseyVectorVertex*
UOdysseyVectorPath::GetFirstVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.front();
}

void
UOdysseyVectorPath::DrawStructure( ::ULIS::FRectD& iRoi )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    blctx.setStrokeStyle( BLRgba32( 0xFF00FF00 ) );
    blctx.setStrokeWidth(1.0f);

    for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        UOdysseyVectorSegment *segment = (*it);

        segment->DrawStructure( iRoi );
    }
}

void
UOdysseyVectorPath::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        UOdysseyVectorSegment *segment = (*it);

        segment->Draw( iRoi );
    }

    DrawLoops( iRoi, iFlags );
}

void
UOdysseyVectorPath::Serialize(FArchive& Ar)
{
UE_LOG(LogTemp, Warning, TEXT("UOdysseyVectorPath::Serialize"));
}
