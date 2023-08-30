#include "OdysseyVectorCycle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorCycle::~FOdysseyVectorCycle()
{
}

//static
FOdysseyVectorCycle::FOdysseyVectorCycle( FOdysseyVectorObject* iOwner
                                        , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                        , std::vector<FOdysseyVectorSection*>& iSectionArray )
    : mOwner( iOwner )
    , mBucket( nullptr )
    , mPropagatedBucket( nullptr )
    , mVertexArray (iVertexArray)
    , mSectionArray (iSectionArray)
    , mParentCycle( nullptr )
    , mPropagated( false )
{
    Build( mVertexArray, mSectionArray );
}

void
FOdysseyVectorCycle::Merge( FOdysseyVectorCycle* iMergeCycle )
{
    for( int i = 0; i < iMergeCycle->mSectionArray.size(); i++ )
    {
        // This will add the cycle C to the section ONLY if the section does not already
        // belongs to 2 cycles. Indeed, in the case described below, some sections may
        // not be boundary sections. For instance, the section noted as ===== already
        // belongs to cycles A and B.
        //   ___________________
        //  |    ___________    |
        //  |   |     A     |   |
        //  |   |===========| C |
        //  |   |     B     |   |
        //  |   |___________|   |
        //  |___________________|
        //
        // note: we use mCombinedPath only for the filling part.
        //
        if( iMergeCycle->mSectionArray[i]->GetCycleCount() < 2 )
        {
            iMergeCycle->mSectionArray[i]->AddCycle( this );

            mInnerSectionArray.push_back( iMergeCycle->mSectionArray[i] );
        }
    }

    mCombinedPath.addPath( iMergeCycle->mContourPath );
}

//static
void
FOdysseyVectorCycle::ToBucketArray( std::vector<FOdysseyVectorCycle*>& iCyleArray
                                  , std::vector<FOdysseyVectorBucket*>& oBucketArray )
{
    oBucketArray.clear();

    if( iCyleArray.size() ) 
    {
        oBucketArray.reserve( iCyleArray.size() );

        for( int i = 0; i < iCyleArray.size(); i++ )
        {
            FOdysseyVectorBucket* bucket = iCyleArray[i]->GetBucket();

            if( bucket )
            {
                oBucketArray.push_back( bucket );
            }
        }
    }
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
    // which would in that case mean that we do not fit in the parent cycle
    for( int i = 0; i < mSectionArray.size(); i++ )
    {
        if( mSectionArray[i]->GetOtherCycle( this ) == iParentCandidate )
        {
            return false;
        }
    }

    // Then check if all vertices lies within the parent candidate
    for( int i = 0; i < mSectionArray.size(); i++ )
    {
        ::ULIS::FVec2D vCoords = mSectionArray[i]->GetVertexCoords( mVertexArray[i] );
        BLPoint pt = BLPoint( vCoords.x, vCoords.y );
        uint32 ret = iParentCandidate->mContourPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

        if( ret != BL_HIT_TEST_IN )
        {
            return false;
        }
    }

    return true;
}

void
FOdysseyVectorCycle::Build( std::vector<FOdysseyVectorVertex*>& iVertexArray
                          , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    int32 arraySize = iSectionArray.size();
    int seg = 0;

    if ( iSectionArray.size() ) 
    {
        // Note: section::GetVertexCoords() return the coords in paintgroup's coordinates
        ::ULIS::FVec2D originAt = iSectionArray[0]->GetVertexCoords( iVertexArray[0] );

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

            // check if we need to revert the bezier. Indeed, a cycle is a combination of sections
            // that may not go the same way. We have to run through them the same way.
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
FOdysseyVectorCycle::PropagateBucket( std::vector<FOdysseyVectorSection*> iSectionArray )
{
    // test outer sections
    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        FOdysseyVectorCycle* neighbourCycle = iSectionArray[i]->GetOtherCycle( this );

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

bool
FOdysseyVectorCycle::PropagateBucket()
{
    // check outer sections for a propagated bucket
    if( PropagateBucket( mSectionArray ) == false )
    {
        // check inner sections for a propagated bucket
        if( PropagateBucket( mInnerSectionArray ) )
        {
            return true;
        }

        return false;
    }

    return true;
}

bool
FOdysseyVectorCycle::HitTest( double iX, double iY )
{
    BLContext* blctx = mOwner->GetScene()->GetEngine()->GetBLContext();
    BLPoint pt = { iX, iY };

    uint32 ret = mCombinedPath.hitTest( pt, BL_FILL_RULE_EVEN_ODD );

    return ( ret == BL_HIT_TEST_IN ) ? true : false;
}

FOdysseyVectorObject*
FOdysseyVectorCycle::GetOwner()
{
    return mOwner;
}

void
FOdysseyVectorCycle::StrokePath( bool iWorld )
{
    BLContext* blctx = mOwner->GetScene()->GetEngine()->GetBLContext();

    if( iWorld == true )
    {
        BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();
        BLPath contourPath = mContourPath;

        blctx->save();

        if( iWorld )
        {
            blctx->resetMatrix();
        }

        if( iWorld )
        {
            contourPath.transform( worldMatrix );
        }

        blctx->strokePath( contourPath );

        for( int i = 0; i < mInnerSectionArray.size(); i++ )
        {
            ::ULIS::FVec2D* bezier = mInnerSectionArray[i]->GetBezier();
            BLPoint pt[4] = { iWorld ? worldMatrix.mapPoint( bezier[0].x, bezier[0].y ) : BLPoint( bezier[0].x, bezier[0].y )
                            , iWorld ? worldMatrix.mapPoint( bezier[1].x, bezier[1].y ) : BLPoint( bezier[1].x, bezier[1].y )
                            , iWorld ? worldMatrix.mapPoint( bezier[2].x, bezier[2].y ) : BLPoint( bezier[2].x, bezier[2].y )
                            , iWorld ? worldMatrix.mapPoint( bezier[3].x, bezier[3].y ) : BLPoint( bezier[3].x, bezier[3].y ) };
            BLPath sectionPath;

            sectionPath.moveTo ( pt[0] );
            sectionPath.cubicTo( pt[1], pt[2], pt[3] );

            blctx->strokePath( sectionPath );
        }

        blctx->restore();
    }

    blctx->flush( BL_CONTEXT_FLUSH_SYNC );
}

// for debugging purposes
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

::ULIS::FRectD
FOdysseyVectorCycle::GetBBox()
{
    BLBox bbox;

    mContourPath.getBoundingBox( &bbox );

    return ::ULIS::FRectD::FromMinMax( bbox.x0, bbox.y0, bbox.x1, bbox.y1 );
}

void
FOdysseyVectorCycle::Draw( uint64 iFlags, bool iMonochrome, FColor iMonochromeColor )
{
    BLContext* blctx = mOwner->GetScene()->GetEngine()->GetBLContext();
    FOdysseyVectorBucket* bucket = mBucket ? mBucket : mPropagatedBucket;
    BLMatrix2D& worldMatrix = mOwner->GetWorldMatrix();

    if( iMonochrome )
    {
        BLRgba32 BLColor = BLRgba32( iMonochromeColor.R
                                   , iMonochromeColor.G
                                   , iMonochromeColor.B
                                   , iMonochromeColor.A );

        blctx->setStrokeStyle( BLColor );
        blctx->setFillStyle( BLColor );
    }
    else
    {
        if( bucket )
        {
            switch ( bucket->GetColorMode() )
            {
                case eBucketColorMode::LinearGradient :
                {
                    eBucketSpreadingPolicy spreadingPolicy = bucket->GetSpreadingPolicy();
                    ::ULIS::FRectD bbox = spreadingPolicy == eBucketSpreadingPolicy::Group ? mOwner->GetBBox( false ) : GetBBox();
                    double linearMinX = /*bbox.x0*/bbox.x;
                    double linearMinY = /*bbox.y0*/bbox.y;
                    double linearMaxX = /*bbox.x1*/bbox.x + bbox.w;
                    double linearMaxY = /*bbox.y1*/bbox.y + bbox.h;
                    BLGradient linear( BLLinearGradientValues( 0, 0, bbox.w, 0 ) );
                    FColor& gradientColor0 = bucket->GetGradientColor0();
                    FColor& gradientColor1 = bucket->GetGradientColor1();
                    BLRgba32 BLColor0;
                    BLRgba32 BLColor1;
                    // easier to deal with degrees to position the gradient
                    double rotate = bucket->GetRotation() / M_PI * 180.0f;

                    if( ( rotate >=  0.0f ) && ( rotate <  90.0f ) )
                        linear.translate( linearMinX, linearMinY );
                    if( ( rotate >  90.0f ) && ( rotate < 180.0f ) )
                        linear.translate( linearMaxX, linearMinY );
                    if( ( rotate > 180.0f ) && ( rotate < 270.0f ) )
                        linear.translate( linearMaxX, linearMaxY );
                    if( ( rotate > 270.0f ) && ( rotate < 360.0f ) )
                        linear.translate( linearMinX, linearMaxY );

                    linear.rotate( bucket->GetRotation() );

                    BLColor0.setR( gradientColor0.R );
                    BLColor0.setG( gradientColor0.G );
                    BLColor0.setB( gradientColor0.B );
                    BLColor0.setA( gradientColor0.A );

                    BLColor1.setR( gradientColor1.R );
                    BLColor1.setG( gradientColor1.G );
                    BLColor1.setB( gradientColor1.B );
                    BLColor1.setA( gradientColor1.A );

                    linear.addStop( 0.0, BLColor0 );
                    linear.addStop( 1.0, BLColor1 );

                    blctx->setStrokeStyle( linear );
                    blctx->setFillStyle( linear );
                }
                break;

                case eBucketColorMode::RadialGradient :
                {
                    eBucketSpreadingPolicy spreadingPolicy = bucket->GetSpreadingPolicy();
                    ::ULIS::FRectD bbox = spreadingPolicy == eBucketSpreadingPolicy::Group ? mOwner->GetBBox( false ) : GetBBox();
                    double radialMinX = /*bbox.x0*/bbox.x;
                    double radialMinY = /*bbox.y0*/bbox.y;
                    double radialMaxX = /*bbox.x1*/bbox.x + bbox.w;
                    double radialMaxY = /*bbox.y1*/bbox.y + bbox.h;
                    BLGradient radial( BLRadialGradientValues( bbox.x + (bbox.w * 0.5f)
                                                             , bbox.y + (bbox.h * 0.5f)
                                                             , bbox.x + (bbox.w * 0.5f)
                                                             , bbox.y + (bbox.h * 0.5f)
                                                             , 45.0f ) );
                    FColor& gradientColor0 = bucket->GetGradientColor0();
                    FColor& gradientColor1 = bucket->GetGradientColor1();
                    BLRgba32 BLColor0;
                    BLRgba32 BLColor1;

                    BLColor0.setR( gradientColor0.R );
                    BLColor0.setG( gradientColor0.G );
                    BLColor0.setB( gradientColor0.B );
                    BLColor0.setA( gradientColor0.A );

                    BLColor1.setR( gradientColor1.R );
                    BLColor1.setG( gradientColor1.G );
                    BLColor1.setB( gradientColor1.B );
                    BLColor1.setA( gradientColor1.A );

                    radial.addStop( 0.0, BLColor0 );
                    radial.addStop( 1.0, BLColor1 );

                    blctx->setStrokeStyle( radial );
                    blctx->setFillStyle( radial );
                }
                break;

                default:
                {
                    FColor color = bucket->GetColor();
                    BLRgba32 BLColor = BLRgba32( color.R, color.G, color.B, color.A );

                    blctx->setStrokeStyle( BLColor );
                    blctx->setFillStyle( BLColor );
                }
                break;
            }
        }
        else
        {
            FColor& color = mOwner->GetBackgroundBucket().GetSolidColor();
            BLRgba32 BLColor = BLRgba32( color.R, color.G, color.B, color.A );

           blctx->setStrokeStyle( BLColor );
           blctx->setFillStyle( BLColor );
        }
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
