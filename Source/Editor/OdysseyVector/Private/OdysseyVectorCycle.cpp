#include "OdysseyVectorCycle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorCycle::~FOdysseyVectorCycle()
{
}

//static
FOdysseyVectorCycle::FOdysseyVectorCycle( FOdysseyVectorObject& iParent
                                        , uint64 iID
                                        , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                        , std::vector<FOdysseyVectorSection*>& iSectionArray )
    : mParent( iParent )
    , mID( iID )
    , mBucket( nullptr )
    , mVertexArray (iVertexArray)
    , mSectionArray (iSectionArray)
    , mFlags (0)
    , mParentCycle( nullptr )
    , mPropagated( false )
{
    Build( mVertexArray, mSectionArray );
}

void
FOdysseyVectorCycle::Merge( FOdysseyVectorCycle* iMergeCycle )
{
    mVertexArray.insert(mVertexArray.end(), iMergeCycle->mVertexArray.begin(), iMergeCycle->mVertexArray.end());
    mSectionArray.insert(mSectionArray.end(), iMergeCycle->mSectionArray.begin(), iMergeCycle->mSectionArray.end());


    Build( iMergeCycle->mVertexArray, iMergeCycle->mSectionArray );
}

FOdysseyVectorCycle*
FOdysseyVectorCycle::GetParentCycle()
{
    return mParentCycle;
}

void
FOdysseyVectorCycle::SetParentCycle( FOdysseyVectorCycle *iParent )
{
    this->mParentCycle = iParent;
}

bool
FOdysseyVectorCycle::FitsIn( FOdysseyVectorCycle* iParentCandidate )
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

// static
uint64
FOdysseyVectorCycle::GenerateID( std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    uint64 loopID = 0;

    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        loopID = loopID ^ ( uint64 ) iSectionArray[i];
    }

    return loopID;
}

void
FOdysseyVectorCycle::BuildSegmentCubic( FOdysseyVectorSegmentCubic& iSegment
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

void
FOdysseyVectorCycle::Build( std::vector<FOdysseyVectorVertex*>& iVertexArray
                          , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    int32 arraySize = iVertexArray.size();
    /*double xmin, ymin, xmax, ymax;*/
    int seg = 0;

    //mPath.clear();

     //mPointArray.clear();
     //mPointArray.reserve(200);

    if ( iVertexArray.size() ) 
    {
        ::ULIS::FVec2D originAt = iVertexArray[0]->GetCoords();

        mMin.x = mMax.x = originAt.x;
        mMin.y = mMax.y = originAt.y;

        mPath.moveTo( originAt.x, originAt.y );

        for( int i = 0; i < arraySize; i++ )
        {
            int n = ( i + 1 ) % arraySize;
            FOdysseyVectorSection* section = iSectionArray[i];
            FOdysseyVectorSegment* segment = section->GetSegment();
            FOdysseyVectorVertex* vertexi = iVertexArray[i];
            FOdysseyVectorVertex* vertexn = iVertexArray[n];

            section->AddCycle( this );

            if( vertexn->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(vertexn);

                if( ( iSectionArray[i]->GetSegment() != iSectionArray[n]->GetSegment() )
                 || ( intersectionVertex->SelfIntersects() == true ) )
                {
                    vertexn = intersectionVertex->GetPartner();
                }
            }

            if( segment )
            {
                double currentVertexT = vertexi->GetT( segment );
                double    nextVertexT = vertexn->GetT( segment );
                /*::ULIS::FVec2D currentAt = currentVertex->GetPosition( segment );

                if( i == 0 )
                {
                    mPath.moveTo( currentAt.x, currentAt.y );
                }
                else
                {
                    mPath.lineTo( currentAt.x, currentAt.y );
                }*/

                BuildSegmentCubic ( static_cast<FOdysseyVectorSegmentCubic&>(*segment), currentVertexT, nextVertexT );
            }
            else
            {
                ::ULIS::FVec2D bridgeTo = vertexn->GetCoords();

                mPath.lineTo( bridgeTo.x, bridgeTo.y );
            }
        }

        //mPath.close();
    }
}

void
FOdysseyVectorCycle::SetBucket( FOdysseyVectorBucket* iBucket )
{
    mBucket = iBucket;
}

FOdysseyVectorBucket*
FOdysseyVectorCycle::GetBucket()
{
    return mBucket;
}

void
FOdysseyVectorCycle::SetPropagated( bool iPropagated )
{
    mPropagated = iPropagated;
}

bool
FOdysseyVectorCycle::IsPropagated()
{
    return mPropagated;
}

void
FOdysseyVectorCycle::PropagateBucket()
{
    if( mBucket )
    {
        for( int i = 0; i < mSectionArray.size(); i++ )
        {
            FOdysseyVectorCycle* otherCycle = mSectionArray[i]->GetOtherCycle( this );

            if( otherCycle )
            {
                if( otherCycle->GetBucket() == nullptr )
                {
                    otherCycle->SetBucket( mBucket );
                }
            }
        }
    }

    SetPropagated( true );
}

std::vector<FOdysseyVectorSection*>&
FOdysseyVectorCycle::GetSectionArray()
{
    return mSectionArray;
}

bool
FOdysseyVectorCycle::HitTest( double iX, double iY )
{
/*
    mPath.clear();
    mPath.moveTo(0,0);
    mPath.lineTo(0,0);
*/
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
    BLPoint pt = { iX, iY };
/*
    BLPoint* vertex = ( BLPoint*) mPath.vertexData();
    BLPath combinedPath = mPath;

    for( std::list<FOdysseyVectorCycle*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorCycle *child = (*oit);

        combinedPath.addPath( child->mPath );
    }
*/
    // WARNING: looks like in this version the return value is a bool (in the shape of an int) but in later version is a enum value. We will have to fix that.
    uint32 ret = mPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

    return ( ret ) ? true : false;
}

void
FOdysseyVectorCycle::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
/*
    BLPath combinedPath = mPath;

    for( std::list<FOdysseyVectorCycle*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorCycle *child = (*oit);

        combinedPath.addPath( child->mPath );
    }
*/
    if( mBucket )
    {
        if( mBucket->IsGradient() )
        {
            double difX = mMax.x - mMin.x;
            double difY = mMax.y - mMin.y;
            double linearMinX = mMin.x;
            double linearMinY = mMin.y;
            double linearMaxX = mMax.x;
            double linearMaxY = mMax.y;
            BLGradient linear( BLLinearGradientValues( linearMinX, linearMinY, linearMaxX, linearMaxY ) );
            FColor& gradientColor0 = mBucket->GetGradientColor0();
            FColor& gradientColor1 = mBucket->GetGradientColor1();
            BLRgba32 BLColor0;
            BLRgba32 BLColor1;
            double angle = mBucket->GetGradientRotationInDegrees() * M_PI / 180;

            linear.rotate( angle );

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

            blctx->setStrokeStyle( linear );
            blctx->setFillStyle( linear );
        }
        else
        {
            FColor& color = mBucket->GetColor();
            BLRgba32 BLColor;

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor.r = color.B;
            BLColor.g = color.G;
            BLColor.b = color.R;
            BLColor.a = color.A;

            blctx->setStrokeStyle( BLColor );
            blctx->setFillStyle( BLColor );
        }
    }
    else
    {
       blctx->setStrokeStyle( BLRgba32( 0xFFA0A0A0 ) );
       blctx->setFillStyle( BLRgba32( 0xFFA0A0A0 ) );
    }

    blctx->setFillRule( BL_FILL_RULE_EVEN_ODD );

       /*iBLContext.fillPolygon( &mPointArray[0], mPointArray.size() );*/
    //blctx->strokePath( combinedPath );
    blctx->fillPath( mPath );
    /*}*/
}

uint64
FOdysseyVectorCycle::GetID()
{
    return mID;
}
