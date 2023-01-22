#include "OdysseyVectorLoop.h"

FOdysseyVectorLoop::~FOdysseyVectorLoop()
{
}

//static
FOdysseyVectorLoop::FOdysseyVectorLoop( UOdysseyVectorObject& iParent
                                      , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                      , std::vector<FOdysseyVectorSection*>& iSectionArray )
    : mParent( iParent )
{
    Build( iVertexArray, iSectionArray );
}

/*
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
*/


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

/*
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
*/

    mPath.clear();

    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        FOdysseyVectorSection* section = iSectionArray[i];
        UOdysseyVectorSegment* segment = section->GetSegment();
        ::ULIS::FVec2D originAt = iVertexArray[i]->GetPosition( *segment );
UE_LOG(LogTemp, Warning, TEXT("pointAt %d %f %f"), segment, originAt.x, originAt.y );
        if( i == 0 ) mPath.moveTo( originAt.x, originAt.y );
        else         mPath.lineTo( originAt.x, originAt.y );
    }

    mPath.close();
}

void
FOdysseyVectorLoop::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();

    /*if ( mVertexList.size() ) 
    {*/
       blctx->setFillStyle( BLRgba32( /*mFillColor*/0xFF0080FF ) );

       /*iBLContext.fillPolygon( &mPointArray[0], mPointArray.size() );*/
       blctx->fillPath( mPath );
    /*}*/
}
