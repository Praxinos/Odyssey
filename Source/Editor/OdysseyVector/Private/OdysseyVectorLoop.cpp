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
    , mColor( 0xA0, 0xA0, 0xA0, 0xFF )
    , mBucket( nullptr )
    , mVertexArray (iVertexArray)
    , mSectionArray (iSectionArray)
    , mFlags (0)
    , mValence (0)
    , mParentCycle( nullptr )
{
/*
    Cast<UOdysseyVectorVertexIntersection>(iVertexArray[0])->AttachLoop( this );
*/


    Build( mVertexArray, mSectionArray );
}

FOdysseyVectorLoop*
FOdysseyVectorLoop::GetParentCycle()
{
    return mParentCycle;
}

void
FOdysseyVectorLoop::AppendChild( FOdysseyVectorLoop *iChild )
{
    mChildrenList.push_back( iChild );

    iChild->mParentCycle = this;
}

void
FOdysseyVectorLoop::RemoveChild( FOdysseyVectorLoop *iChild )
{
    mChildrenList.remove( iChild );
}

bool
FOdysseyVectorLoop::FitsIn( FOdysseyVectorLoop* iParentCandidate )
{
    for( int i = 0; i < mVertexArray.size(); i++ )
    {
        ::ULIS::FVec2D& vCoords = mVertexArray[i]->GetCoords();
        BLPoint pt = { vCoords.x, vCoords.y };
        uint32 ret =  iParentCandidate->mPath.hitTest( pt, BL_FILL_RULE_NON_ZERO );

        // we also need to test if the point lies on the loop boundaries
        /*if( ret == 0 )
        {
            for ( int j = 0; j < iParentCandidate->mVertexArray.size(); j++ )
            {
                if( mVertexArray[i] == iParentCandidate->mVertexArray[j] )
                {
                    ret = 1;
                }
            }
        }*/

        if( ret == 0 )
        {
            return false;
        }
    }

    return true;
}

std::vector<UOdysseyVectorVertex*>& 
FOdysseyVectorLoop::GetVertexArray()
{
    return mVertexArray;
}

std::vector<FOdysseyVectorSection*>&
FOdysseyVectorLoop::GetSectionArray()
{
    return mSectionArray;
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
/*
FOdysseyVectorLoop*
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
                    return cycle;
                }
            }
        }
    }

    return nullptr;
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

                // take advantage of this func to set the bouding limits.
                if ( to.x < mMin.x ) mMin.x = to.x;
                if ( to.y < mMin.y ) mMin.y = to.y;
                if ( to.x > mMax.x ) mMax.x = to.x;
                if ( to.y > mMax.y ) mMax.y = to.y;
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

                // take advantage of this func to set the bouding limits.
                if ( to.x < mMin.x ) mMin.x = to.x;
                if ( to.y < mMin.y ) mMin.y = to.y;
                if ( to.x > mMax.x ) mMax.x = to.x;
                if ( to.y > mMax.y ) mMax.y = to.y;
            }
        }
    }
}

uint32
FOdysseyVectorLoop::GetValence()
{
    return mValence;
}

void
FOdysseyVectorLoop::Build( std::vector<UOdysseyVectorVertex*>& iVertexArray
                         , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    /*double xmin, ymin, xmax, ymax;*/
    int seg = 0;

    mValence = 0;
    mPath.clear();

     //mPointArray.clear();
     //mPointArray.reserve(200);

    if ( iVertexArray.size() ) 
    {
        FOdysseyVectorSection* firstSection = iSectionArray.front();
        UOdysseyVectorSegment* firstSegment = firstSection->GetSegment();
        ::ULIS::FVec2D originAt = iVertexArray[0]->GetPosition( *firstSegment );
        UOdysseyVectorVertex* currentVertex = iVertexArray[0];

        mMin.x = mMax.x = originAt.x;
        mMin.y = mMax.y = originAt.y;

        for( int i = 0; i < iSectionArray.size(); i++ )
        {
            FOdysseyVectorSection* section = iSectionArray[i];
            UOdysseyVectorSegment* segment = section->GetSegment();
            UOdysseyVectorVertex* nextVertex = ( currentVertex == section->GetVertex(0) ) ? section->GetVertex(1) : section->GetVertex(0);
            double currentVertexT = currentVertex->GetT( *segment );
            double    nextVertexT =    nextVertex->GetT( *segment );
            ::ULIS::FVec2D currentAt = currentVertex->GetPosition( *segment );

            if( i == 0 )
            {
                mPath.moveTo( currentAt.x, currentAt.y );
            }
            else
            {
                mPath.lineTo( currentAt.x, currentAt.y );
            }

            BuildSegmentCubic ( static_cast<UOdysseyVectorSegmentCubic&>(*segment), currentVertexT, nextVertexT );

            currentVertex = nextVertex;

            if( iVertexArray[i]->GetClass() == UOdysseyVectorVertexIntersection::StaticClass() )
            {
                mValence++;
            }
        }

        mPath.close();
    }

/*
    mPath.clear();
//UE_LOG(LogTemp, Warning, TEXT("FOdysseyVectorLoop::Build: Array size %d"), iSectionArray.size() );
    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        FOdysseyVectorSection* section = iSectionArray[i];
        UOdysseyVectorSegment* segment = section->GetSegment();
        ::ULIS::FVec2D originAt = iVertexArray[i]->GetPosition( *segment );
//UE_LOG(LogTemp, Warning, TEXT("pointAt %d %f %f"), segment, originAt.x, originAt.y );
        if( i == 0 ) mPath.moveTo( originAt.x, originAt.y );
        else         mPath.lineTo( originAt.x, originAt.y );
    }

    mPath.close();
*/
}

FColor
FOdysseyVectorLoop::GetColor()
{
    return ( mBucket ) ? mBucket->GetColor() : mColor;
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
    BLPath combinedPath = mPath;

    for( std::list<FOdysseyVectorLoop*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorLoop *child = (*oit);

        combinedPath.addPath( child->mPath );
    }

    // WARNING: looks like in this version the return value is a bool (in the shape of an int) but in later version is a enum value. We will have to fix that.
    uint32 ret = combinedPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

    return ( ret ) ? true : false;
}

void
FOdysseyVectorLoop::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();
    BLPath combinedPath = mPath;

    for( std::list<FOdysseyVectorLoop*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorLoop *child = (*oit);

        combinedPath.addPath( child->mPath );
    }

    if( mBucket )
    {
        if( mBucket->IsGradient() )
        {
            double difX = mMax.x - mMin.x;
            double difY = mMax.y - mMin.y;
            double linearMinX = mMin.x;
            double linearMinY = mMin.y;
            double angle = acos( fabs( mBucket->GetHandleDotProduct() ) );
            double linearMaxX = mMin.x + ( difX * cos( angle ) );
            double linearMaxY = mMin.y + ( difY * sin( angle ) );
            BLGradient linear( BLLinearGradientValues( mMin.x, mMin.y, linearMaxX, linearMaxY ) );
            FColor& gradientColor0 = mBucket->GetGradientColor0();
            FColor& gradientColor1 = mBucket->GetGradientColor1();
            BLRgba32 BLColor0;
            BLRgba32 BLColor1;

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor0.r = gradientColor0.B;
            BLColor0.g = gradientColor0.G;
            BLColor0.b = gradientColor0.R;
            BLColor0.a = gradientColor0.A;

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor1.r = gradientColor1.B;
            BLColor1.g = gradientColor1.G;
            BLColor1.b = gradientColor1.R;
            BLColor1.a = gradientColor1.A;

            linear.addStop( 0.0, BLColor0 );
            linear.addStop( 1.0, BLColor1 );

            blctx->setFillStyle( linear );
        }
        else
        {
            FColor& gradientColor = mBucket->GetColor();
            BLRgba32 BLColor;

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor.r = gradientColor.B;
            BLColor.g = gradientColor.G;
            BLColor.b = gradientColor.R;
            BLColor.a = gradientColor.A;

            blctx->setFillStyle( BLColor );
        }
    }
    else
    {
       blctx->setFillStyle( BLRgba32( 0xFFA0A0A0 ) );
    }

    blctx->setFillRule( BL_FILL_RULE_EVEN_ODD );

       /*iBLContext.fillPolygon( &mPointArray[0], mPointArray.size() );*/
    blctx->fillPath( combinedPath );
    /*}*/
}

void 
FOdysseyVectorLoop::SetMarched( bool iMarched )
{
    if( iMarched == true )
    {
        mFlags |= MARCHED;
    }
    else
    {
        mFlags &= (~MARCHED);
    }
}

bool
FOdysseyVectorLoop::IsMarched()
{
    return ( mFlags & MARCHED ) ? true : false;
}


uint64
FOdysseyVectorLoop::GetID()
{
    return mID;
}
