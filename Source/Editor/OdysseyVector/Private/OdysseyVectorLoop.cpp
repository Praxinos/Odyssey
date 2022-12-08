#include "OdysseyVectorLoop.h"

UOdysseyVectorLoop::~UOdysseyVectorLoop()
{
}

void
UOdysseyVectorLoop::Init( UOdysseyVectorPath* iParent
                        , uint64 iID
                        , FOdysseyVectorPoint* iLoopPoint
                        , std::list<FOdysseyVectorPoint*>& iLoopPointList
                        , std::list<FOdysseyVectorSection*>& iLoopSectionList )

{
    mParent = Cast<UOdysseyVectorObject>(iParent);

    mID = iID;
    mLoopPoint = iLoopPoint;
    mPointList = iLoopPointList;
    mSectionList = iLoopSectionList;

    Build();
}

uint64
UOdysseyVectorLoop::GetID()
{
    return mID;
}

void
UOdysseyVectorLoop::Invalidate()
{
    static_cast<UOdysseyVectorPath*>(mParent)->InvalidateLoop( this );
}

void
UOdysseyVectorLoop::UpdateShape()
{
    Build();
}

UOdysseyVectorObject*
UOdysseyVectorLoop::CopyShape()
{
    return nullptr;
}

UOdysseyVectorObject*
UOdysseyVectorLoop::PickShape( double iX, double iY, double iRadius )
{
    /*BLPath path;*/
    BLPoint p = { iX, iY };
    BLBox bbox;

/*
    if ( mPointArray.size() )
    {
        path.moveTo ( mPointArray[0].x, mPointArray[0].y );

        for ( int i = 1; i < mPointArray.size(); i++ )
        {
            path.lineTo ( mPointArray[i].x, mPointArray[i].y );
        }

        path.lineTo ( mPointArray[0].x, mPointArray[0].y );
    }
*/
    mPath.getBoundingBox( &bbox );

    if ( ( iX > bbox.x0 ) && ( iX < bbox.x1 ) && ( iY > bbox.y0 ) && ( iY < bbox.y1 ) )
    {
        return this;
    }

/*
    if ( mPath.hitTest( p, BL_FILL_RULE_NON_ZERO ) == BL_HIT_TEST_IN  )
    {
printf("%d\n", mID );
        return this;
    }
*/
    return nullptr;
};

uint64
UOdysseyVectorLoop::GenerateID( std::list<FOdysseyVectorSection*> iSectionList )
{
    uint64 loopID = 0;

    for( std::list<FOdysseyVectorSection*>::iterator it = iSectionList.begin(); it != iSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        loopID = loopID ^ ( uint64 ) section;
    }

    return loopID;
}

void
UOdysseyVectorLoop::Detach()
{
    for( std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it )
    {
        FOdysseyVectorPoint* point = static_cast<FOdysseyVectorPoint*>(*it);

        point->RemoveLoop( this );
    }

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        section->RemoveLoop( this );
    }
}

void
UOdysseyVectorLoop::Attach()
{
    for( std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it )
    {
        FOdysseyVectorPoint* point = static_cast<FOdysseyVectorPoint*>(*it);

        point->AddLoop( this );
    }

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        section->AddLoop( this );
    }
}

void
UOdysseyVectorLoop::BuildSegmentCubic( std::vector<BLPoint>& iPointArray
                                     , FOdysseyVectorSegmentCubic& iSegment
                                     , double iFromT
                                     , double iToT )
{
    std::vector<FPolygon>& polygonCache = iSegment.GetPolygonCache();
    uint32 polyCount = iSegment.GetPolygonCount();
    bool revert = ( iFromT < iToT ) ? false : true;

    if ( revert == false )
    {
        for( uint32 i = 0; i < polyCount; i++ )
        {
            if( polygonCache[i].toT >= iFromT && polygonCache[i].fromT <= iToT )
            {
                ::ULIS::FVec2D to   = { polygonCache[i].lineVertex[1].x, polygonCache[i].lineVertex[1].y };
                ::ULIS::FVec2D from = { polygonCache[i].lineVertex[0].x, polygonCache[i].lineVertex[0].y };

                // clipping part
                if ( polygonCache[i].toT > iToT )
                {
                    ::ULIS::FVec2D dir = polygonCache[i].lineVertex[1] - polygonCache[i].lineVertex[0];

                    to.x = polygonCache[i].lineVertex[0].x + dir.x * ( ( iToT - polygonCache[i].fromT ) / ( polygonCache[i].toT - polygonCache[i].fromT ) );
                    to.y = polygonCache[i].lineVertex[0].y + dir.y * ( ( iToT - polygonCache[i].fromT ) / ( polygonCache[i].toT - polygonCache[i].fromT ) );
                }

                BLPoint p = { to.x, to.y };

                /*iPointArray.push_back(p);*/

                mPath.lineTo( p );
            }
        }
    }
    else
    {
        double tmp = iFromT;
        iFromT = iToT;
        iToT = tmp;

        for( int i = polyCount - 1; i > 0; i-- )
        {
            if( polygonCache[i].toT >= iFromT && polygonCache[i].fromT <= iToT )
            {
                ::ULIS::FVec2D to   = { polygonCache[i].lineVertex[1].x, polygonCache[i].lineVertex[1].y };
                ::ULIS::FVec2D from = { polygonCache[i].lineVertex[0].x, polygonCache[i].lineVertex[0].y };

                // clipping part
                if ( polygonCache[i].fromT < iFromT )
                {
                    ::ULIS::FVec2D dir = polygonCache[i].lineVertex[1] - polygonCache[i].lineVertex[0];

                    from.x = polygonCache[i].lineVertex[0].x + dir.x * ( ( iFromT - polygonCache[i].fromT ) / ( polygonCache[i].toT - polygonCache[i].fromT ) );
                    from.y = polygonCache[i].lineVertex[0].y + dir.y * ( ( iFromT - polygonCache[i].fromT ) / ( polygonCache[i].toT - polygonCache[i].fromT ) );
                }

                BLPoint p = { from.x, from.y };

                /*iPointArray.push_back(p);*/

                mPath.lineTo( p );
            }
        }
    }
}

void
UOdysseyVectorLoop::DrawPoints( ::ULIS::FRectD& iRoi )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    blctx.setStrokeStyle( BLRgba32( 0xFFFF8000 ) );
    blctx.setFillStyle( BLRgba32( 0xFFFF8000 ) );

    if ( mPointList.size() ) 
    {
        for( std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it )
        {
            FOdysseyVectorPoint* point = static_cast<FOdysseyVectorPoint*>(*it);
            ::ULIS::FVec2D& pointAt = point->GetCoords();

            blctx.fillRect ( pointAt.x - 5, pointAt.y - 5, 10, 10 );
        }
    }
}

void
UOdysseyVectorLoop::Build()
{
    int seg = 0;

    mPath.clear();
/*
     mPointArray.clear();
     mPointArray.reserve(200);
*/
    if ( mSectionList.size() ) 
    {
        FOdysseyVectorSection* firstSection = mSectionList.front();
        FOdysseyVectorSegment& firstSegment = firstSection->GetSegment();
        ::ULIS::FVec2D originAt = mLoopPoint->GetPosition( firstSegment );
        FOdysseyVectorPoint* currentPoint = mLoopPoint;

        mPath.moveTo( originAt.x, originAt.y );

        for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
        {
            FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
            FOdysseyVectorSegment& segment = section->GetSegment();
            FOdysseyVectorPoint* nextPoint = ( currentPoint == section->GetPoint(0) ) ? section->GetPoint(1) : section->GetPoint(0);
            double currentPointT = currentPoint->GetT( segment );
            double    nextPointT =    nextPoint->GetT( segment );

            BuildSegmentCubic ( mPointArray, static_cast<FOdysseyVectorSegmentCubic&>(segment), currentPointT, nextPointT );

            currentPoint = nextPoint;
        }

        mPath.close();
    }
}


void
UOdysseyVectorLoop::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    if ( IsFilled() )
    {
    /*if ( mPointList.size() ) 
    {*/
       blctx.setFillStyle( BLRgba32( mFillColor ) );

       /*iBLContext.fillPolygon( &mPointArray[0], mPointArray.size() );*/
       blctx.fillPath( mPath );
    /*}*/
    }
}
