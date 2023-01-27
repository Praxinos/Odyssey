#include "OdysseyVectorLoop.h"

FOdysseyVectorLoop::~FOdysseyVectorLoop()
{
}

//static
FOdysseyVectorLoop::FOdysseyVectorLoop( UOdysseyVectorObject& iParent
                                      , uint64 iID
                                      , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                      , std::vector<FOdysseyVectorSection*>& iSectionArray )
    : mParent( iParent )
    , mID( iID )
    , mColor( 0xFF808080 )
    , mBucket( nullptr )
{
    Cast<UOdysseyVectorVertexIntersection>(iVertexArray[0])->AttachLoop( this );

    Build( iVertexArray, iSectionArray );
}

// static
uint64
FOdysseyVectorLoop::GenerateID( std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    uint64 loopID = 0;

    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        loopID = loopID ^ ( uint64 ) iSectionArray[i];
    }

    return loopID;
}

// static
bool
FOdysseyVectorLoop::Exists( uint64 iID
                          , std::vector<UOdysseyVectorVertex*>& iVertexArray
                          , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        if( iVertexArray[i]->GetClass() == UOdysseyVectorVertexIntersection::StaticClass() )
        {
            UOdysseyVectorVertexIntersection* intersectionVertex = Cast<UOdysseyVectorVertexIntersection>( iVertexArray[i] );
            FOdysseyVectorLoop* cycle = intersectionVertex->GetLoop();

            if( cycle )
            {
                if( cycle->mID == iID )
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void
FOdysseyVectorLoop::BuildSegmentCubic( UOdysseyVectorSegmentCubic& iSegment
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

                mPath.lineTo( to.x, to.y );
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

                mPath.lineTo( from.x, from.y );
            }
        }
    }
}

void
FOdysseyVectorLoop::Build( std::vector<UOdysseyVectorVertex*>& iVertexArray
                         , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    int seg = 0;

    mPath.clear();

     //mPointArray.clear();
     //mPointArray.reserve(200);

    if ( iVertexArray.size() ) 
    {
        FOdysseyVectorSection* firstSection = iSectionArray.front();
        UOdysseyVectorSegment* firstSegment = firstSection->GetSegment();
        ::ULIS::FVec2D originAt = iVertexArray[0]->GetPosition( *firstSegment );
        UOdysseyVectorVertex* currentVertex = iVertexArray[0];

        mPath.moveTo( originAt.x, originAt.y );

        for( int i = 0; i < iSectionArray.size(); i++ )
        {
            FOdysseyVectorSection* section = iSectionArray[i];
            UOdysseyVectorSegment* segment = section->GetSegment();
            UOdysseyVectorVertex* nextVertex = ( currentVertex == section->GetVertex(0) ) ? section->GetVertex(1) : section->GetVertex(0);
            double currentVertexT = currentVertex->GetT( *segment );
            double    nextVertexT =    nextVertex->GetT( *segment );

            BuildSegmentCubic ( static_cast<UOdysseyVectorSegmentCubic&>(*segment), currentVertexT, nextVertexT );

            currentVertex = nextVertex;
        }

        mPath.close();
    }

/*
    mPath.clear();
UE_LOG(LogTemp, Warning, TEXT("FOdysseyVectorLoop::Build: Array size %d"), iSectionArray.size() );
    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        FOdysseyVectorSection* section = iSectionArray[i];
        UOdysseyVectorSegment* segment = section->GetSegment();
        ::ULIS::FVec2D originAt = iVertexArray[i]->GetPosition( *segment );
//UE_LOG(LogTemp, Warning, TEXT("pointAt %d %f %f"), segment, originAt.x, originAt.y );
        if( i == 0 ) mPath.moveTo( originAt.x, originAt.y );
        else         mPath.lineTo( originAt.x, originAt.y );
    }
*/
    mPath.close();
}

uint32
FOdysseyVectorLoop::GetColor()
{
    return ( mBucket ) ? mBucket->GetColor() : 0xFF808080;
}

void
FOdysseyVectorLoop::SetBucket( FOdysseyVectorBucket* iBucket )
{
    mBucket = iBucket;
}

FOdysseyVectorBucket*
FOdysseyVectorLoop::GetBucket()
{
    return mBucket;
}

bool
FOdysseyVectorLoop::HitTest( double iX, double iY )
{
/*
    mPath.clear();
    mPath.moveTo(0,0);
    mPath.lineTo(0,0);
*/
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();
    BLPoint pt = { iX, iY };
    BLPoint* vertex = ( BLPoint*) mPath.vertexData();

    // WARNING: looks like in this version the return value is a bool (in the shape of an int) but in later version is a enum value. We will have to fix that.
    uint32 ret = mPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

    return ( ret ) ? true : false;
}

void
FOdysseyVectorLoop::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();

    /*if ( mVertexList.size() ) 
    {*/
       blctx->setFillStyle( BLRgba32( /*mFillColor*/GetColor() ) );

       /*iBLContext.fillPolygon( &mPointArray[0], mPointArray.size() );*/
       blctx->fillPath( mPath );
    /*}*/
}
