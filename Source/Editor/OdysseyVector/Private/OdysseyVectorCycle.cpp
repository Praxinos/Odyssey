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
    , mPropagatedBucket( nullptr )
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
    mVertexArray.insert( mVertexArray.end(), iMergeCycle->mVertexArray.begin(), iMergeCycle->mVertexArray.end() );
    mSectionArray.insert( mSectionArray.end(), iMergeCycle->mSectionArray.begin(), iMergeCycle->mSectionArray.end() );

    for( int i = 0; i < iMergeCycle->mSectionArray.size(); i++ )
    {
        iMergeCycle->mSectionArray[i]->AddCycle( this );
    }

    mPath.addPath( iMergeCycle->mPath );
/*
    mMin.x = ::ULIS::FMath::Min( mMin.x, iMergeCycle->mMin.x );
    mMin.y = ::ULIS::FMath::Min( mMin.y, iMergeCycle->mMin.y );
    mMax.x = ::ULIS::FMath::Max( mMax.x, iMergeCycle->mMax.x );
    mMax.y = ::ULIS::FMath::Max( mMax.y, iMergeCycle->mMax.y );
*/
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
    // First test : get sure they don't share a common section
    for( int i = 0; i < mSectionArray.size(); i++ )
    {
        if( mSectionArray[i]->GetOtherCycle( this ) == iParentCandidate )
        {
            return false;
        }
    }

    for( int i = 0; i < mVertexArray.size(); i++ )
    {
        ::ULIS::FVec2D& vCoords = mVertexArray[i]->GetCoords();
        BLPoint pt = { vCoords.x, vCoords.y };
        uint32 ret =  iParentCandidate->mPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

        if( ret != BL_HIT_TEST_IN )
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
    ::ULIS::FVec2D& point0 = iSegment.GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSegment.GetVertex(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = iSegment.GetHandle(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = iSegment.GetHandle(1)->GetCoords();
    ::ULIS::FVec2D samplePoint0;
    ::ULIS::FVec2D samplePoint1;
    ::ULIS::FVec2D sampleCtrlPoint0;
    ::ULIS::FVec2D sampleCtrlPoint1;

    if( iFromT < iToT )
    {
        FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                     , iFromT, iToT
                                     , samplePoint0, sampleCtrlPoint0, sampleCtrlPoint1, samplePoint1 );

        mPath.cubicTo( BLPoint( sampleCtrlPoint0.x, sampleCtrlPoint0.y )
                     , BLPoint( sampleCtrlPoint1.x, sampleCtrlPoint1.y )
                     , BLPoint( samplePoint1.x, samplePoint1.y ) );
    }
    else
    {
        FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                     , iToT, iFromT
                                     , samplePoint0, sampleCtrlPoint0, sampleCtrlPoint1, samplePoint1 );

        mPath.cubicTo( BLPoint( sampleCtrlPoint1.x, sampleCtrlPoint1.y )
                     , BLPoint( sampleCtrlPoint0.x, sampleCtrlPoint0.y )
                     , BLPoint( samplePoint0.x, samplePoint0.y ) );
    }
}

/*
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
*/

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

        //mMin.x = mMax.x = originAt.x;
        //mMin.y = mMax.y = originAt.y;

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

            double currentVertexT = vertexi->GetT( segment );
            double    nextVertexT = vertexn->GetT( segment );

            BuildSegmentCubic ( static_cast<FOdysseyVectorSegmentCubic&>(*segment), currentVertexT, nextVertexT );
        }

        mPath.close();
    }
}

void
FOdysseyVectorCycle::SetBucket( FOdysseyVectorBucket* iBucket )
{
    mBucket = iBucket;

    if( mBucket )
    {
        mPropagatedBucket = mBucket->IsPropagated() ? mBucket : nullptr;
    }
    else
    {
        mPropagatedBucket = nullptr;
    }
}

FOdysseyVectorBucket*
FOdysseyVectorCycle::GetBucket()
{
    return mBucket;
}

void
FOdysseyVectorCycle::SetPropagatedBucket( FOdysseyVectorBucket* iPropagatedBucket )
{
    mPropagatedBucket = iPropagatedBucket;
}

FOdysseyVectorBucket*
FOdysseyVectorCycle::GetPropagatedBucket()
{
    return mPropagatedBucket;
}

bool
FOdysseyVectorCycle::PropagateBucket()
{
    for( int i = 0; i < mSectionArray.size(); i++ )
    {
        FOdysseyVectorCycle* neighbourCycle = mSectionArray[i]->GetOtherCycle( this );

        if( neighbourCycle )
        {
            FOdysseyVectorBucket* neighbourPropagatedBucket = neighbourCycle->GetPropagatedBucket();

            if( neighbourPropagatedBucket )
            {
                this->SetPropagatedBucket( neighbourPropagatedBucket );

                return true;
            }
        }
    }

    return false;
}

std::vector<FOdysseyVectorSection*>&
FOdysseyVectorCycle::GetSectionArray()
{
    return mSectionArray;
}

bool
FOdysseyVectorCycle::HitTest( double iX, double iY )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
    BLPoint pt = { iX, iY };

    // WARNING: looks like in this version the return value is a bool (in the shape of an int) but in later version is a enum value. We will have to fix that.
    uint32 ret = mPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

    return ( ret == BL_HIT_TEST_IN ) ? true : false;
}

void
FOdysseyVectorCycle::FillPath()
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
    blctx->setFillRule( BL_FILL_RULE_EVEN_ODD );
    blctx->fillPath( mPath );
}

void
FOdysseyVectorCycle::StrokePath( bool iWorld )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();

    if( iWorld == true )
    {
        BLPath worldPath = mPath;

        worldPath.transform( mParent.GetWorldMatrix() );

        blctx->save();
        blctx->resetMatrix();
        blctx->strokePath( worldPath );
        blctx->restore();
    }
    else
    {
        blctx->strokePath( mPath );
    }
}

static void
ShowCycle( std::vector<FOdysseyVectorVertex*>& vertexArray
          , std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = sectionArray[i]->GetSegment();
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
        FOdysseyVectorPath* path = vertex0->GetPath();
        BLPoint pt0 = path->GetWorldMatrix().mapPoint( vertex0->GetCoords().x, vertex0->GetCoords().y );
        BLPoint pt1 = path->GetWorldMatrix().mapPoint( vertex1->GetCoords().x, vertex1->GetCoords().y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

void
FOdysseyVectorCycle::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
    FOdysseyVectorBucket* bucket = mBucket ? mBucket : mPropagatedBucket;
/*
    BLPath combinedPath = mPath;

    for( std::list<FOdysseyVectorCycle*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorCycle *child = (*oit);

        combinedPath.addPath( child->mPath );
    }
*/
    if( bucket )
    {
        if( bucket->IsGradient() )
        {
/*
            double difX = mMax.x - mMin.x;
            double difY = mMax.y - mMin.y;
            double linearMinX = mMin.x;
            double linearMinY = mMin.y;
            double linearMaxX = mMax.x;
            double linearMaxY = mMax.y;
            BLGradient linear( BLLinearGradientValues( linearMinX, linearMinY, linearMaxX, linearMaxY ) );
            FColor& gradientColor0 = bucket->GetGradientColor0();
            FColor& gradientColor1 = bucket->GetGradientColor1();
            BLRgba32 BLColor0;
            BLRgba32 BLColor1;
            double angle = bucket->GetGradientRotationInDegrees() * M_PI / 180;

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
*/
        }
        else
        {
            FColor& color = bucket->GetColor();
            BLRgba32 BLColor;

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor.setR( color.B );
            BLColor.setG( color.G );
            BLColor.setB( color.R );
            BLColor.setA( color.A );

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
    //blctx->strokePath( mPath );
    /*}*/
}

uint64
FOdysseyVectorCycle::GetID()
{
    return mID;
}
