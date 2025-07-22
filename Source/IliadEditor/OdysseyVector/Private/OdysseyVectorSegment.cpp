// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorSegment::~FOdysseyVectorSegment()
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment( FOdysseyVectorObject* iOwner
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1
                                            , bool iNeedsWidth )
    : FOdysseyVectorLink( iVertex0, iVertex1 )
    , mOwner( iOwner )
    , mIsInvalidated( false )
    //, mIsPaintingReady( false )
    , mPaintingCode( 0 )
    , mLength( 0.0f )
    , mIntersectionSlotCount( 0 )
    , mTextureStartU ( 0.0f )
    , mTextureEndU ( 0.0f )
    , mNeedsWidth ( iNeedsWidth )
    , mSectionBuffer ( nullptr ) // used for erasing sections
    , mSectionCount ( 0 ) // used for erasing sections
{
}

uint32
FOdysseyVectorSegment::GetFractionCount()
{
    return mFractionCache.size();
}

std::vector<FOdysseyVectorFraction>&
 FOdysseyVectorSegment::GetFractionCache()
{
    return mFractionCache;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetFractionCacheStartPointInParent()
{
    if( mFractionCache.size() )
    {
        uint32 index = 0;

        return ::ULIS::FVec2D( mFractionCache[index].pointCoordsInParent[0].x
                             , mFractionCache[index].pointCoordsInParent[0].y );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetFractionCacheEndPointInParent()
{
    if( mFractionCache.size() )
    {
        uint32 index = mFractionCache.size() - 1;

        return ::ULIS::FVec2D( mFractionCache[index].pointCoordsInParent[1].x
                             , mFractionCache[index].pointCoordsInParent[1].y );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

FOdysseyVectorIntersection*
FOdysseyVectorSegment::GetClosestIntersection( FOdysseyVectorVertex* iVertex )
{
    if( mIntersectionList.size() )
    {
        return ( mPoint[0] == iVertex ) ? mIntersectionList.front()
                                        : mIntersectionList.back();
    }

    return nullptr;
}

void
FOdysseyVectorSegment::DrawFractionCache( BLContext* iBLContext
                                        , FOdysseyVectorEngine* iVectorEngine )
{
    const BLMatrix2D worldMatrix = iBLContext->userMatrix();
    std::vector<BLPoint>& pointPool = iVectorEngine->GetBLPointPool( ( mFractionCache.size() * 2 ) + 2 );
    uint32 j = 0;

    iBLContext->setStrokeWidth( 1.0f );
    // for filled overlaps
    iBLContext->setFillRule( BL_FILL_RULE_NON_ZERO );

    for ( std::vector<FOdysseyVectorFraction>::iterator it = mFractionCache.begin(); it != mFractionCache.end(); ++it, j++ )
    {
        FOdysseyVectorFraction& fraction = *it;

        pointPool[j].x = fraction.polygon.point[1].x;
        pointPool[j].y = fraction.polygon.point[1].y;
    }

    pointPool[j].x = mFractionCache.back().polygon.point[2].x;
    pointPool[j].y = mFractionCache.back().polygon.point[2].y;

    j++;

    for ( std::vector<FOdysseyVectorFraction>::reverse_iterator it = mFractionCache.rbegin(); it != mFractionCache.rend(); ++it, j++ )
    {
        FOdysseyVectorFraction& fraction = *it;

        pointPool[j].x = fraction.polygon.point[4].x;
        pointPool[j].y = fraction.polygon.point[4].y;
    }

    pointPool.back().x = mFractionCache.front().polygon.point[5].x;
    pointPool.back().y = mFractionCache.front().polygon.point[5].y;

    iBLContext->fillPolygon( &pointPool[0], pointPool.size() );

#ifdef UNUSED
    for ( int i = 0; i < mFractionCache.size(); i++ )
    {
        BLPoint pt[6] = { { mFractionCache[i].polygon.point[0].x, mFractionCache[i].polygon.point[0].y }
                        , { mFractionCache[i].polygon.point[1].x, mFractionCache[i].polygon.point[1].y }
                        , { mFractionCache[i].polygon.point[2].x, mFractionCache[i].polygon.point[2].y }
                        , { mFractionCache[i].polygon.point[3].x, mFractionCache[i].polygon.point[3].y }
                        , { mFractionCache[i].polygon.point[4].x, mFractionCache[i].polygon.point[4].y }
                        , { mFractionCache[i].polygon.point[5].x, mFractionCache[i].polygon.point[5].y } };

        // the stroke thing is very slow and slows the all thing, we have to find something better
        //iBLContext.strokePolygon( mFractionCache[i].vertex, 4 );
/*
        BLPoint pt[4] = { { mFractionCache[i].polygon.point[0].x, mFractionCache[i].polygon.point[0].y }
                        , { mFractionCache[i].polygon.point[1].x, mFractionCache[i].polygon.point[1].y }
                        , { mFractionCache[i].polygon.point[2].x, mFractionCache[i].polygon.point[2].y }
                        , { mFractionCache[i].polygon.point[3].x, mFractionCache[i].polygon.point[3].y } };
*/
        iBLContext->strokePolygon( pt, 6 );
    }
#endif
#ifdef UNUSED
    // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
    // line stroking is done in world coordinates because we need a 1-3 pixel width

    iBLContext->save();
    iBLContext->resetMatrix();
    iBLContext->setStrokeWidth( 3.0f ); // 1 pixel is not enough due to antialiasing. Lets go with 3

    for ( int i = 1; i < mFractionCache.size(); i++ )
    {
        BLPath thinLine;

        thinLine.moveTo( worldMatrix.mapPoint( mFractionCache[i].polygon.point[5].x, mFractionCache[i].polygon.point[5].y ) );
        thinLine.lineTo( worldMatrix.mapPoint( mFractionCache[i].polygon.point[0].x, mFractionCache[i].polygon.point[0].y ) );
        thinLine.lineTo( worldMatrix.mapPoint( mFractionCache[i].polygon.point[1].x, mFractionCache[i].polygon.point[1].y ) );

        iBLContext->strokePath( thinLine );
    }
    iBLContext->restore();
#endif
}

void
FOdysseyVectorSegment::SetTextureU( double iTextureStartU, double iTextureEndU )
{
    mTextureStartU = iTextureStartU;
    mTextureEndU = iTextureEndU;
}

double
FOdysseyVectorSegment::GetTextureStartU()
{
    return mTextureStartU;
}

double
FOdysseyVectorSegment::GetTextureEndU()
{
    return mTextureEndU;
}

FOdysseyVectorHandleSegment*
FOdysseyVectorSegment::GetHandle( int iCtrlPointNum )
{
    return nullptr;
}

FOdysseyVectorHandleSegment*
FOdysseyVectorSegment::GetHandle( FOdysseyVectorVertex* iVertex )
{
    return nullptr;
}

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetOtherVertex( FOdysseyVectorVertex* iVertex )
{
    return ( iVertex == GetVertex(0) ) ? GetVertex(1) : GetVertex(0);
}

void
FOdysseyVectorSegment::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FOdysseyVectorSegment::GetID()
{
    return mID;
}

void
FOdysseyVectorSegment::SetPaintingCode( uint32 iPaintingCode )
{
    mPaintingCode = iPaintingCode;
}

uint32
FOdysseyVectorSegment::GetPaintingCode()
{
    return mPaintingCode;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetPointAt( double t )
{
    ::ULIS::FVec2D unimplemented;

    return unimplemented;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetTangentAt( double t, bool iNormalize )
{
    ::ULIS::FVec2D unimplemented;

    return unimplemented;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorAtStart( bool iNormalize )
{
    ::ULIS::FVec2D vec = GetVertex(1)->GetCoords() - GetVertex(0)->GetCoords();

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

void
FOdysseyVectorSegment::Link()
{
    GetVertex(0)->AddSegment(this);
    GetVertex(1)->AddSegment(this);
}

void
FOdysseyVectorSegment::Unlink()
{
    GetVertex(0)->RemoveSegment(this);
    GetVertex(1)->RemoveSegment(this);
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorAtEnd( bool iNormalize )
{
    return - GetVectorAtStart( iNormalize );
}

double
FOdysseyVectorSegment::ProjectConstrained( const ::ULIS::FVec2D& iPoint
                                         , ::ULIS::FVec2D& oProjectedPoint )
{
    FOdysseyVectorFraction* closestFraction = nullptr;
    ::ULIS::FVec2D projectedPoint;
    double minDistance = DBL_MAX;
    double projectedPointT = -DBL_MAX;

    // first step : find closest fraction
    for( FOdysseyVectorFraction& fraction : mFractionCache )
    {
        double distance;
        double t = FOdysseyVector::DistanceToSegmentConstrained( iPoint
                                                               , fraction.point[0]->GetCoords()
                                                               , fraction.point[1]->GetCoords()
                                                               , distance );

        if( distance < minDistance )
        {
            ::ULIS::FVec2D fractionVector = ( fraction.point[1]->GetCoords()
                                            - fraction.point[0]->GetCoords() );

            minDistance = distance;

            oProjectedPoint = fraction.point[0]->GetCoords() + ( fractionVector * t );

            projectedPointT = fraction.fromT + ( ( fraction.toT - fraction.fromT ) * t );
        }
    }

    return projectedPointT;
}

void
FOdysseyVectorSegment::Split( const ::ULIS::FVec2D& iPoint
                            , double iPoinT
                            , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                            , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray )
{
    /** unimplemented */
}

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetVertex( uint32 iVertexID )
{
    return static_cast<FOdysseyVectorVertex*>(GetPoint( iVertexID ));
}

void
FOdysseyVectorSegment::SetOwner( FOdysseyVectorObject* iOwner )
{
    mOwner = iOwner;
}

FOdysseyVectorPath*
FOdysseyVectorSegment::GetOwnerAsPath()
{
    return static_cast<FOdysseyVectorPath*>(mOwner);
}

FOdysseyVectorObject*
FOdysseyVectorSegment::GetOwner()
{
    return mOwner;
}

void
FOdysseyVectorSegment::CreateSections( FOdysseyVectorObject* iSectionOwner
                                     , std::vector<FOdysseyVectorSection>& iSectionBuffer
                                     , bool iStitchShortSections
                                     , std::vector<FOdysseyVectorSection*>* iShortSectionArray )
{
    std::list<FOdysseyVectorIntersection*>& intersectionList = mIntersectionList;
    FOdysseyVectorVertex* segmentVertex0 = GetVertex(0);
    FOdysseyVectorVertex* segmentVertex1 = GetVertex(1);
    FOdysseyVectorVertex* sectionVertex0 = segmentVertex0;
    double sectionVertex0T = 0.0f;
    FOdysseyVectorSection* sectionBufferStart = &iSectionBuffer[iSectionBuffer.size()];

    if( intersectionList.size() )
    {
        for( FOdysseyVectorIntersection* intersection : intersectionList )
        {
            FOdysseyVectorVertex* sectionVertex1 = intersection->GetVertex();
            double sectionVertex1T = intersection->GetSegmentT();
            // constructor also links sections to the vertex

            iSectionBuffer.emplace_back( iSectionOwner
                                       , this
                                       , sectionVertex0
                                       , sectionVertex1
                                       , sectionVertex0T
                                       , sectionVertex1T
                                       , iStitchShortSections
                                       , iShortSectionArray );

            sectionVertex0 = sectionVertex1;
            sectionVertex0T = sectionVertex1T;
        }
    }

    iSectionBuffer.emplace_back( iSectionOwner
                               , this
                               , sectionVertex0
                               , segmentVertex1 // segment's second end point
                               , sectionVertex0T
                               , 1.0f
                               , iStitchShortSections
                               , iShortSectionArray );
}

void
FOdysseyVectorSegment::AddIntersection ( FOdysseyVectorIntersection* iIntersection )
{
    std::list<FOdysseyVectorIntersection*>::iterator it = std::find_if ( mIntersectionList.begin()
                                                                       , mIntersectionList.end()
                                                                       , [ this
                                                                         , iIntersection ]( FOdysseyVectorIntersection* intersection )
                                                                        {

                                                                            return ( intersection->GetSegmentT() > iIntersection->GetSegmentT() );
                                                                        } );

    mIntersectionList.insert( it, iIntersection );
}

void
FOdysseyVectorSegment::RemoveIntersection ( FOdysseyVectorIntersection* iIntersection )
{
    mIntersectionList.remove( iIntersection );
}

void
FOdysseyVectorSegment::AddIntersectionSlot()
{
    mIntersectionSlotCount++;
}

uint32
FOdysseyVectorSegment::GetIntersectionSlotCount()
{
    return mIntersectionSlotCount;
}

void
FOdysseyVectorSegment::ResetSectionBuffer()
{
    mSectionBuffer = nullptr;
    mSectionCount = 0;
}

uint32
FOdysseyVectorSegment::GetSectionCount()
{
    return mSectionCount;
}

FOdysseyVectorSection*
FOdysseyVectorSegment::GetSectionBuffer()
{
    return mSectionBuffer;
}


// MUST be called only on segments belonging to this path (because of the section)
void
FOdysseyVectorSegment::ClearIntersections( FOdysseyVectorSection* iSectionBuffer, uint32 iSectionCount )
{
    mIntersectionList.clear();

    mIntersectionSlotCount = 0;

    mSectionBuffer = iSectionBuffer;
    mSectionCount = iSectionCount;
}

bool
FOdysseyVectorSegment::IsInvalidated()
{
    return mIsInvalidated;
}

void
FOdysseyVectorSegment::Invalidate()
{
    if( mOwner )
    {
        if( mOwner->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = GetOwnerAsPath();

            path->InvalidateSegment( this );
        }

        mIsInvalidated = true;

        //mIsPaintingReady = false;
    }
}

void
FOdysseyVectorSegment::Update( uint32 iUpdateFlags )
{
    mIsInvalidated = false;

    mLength = ::ULIS::FVec2D( mPoint[1]->GetCoords() - mPoint[0]->GetCoords() ).Distance();
}

std::list<FOdysseyVectorIntersection*>&
FOdysseyVectorSegment::GetIntersectionList()
{
    return mIntersectionList;
}

bool
FOdysseyVectorSegment::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return false;
}

/*
void
FOdysseyVectorSegment::SetPaintingReady( bool iIsPaintingReady )
{
    mIsPaintingReady = iIsPaintingReady;
}

bool
FOdysseyVectorSegment::IsPaintingReady()
{
    return mIsPaintingReady;
}
*/

::ULIS::FVec2D
FOdysseyVectorSegment::GetOffsetPoint( uint32 iSide, double iT )
{
    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetHandleVector( uint32 iHandleID, bool iNormalize )
{
    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetHandleVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FRectD FOdysseyVectorSegment::GetBoundingBox( bool iWorld )
{
    if( iWorld == true )
    {
        BLMatrix2D& worldMatrix = GetOwner()->GetWorldMatrix();
        BLPoint p0 = worldMatrix.mapPoint( mBBox.x          , mBBox.y           );
        BLPoint p1 = worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y           );
        BLPoint p2 = worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y + mBBox.h );
        BLPoint p3 = worldMatrix.mapPoint( mBBox.x          , mBBox.y + mBBox.h );
        ::ULIS::FRectD bbox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                        , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                        , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                        , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return bbox;
    }

    return mBBox;
}

void
FOdysseyVectorSegment::Draw( BLContext* iBLContext, FOdysseyVectorEngine* iVectorEngine )
{

}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    ::ULIS::FVec2D vector = ::ULIS::FVec2D( 0.0f, 0.0f );

    return vector;
}

double
FOdysseyVectorSegment::GetLength()
{
    //return ::ULIS::FVec2D( mPoint[1]->GetCoords() - mPoint[0]->GetCoords() ).Distance();
    return mLength;
}

void
FOdysseyVectorSegment::SetBBoxInParent( const ::ULIS::FRectD& iBBoxInParent )
{
    mBBoxInParent = iBBoxInParent;
}

::ULIS::FRectD&
FOdysseyVectorSegment::GetBBoxInParent()
{
    return mBBoxInParent;
}

std::vector<FOdysseyVectorPoint>&
FOdysseyVectorSegment::GetFractionPointBuffer()
{
    return mFractionPointBuffer;
}

/*
void
FOdysseyVectorSegment::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    GetVertex(0)->BuildExplorationPairs( iExplorationPairsArray );
    GetVertex(1)->BuildExplorationPairs( iExplorationPairsArray );
}
*/
