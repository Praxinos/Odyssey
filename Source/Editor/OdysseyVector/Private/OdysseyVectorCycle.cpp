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

    mCombinedPath.addPath( iMergeCycle->mContourPath );
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
        uint32 ret =  iParentCandidate->mContourPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

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

        mContourPath.cubicTo( BLPoint( sampleCtrlPoint0.x, sampleCtrlPoint0.y )
                            , BLPoint( sampleCtrlPoint1.x, sampleCtrlPoint1.y )
                            , BLPoint( samplePoint1.x, samplePoint1.y ) );
    }
    else
    {
        FOdysseyVector::BezierExtract( point0, ctrlPoint0, ctrlPoint1, point1
                                     , iToT, iFromT
                                     , samplePoint0, sampleCtrlPoint0, sampleCtrlPoint1, samplePoint1 );

        mContourPath.cubicTo( BLPoint( sampleCtrlPoint1.x, sampleCtrlPoint1.y )
                            , BLPoint( sampleCtrlPoint0.x, sampleCtrlPoint0.y )
                            , BLPoint( samplePoint0.x, samplePoint0.y ) );
    }
}

void
FOdysseyVectorCycle::Build( std::vector<FOdysseyVectorVertex*>& iVertexArray
                          , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    int32 arraySize = iVertexArray.size();
    int seg = 0;

    if ( iVertexArray.size() ) 
    {
        ::ULIS::FVec2D originAt = iVertexArray[0]->GetCoords();

        mContourPath.moveTo( originAt.x, originAt.y );

        for( int i = 0; i < arraySize; i++ )
        {
            int n = ( i + 1 ) % arraySize;
            FOdysseyVectorSection* section = iSectionArray[i];
            FOdysseyVectorVertex* sectionVertex0 = section->GetVertex(0);
            FOdysseyVectorVertex* sectionVertex1 = section->GetVertex(1);
            ::ULIS::FVec2D* sectionBezier = section->GetBezier();
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

            // check it goes the same direction
            if( vertexi == sectionVertex0 )
            {
                mContourPath.cubicTo( sectionBezier[1].x, sectionBezier[1].y
                                    , sectionBezier[2].x, sectionBezier[2].y
                                    , sectionBezier[3].x, sectionBezier[3].y );
            }
            else
            {
                mContourPath.cubicTo( sectionBezier[2].x, sectionBezier[2].y
                                    , sectionBezier[1].x, sectionBezier[1].y
                                    , sectionBezier[0].x, sectionBezier[0].y );
            }
        }

        mContourPath.close();
    }

    mCombinedPath = mContourPath;
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
    uint32 ret = mCombinedPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

    return ( ret == BL_HIT_TEST_IN ) ? true : false;
}

void
FOdysseyVectorCycle::FillPath()
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();

    blctx->setFillRule( BL_FILL_RULE_EVEN_ODD );
    blctx->fillPath( mCombinedPath );
}

void
FOdysseyVectorCycle::StrokePath( bool iWorld )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();

    if( iWorld == true )
    {
        BLPath worldPath = mCombinedPath;

        worldPath.transform( mParent.GetWorldMatrix() );

        blctx->save();
        blctx->resetMatrix();
        blctx->strokePath( worldPath );
        blctx->restore();
    }
    else
    {
        blctx->strokePath( mCombinedPath );
    }

    blctx->flush( BL_CONTEXT_FLUSH_SYNC );
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
    BLMatrix2D& worldMatrix = mParent.GetWorldMatrix();
    BLBox bbox;

    mContourPath.getBoundingBox( &bbox );

    if( bucket )
    {
        if( bucket->IsGradient() )
        {
            double linearMinX = bbox.x0;
            double linearMinY = bbox.y0;
            double linearMaxX = bbox.x1;
            double linearMaxY = bbox.y1;
            BLGradient linear( BLLinearGradientValues( linearMinX, linearMinY, linearMaxX, linearMaxY ) );
            FColor& gradientColor0 = bucket->GetGradientColor0();
            FColor& gradientColor1 = bucket->GetGradientColor1();
            BLRgba32 BLColor0;
            BLRgba32 BLColor1;
            double angle = bucket->GetGradientRotationInDegrees() * M_PI / 180;

            linear.rotate( angle );

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor0.setR( gradientColor0.B );
            BLColor0.setG( gradientColor0.G );
            BLColor0.setB( gradientColor0.R );
            BLColor0.setA( gradientColor0.A );

            // Note: Blend2D color format is 0xAARRGGBB
            BLColor1.setR( gradientColor1.B );
            BLColor1.setG( gradientColor1.G );
            BLColor1.setB( gradientColor1.R );
            BLColor1.setA( gradientColor1.A );

            linear.addStop( 0.0, BLColor0 );
            linear.addStop( 1.0, BLColor1 );

            blctx->setStrokeStyle( linear );
            blctx->setFillStyle( linear );
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
    blctx->fillPath( mCombinedPath );

    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeWidth( 1.0f );

    // stroke borders or else there will be a small 1 pixel gap. We draw it only once: the cycle responsible for drawing the 
    // section is the cycle that was first attached to the section. That way we don't draw it twice. The paint group could be
    // responsible for drawing the sections as well, but then we have to retrieve the bucket color, if any. this would be to
    // complicated. We draw in world coordinates to be sure to get 1 pixel-width strokes.
    for( int i = 0; i < mSectionArray.size(); i++ )
    {
        FOdysseyVectorSection* section = mSectionArray[i];

        if( section->GetCycle(0) == this )
        {
            ::ULIS::FVec2D* sectionBezier = section->GetBezier();
            BLPoint pt[4] = { worldMatrix.mapPoint( sectionBezier[0].x, sectionBezier[0].y )
                            , worldMatrix.mapPoint( sectionBezier[1].x, sectionBezier[1].y )
                            , worldMatrix.mapPoint( sectionBezier[2].x, sectionBezier[2].y )
                            , worldMatrix.mapPoint( sectionBezier[3].x, sectionBezier[3].y ) };
            BLPath path;

            path.moveTo( pt[0] );
            path.cubicTo( pt[1], pt[2], pt[3] );

            blctx->strokePath( path );
        }
    }

    blctx->restore();
}

uint64
FOdysseyVectorCycle::GetID()
{
    return mID;
}
