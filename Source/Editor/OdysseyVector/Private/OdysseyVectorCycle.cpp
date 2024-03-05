#include "OdysseyVectorCycle.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorEngine.h"
// for measurements
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorCycle::~FOdysseyVectorCycle()
{
}

//static
FOdysseyVectorCycle::FOdysseyVectorCycle( FOdysseyVectorObject* iOwner
                                        , const std::vector<uint32>& iVertexIndexArray
                                        , const std::vector<FOdysseyVectorSection*>& iSectionArray )
    : mOwner( iOwner )
    , mBucket( nullptr )
    , mPropagatedBucket( nullptr )
    , mContourVertexIndexArray (iVertexIndexArray)
    , mContourSectionArray (iSectionArray)
    , mParentCycle( nullptr )
    , mPropagated( false )
{
    Build( );
}

void
FOdysseyVectorCycle::Merge( FOdysseyVectorCycle* iMergeCycle )
{
    for( int i = 0; i < iMergeCycle->mContourSectionArray.size(); i++ )
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
        if( iMergeCycle->mContourSectionArray[i]->GetCycleCount() < 2 )
        {
            iMergeCycle->mContourSectionArray[i]->AddCycle( this );

            mInnerSectionArray.push_back( iMergeCycle->mContourSectionArray[i] );
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
    for( int i = 0; i < mContourSectionArray.size(); i++ )
    {
        if( mContourSectionArray[i]->GetOtherCycle( this ) == iParentCandidate )
        {
            return false;
        }
    }

    // Then check if all vertices lies within the parent candidate
    for( int i = 0; i < mContourSectionArray.size(); i++ )
    {
        FOdysseyVectorSection* section = mContourSectionArray[i];
        uint32 contourVertexIndex = mContourVertexIndexArray[i];
        FOdysseyVectorVertex* contourVertex = section->GetVertex( contourVertexIndex );
        ::ULIS::FVec2D vCoords = section->GetVertexCoords( contourVertex );
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
FOdysseyVectorCycle::Build( /*std::vector<FOdysseyVectorVertex*>& iVertexArray
                          , std::vector<FOdysseyVectorSection*>& iSectionArray*/ )
{
    int32 arraySize = mContourSectionArray.size();
    int seg = 0;
    BLBox bbox;

    if ( mContourSectionArray.size() ) 
    {
        FOdysseyVectorSection* firstSection = mContourSectionArray[0];
        FOdysseyVectorVertex* firstVertex = firstSection->GetVertex( mContourVertexIndexArray[0] );
        // Note: section::GetVertexCoords() return the coords in paintgroup's coordinates
        ::ULIS::FVec2D originAt = mContourSectionArray[0]->GetVertexCoords( firstVertex );

        mContourPath.moveTo( originAt.x, originAt.y );

        for( int i = 0; i < arraySize; i++ )
        {
            int n = ( i + 1 ) % arraySize;
            FOdysseyVectorSection* section = mContourSectionArray[i];
            uint32 sectionVertexIndex = mContourVertexIndexArray[i];
            uint32 sectionNextVertexIndex = ( sectionVertexIndex == 0 ) ? 1 : 0;
            ::ULIS::FVec2D* sectionBezier = section->GetBezier();

            section->AddCycle( this );

            // check if we need to revert the bezier. Indeed, a cycle is a combination of sections
            // that may not go the same way. We have to run through them the same way.
            if ( sectionVertexIndex < sectionNextVertexIndex )
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

    mContourPath.getBoundingBox( &bbox );

    mBBox = ::ULIS::FRectD::FromMinMax( bbox.x0, bbox.y0, bbox.x1, bbox.y1 );
}

void
FOdysseyVectorCycle::SetBucket( FOdysseyVectorBucket* iBucket )
{
    mBucket = mPropagatedBucket = iBucket;
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

void
FOdysseyVectorCycle::PropagateBucket( std::vector<FOdysseyVectorSection*> iSectionArray
                                    , std::vector<FOdysseyVectorCycle*>& oNextCycleArray )
{
    // test outer sections
    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        FOdysseyVectorCycle* neighbourCycle = iSectionArray[i]->GetOtherCycle( this );

        if( neighbourCycle )
        {
            FOdysseyVectorBucket* neighbourPropagatedBucket = neighbourCycle->GetPropagatedBucket();

            if( neighbourPropagatedBucket == nullptr )
            {
                neighbourCycle->SetPropagatedBucket( this->GetPropagatedBucket() );

                oNextCycleArray.push_back( neighbourCycle );
            }
        }
    }
}

void
FOdysseyVectorCycle::PropagateBucket( std::vector<FOdysseyVectorCycle*>& oNextCycleArray )
{
    FOdysseyVectorBucket* bucket = mBucket ? mBucket : mPropagatedBucket;

    if( bucket && bucket->IsPropagated() )
    {
        PropagateBucket( mContourSectionArray, oNextCycleArray );
        PropagateBucket( mInnerSectionArray  , oNextCycleArray );
    }
}

bool
FOdysseyVectorCycle::HitTest( double iX, double iY )
{
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
FOdysseyVectorCycle::StrokePath( BLContext* iBLContext, bool iWorld )
{
    if( iWorld == true )
    {
        BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();
        BLPath contourPath = mContourPath;

        iBLContext->save();

        if( iWorld )
        {
            iBLContext->resetMatrix();
        }

        if( iWorld )
        {
            contourPath.transform( worldMatrix );
        }

        iBLContext->strokePath( contourPath );

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

            iBLContext->strokePath( sectionPath );
        }

        iBLContext->restore();
    }

    iBLContext->flush( BL_CONTEXT_FLUSH_SYNC );
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
        FOdysseyVectorObject* owner = vertex0->GetOwner();
        BLPoint pt0 = owner->GetWorldMatrix().mapPoint( vertex0->GetCoords().x, vertex0->GetCoords().y );
        BLPoint pt1 = owner->GetWorldMatrix().mapPoint( vertex1->GetCoords().x, vertex1->GetCoords().y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

::ULIS::FRectD
FOdysseyVectorCycle::GetBBox( bool iWorld )
{
    if( iWorld )
    {
        BLMatrix2D& worldMatrix = mOwner->GetWorldMatrix();
        BLPoint pt[4] = { worldMatrix.mapPoint( mBBox.x          , mBBox.y           )
                        , worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y           )
                        , worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y + mBBox.h )
                        , worldMatrix.mapPoint( mBBox.x          , mBBox.y + mBBox.h ) };
        double xmin = ::ULIS::FMath::Min4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymin = ::ULIS::FMath::Min4( pt[0].y, pt[1].y, pt[2].y, pt[3].y )
             , xmax = ::ULIS::FMath::Max4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymax = ::ULIS::FMath::Max4( pt[0].y, pt[1].y, pt[2].y, pt[3].y );

        return ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    }

    return mBBox;
}

void
FOdysseyVectorCycle::Draw( BLContext* iBLContext
                         , double iOpacity
                         , uint64 iFlags
                         , bool iMonochrome
                         , FColor iMonochromeColor )
{
    FOdysseyVectorBucket* bucket = mBucket ? mBucket : mPropagatedBucket;
    BLMatrix2D& worldMatrix = mOwner->GetWorldMatrix();

    if( iMonochrome /*|| ( iFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR )*/ )
    {
        BLRgba32 BLColor = BLRgba32( iMonochromeColor.R
                                    , iMonochromeColor.G
                                    , iMonochromeColor.B
                                    , iMonochromeColor.A );

        iBLContext->setStrokeStyle( BLColor );
        iBLContext->setFillStyle( BLColor );
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
                    ::ULIS::FRectD bbox = spreadingPolicy == eBucketSpreadingPolicy::Group ? mOwner->GetBBox( false ) : GetBBox( false );
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
                    BLColor0.setA( gradientColor0.A * iOpacity );

                    BLColor1.setR( gradientColor1.R );
                    BLColor1.setG( gradientColor1.G );
                    BLColor1.setB( gradientColor1.B );
                    BLColor1.setA( gradientColor1.A * iOpacity );

                    linear.addStop( 0.0, BLColor0 );
                    linear.addStop( 1.0, BLColor1 );

                    iBLContext->setStrokeStyle( linear );
                    iBLContext->setFillStyle( linear );
                }
                break;

                case eBucketColorMode::RadialGradient :
                {
                    eBucketSpreadingPolicy spreadingPolicy = bucket->GetSpreadingPolicy();
                    ::ULIS::FRectD bbox = spreadingPolicy == eBucketSpreadingPolicy::Group ? mOwner->GetBBox( false ) : GetBBox( false );
                    ::ULIS::FVec2D& radialOffset = bucket->GetRadialOffset();
                    ::ULIS::FVec2D& bucketCoords = bucket->GetCoords();
                    BLGradient radial( BLRadialGradientValues( bucketCoords.x + radialOffset.x
                                                                , bucketCoords.y + radialOffset.y
                                                                , bucketCoords.x + radialOffset.x
                                                                , bucketCoords.y + radialOffset.y
                                                                , bucket->GetRadialRadius() ) );
                    FColor& gradientColor0 = bucket->GetGradientColor0();
                    FColor& gradientColor1 = bucket->GetGradientColor1();
                    BLRgba32 BLColor0;
                    BLRgba32 BLColor1;

                    BLColor0.setR( gradientColor0.R );
                    BLColor0.setG( gradientColor0.G );
                    BLColor0.setB( gradientColor0.B );
                    BLColor0.setA( gradientColor0.A * iOpacity );

                    BLColor1.setR( gradientColor1.R );
                    BLColor1.setG( gradientColor1.G );
                    BLColor1.setB( gradientColor1.B );
                    BLColor1.setA( gradientColor1.A * iOpacity );

                    radial.addStop( 0.0, BLColor0 );
                    radial.addStop( 1.0, BLColor1 );

                    iBLContext->setStrokeStyle( radial );
                    iBLContext->setFillStyle( radial );
                }
                break;

                default:
                {
                    FColor color = bucket->GetColor();
                    BLRgba32 BLColor = BLRgba32( color.R
                                                , color.G
                                                , color.B
                                                , color.A * iOpacity );

                    iBLContext->setStrokeStyle( BLColor );
                    iBLContext->setFillStyle( BLColor );
                }
                break;
            }
        }
        else
        {
            FColor& color = mOwner->GetBackgroundBucket().GetSolidColor();
            BLRgba32 BLColor = BLRgba32( color.R
                                        , color.G
                                        , color.B
                                        , color.A * iOpacity );

            iBLContext->setStrokeStyle( BLColor );
            iBLContext->setFillStyle( BLColor );
        }
    }

    iBLContext->setFillRule( BL_FILL_RULE_EVEN_ODD );
    iBLContext->fillPath( mCombinedPath );

    iBLContext->save();
    iBLContext->resetMatrix();
    iBLContext->setStrokeWidth( 1.0f );

    // stroke borders or else there will be a small 1 pixel gap. We draw it only once: the cycle responsible for drawing the 
    // section is the cycle that was first attached to the section. That way we don't draw it twice. The paint group could be
    // responsible for drawing the sections as well, but then we have to retrieve the bucket color, if any. this would be to
    // complicated. We draw in world coordinates to be sure to get 1 pixel-width strokes.
    for( int i = 0; i < mContourSectionArray.size(); i++ )
    {
        FOdysseyVectorSection* section = mContourSectionArray[i];

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

            iBLContext->strokePath( path );
        }
    }

    iBLContext->restore();
}
