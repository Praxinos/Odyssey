#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorSegmentCubicGap.h"
#include "OdysseyVectorBucket.h"
#include "OdysseyVectorCycle.h"
#include <execution>

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

// Some explanations are needed here, as this is by far the most complex process
// of Odyssey's vector features. The principles is to find cycles chordless determined by
// the intersected paths. To do so we have a multi-step process :
// - find intersections
// - Build a graph by creating sections
// - find chordless cycles by exploring sections :
//    -> To find chordless cycles, we have to always go the same way, either
//       always left or always right, it does not matter but we always go the same way.
//       This is the basic principle of the method. To find the correct way, we simply compute 
//       the cross product, which will be either positive or negative relative to the direction.
//       The only case when we take the negative-direction is when there is no positive-direction.
//         
//         Let's say we only take the first section going to the right :
//         
//             /              Sections B and A are both at the right side of section S
//            /               This can be determined by computing the cross products SxA and SxB
//           B                However, the closest section to segment S is section A. This can be
//          /                 Determined by the dot product. The biggest dot product wins (S.A).
//         /_____ A______
//         o
//         |
//         |
//         S
//         |
//         |
//
//          But what if there is no section going the right way ?
//         Then we choose the section with the biggest dot product
//
//                 B
//                  \         Here, the smallest dot product wins (S.B).
//      _____ A______\
//                   o
//                   |
//                   |
//                   S
//                   |
//                   |
//
// See functions :
//    FOdysseyVectorVertex::GetCycleNextSection()
//    FOdysseyVectorVertexIntersection::GetCycleNextSection()
//
// Using these principles, we are guaranteed to always turn in the right direction.
// Then, to tell whether or not we have found a cycle, we just check that the last vertex
// we met is the same as the first vertex we explored the graph from.
//
// Another important technique is blocking the sections once they were explored in one way.
// Indeed, a set of connected cycles always face the same direction because their vertices "turn"
// in the same direction. It is the same as face orientation on a 3D-Mesh.
// 
//      o______________o______________o
//      |   ------->   |   ------->   |     What do we notice here ? Although cycles have the same
//      |  ^        |  |  ^        |  |     orientation (cross product facing in the same direction),
//      |  |        |  A  |        |  |     sections A, B, C, D are never "explored" twice in the same
//      |  |        v  |  |        v  |     direction (look at the arrows above and below). We use 
//      |   <-------   |   <-------   |     this as an advantage to prevent double detection of the
//      o_______C______o_______D______o     same cycle, which will speed up things. Each time a cycle
//      |   ------->   |   ------->   |     is detected, its sections are blocked one-way, guaranteing
//      |  ^        |  |  ^        |  |     that there will be no other detection. By and by, the whole
//      |  |        |  B  |        |  |     graph exploration simplifies itself.
//      |  |        v  |  |        v  |
//      |   <-------   |   <-------   |
//      o______________o______________o
//
// See functions:
//     FOdysseyVectorSection::Block()
//
//                  Some requirements :
//
//         the algorithm had to work even in this case:
//          ______________________________
//         |                              |
//         |           _______            |
//         |          |       |           |
//         |           \     /            |
//         |            \ o /             |
//         \             / \              /
//          \___________/   \____________/
//
//          Here there are 2 chordless cycles :
//
//                      Cycle 1
//          _______________________________
//         |...............................|
//         |............_______............|
//         |...........|       |...........|
//         |............\     /............|
//         |.............\ o /.............|
//         \............../ \............../
//          \____________/   \____________/
//
//                      Cycle 2
//                      _______
//                     |.......|
//                      \...../
//                       \ o /
//
//  But a naive approach could first detect this cycle below, because
// at intersection point o, there would indeed be a loop detection.
//          _______________________________
//         |...............................|
//         |...............................|
//         |...............................|
//         |...............................|
//         |...............o...............|
//         \............../ \............../
//          \____________/   \____________/
//
// What is the solution ? Detecting a loop only by comparing its
// initial and final vertices is not enough, we also have to check if the
// recursive exploration process has ended on a section that is allowed.
// This is why we introduced the concept of exploration pairs. Exploration pairs
// consists in a "depart section", a vertex, and an "return section". So, for each
// intersection point, we first determine as many exploration pairs as sections connected
// to this vertex. At intersection point o, we would have 4 exploration pairs :
//
//                        1.      2.        3.          4.
//                      \                      /     \     /
//                       \ o       o        o /       \ o /
//                        /       / \        \
//                       /       /   \        \
//
// See functions:
//     FOdysseyVectorVertexIntersection::BuildExplorationPairs()
//     FOdysseyVectorVertex::BuildExplorationPairs()
//
// A cycle is detected only if the last section matches the allowed return section
// OR, if the allowed return section was removed from the graph, then any return
// section is allowed. Indeed, there are orphaned section (that leads to nowhere),
// and we get rid of them because the would cause problems. E.g :
//              o
//              |
//              |
//       o______o______o
//              |
//              |
//              o
//
// Here there is no cycle. But if we explore those sections, we would find one, and it would look
// like this cross. So we get rid of these kind of sections. How ? Simply get rid of any section
// whose vertices are not connected to another section. This implies that this is a multi-pass
// process, as we can have such cases :
//
//              o
//              |       <--- deletion at first pass 
//              |
//              o
//              |       <--- deletion at second pass 
//              |
//       o______o______o
//       |      |      |  
//       |      |      |      <--- keep those sections.
//       o____  o______o
//
// See functions:
//        FOdysseyVectorGroupPaint::SimplifyGraph()
//
// When a cycle is detected, it is not guaranteed that it will be correctly oriented. Indeed,
// a contour can be detected as a cycle, we always took the best section possible, but the
//  overall cycle isn't correctly oriented :
//
//      <-----------------------------
//  |                                      ^
//  |    o______________o______________o   |
//  |    |   ------->   |   ------->   |   |
//  |    |  ^        |  |  ^        |  |   |
//  |    |  |        |  A  |        |  |   |
//  |    |  |        v  |  |        v  |   |
//  |    |   <-------   |   <-------   |   |
//  |    o_______C______o_______D______o   |
//  |    |   ------->   |   ------->   |   |
//  |    |  ^        |  |  ^        |  |   |
//  |    |  |        |  B  |        |  |   |
//  |    |  |        v  |  |        v  |   |
//  |    |   <-------   |   <-------   |   |
//  |    o______________o______________o   |
//  v
//      ------------------------------>
//
// The contour cycle detected was the only possibility, but in the end
// is not well oriented. That's why, for any cycle we find, we always
// have to check its overall orientation anyways. This one will be discarded.
//
// See functions:
//    static GetCycleNormalVector()
//
// Stay focused, it's not over yet !
//
// Once we have detected the cycles, how to deal with cycles that are the one inside the other ?
// we have to check which one fits into which one and then merge them :
//
// See functions:
//     FOdysseyVectorGroupPaint::OrderCycles()
//     FOdysseyVectorGroupPaint::MergeCycles()
//
//  That's basically it !

// some measurements :
//Clear Exec time 179
//BuildGraph Exec time 15396
//pair Exec time 78
//SimplifyGraph Exec time 131
//Explore Exec time 631
//OrderCycles Exec time 553
//MergeCycles Exec time 9
//Unlink Exec time 44


// MUST be even number
#define EDGESUBSAMPLES 8

static void PrintVertex( FOdysseyVectorVertex* iVertex );

static double
GetCycleNormalVector( std::vector<uint32>& iVertexIndexArray
                    , std::vector<FOdysseyVectorSection*>& iSectionArray );

FOdysseyVectorGroupPaint::~FOdysseyVectorGroupPaint()
{
    Clear();

    // remove because segmrnts will be freed by path's dtor
    mCanvasPath.RemoveSegment( &mCanvasSegment[0] );
    mCanvasPath.RemoveSegment( &mCanvasSegment[1] );
    mCanvasPath.RemoveSegment( &mCanvasSegment[2] );
    mCanvasPath.RemoveSegment( &mCanvasSegment[3] );

    // remove because vertices will be freed by path's dtor
    mCanvasPath.RemoveVertex( &mCanvasVertex[0] );
    mCanvasPath.RemoveVertex( &mCanvasVertex[1] );
    mCanvasPath.RemoveVertex( &mCanvasVertex[2] );
    mCanvasPath.RemoveVertex( &mCanvasVertex[3] );

    for( FOdysseyVectorBucket* bucket : mBucketList )
    {
        delete bucket;
    }

    mBucketList.clear();
}

FOdysseyVectorGroupPaint::FOdysseyVectorGroupPaint( const FString& iName )
    : FOdysseyVectorGroup( iName )
    , mCanvasPath ( "CanvasPath" )
    , mCanvasVertex { FOdysseyVectorVertex( 0, 0, 0 )
                     , FOdysseyVectorVertex( 0, 0, 0 )
                     , FOdysseyVectorVertex( 0, 0, 0 )
                     , FOdysseyVectorVertex( 0, 0, 0 ) }
    , mCanvasSegment { FOdysseyVectorSegmentCubic( &mCanvasPath
                                                  , &mCanvasVertex[0]
                                                  , &mCanvasVertex[1]
                                                  , true )
                      , FOdysseyVectorSegmentCubic( &mCanvasPath
                                                  , &mCanvasVertex[1]
                                                  , &mCanvasVertex[2]
                                                  , true )
                      , FOdysseyVectorSegmentCubic( &mCanvasPath
                                                  , &mCanvasVertex[2]
                                                  , &mCanvasVertex[3]
                                                  , true )
                      , FOdysseyVectorSegmentCubic( &mCanvasPath
                                                  , &mCanvasVertex[3]
                                                  , &mCanvasVertex[0]
                                                  , true ) }

{
    SetName( iName );

    mXIntersectionRecordArray.reserve( 100 );
    mTIntersectionRecordArray.reserve( 100 );
    mShortSectionArray.reserve( 100 );

    bPainted = true;
    bMonochrome = false;
    mMonochromeColor = FColor( 160, 160, 160, 255 );
    mGapTolerance = 12.0f;
    bRealtime = false;
    bWireframe = false;
    mWireframeColor = FColor( 255, 255, 255, 255 );
#if PLATFORM_MAC
    bMultithreaded = false;
#else
    bMultithreaded = true;
#endif
    mPaintingCode = 0;

    mBackgroundBucket.SetSolidColor( 160, 160, 160, 0 );

    //mGapSegmentBuffer.reserve( 200 );
    //mSectionBuffer.reserve( 200 );

    SetIntersectsCanvas( false ); // also sets hierarchy invalidation flag

    mCanvasPath.AddVertex( &mCanvasVertex[0] );
    mCanvasPath.AddVertex( &mCanvasVertex[1] );
    mCanvasPath.AddVertex( &mCanvasVertex[2] );
    mCanvasPath.AddVertex( &mCanvasVertex[3] );

    mCanvasPath.AddSegment( &mCanvasSegment[0] );
    mCanvasPath.AddSegment( &mCanvasSegment[1] );
    mCanvasPath.AddSegment( &mCanvasSegment[2] );
    mCanvasPath.AddSegment( &mCanvasSegment[3] );
}

std::vector<FOdysseyVectorVertexIntersection>&
FOdysseyVectorGroupPaint::GetIntersectionVertexArray()
{
    return mIntersectionVertexArray;
}

void
FOdysseyVectorGroupPaint::SetRealtime( bool iRealtime )
{
    bRealtime = iRealtime;
}

void
FOdysseyVectorGroupPaint::SetIntersectsCanvas( bool iIntersectsCanvas )
{
    bIntersectsCanvas = iIntersectsCanvas;

    Invalidate( INVALIDATE_SHAPE | INVALIDATE_HIERARCHY );
}

bool
FOdysseyVectorGroupPaint::IntersectsCanvas()
{
    return bIntersectsCanvas;
}

void
FOdysseyVectorGroupPaint::MakeCanvasPath()
{
    FOdysseyVectorGroupPaint* vectorScene = GetScene();

    // vectorScene can be null when the object is copied
    if( vectorScene )
    {
        FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
        uint32 width  = vectorEngine->GetPreferredWidth();
        uint32 height = vectorEngine->GetPreferredHeight();
        // Note: mCanvasPath has identity matrix
        BLPoint pt[4] = { BLPoint( 0    , 0      )
                        , BLPoint( width, 0      )
                        , BLPoint( width, height )
                        , BLPoint( 0    , height ) };

        mCanvasVertex[0].Set( pt[0].x, pt[0].y );
        mCanvasVertex[1].Set( pt[1].x, pt[1].y );
        mCanvasVertex[2].Set( pt[2].x, pt[2].y );
        mCanvasVertex[3].Set( pt[3].x, pt[3].y );

        mCanvasSegment[0].GetHandle(0)->Set( pt[0].x, pt[0].y );
        mCanvasSegment[0].GetHandle(1)->Set( pt[1].x, pt[1].y );

        mCanvasSegment[1].GetHandle(0)->Set( pt[1].x, pt[1].y );
        mCanvasSegment[1].GetHandle(1)->Set( pt[2].x, pt[2].y );

        mCanvasSegment[2].GetHandle(0)->Set( pt[2].x, pt[2].y );
        mCanvasSegment[2].GetHandle(1)->Set( pt[3].x, pt[3].y );

        mCanvasSegment[3].GetHandle(0)->Set( pt[3].x, pt[3].y );
        mCanvasSegment[3].GetHandle(1)->Set( pt[0].x, pt[0].y );
    }
}

void
FOdysseyVectorGroupPaint::UpdateMatrix()
{
    FOdysseyVectorObject::UpdateMatrix();

    if( bIntersectsCanvas )
    {
        Invalidate( FOdysseyVectorObject::INVALIDATE_SHAPE );
    }
}

bool
FOdysseyVectorGroupPaint::IsRealtime()
{
    return bRealtime;
}

// https://www.particleincell.com/2013/cubic-line-intersection/
// sign of number
double sgn( double x )
{
    return ( x < 0.0f ) ? -1.0f : 1.0f;
}

// https://www.particleincell.com/2013/cubic-line-intersection/
void cubicRoots( const double iPoly[4], double oRoots[3] )
{
    if( iPoly[0] )
    {
        double A = iPoly[1] / iPoly[0];
        double B = iPoly[2] / iPoly[0];
        double C = iPoly[3] / iPoly[0];
        double Q = ( 3 * B     - A * A ) / 9;
        double R = ( 9 * A * B - 27 * C - 2 * A * A * A ) / 54;
        double D = Q * Q * Q + R * R; // polynomial discriminant
 
        if( D >= 0 ) // complex or duplicate roots
        {
            double sqrtD = sqrt(D);
            double S = sgn( R + sqrtD ) * pow( fabs( R + sqrtD ), ( 1 / 3 ) );
            double T = sgn( R - sqrtD ) * pow( fabs( R - sqrtD ), ( 1 / 3 ) );
            double Im = fabs( sqrt(3)*(S - T)/2); // complex part of root pair

            oRoots[0] = -A / 3 + ( S + T );     // real root
            oRoots[1] = -A / 3 - ( S + T ) / 2; // real part of complex root
            oRoots[2] = -A / 3 - ( S + T ) / 2; // real part of complex root
 
            /*discard complex roots*/
            if ( Im != 0 )
            {
                oRoots[1] = -1.0f;
                oRoots[2] = -1.0f;
            }
        }
        else                                          // distinct real roots
        {
            double th = acos( R / sqrt( -pow( Q, 3 ) ) );
            double sqrtMinusQ = sqrt( -Q );

            oRoots[0] = 2 * sqrtMinusQ * cos( th / 3 ) - A / 3;
            oRoots[1] = 2 * sqrtMinusQ * cos( ( th + 2 * M_PI ) / 3 ) - A / 3;
            oRoots[2] = 2 * sqrtMinusQ * cos( ( th + 4 * M_PI ) / 3 ) - A / 3;
        }
    }
}

//https://www.xarg.org/book/computer-graphics/line-segment-bezier-curve-intersection/
::ULIS::FVec2D lineBezier( const ::ULIS::FVec2D& iLineP0
                         , const ::ULIS::FVec2D& iLineP1
                         , const ::ULIS::FVec2D iBezier[4] )
{
    double Ax = 3 * (iBezier[1].x - iBezier[2].x) + iBezier[3].x - iBezier[0].x;
    double Ay = 3 * (iBezier[1].y - iBezier[2].y) + iBezier[3].y - iBezier[0].y;
    double Bx = 3 * (iBezier[0].x - 2 * iBezier[1].x + iBezier[2].x);
    double By = 3 * (iBezier[0].y - 2 * iBezier[1].y + iBezier[2].y);
    double Cx = 3 * (iBezier[1].x - iBezier[0].x);
    double Cy = 3 * (iBezier[1].y - iBezier[0].y);
    double Dx = iBezier[0].x;
    double Dy = iBezier[0].y;
    double vx = iLineP1.y - iLineP0.y;
    double vy = iLineP0.x - iLineP1.x;
    double d = iLineP0.x * vx + iLineP0.y * vy;
    double poly[4] = { vx * Ax + vy * Ay
                     , vx * Bx + vy * By
                     , vx * Cx + vy * Cy
                     , vx * Dx + vy * Dy - d };
    double roots[3];

    cubicRoots( poly, roots );

    for( double& t : roots )
    {
        if ( ( t < 0.0f ) || ( t > 1.0f ) ) continue;

        return ::ULIS::FVec2D( ( ( Ax * t + Bx ) * t + Cx ) * t + Dx
                             , ( ( Ay * t + By ) * t + Cy ) * t + Dy );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FOdysseyVectorGroupPaint::IntersectVertex( FOdysseyVectorVertex* iVertex0
                                         , const ::ULIS::FVec2D& iPoint0InParent
                                         , FOdysseyVectorVertex* iVertex1
                                         , const ::ULIS::FVec2D& iPoint1InParent )
{
    if( iVertex0 != iVertex1 )
    {
        // We only test if vertex0 belongs to only 1 segment. Indeed, vertex1 can be between 2 segments
        // for example when nearestSegmentT equals 0 or 1.
        if( iVertex0->GetSegmentCount() == 1 )
        {
            if( mGapTolerance )
            {
                ::ULIS::FVec2D dif = ::ULIS::FVec2D( iPoint1InParent - iPoint0InParent );
                double distance = dif.Distance();

                if( distance < mGapTolerance )
                {
                    if( distance < iVertex0->GetDistanceToNearestVertex() )
                    {
                        iVertex0->SetNearestVertex( iVertex1, distance );
                    }
                }
            }
        }
    }
}

static bool
IntersectGapSection( FOdysseyVectorSection* iGapSection
                   , FOdysseyVectorSegment* iSegment )
{
    std::vector<FOdysseyVectorFraction>& segmentFractionCache = iSegment->GetFractionCache();
    ::ULIS::FVec2D* bezier = iGapSection->GetBezier();
    ::ULIS::FVec2D gapSectionPoint0 = bezier[0];
    ::ULIS::FVec2D gapSectionPoint1 = bezier[3];

    for( int j = 0; j < segmentFractionCache.size(); j++ )
    {
        FOdysseyVectorFraction* segmentPoly = &segmentFractionCache[j];
        double gapSectionT, segmentPolySubT;

        // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
        //if( ( gapSectionPoint0.x > segmentPoly->xMinInParent ) && ( segment0Poly->xMinInParent < segmentPoly->xMaxInParent )
        // && ( gapSectionPoint1.x > segmentPoly->yMinInParent ) && ( segment0Poly->yMinInParent < segmentPoly->yMaxInParent ) )
        //{
            // Test intersections in PaintGroup's coordinates system (struct member lineVertexInParent).
            if ( FOdysseyVector::IntersectSegment ( gapSectionPoint0
                                                  , gapSectionPoint1
                                                  , segmentPoly->pointCoordsInParent[0]
                                                  , segmentPoly->pointCoordsInParent[1]
                                                  , &gapSectionT
                                                  , &segmentPolySubT ) )
            {
                double segmentT = segmentPoly->fromT + ( segmentPolySubT * ( segmentPoly->toT - segmentPoly->fromT ) );

                if( ( segmentT > 0.0f && segmentT < 1.0f )
                 && ( segmentT > 0.0f && segmentT < 1.0f ) )
                {
                    return true;
                }
            }
        //}
    }

    return false;
}

// CubicSegment-CubicSegment intersection test. The test is performed using straight sub-segments
// that are precomputed by the PaintGroup object when updated and stored in the path's FractionCache,
// as it would be too complicated to do maths using the parametric bezier and I'm not that smart.
// Actual intersections vertices are created in this method. We create 2 vertices per intersection.
// This is required because a segment can intersect itself, in that case we need to be able to create
// a section that has 2 different vertices as endpoints and not the same one. 
// A Tolerance value is accepted to test for near-intersections, that will be created later in the
// process.
void
FOdysseyVectorGroupPaint::IntersectSegment( FOdysseyVectorSegment* iSegment0
                                          , FOdysseyVectorSegment* iSegment1
                                          , const ::ULIS::FVec2D& iSegment1MinInParentWithTolerance
                                          , const ::ULIS::FVec2D& iSegment1MaxInParentWithTolerance
                                          , std::vector<FXIntersectionRecord>& oIntersectionRecordArray )
{
    FOdysseyVectorVertex* segment0Vertex0 = iSegment0->GetVertex(0);
    FOdysseyVectorVertex* segment0Vertex1 = iSegment0->GetVertex(1);
    std::vector<FOdysseyVectorFraction>& segment0FractionCache = iSegment0->GetFractionCache();
    ::ULIS::FVec2D segment0Point0InParent = iSegment0->GetFractionCacheStartPointInParent();
    ::ULIS::FVec2D segment0Point1InParent = iSegment0->GetFractionCacheEndPointInParent();
    uint32 segment0Vertex0SegmentCount = segment0Vertex0->GetSegmentCount();
    uint32 segment0Vertex1SegmentCount = segment0Vertex1->GetSegmentCount();

    FOdysseyVectorVertex* segment1Vertex0 = iSegment1->GetVertex(0);
    FOdysseyVectorVertex* segment1Vertex1 = iSegment1->GetVertex(1);
    std::vector<FOdysseyVectorFraction>& segment1FractionCache = iSegment1->GetFractionCache();
    ::ULIS::FVec2D segment1Point0InParent = iSegment1->GetFractionCacheStartPointInParent();
    ::ULIS::FVec2D segment1Point1InParent = iSegment1->GetFractionCacheEndPointInParent();
    uint32 segment1Vertex0SegmentCount = segment1Vertex0->GetSegmentCount();
    uint32 segment1Vertex1SegmentCount = segment1Vertex1->GetSegmentCount();

    for ( int i = 0; i < segment0FractionCache.size(); i++ )
    {
        FOdysseyVectorFraction* segment0Poly = &segment0FractionCache[i];
        ::ULIS::FVec2D& segment0P0Coords = segment0Poly->point[0]->GetCoords();
        ::ULIS::FVec2D& segment0P1Coords = segment0Poly->point[1]->GetCoords();

        ::ULIS::FVec2D segment0PolyVector = ( segment0P1Coords - segment0P0Coords );
        ::ULIS::FVec2D segment0PolyVectorInParent = ( segment0Poly->pointCoordsInParent[1] - segment0Poly->pointCoordsInParent[0] );
        int p = i - 1;
        int n = i + 1;

        if( iSegment0 >= iSegment1 )
        {
            if( ( segment0Poly->xMinInParent <= iSegment1MaxInParentWithTolerance.x )
             && ( segment0Poly->xMaxInParent >= iSegment1MinInParentWithTolerance.x )
             && ( segment0Poly->yMinInParent <= iSegment1MaxInParentWithTolerance.y )
             && ( segment0Poly->yMaxInParent >= iSegment1MinInParentWithTolerance.y ) )
            {
                for( int j = 0; j < segment1FractionCache.size(); j++ )
                {
                    FOdysseyVectorFraction* segment1Poly = &segment1FractionCache[j];

                  if(    ( iSegment0 >  iSegment1 )
                    // test only once in case of self-intersecting segment
                    || ( ( iSegment0 == iSegment1 ) && ( segment0Poly > segment1Poly ) ) )
                  {
                    ::ULIS::FVec2D& segment1P0Coords = segment1Poly->point[0]->GetCoords();
                    ::ULIS::FVec2D& segment1P1Coords = segment1Poly->point[1]->GetCoords();
                    double segment0PolySubT, segment1PolySubT;

                    // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                    if( ( segment0Poly->xMaxInParent >= segment1Poly->xMinInParent ) && ( segment0Poly->xMinInParent <= segment1Poly->xMaxInParent )
                     && ( segment0Poly->yMaxInParent >= segment1Poly->yMinInParent ) && ( segment0Poly->yMinInParent <= segment1Poly->yMaxInParent ) )
                    {
                        ::ULIS::FVec2D segment1PolyVector = ( segment1P1Coords - segment1P0Coords );
                        ::ULIS::FVec2D segment1PolyVectorInParent = ( segment1Poly->pointCoordsInParent[1] - segment1Poly->pointCoordsInParent[0] );

                        // check this is not the same sub-segment or adjacent sub-segment,
                        // i.e check they don't share a point in common
                        // or else they would always intersect
                        if( ( segment0Poly->point[0] != segment1Poly->point[1] )
                         && ( segment0Poly->point[0] != segment1Poly->point[0] )
                         && ( segment0Poly->point[1] != segment1Poly->point[0] )
                         && ( segment0Poly->point[1] != segment1Poly->point[1] ) )
                        {

                            // Test intersections in PaintGroup's coordinates system (struct member lineVertexInParent).
                            if ( FOdysseyVector::IntersectSegment ( segment0Poly->pointCoordsInParent[0]
                                                                  , segment0Poly->pointCoordsInParent[1]
                                                                  , segment1Poly->pointCoordsInParent[0]
                                                                  , segment1Poly->pointCoordsInParent[1]
                                                                  , &segment0PolySubT
                                                                  , &segment1PolySubT ) )
                            {
                                // Find intersections coordinates in respective coordinates systems (struct member lineVertex).
                                // Note: we cannot use segment->GetPointAt() to determine the position
                                // of the intersection because it will not match the intersection that
                                // we detect via linear means. Cubic segments are not linear.
                                ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->pointCoordsInParent[0].x + ( segment0PolyVectorInParent.x * segment0PolySubT )
                                                                   , segment0Poly->pointCoordsInParent[0].y + ( segment0PolyVectorInParent.y * segment0PolySubT ) };
                                ::ULIS::FVec2D segment1ISXCoords = { segment1Poly->pointCoordsInParent[0].x + ( segment1PolyVectorInParent.x * segment1PolySubT )
                                                                   , segment1Poly->pointCoordsInParent[0].y + ( segment1PolyVectorInParent.y * segment1PolySubT ) };
                                // find value T at intersection. This is coordinates system-independent.
                                double segment0T = segment0Poly->fromT + ( segment0PolySubT * ( segment0Poly->toT - segment0Poly->fromT ) );
                                double segment1T = segment1Poly->fromT + ( segment1PolySubT * ( segment1Poly->toT - segment1Poly->fromT ) );

                                if( ( segment0T >= 0.0f && segment0T <= 1.0f )
                                 && ( segment1T >= 0.0f && segment1T <= 1.0f ) )
                                {
                                    // ignore intersection at end points if there is only
                                    // one segment. The intersection is handled by the gap
                                    // management
                                    {
                                        if( ( segment0T == 0.0f )
                                         && ( segment0Vertex0SegmentCount == 1 ) )
                                        {
                                            continue;
                                        }

                                        if( ( segment0T == 1.0f )
                                         && ( segment0Vertex1SegmentCount == 1 ) )
                                        {
                                            continue;
                                        }

                                        if( ( segment1T == 0.0f )
                                         && ( segment1Vertex0SegmentCount == 1 ) )
                                        {
                                            continue;
                                        }

                                        if( ( segment1T == 1.0f )
                                         && ( segment1Vertex1SegmentCount == 1 ) )
                                        {
                                            continue;
                                        }
                                    }

                                    // save in temporary struct array will allow to alloc vertices in one go.
                                    mMutex.lock();
                                    oIntersectionRecordArray.emplace_back( segment0ISXCoords.x
                                                                         , segment0ISXCoords.y
                                                                         , iSegment0
                                                                         , segment0T
                                                                         , iSegment1
                                                                         , segment1T );
                                    mMutex.unlock();
                                }
                            }
                        }
                    }
                  }
                }
            }
        }

// this part is for detecting near-intersections. We only consider path tips (segmentCount = 1)
////////////////////////////// Gap detection /////////////////////////////////
        if( ( mGapTolerance  ) && ( iSegment0 != iSegment1 ) )
        {
            if( segment1Vertex0SegmentCount == 1 )
            {
                // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                if ( ( segment1Point0InParent.x >= ( segment0Poly->xMinInParent - mGapTolerance ) ) && ( segment1Point0InParent.x <= ( segment0Poly->xMaxInParent + mGapTolerance ) )
                  && ( segment1Point0InParent.y >= ( segment0Poly->yMinInParent - mGapTolerance ) ) && ( segment1Point0InParent.y <= ( segment0Poly->yMaxInParent + mGapTolerance ) ) )
                {
                    // check distance at endpoints
                    double distance;
                    double t = FOdysseyVector::DistanceToSegmentConstrained( segment1Point0InParent
                                                                           , segment0Poly->pointCoordsInParent[0]
                                                                           , segment0Poly->pointCoordsInParent[1]
                                                                           , distance );

                    //if ( ( t > 0.0f ) && ( t < 1.0f ) )
                    {
                        mMutex.lock();
                        if( ( distance < mGapTolerance )
                         && ( distance < segment1Vertex0->GetDistanceToNearestSegment() ) )
                        {
                            double segmentT = segment0Poly->fromT + ( ( segment0Poly->toT - segment0Poly->fromT ) * t );
                            // Note: we cannot use segment->GetPointAt() to determine the position
                            // of the intersection because it will not match the intersection that
                            // we detect via linear means. Cubic segments are not linear.
                            ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->pointCoordsInParent[0].x + ( segment0PolyVectorInParent.x * t )
                                                               , segment0Poly->pointCoordsInParent[0].y + ( segment0PolyVectorInParent.y * t ) };

                            segment1Vertex0->SetNearestSegment( iSegment0, distance, segmentT, segment0ISXCoords );

                        }
                        mMutex.unlock();
                    }
                }
            }

            if( segment1Vertex1SegmentCount == 1 )
            {
                // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                if ( ( segment1Point1InParent.x >= ( segment0Poly->xMinInParent - mGapTolerance ) ) && ( segment1Point1InParent.x <= ( segment0Poly->xMaxInParent + mGapTolerance ) )
                  && ( segment1Point1InParent.y >= ( segment0Poly->yMinInParent - mGapTolerance ) ) && ( segment1Point1InParent.y <= ( segment0Poly->yMaxInParent + mGapTolerance ) ) )
                {
                    double distance;
                    double t = FOdysseyVector::DistanceToSegmentConstrained( segment1Point1InParent
                                                                           , segment0Poly->pointCoordsInParent[0]
                                                                           , segment0Poly->pointCoordsInParent[1]
                                                                           , distance );

                    //if ( ( t > 0.0f ) && ( t < 1.0f ) )
                    {
                        mMutex.lock();
                        if( ( distance < mGapTolerance )
                         && ( distance < segment1Vertex1->GetDistanceToNearestSegment() ) )
                        {
                            double segmentT = segment0Poly->fromT + ( ( segment0Poly->toT - segment0Poly->fromT ) * t );
                            // Note: we cannot use segment->GetPointAt() to determine the position
                            // of the intersection because it will not match the intersection that
                            // we detect via linear means. Cubic segments are not linear.
                            ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->pointCoordsInParent[0].x + ( segment0PolyVectorInParent.x * t )
                                                               , segment0Poly->pointCoordsInParent[0].y + ( segment0PolyVectorInParent.y * t ) };

                            segment1Vertex1->SetNearestSegment( iSegment0, distance, segmentT, segment0ISXCoords );
                        }
                        mMutex.unlock();
                    }
                }
            }
        }
///////////////////////////////////
    }

    mMutex.lock();
    IntersectVertex( segment0Vertex0, segment0Point0InParent, segment1Vertex0, segment1Point0InParent );
    IntersectVertex( segment0Vertex0, segment0Point0InParent, segment1Vertex1, segment1Point1InParent );
    IntersectVertex( segment0Vertex1, segment0Point1InParent, segment1Vertex0, segment1Point0InParent );
    IntersectVertex( segment0Vertex1, segment0Point1InParent, segment1Vertex1, segment1Point1InParent );
    mMutex.unlock();
}

void
FOdysseyVectorGroupPaint::SelectAllBuckets()
{
    for( FOdysseyVectorBucket* bucket : mBucketList )
    {
        if( bucket->IsSelected() == false )
        {
            SelectBucket( bucket );
        }
    }
}

void
FOdysseyVectorGroupPaint::UnselectAllBuckets()
{
    mSelectedBucketList.remove_if( []( FOdysseyVectorBucket* iSelectedBucket )
                                   {
                                       iSelectedBucket->SetSelected( false );

                                       return true;
                                   } );
}

void
FOdysseyVectorGroupPaint::UnselectBucket( FOdysseyVectorBucket* iSelectedBucket )
{
    mSelectedBucketList.remove( iSelectedBucket );

    iSelectedBucket->SetSelected( false );
}

void
FOdysseyVectorGroupPaint::SelectBucket( FOdysseyVectorBucket* iSelectedBucket )
{
    if( iSelectedBucket->IsSelected() == false )
    {
        mSelectedBucketList.push_back( iSelectedBucket );

        iSelectedBucket->SetSelected( true );
    }
}

std::list<FOdysseyVectorBucket*>&
FOdysseyVectorGroupPaint::GetSelectedBucketList()
{
    return mSelectedBucketList;
}

bool
FOdysseyVectorGroupPaint::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorGroup::HasBaseClass( iBaseClassID );
}

std::list<FOdysseyVectorBucket*>&
FOdysseyVectorGroupPaint::GetBucketList()
{
    return mBucketList;
}

std::list<FOdysseyVectorCycle*>&
FOdysseyVectorGroupPaint::GetCycleList()
{
    return mCycleList;
}

void
FOdysseyVectorGroupPaint::ApplyMatrix( BLMatrix2D& iMatrix )
{
    for( FOdysseyVectorBucket* bucket : mBucketList )
    {
        ::ULIS::FVec2D& point = bucket->GetCoords();
        BLPoint localPt = iMatrix.mapPoint( point.x, point.y );

        bucket->Set( localPt.x, localPt.y );
    }
}

void
FOdysseyVectorGroupPaint::ApplyTransformations()
{
    BLMatrix2D& parentInverseWorldMatrix = mParent->GetInverseWorldMatrix();
    BLMatrix2D conversionMatrix = mLocalMatrix;

    FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, mWorldMatrix, conversionMatrix );

    ApplyMatrix( conversionMatrix );

    FOdysseyVectorObject::ApplyTransformations();
}

void
FOdysseyVectorGroupPaint::PropagateBuckets()
{
    std::vector<FOdysseyVectorCycle*> cycleStack;
    std::vector<FOdysseyVectorCycle*> nextCycleStack;
    uint32 cycleCount = mCycleList.size();

    cycleStack.reserve( cycleCount );

    // populate with cycles that have a bucket
    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        if( cycle->GetBucket() )
        {
            cycleStack.push_back( cycle );
        }
    }

    while( cycleStack.size() )
    {
        nextCycleStack.clear();
        nextCycleStack.reserve( cycleCount );

        for( FOdysseyVectorCycle *cycle : cycleStack )
        {
            cycle->PropagateBucket( nextCycleStack );
        }

        cycleStack = nextCycleStack;
    }
}

void
FOdysseyVectorGroupPaint::Colorize()
{
    // reset color for all cycles first
    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        cycle->SetBucket( nullptr );
    }

    for( FOdysseyVectorBucket *bucket : mBucketList )
    {
        ApplyBucket( bucket );
    }

    PropagateBuckets();
}

void
FOdysseyVectorGroupPaint::ApplyBucket( FOdysseyVectorBucket* iBucket )
{
    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        if( cycle->HitTest( iBucket->GetCoords().x, iBucket->GetCoords().y ) )
        {
            cycle->SetBucket( iBucket );

            // a single bucket per loop;
            return;
        }
    }

    /*Invalidate();*/
}

void
FOdysseyVectorGroupPaint::UpdateShape( uint32 iUpdateFlags )
{
    BLMatrix2D identityMatrix = BLMatrix2D( BLMatrix2D::makeIdentity() );

    if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY )
    {
        UpdatePathList();
    }

    if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE )
    {
        if( bIntersectsCanvas )
        {
            MakeCanvasPath();
        }
    }

    if( bPainted )
    {
        mCanvasPath.Update( 0 );

        if( ( bRealtime == true  )
       || ( ( bRealtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATE_PAINTGROUPS ) ) )
        {
            /*std::for_each( std::execution::par_unseq
                          , mPathList.begin()
                          , mPathList.end()
                          , [this]( FOdysseyVectorPath *childPath )
            {*/

            for( FOdysseyVectorObject* childPath : mPathList )
            {
                if( childPath->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(childPath);
                    std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
                    BLMatrix2D conversionMatrix;

                    FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, path->GetWorldMatrix(), conversionMatrix );

                    /*std::for_each( std::execution::par_unseq
                                 , segmentList.begin()
                                 , segmentList.end()
                                 , [this, &conversionMatrix]( FOdysseyVectorSegment *segment )
                    {*/

                    for( FOdysseyVectorSegment* segment : segmentList )
                    {
                        if( segment->IsPaintingReady() == false )
                        {
                            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                            {
                                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                                std::vector<FOdysseyVectorFraction>& fractionCache = cubicSegment->GetFractionCache();

                                // convert polygon cache coordinates to paintgroup's coordinates
                                // for faster intersection test
                                for( int i = 0; i < fractionCache.size(); i++ )
                                {
                                    FOdysseyVectorFraction* fraction = &fractionCache[i];
                                    ::ULIS::FVec2D& p0Coords = fraction->point[0]->GetCoords();
                                    ::ULIS::FVec2D& p1Coords = fraction->point[1]->GetCoords();

                                    BLPoint lineVertex0 = conversionMatrix.mapPoint( p0Coords.x, p0Coords.y );
                                    fraction->pointCoordsInParent[0].x = lineVertex0.x;
                                    fraction->pointCoordsInParent[0].y = lineVertex0.y;

                                    BLPoint lineVertex1 = conversionMatrix.mapPoint( p1Coords.x, p1Coords.y );
                                    fraction->pointCoordsInParent[1].x = lineVertex1.x;
                                    fraction->pointCoordsInParent[1].y = lineVertex1.y;

                                    // this may be a bit too memory-consuming. Don't know. Keep it for now.
                                    fraction->xMaxInParent = ::ULIS::FMath::Max( fraction->pointCoordsInParent[0].x, fraction->pointCoordsInParent[1].x );
                                    fraction->yMaxInParent = ::ULIS::FMath::Max( fraction->pointCoordsInParent[0].y, fraction->pointCoordsInParent[1].y );
                                    fraction->xMinInParent = ::ULIS::FMath::Min( fraction->pointCoordsInParent[0].x, fraction->pointCoordsInParent[1].x );
                                    fraction->yMinInParent = ::ULIS::FMath::Min( fraction->pointCoordsInParent[0].y, fraction->pointCoordsInParent[1].y );
                                }

                                // take advantage of this loop to compute the box.
                                // Note: bbox could be computed from the above polygon processing thing
                                SetSegmentBBox( segment, conversionMatrix );

                                segment->SetPaintingReady( true );
                            }
                        }
                    } //);
                }
            } //);
        }
    } // < execution time measurement : about monoT:200 microsecs over 18000, multiT:175


    FOdysseyVectorGroup::UpdateShape( iUpdateFlags ); // updates BBox

    if( bPainted )
    {
        if( ( bRealtime == true  )
       || ( ( bRealtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATE_PAINTGROUPS ) ) )
        {
            if( ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY      )
             || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
             || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   )
             || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
             || ( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE          ) )
            {
                FindCycles(); // also calls Clear()
            }

            Colorize();
        }
    }
    else
    {
        Clear();
    }
}

void
FOdysseyVectorGroupPaint::AddBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.push_back( iBucket );

    //iBucket->SetParent( this );
    Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorGroupPaint::RemoveBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.remove( iBucket );

    if( iBucket->IsSelected() )
    {
        UnselectBucket( iBucket );
    }

    Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorGroupPaint::RemoveAllBuckets()
{
    mBucketList.remove_if( [this]( FOdysseyVectorBucket* iBucket )
                           {
                                if( iBucket->IsSelected() )
                                {
                                    UnselectBucket( iBucket );
                                }

                                return true;
                            } );

    Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorGroupPaint::DrawShape( BLContext* iBLContext
                                   , const ::ULIS::FRectD& iInvalidationArea
                                   , double iCombinedOpacity
                                   , uint64 iFlags )
{
    FOdysseyVectorEngine* vectorEngine = GetEngine();
    ::ULIS::FRectD worldBBox = GetBBox( true );
    ::ULIS::FVec2D worldBBoxMin;
    ::ULIS::FVec2D worldBBoxMax;
    ::ULIS::FVec2D invalidationAreaMin;
    ::ULIS::FVec2D invalidationAreaMax;

    FOdysseyVector::GetRectMinMax<double>( worldBBox, worldBBoxMin, worldBBoxMax );

    FOdysseyVector::GetRectMinMax<double>( iInvalidationArea
                                         , invalidationAreaMin
                                         , invalidationAreaMax );

    // do not draw if outside screen
    if( ( bIntersectsCanvas == true )  
     || ( ( ( worldBBoxMin.x ) < invalidationAreaMax.x )
       && ( ( worldBBoxMax.x ) > invalidationAreaMin.x )
       && ( ( worldBBoxMin.y ) < invalidationAreaMax.y )
       && ( ( worldBBoxMax.y ) > invalidationAreaMin.y ) ) )
    {
        if( ( iFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR ) == 0 )
        {
            for( FOdysseyVectorCycle *cycle : mCycleList )
            {
                ::ULIS::FRectD cycleWorldBBox = cycle->GetBBox( true );
                ::ULIS::FVec2D cycleWorldBBoxMin;
                ::ULIS::FVec2D cycleWorldBBoxMax;

                FOdysseyVector::GetRectMinMax<double>( cycleWorldBBox
                                                     , cycleWorldBBoxMin
                                                     , cycleWorldBBoxMax );

                if( cycleWorldBBox.Area() > 1.0f )
                {
                    if( ( ( cycleWorldBBoxMin.x ) < invalidationAreaMax.x )
                     && ( ( cycleWorldBBoxMax.x ) > invalidationAreaMin.x )
                     && ( ( cycleWorldBBoxMin.y ) < invalidationAreaMax.y )
                     && ( ( cycleWorldBBoxMax.y ) > invalidationAreaMin.y ) )
                    {
                        cycle->Draw( iBLContext
                                   , iCombinedOpacity
                                   , iFlags
                                   , bMonochrome
                                   , mMonochromeColor );
                    }
                }
            }
        }

        if( iFlags & FOdysseyVectorEngine::DRAWING_WIREFRAME/* mWireframe*/ )
        {
            iBLContext->save();
            iBLContext->resetMatrix();
            iBLContext->setStrokeWidth( 1.0f );
            iBLContext->setStrokeStyle( BLRgba32( 0xFF, 0x00, 0x00, 0xFF ) );

            for( int i = 0; i < mGapSegmentBuffer.size(); i++ )
            {
                mGapSegmentBuffer[i].DrawStructure( iBLContext, this, true );
            }

    /* Works too
            for( int i = 0; i < mSectionBuffer.size(); i++ )
            {
                ::ULIS::FVec2D* sectionBezier = mSectionBuffer[i].GetBezier();
                BLPoint worldPoint[4] = { mWorldMatrix.mapPoint( sectionBezier[0].x, sectionBezier[0].y )
                                        , mWorldMatrix.mapPoint( sectionBezier[1].x, sectionBezier[1].y )
                                        , mWorldMatrix.mapPoint( sectionBezier[2].x, sectionBezier[2].y )
                                        , mWorldMatrix.mapPoint( sectionBezier[3].x, sectionBezier[3].y ) };
                BLPath path;

                path.moveTo( worldPoint[0].x, worldPoint[0].y );
                path.cubicTo( worldPoint[1].x, worldPoint[1].y
                            , worldPoint[2].x, worldPoint[2].y
                            , worldPoint[3].x, worldPoint[3].y );

                blctx->strokePath( path );
            }
    */

            iBLContext->restore();
        }
    }
}

bool
FOdysseyVectorGroupPaint::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    return FOdysseyVectorGroup::PickShape( iRoi, iSelectionFlags );
/*
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        for( FOdysseyVectorCycle *cycle : mCycleList )
        {
            if( cycle->HitTest( pt.x, pt.y ) )
            {
                return true;
            }
        }
    }

    return false;
*/
}

void
FOdysseyVectorGroupPaint::CreateVertexGapSegment( FOdysseyVectorVertex* iVertex )
{
    FOdysseyVectorVertex* nearestVertex = iVertex->GetNearestVertex();

    if( nearestVertex )
    {
        // if both vertices are regular vertices, only the one with the highest ptr has the right to create the segment
        if( ( ( nearestVertex->GetNearestVertex() == iVertex )  && ( iVertex > nearestVertex ) )
        ||    ( nearestVertex->GetNearestVertex() != iVertex )
        // or if the nearest vertex is an intersection vertex.
        ||    ( nearestVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubicGap& newGapSegment = mGapSegmentBuffer.emplace_back( this, nearestVertex, iVertex );
            //iGapSegmentBuffer[gapCount].Link(); // not necessary. saves us some cpu cycles
            newGapSegment.Update( 0 );

            FOdysseyVectorSection& newGapSection = mGapSectionBuffer.emplace_back( this, &newGapSegment, nearestVertex, iVertex, 0.0f, 1.0f, mShortSectionArray );
            //newGapSection.Link();

            // unlink now if gap section is colinear with vertex segment. It creates a mess in
            // case there are multiple gap sections.
            //newGapSection.GetVectorFromVertex( iVertex )
        }
    }
}

void
FOdysseyVectorGroupPaint::CreateSegmentSections( FOdysseyVectorSegment* iSegment )
{
    std::list<FOdysseyVectorIntersection*>& intersectionList = iSegment->GetIntersectionList();
    FOdysseyVectorVertex* segmentVertex0 = iSegment->GetVertex(0);
    FOdysseyVectorVertex* segmentVertex1 = iSegment->GetVertex(1);
    FOdysseyVectorVertex* sectionVertex0 = segmentVertex0;
    double sectionVertex0T = 0.0f;

    if( intersectionList.size() )
    {
        for( FOdysseyVectorIntersection* intersection : intersectionList )
        {
            FOdysseyVectorVertex* sectionVertex1 = intersection->GetIntersectionVertex();
            double sectionVertex1T = intersection->GetSegmentT();
            // constructor also links sections to the vertex

            mSectionBuffer.emplace_back( this
                                       , iSegment
                                       , sectionVertex0
                                       , sectionVertex1
                                       , sectionVertex0T
                                       , sectionVertex1T
                                       , mShortSectionArray );

            sectionVertex0 = sectionVertex1;
            sectionVertex0T = sectionVertex1T;
        }
    }

    mSectionBuffer.emplace_back( this
                               , iSegment
                               , sectionVertex0
                               , segmentVertex1 // segment's second end point
                               , sectionVertex0T
                               , 1.0f
                               , mShortSectionArray );

    iSegment->ClearIntersections();
}

void
FOdysseyVectorGroupPaint::CreatePathSections( FOdysseyVectorPath* iPath
                                            , BLMatrix2D* iConversionMatrix )
{
    for( FOdysseyVectorSegment *segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex( 0 );
        FOdysseyVectorVertex* vertex1 = segment->GetVertex( 1 );

        CreateSegmentSections( segment );

        //TODO::Possible optimization: call only if nearestVertex exists
        CreateVertexGapSegment( vertex0 );
        CreateVertexGapSegment( vertex1 );
    }
}

void
FOdysseyVectorGroupPaint::IntersectSegmentWithList( FOdysseyVectorSegment* iSegment
                                                  , const std::list<FOdysseyVectorSegment*>& iSegmenList )
{
    ::ULIS::FRectD& segmentBBoxInParent = iSegment->GetBBoxInParent();
    ::ULIS::FVec2D segmentMinInParentWithTolerance( segmentBBoxInParent.x - mGapTolerance
                                                  , segmentBBoxInParent.y - mGapTolerance );
    ::ULIS::FVec2D segmentMaxInParentWithTolerance( segmentBBoxInParent.x + segmentBBoxInParent.w + mGapTolerance
                                                  , segmentBBoxInParent.y + segmentBBoxInParent.h + mGapTolerance );

    for( FOdysseyVectorSegment *intersectSegment : iSegmenList )
    {
        ::ULIS::FRectD& intersectSegmentBBoxInParent = intersectSegment->GetBBoxInParent();
        ::ULIS::FVec2D intersectSegmentMinInParentWithTolerance( intersectSegmentBBoxInParent.x - mGapTolerance
                                                               , intersectSegmentBBoxInParent.y - mGapTolerance );
        ::ULIS::FVec2D intersectSegmentMaxInParentWithTolerance( intersectSegmentBBoxInParent.x + intersectSegmentBBoxInParent.w + mGapTolerance
                                                               , intersectSegmentBBoxInParent.y + intersectSegmentBBoxInParent.h + mGapTolerance );

        if( ( segmentMinInParentWithTolerance.x < intersectSegmentMaxInParentWithTolerance.x )
         && ( segmentMaxInParentWithTolerance.x > intersectSegmentMinInParentWithTolerance.x )
         && ( segmentMinInParentWithTolerance.y < intersectSegmentMaxInParentWithTolerance.y )
         && ( segmentMaxInParentWithTolerance.y > intersectSegmentMinInParentWithTolerance.y ) )
        {
            IntersectSegment( static_cast<FOdysseyVectorSegmentCubic*>(iSegment)
                            , static_cast<FOdysseyVectorSegmentCubic*>(intersectSegment)
                            , intersectSegmentMinInParentWithTolerance
                            , intersectSegmentMaxInParentWithTolerance
                            , mXIntersectionRecordArray );
        }
    }

/* this version is 4 times slower than the above !!!
    uint32 intersectionCount = 0;

    for( FOdysseyVectorSegment *intersectSegment : iSegmenList )
    {
        //if( intersectSegment->GetPaintingCode() != mPaintingCode )
        {
            ::ULIS::FRectD intersectRect = intersectSegment->GetBBoxInParent() & iSegment->GetBBoxInParent();

            if( intersectRect.Area() )
            {
                IntersectSegment( static_cast<FOdysseyVectorSegmentCubic*>(iSegment)
                                , static_cast<FOdysseyVectorSegmentCubic*>(intersectSegment)
                                , mGapTolerance
                                , iIntersectionArray );
            }
        }
    }
*/

    //return intersectionCount;
}

static void
PrintVertex( FOdysseyVectorVertex* iVertex )
{
    BLPoint pt = iVertex->GetOwner()->GetWorldMatrix().mapPoint( iVertex->GetCoords().x, iVertex->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Vertex: [x:%f y:%f]"), pt.x, pt.y);
}

static void
PrintSection( FOdysseyVectorSection* iSection)
{
    FOdysseyVectorObject* owner0 = iSection->GetVertex(0)->GetOwner();
    FOdysseyVectorObject* owner1 = iSection->GetVertex(1)->GetOwner();
    FOdysseyVectorSegment* segment = iSection->GetSegment();
    BLPoint pt0 = owner0->GetWorldMatrix().mapPoint( iSection->GetVertex(0)->GetCoords().x, iSection->GetVertex(0)->GetCoords().y );
    BLPoint pt1 = owner1->GetWorldMatrix().mapPoint( iSection->GetVertex(1)->GetCoords().x, iSection->GetVertex(1)->GetCoords().y );
    BLPoint segpt0 = owner0->GetWorldMatrix().mapPoint( segment->GetVertex(0)->GetCoords().x, segment->GetVertex(0)->GetCoords().y );
    BLPoint segpt1 = owner1->GetWorldMatrix().mapPoint( segment->GetVertex(1)->GetCoords().x, segment->GetVertex(1)->GetCoords().y );

    //UE_LOG(LogTemp,Warning,TEXT("Section: [x:%.8f y:%.8f] -- [x:%.8f y:%.8f]/segment[x:%f y:%f] -- [x:%f y:%f] - flags : %d"), pt0.x, pt0.y, pt1.x, pt1.y, segpt0.x, segpt0.y, segpt1.x, segpt1.y, iSection->GetFlags() );
    UE_LOG(LogTemp,Warning,TEXT("Section: [x:%.8f y:%.8f] -- [x:%.8f y:%.8f]"), pt0.x, pt0.y, pt1.x, pt1.y );
}

static void
PrintCycle( std::vector<FOdysseyVectorVertex*>& vertexArray
          , std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = sectionArray[i]->GetSegment();
        BLPoint pt0 = segment->GetOwnerAsPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(0)->GetCoords().x, sectionArray[i]->GetVertex(0)->GetCoords().y );
        BLPoint pt1 = segment->GetOwnerAsPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(1)->GetCoords().x, sectionArray[i]->GetVertex(1)->GetCoords().y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

uint32
FOdysseyVectorGroupPaint::FindPath( FOdysseyVectorSection* iReturnSection
                                  , uint32 iSectionVertexIndex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<uint32>& oVertexIndexArray
                                  , std::vector<FOdysseyVectorSection*>& oSectionArray
                                  , double iOrientation
                                  , uint32 iDepth ) // we could also use iVertexArray.size()
{
    FOdysseyVectorVertex* sectionVertex = iSection->GetVertex( iSectionVertexIndex );
    uint32 sectionNextVertexIndex = ( iSectionVertexIndex == 0 ) ? 1 : 0;
    FOdysseyVectorVertex* sectionNextVertex = iSection->GetVertex( sectionNextVertexIndex );
    uint32 ret = FOdysseyVectorGroupPaint::NOCYCLE;
    bool isLoop = false;

    oVertexIndexArray.push_back( iSectionVertexIndex );
    oSectionArray.push_back( iSection );
    iSection->Block( iSectionVertexIndex );

//UE_LOG(LogTemp, Warning, TEXT("Next vertex valence %d"), sectionNextVertex->GetSectionCount() );
    //iSection->Print();

//UE_LOG(LogTemp, Warning, TEXT("vertices %x %x"), sectionNextVertex, oSectionArray[0]->GetVertex(oVertexIndexArray[0]) );
    //isLoop = ( oSectionArray[0]->GetVertex(oVertexIndexArray[0])->GetID() == sectionNextVertex->GetID() );

    isLoop = ( oSectionArray[0]->GetVertex(oVertexIndexArray[0]) == sectionNextVertex );

    if( ( isLoop == true )// cycle detected
    && ( ( ( iReturnSection->IsLinked() == true ) && ( iReturnSection == iSection ) ) // 1 return path accepted
        || ( iReturnSection->IsLinked() == false ) ) ) // any return path accepted
    {
        double normalVector = GetCycleNormalVector( oVertexIndexArray, oSectionArray );

//UE_LOG(LogTemp, Warning, TEXT("Cycle detected %f size %d"), GetCycleNormalVector( oVertexIndexArray, oSectionArray ), oSectionArray.size() ); 
        if (normalVector > 0.0f)
        {
//UE_LOG(LogTemp, Warning, TEXT("Cycle committed") ); 
            mCycleList.push_back( new FOdysseyVectorCycle( this, oVertexIndexArray, oSectionArray ) );
        }

        ret = FOdysseyVectorGroupPaint::HASCYCLE;
    }
    else
    {
        FSectionLinkInfo* sectionLinkInfo = sectionNextVertex->GetSectionLinkInfo( iSection, sectionNextVertexIndex );
        FSectionLinkInfo* nextSectionLinkInfo = sectionNextVertex->GetCycleNextSection( sectionLinkInfo, 1.0f );

        if( nextSectionLinkInfo )
        {
            FOdysseyVectorSection* nextSection = nextSectionLinkInfo->section;

            if( nextSection->IsBlocked( nextSectionLinkInfo->sectionVertexIndex ) == false )
            {
                ret = FindPath( iReturnSection
                                , nextSectionLinkInfo->sectionVertexIndex
                                , nextSectionLinkInfo->section
                                , oVertexIndexArray
                                , oSectionArray
                                , iOrientation
                                , iDepth + 1 );
            }
            else
            {
                ret = FOdysseyVectorGroupPaint::BLOCKED;
            }
        }
    }

    // propbably useless
    oVertexIndexArray.pop_back();
    oSectionArray.pop_back();

    return ret;
}

// find the overall orientation of the cycle
static double
GetCycleNormalVector( std::vector<uint32>& iVertexIndexArray
                    , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    double z = 0;
    int32 arraySize = iSectionArray.size();

    for( int i = 0; i < arraySize; i++ )
    {
        int n = ( i + 1 ) % arraySize;
        FOdysseyVectorSection* sectioni = iSectionArray[i];
        FOdysseyVectorSegment* segment = iSectionArray[i]->GetSegment();
        uint32 vertexIndex = iVertexIndexArray[i];
        FOdysseyVectorVertex* vertex = sectioni->GetVertex(iVertexIndexArray[i]);
        uint32 nextVertexIndex = ( vertexIndex == 0 ) ? 1 : 0;
        FOdysseyVectorVertex* nextVertex = sectioni->GetVertex(nextVertexIndex);

        // Note: we use FOdysseyVectorSection::GetVertexCoords() because coords will be in Paintgroup's space.

        ::ULIS::FVec2D& viCoords = sectioni->GetVertexCoords( vertex     );
        ::ULIS::FVec2D& vnCoords = sectioni->GetVertexCoords( nextVertex );

        if( segment->GetClass() == FOdysseyVectorSegmentCubicGap::StaticClass() )
        {
            z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
        }

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            double deltaT = (double)nextVertexIndex - (double)vertexIndex;
            int subdiv = 8;
            double stepT = deltaT / subdiv;
            double t0 = vertexIndex;

            // By relying only on start and end points of a section, we lack precision. 
            // Here we rely on more acurate computation by getting intermediate points.
            for( int j = 0; j < subdiv; j++ )
            {
                double t1 = t0 + stepT;
                // however at end points, we need the same coordinates for each section. Relying on T value does not guarantee that
                // due to imprecision and would fake the calculation. So, we use the value stored in viCoords and vnCoords.
                ::ULIS::FVec2D v0Coords = ( j == 0          ) ? viCoords : sectioni->GetPointAt( t0 );
                ::ULIS::FVec2D v1Coords = ( j == subdiv - 1 ) ? vnCoords : sectioni->GetPointAt( t1 );

                z += ( ( v0Coords.x - v1Coords.x ) * ( v0Coords.y + v1Coords.y ) );

                t0 += stepT;
            }
        }


// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
// Newell's method
        //z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
    }

    return z;
}

uint32
FOdysseyVectorGroupPaint::Explore( FExplorationPair* iExplorationPair )
{
    //UE_LOG(LogTemp, Warning, TEXT("Valence: %d"), iExplorationPair->departSection->GetVertex( iExplorationPair->departVertexIndex )->GetSectionCount() );
    //PrintVertex( iExplorationPair->departSection->GetVertex( iExplorationPair->departVertexIndex ) );

    if( iExplorationPair->departSection )
    {
        if( iExplorationPair->departSection->IsLinked() == true )
        {
            if( iExplorationPair->departSection->IsBlocked( iExplorationPair->departVertexIndex ) == false )
            {
                std::vector<uint32> vertexIndexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                vertexIndexArray.reserve( 10 );
                sectionArray.reserve( 10 );

                uint32 ret = FindPath( iExplorationPair->returnSection
                                     , iExplorationPair->departVertexIndex // lies on departSection
                                     , iExplorationPair->departSection
                                     , vertexIndexArray
                                     , sectionArray
                                     , 1.0f
                                     , 0 );
            }
        }
    }

    return 0;
}

// static
void
FOdysseyVectorGroupPaint::SetSegmentBBox( FOdysseyVectorSegment* iSegment
                                        , BLMatrix2D& iConversionMatrix )
{
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
        ::ULIS::FVec2D* bezier = cubicSegment->GetBezier();
        BLPoint p[4] = { iConversionMatrix.mapPoint( bezier[0].x, bezier[0].y )
                       , iConversionMatrix.mapPoint( bezier[1].x, bezier[1].y )
                       , iConversionMatrix.mapPoint( bezier[2].x, bezier[2].y )
                       , iConversionMatrix.mapPoint( bezier[3].x, bezier[3].y ) };
        double xmin = ::ULIS::FMath::Min4( p[0].x, p[1].x, p[2].x, p[3].x )
             , ymin = ::ULIS::FMath::Min4( p[0].y, p[1].y, p[2].y, p[3].y )
             , xmax = ::ULIS::FMath::Max4( p[0].x, p[1].x, p[2].x, p[3].x )
             , ymax = ::ULIS::FMath::Max4( p[0].y, p[1].y, p[2].y, p[3].y );

        iSegment->SetBBoxInParent( ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax ) );
    }
}

void
FOdysseyVectorGroupPaint::FindCycles()
{
    std::vector<FExplorationPair> explorationPairsBuffer;

    //auto startTotal = std::chrono::high_resolution_clock::now();

    Clear(); // also build segments bounding boxes in parent

    explorationPairsBuffer.reserve( 100 );

    BuildGraph();

    // Build exploration pair before simplification
    for( FOdysseyVectorVertexIntersection& intersectionVertex : mIntersectionVertexArray )
    {
        //intersectionVertex.SetID( mVertexID++ );

        intersectionVertex.BuildExplorationPairs( explorationPairsBuffer );
    }

    // Build exploration pair for vertex-vertex gaps before simplification
    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
    {
        //if( mGapSectionBuffer[i].IsLinked() ) // not sure if really needed anymore
        {
            FOdysseyVectorVertex* vertex0 = mGapSectionBuffer[i].GetVertex(0);
            FOdysseyVectorVertex* vertex1 = mGapSectionBuffer[i].GetVertex(1);

            // exploration pairs only needed for vertex-vertex gaps because 
            // for vertex-segment gaps, there is an intersection that have created
            // the exploration pairs in the previous loop.
            if( ( vertex0->GetClass() == FOdysseyVectorVertex::StaticClass() )
             && ( vertex1->GetClass() == FOdysseyVectorVertex::StaticClass() ) )
            {
                vertex0->BuildExplorationPairs( explorationPairsBuffer );
                // Note: Technically, we don't have to check the second vertex. Only
                // one is needed to build an exploration pair. however, due to section stitching
                // it is not guaranted that the first vertex will be linked to any section
                // so we build an exploration pair for both of them.
                vertex1->BuildExplorationPairs( explorationPairsBuffer );
            }
        }
    }

    // sort exploration pairs in order to always have a propagation that starts from
    // the same vertex/section between sessions. This is needed in monothread and 
    // multihread modes because the exploration pairs won't be in the same order 
    // and we may switch from one to the other.
    std::sort( explorationPairsBuffer.begin()
             , explorationPairsBuffer.end()
             , []( FExplorationPair& iPairA, FExplorationPair& iPairB )
               {
                 return iPairA.sectionLength > iPairB.sectionLength;
               } );

    SimplifyGraph();

    // explore the graph from intersections
    for( int i = 0; i < explorationPairsBuffer.size(); i++ )
    {
        Explore( &explorationPairsBuffer[i] );
    }

    OrderCycles();

    MergeCycles();

    // section topology must be unlinked now or else if we transfer path to another group, it may not be cleaned
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        FOdysseyVectorSection *section = &mSectionBuffer[i];
        // Note: unlinking with true must be done on all section, not only the ones linked.
        section->Unlink( true );
    }

    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
    {
        FOdysseyVectorSection *section = &mGapSectionBuffer[i];
        // Note: unlinking with true must be done on all section, not only the ones linked.
        section->Unlink( true );
    }

    //auto stopTotal = std::chrono::high_resolution_clock::now();
    //auto durationTotal = std::chrono::duration_cast<std::chrono::microseconds>(stopTotal - startTotal);
    //UE_LOG(LogTemp, Warning, TEXT("FindCycles Exec time %llu"), durationTotal.count() );
}

void
FOdysseyVectorGroupPaint::CreateNearIntersection( FOdysseyVectorVertex *iVertex )
{
    FOdysseyVectorSegment *nearestSegment = iVertex->GetNearestSegment();
    FOdysseyVectorVertex* nearestVertex = iVertex->GetNearestVertex();
    double distanceToNearestSegment = iVertex->GetDistanceToNearestSegment();
    double distanceToNearestVertex = iVertex->GetDistanceToNearestVertex();

    if ( nearestSegment && ( distanceToNearestSegment < distanceToNearestVertex ) )
    {
        double nearestSegmentT = iVertex->GetNearestSegmentT();
        bool nearsetVertexIsOnNearestSegment = nearestVertex && nearestVertex->HasSegment( nearestSegment ) ? true : false;
 
        // when the nearest intersection is too close from the segment endpoint, it can create
        // issues with the geometry so we need to do this check.
        if( ( ( nearestSegmentT > 0.0001f ) && ( nearestSegmentT < 0.9999f ) && ( nearsetVertexIsOnNearestSegment == true  ) )
         || ( ( nearestSegmentT > 0.0f    ) && ( nearestSegmentT < 1.0f    ) && ( nearsetVertexIsOnNearestSegment == false ) ) )
        {
        //if( ( nearestSegmentT > 0.0f ) && ( nearestSegmentT < 1.0f ) )
        //{
            //::ULIS::FVec2D nearestVertexAt = nearestSegment->GetPointAt( nearestSegmentT );
            ::ULIS::FVec2D nearestVertexAt = iVertex->GetNearestSegmentIntersectionCoords();

            mTIntersectionRecordArray.emplace_back( nearestVertexAt.x
                                                  , nearestVertexAt.y
                                                  , nearestSegment
                                                  , nearestSegmentT
                                                  , iVertex );

            if( nearestVertex )
            {
                iVertex->ResetNearestVertex();
            }
        }
    }
}

double
FOdysseyVectorGroupPaint::GetGapTolerance()
{
    return mGapTolerance;
}

void
FOdysseyVectorGroupPaint::SetGapTolerance( double iGapTolerance )
{
    mGapTolerance = iGapTolerance;

    Invalidate( FOdysseyVectorObject::INVALIDATE_SHAPE );
}

bool
FOdysseyVectorGroupPaint::IsMonochrome()
{
    return bMonochrome;
}

void
FOdysseyVectorGroupPaint::SetMonochrome( bool iIsMonochrome )
{
    bMonochrome = iIsMonochrome;
}

FColor&
FOdysseyVectorGroupPaint::GetMonochromeColor()
{
    return mMonochromeColor;
}

void
FOdysseyVectorGroupPaint::SetMonochromeColor( const FColor& iMonochromeColor )
{
    mMonochromeColor = iMonochromeColor;
}

void
FOdysseyVectorGroupPaint::SetMonochromeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mMonochromeColor.R = iR;
    mMonochromeColor.G = iG;
    mMonochromeColor.B = iB;
    mMonochromeColor.A = iA;
}

void
FOdysseyVectorGroupPaint::GetMonochromeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA )
{
    oR = mMonochromeColor.R;
    oG = mMonochromeColor.G;
    oB = mMonochromeColor.B;
    oA = mMonochromeColor.A;
}

bool
FOdysseyVectorGroupPaint::IsWireframe()
{
    return bWireframe;
}

void
FOdysseyVectorGroupPaint::SetWireframe( bool iIsWireframe )
{
    bWireframe = iIsWireframe;
}

FColor&
FOdysseyVectorGroupPaint::GetWireframeColor()
{
    return mWireframeColor;
}

void
FOdysseyVectorGroupPaint::SetWireframeColor( const FColor& iWireframeColor )
{
    mWireframeColor = iWireframeColor;
}

void
FOdysseyVectorGroupPaint::SetWireframeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mWireframeColor.R = iR;
    mWireframeColor.G = iG;
    mWireframeColor.B = iB;
    mWireframeColor.A = iA;
}

void
FOdysseyVectorGroupPaint::GetWireframeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA )
{
    oR = mWireframeColor.R;
    oG = mWireframeColor.G;
    oB = mWireframeColor.B;
    oA = mWireframeColor.A;
}

bool
FOdysseyVectorGroupPaint::IsPainted()
{
    return bPainted;
}

void
FOdysseyVectorGroupPaint::SetPainted( bool iPainted )
{
    bPainted = iPainted;

    Invalidate( FOdysseyVectorObject::INVALIDATE_SHAPE );
}

bool
FOdysseyVectorGroupPaint::IsMultithreaded()
{
    return bMultithreaded;
}

void
FOdysseyVectorGroupPaint::SetMultithreaded( bool iMultithreaded )
{
    bMultithreaded = iMultithreaded;
}

// static
void
FOdysseyVectorGroupPaint::RecursiveUpdatePathList( FOdysseyVectorObject* iCandidateObject
                                                 , std::list<FOdysseyVectorPath*>& iPathList )
{
    if( iCandidateObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iCandidateObject);

        iPathList.push_back( path );
    }

    // recurse if simple group
    if( iCandidateObject->GetClass() == FOdysseyVectorGroup::StaticClass() )
    {
        for( FOdysseyVectorObject* child : iCandidateObject->GetChildrenList() )
        {
            RecursiveUpdatePathList( child, iPathList );
        }
    }
}

void
FOdysseyVectorGroupPaint::UpdatePathList()
{
    mPathList.clear();

    if( bIntersectsCanvas )
    {
        mPathList.push_back( &mCanvasPath );
    }

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        RecursiveUpdatePathList( child, mPathList );
    }

/*
    for( FOdysseyVectorObject* child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            mPathList.push_back( path );
        }
    }
*/
}
/*
void
FOdysseyVectorGroupPaint::AddChild( FOdysseyVectorObject* iChild
                                  , FOdysseyVectorObject* iInsertAfter )
{
    FOdysseyVectorGroup::AddChild( iChild, iInsertAfter );

    UpdatePathList();
}

void
FOdysseyVectorGroupPaint::RemoveChild()
{
}
*/

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    //std::list<FOdysseyVectorPath*> intersectedPathList = mPathList; // copy
    uint32 totalGapSegmentCount = 0;
    uint32 totalSectionCount = 0;

    // act as boolean without the need to reinitialize its value
    ++mPaintingCode;

    if( bMultithreaded )
    {
#if !PLATFORM_MAC
#ifdef perpath
        std::for_each( std::execution::par_unseq
                     , mPathList.begin()
                     , mPathList.end()
                     , [this]( FOdysseyVectorPath *path )
        {
            for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
            {
                for( FOdysseyVectorPath *intersectedPath : mPathList )
                {
                    // populate mXIntersectionRecordArray
                    /*intersectionCount += */IntersectSegmentWithList ( segment
                                                                      , intersectedPath->GetSegmentList() );
                }
            }
        } );
#endif
#ifndef perpathpersegment
        std::for_each( std::execution::par_unseq
                      , mPathList.begin()
                      , mPathList.end()
                      , [ this ]( FOdysseyVectorPath *path )
        {
        //for( FOdysseyVectorPath *path : mPathList )
        //{
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            std::for_each( std::execution::par_unseq
                         , segmentList.begin()
                         , segmentList.end()
                         , [ this ]( FOdysseyVectorSegment *segment )
            {
                for( FOdysseyVectorPath *intersectedPath : mPathList )
                {
                    // populate mXIntersectionRecordArray
                    IntersectSegmentWithList ( segment
                                             , intersectedPath->GetSegmentList() );
                }
            } );
        } );
#endif
#endif
    }
    else
    {
        for( FOdysseyVectorPath *path : mPathList )
        {
            for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
            {
                for( FOdysseyVectorPath *intersectedPath : mPathList )
                {
                    // populate mXIntersectionRecordArray
                    IntersectSegmentWithList ( segment
                                             , intersectedPath->GetSegmentList() );
                }
            }
        }
    }

    for( FOdysseyVectorPath *path : mPathList )
    {
        for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
        {
            //segment->SetPaintingCode( mPaintingCode ); // set as treated. It will be excluded from later intersection tests.

            if( segment->GetIntersectionSlotCount() )
            {
                segment->GetOwnerAsPath()->SetPaintingCode( mPaintingCode );
            }
        }

        // create near-intersections
        for( FOdysseyVectorVertex *vertex : path->GetVertexList() )
        {
            if( vertex->GetNearestSegment() )
            {
                // populate mTIntersectionRecordArray
                CreateNearIntersection( vertex );

                vertex->GetNearestSegment()->GetOwnerAsPath()->SetPaintingCode( mPaintingCode );
                vertex->GetOwnerAsPath()->SetPaintingCode( mPaintingCode );
            }
        }

        if( path->IsLoop() == true )
        {
            // acts as a boolean flag
            path->SetPaintingCode( mPaintingCode );
        }

        //intersectedPathList.pop_front(); // we don't need the path anymore. By and by the list will empty by itself.
    }

    // we now need another loop to count and to reserve the memory in one block to create the sections.
    // we do that so that we can alloc the buffers at once instead of allocating a lot of new sections/segments.
    // This is needed especially with gaps because segments get all their intersections at the end of the whole intersecting process.
    for( FOdysseyVectorPath *path : mPathList )
    {
        if( path->GetPaintingCode() == mPaintingCode )
        {
            for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
            {
                FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
                FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

//vertex0->SetNearestVertex(nullptr, DBL_MAX);
//vertex1->SetNearestVertex(nullptr, DBL_MAX);

                FOdysseyVectorVertex* nearestVertex0 = vertex0->GetNearestVertex();
                FOdysseyVectorVertex* nearestVertex1 = vertex1->GetNearestVertex();

                totalSectionCount += ( 1 + segment->GetIntersectionSlotCount() );

                if( nearestVertex0 )
                {
                    if( ( ( nearestVertex0->GetNearestVertex() == vertex0 ) && ( vertex0 > nearestVertex0 ) )
                       || ( nearestVertex0->GetNearestVertex() != vertex0 ) ) // Note : intersections don't have nearest vertices.
                    {
                        totalGapSegmentCount++;
                        //totalSectionCount++;
                    }
                }

                if( nearestVertex1 )
                {
                    if( ( ( nearestVertex1->GetNearestVertex() == vertex1 ) && ( vertex1 > nearestVertex1 ) )
                       || ( nearestVertex1->GetNearestVertex() != vertex1 ) ) // Note : intersections don't have nearest vertices.
                    {
                        totalGapSegmentCount++;
                        //totalSectionCount++;
                    }
                }

            }
        }
    }

    // reserving whole block is required to avoid memory shifting.
    mGapSegmentBuffer.reserve( totalGapSegmentCount + mTIntersectionRecordArray.size() );
    // we most store gap sections separately to be able to check afterwards if they don't intersect
    // without going through the whole section buffer.
    mGapSectionBuffer.reserve( totalGapSegmentCount + mTIntersectionRecordArray.size() );
    // reserving whole block is required to avoid memory shifting.
    mSectionBuffer.reserve( totalSectionCount );
    // reserve memory to vertices in one go.
    mIntersectionVertexArray.reserve( mXIntersectionRecordArray.size()
                                    + mTIntersectionRecordArray.size() );

    // then init X-Junction intersection vertices.
    for( FXIntersectionRecord& XintersectionRecord : mXIntersectionRecordArray )
    {
        mIntersectionVertexArray.emplace_back( this
                                             , XintersectionRecord.x
                                             , XintersectionRecord.y 
                                             , XintersectionRecord.segment0
                                             , XintersectionRecord.segment0T
                                             , XintersectionRecord.segment1
                                             , XintersectionRecord.segment1T );
    }

    // then init T-Junction intersection vertices (gaps).
    for( FTIntersectionRecord& TintersectionRecord : mTIntersectionRecordArray )
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = 
        &mIntersectionVertexArray.emplace_back( this
                                              , TintersectionRecord.x
                                              , TintersectionRecord.y 
                                              , TintersectionRecord.segment
                                              , TintersectionRecord.segmentT
                                              , TintersectionRecord.vertex );

        // commented-out. Called in the above constructor for FOdysseyVectorVertexIntersection()
        //TintersectionRecord.vertex->SetNearestVertex( intersectionVertex, 0.0f );
    }

    // create sections for exact intersections on each segment

    for( FOdysseyVectorPath *path : mPathList )
    {
        // only for path that have intersected segments.
        if( path->GetPaintingCode() == mPaintingCode )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
            BLMatrix2D& pathWorldMatrix = path->GetWorldMatrix();
            BLMatrix2D conversionMatrix;
            // get result here before intersections get cleared in CreatePathSections()
            bool hasIntersections = path->HasIntersections();

            // TODO: possible optimization: the conversion matrix is also computed in Clear(). It could be stored
            FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, pathWorldMatrix, conversionMatrix );

            CreatePathSections( path, &conversionMatrix );

            // create a cycle right now for untouched looped-paths
            if( ( path->IsLoop() == true ) && ( hasIntersections == false ) )
            {
                std::vector<uint32> vertexIndexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                path->ToVertexIndexAndSectionArray( vertexIndexArray, sectionArray );

                if( vertexIndexArray.size() )
                {
                    mCycleList.push_back( new FOdysseyVectorCycle( this
                                                                 , vertexIndexArray
                                                                 , sectionArray ) );
                }
            }
        }
    }

    // Get rid of section of length 0
    for( FOdysseyVectorSection* shortSection : mShortSectionArray )
    {
        // Unlink() and stitch
        shortSection->Stitch();
    }
}

void
FOdysseyVectorGroupPaint::MergeCycles()
{
    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        FOdysseyVectorCycle* parentCycle = cycle->GetParentCycle();

        if( parentCycle )
        {
            parentCycle->Merge( cycle );
        }
    }
}

void
FOdysseyVectorGroupPaint::OrderCycles()
{
    for( FOdysseyVectorCycle* cycle : mCycleList )
    {
        for( FOdysseyVectorCycle* innerCycle : mCycleList )
        {
           if( cycle != innerCycle )
           {
               if( innerCycle->FitsIn( cycle ) )
               {
                   FOdysseyVectorCycle* parentCycle = innerCycle->GetParentCycle();

                   if( parentCycle )
                   {
                       if( parentCycle->FitsIn( cycle ) == false )
                       {
                           innerCycle->SetParentCycle( cycle );
                       }
                   }
                   else
                   {
                       innerCycle->SetParentCycle( cycle );
                   }
               }
           }
        }
    }
}

static bool
GapSectionIntersects( FOdysseyVectorSection& iSection
                    , std::vector<FOdysseyVectorSection>& iGapSectionBuffer )
{
    ::ULIS::FVec2D* sectionBezier = iSection.GetBezier();
    FOdysseyVectorVertex* vertex0 = iSection.GetVertex(0);
    FOdysseyVectorVertex* vertex1 = iSection.GetVertex(1);

    // check a gap section does not cross another gap section.
    for( FOdysseyVectorSection& otherSection : iGapSectionBuffer )
    {
        if( ( &otherSection != &iSection ) && otherSection.IsLinked() )
        {
            FOdysseyVectorVertex* otherVertex0 = otherSection.GetVertex(0);
            FOdysseyVectorVertex* otherVertex1 = otherSection.GetVertex(1);
            ::ULIS::FVec2D* otherSectionBezier = otherSection.GetBezier();
            double otherSectionT;
            double sectionT;

            if( ( ( otherVertex0 == vertex0 ) && ( otherVertex1 == vertex1 ) )
            ||  ( ( otherVertex1 == vertex0 ) && ( otherVertex0 == vertex1 ) ) )
            {
                return true;
            }

            if( FOdysseyVector::IntersectSegment( sectionBezier[0]
                                                , sectionBezier[3]
                                                , otherSectionBezier[0]
                                                , otherSectionBezier[3]
                                                , &sectionT
                                                , &otherSectionT ) )
            {
                // dont consider intersections at endpoints
                if( ( sectionT      > 0.0f ) && ( sectionT      < 1.0f )
                 && ( otherSectionT > 0.0f ) && ( otherSectionT < 1.0f ) )
                {
                    return true;
                }
            }
        }
    }

    // check the gap section does not cross the segment it is connected to.
    if( ( vertex0->GetClass() == FOdysseyVectorVertex::StaticClass() )
     && ( vertex1->GetClass() == FOdysseyVectorVertex::StaticClass() ) )
    {
        // Reminder: the gap segment is not linked, thus we only retrieve an original segment
        // and test for intersection with this one.
        FOdysseyVectorSegment* vertex0Segment = vertex0->GetSegment( vertex1 );
        FOdysseyVectorSegment* vertex1Segment = vertex1->GetSegment( vertex0 );

        if( vertex0Segment && vertex1Segment )
        {
            if( vertex0Segment == vertex1Segment )
            {
                if( IntersectGapSection( &iSection, vertex0Segment ) )
                {
                    return true;
                }

                // check the section is not colinear with the segment.
                // this confuses the algorithm from cross and dot products.
                // this happens when a vertex-vertex gap is created between to endpoints of
                // the same segment and this segment is small and straight. This is why the
                // simpliest check is to compare the length of the section and the said segment
                if( iSection.GetLength() == vertex0Segment->GetLength() )
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void
FOdysseyVectorGroupPaint::SimplifyGraph()
{
    bool keepSimplifying;

    // unlink gap sections that intersect with other gap sections.
    for( FOdysseyVectorSection& section : mGapSectionBuffer )
    {
        if( GapSectionIntersects( section, mGapSectionBuffer ) )
        {
            section.Unlink( false );
        }
    }

    do
    {
        keepSimplifying = false;

        for( int i = 0; i < mSectionBuffer.size(); i++ )
        {
            FOdysseyVectorSection *section = &mSectionBuffer[i];

            if( section->IsLinked() == true )
            {
                if( section->GetVertex(0) != section->GetVertex(1) ) // exclude loops
                {
                    if( ( section->GetVertex(0)->GetSectionCount() == 1 )
                    ||  ( section->GetVertex(1)->GetSectionCount() == 1 )
                    // filter invalid section with length 0. Yes, this can happend due to floating point imprecision
                    // when an intersection is very very close to the end vertex.
                    /*||  ( section->IsValid() == false )*/ )
                    {
                        keepSimplifying = true;

                        section->Unlink( false );
                    }
                }
            }
        }

        for( int i = 0; i < mGapSectionBuffer.size(); i++ )
        {
            FOdysseyVectorSection *section = &mGapSectionBuffer[i];

            if( section->IsLinked() == true )
            {
                //if( section->GetSegment() )
                {
                    if( ( section->GetVertex(0)->GetSectionCount() == 1 )
                    ||  ( section->GetVertex(1)->GetSectionCount() == 1 ) )
                    {
                        keepSimplifying = true;

                        section->Unlink( false );
                    }
                }
            }
        }
    } while ( keepSimplifying );
}

void
FOdysseyVectorGroupPaint::Clear()
{
    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        delete cycle;
    }

    mCycleList.clear();
    // clear temporary intersection info
    mXIntersectionRecordArray.clear();
    mTIntersectionRecordArray.clear();
    // Clear gap segments. Not necessary to unlink them, as these are not linked. Only section are linked.
    mGapSegmentBuffer.clear();
    mGapSectionBuffer.clear();
    // clean section topology (unlinking has been moved after the cycle detection).
    mSectionBuffer.clear();
    // handling of sections of length 0
    mShortSectionArray.clear();

    mVertexID = 0;

    // <TODO:MULTITHREADABLE>
    if( bMultithreaded )
    {
#if !PLATFORM_MAC
        std::for_each( std::execution::par_unseq
                     , mPathList.begin()
                     , mPathList.end()
                     , [ this ]( FOdysseyVectorPath *path )
        {
            std::list<FOdysseyVectorVertex*>& vertexList = path->GetVertexList();
            BLMatrix2D conversionMatrix;

            FOdysseyVector::MatrixMultiply( mInverseWorldMatrix
                                          , path->GetWorldMatrix()
                                          , conversionMatrix );

            for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
            {
                // compute the box
                SetSegmentBBox( segment, conversionMatrix );
            }

            for( FOdysseyVectorVertex *vertex : path->GetVertexList() )
            {
                vertex->ResetNearestSegment();
                vertex->ResetNearestVertex();
                // reset vertex ID to get ready for partnerization
                //vertex->SetID( mVertexID++ );
            }
        } );
#endif
    }
    else
    {
        for( FOdysseyVectorObject *child : mPathList )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            BLMatrix2D conversionMatrix;

            FOdysseyVector::MatrixMultiply( mInverseWorldMatrix
                                          , path->GetWorldMatrix()
                                          , conversionMatrix );

            for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
            {
                // compute the box
                SetSegmentBBox( segment, conversionMatrix );
            }

            for( FOdysseyVectorVertex *vertex : path->GetVertexList() )
            {
                vertex->ResetNearestSegment();
                vertex->ResetNearestVertex();
                // reset vertex ID to get ready for partnerization
                //vertex->SetID( mVertexID++ );
            }
        }
    }
    // </TODO:MULTITHREADABLE>

    //for( int i = 0; i < mIntersectionVertexArray.size(); i++ )
    //{
    //    delete mIntersectionVertexArray[i];
    //}

    mIntersectionVertexArray.clear();
}

void
FOdysseyVectorGroupPaint::CopyBuckets( FOdysseyVectorGroupPaint* iDestination, bool iSwitchSpace )
{
     BLMatrix2D conversionMatrix;

     if( iSwitchSpace )
     {
         conversionMatrix = iDestination->GetInverseWorldMatrix();
         conversionMatrix.transform( mWorldMatrix );
     }
     else
     {
         conversionMatrix.reset();
     }

     for( FOdysseyVectorBucket *bucket : mBucketList )
     {
        ::ULIS::FVec2D bucketCoords = bucket->GetCoords();
        BLPoint destinationBucketPosition = conversionMatrix.mapPoint( bucketCoords.x, bucketCoords.y );
        FOdysseyVectorBucket *bucketCopy = new FOdysseyVectorBucket( iDestination, 0.0f, 0.0f, bucket->IsPropagated() );

        bucket->Copy( bucketCopy );

        bucketCopy->Set( destinationBucketPosition.x, destinationBucketPosition.y );

        iDestination->AddBucket( bucketCopy );
     }   
}

FOdysseyVectorObject*
FOdysseyVectorGroupPaint::CopyShape( uint64 iCopyFlags )
{
    FOdysseyVectorGroupPaint* groupPaintCopy = new FOdysseyVectorGroupPaint( "Paint Group Copy" );

    groupPaintCopy->SetPainted( bPainted );
    groupPaintCopy->SetMonochrome( bMonochrome );
    groupPaintCopy->SetMonochromeColor( mMonochromeColor );
    groupPaintCopy->SetRealtime( bRealtime );
    groupPaintCopy->SetGapTolerance( mGapTolerance );
    groupPaintCopy->SetWireframe( bWireframe );
    groupPaintCopy->SetWireframeColor( mWireframeColor );
    groupPaintCopy->SetIntersectsCanvas( bIntersectsCanvas );

    CopyBuckets( groupPaintCopy, false );

    return groupPaintCopy;
}

void
FOdysseyVectorGroupPaint::AlterContourWidth( double iValue, bool iAbsolute )
{
    std::vector<FOdysseyVectorVertex*> vertexArray;
    uint32_t vertexCount = 0;
    bool intersectsCanvas = IntersectsCanvas();

    // rebuild without intersecting the canvas if necessary
    if( intersectsCanvas == true )
    {
        SetIntersectsCanvas( false );

        Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    // set indexes for later fill the array
    for( FOdysseyVectorPath* path : mPathList )
    {
        for( FOdysseyVectorVertex* vertex : path->GetVertexList() )
        {
            vertex->SetID( vertexCount++ );
        }
    }

    vertexArray.resize( vertexCount );

    for( FOdysseyVectorSection& section : mSectionBuffer )
    {
        if( section.GetCycleCount() == 1 )
        {
            FOdysseyVectorVertex* sectionVertex0 = section.GetVertex(0);
            FOdysseyVectorVertex* sectionVertex1 = section.GetVertex(1);

            if( sectionVertex0->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                vertexArray[sectionVertex0->GetID()] = sectionVertex0;
            }

            if( sectionVertex1->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                vertexArray[sectionVertex1->GetID()] = sectionVertex1;
            }
        }
    }

    for( FOdysseyVectorVertex* vertex : vertexArray )
    {
        if( vertex )
        {
            vertex->SetRadius( iAbsolute ? iValue : vertex->GetRadius() * iValue );
        }
    }

    if( intersectsCanvas == true )
    {
        SetIntersectsCanvas( true );

        Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }
}
/*
bool
FOdysseyVectorGroupPaint::GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox, bool iWorld )
{
    bool inited = false;

    for( FOdysseyVectorObject* child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            ::ULIS::FRectD childBBox;

            if( path->GetBBoxFromSelectedVertices( childBBox, true ) )
            {
                oBBox = inited ? oBBox | childBBox
                               : childBBox;

                inited = true;
            }
        }
    }

    if( inited )
    {
        if( iWorld == false )
        {
            double xmin = oBBox.x;
            double xmax = oBBox.x + oBBox.w;
            double ymin = oBBox.y;
            double ymax = oBBox.y + oBBox.h;
            BLPoint p[4] = { mInverseWorldMatrix.mapPoint( xmin, ymin )
                           , mInverseWorldMatrix.mapPoint( xmax, ymin )
                           , mInverseWorldMatrix.mapPoint( xmax, ymax )
                           , mInverseWorldMatrix.mapPoint( xmin, ymax ) };

            oBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p[0].x, p[1].x, p[2].x, p[3].x )
                                              , ::ULIS::FMath::Min4( p[0].y, p[1].y, p[2].y, p[3].y )
                                              , ::ULIS::FMath::Max4( p[0].x, p[1].x, p[2].x, p[3].x )
                                              , ::ULIS::FMath::Max4( p[0].y, p[1].y, p[2].y, p[3].y ) );
        }
    }

    return inited;
}
*/

// Pick from mask image
void
FOdysseyVectorGroupPaint::PickBucket( std::vector<FOdysseyVectorBucket*>& oPickedBucketArray )
{
    BLImage* maskImage = GetEngine()->GetBLMask();
    BLImageData imageData;

    maskImage->getData( &imageData );

    for( FOdysseyVectorBucket* bucket : mBucketList )
    {
        ::ULIS::FVec2D& localCoords = bucket->GetCoords();
        // convert bucket coordinates to world coordinates. Easier to detect collision inside the picking circle.
        BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
        int32 x = (int32) worldCoords.x;
        int32 y = (int32) worldCoords.y;

        if( ( x >= 0 ) && ( x < imageData.size.w )
         && ( y >= 0 ) && ( y < imageData.size.h ) )
        {
            uint8 *pixel = static_cast<uint8*>( imageData.pixelData );
            uint32 offset = ( y * imageData.size.w ) + x;
            uint8 pixelValue = pixel[offset];

            if( pixelValue == 255 )
            {
                oPickedBucketArray.push_back( bucket );
            }
        }
    }
}

FOdysseyVectorCycle*
FOdysseyVectorGroupPaint::PickCycle( double iWorldX, double iWorldY )
{
    BLPoint localCoord = mInverseWorldMatrix.mapPoint( iWorldX, iWorldY );

    if( mBBox.HitTest( ::ULIS::FVec2D( localCoord.x, localCoord.y ) ) )
    {
        for( FOdysseyVectorCycle *cycle : mCycleList )
        {
            // TODO: Bounding volume for cycles for faster search
            if( cycle->HitTest( localCoord.x, localCoord.y ) == true )
            {
                return cycle;
            }
        }
    }

    return nullptr;
}

FOdysseyVectorVertex*
FOdysseyVectorGroupPaint::ReachVertexFromSection( FOdysseyVectorVertex* iVertex
                                                , FOdysseyVectorSection* iFromSection
                                                , FOdysseyVectorSegment* iOwnerSegment )
{
    FOdysseyVectorSection* fromSection = iFromSection;
    FOdysseyVectorVertex* currentVertex = iVertex;

    while( fromSection )
    {
        FOdysseyVectorSection* nextSection = currentVertex->GetOtherSection( fromSection, true );

        if( nextSection && ( nextSection->IsErased() == false ) )
        {
            currentVertex = nextSection->GetOtherVertex( currentVertex );
        }
        else
        {
            break;
        }

        fromSection = nextSection;
    }

    return currentVertex;
}

// this does not need to be in the header, internal use only.
typedef struct _FVertexPair
{
    FOdysseyVectorVertex* vertex[2];

    _FVertexPair( FOdysseyVectorVertex* iVertex0, FOdysseyVectorVertex* iVertex1 )
    {
        vertex[0] = iVertex0;
        vertex[1] = iVertex1;
    }

    bool operator==(const _FVertexPair& rhs)
    {
        return ( ( vertex[0] == rhs.vertex[0] ) && ( vertex[1] == rhs.vertex[1] ) );
    }
} FVertexPair;

bool
FOdysseyVectorGroupPaint::PickSection( FOdysseyVectorSection* iSection
                                     , const ::ULIS::FRectD& iMaskRect
                                     , const uint8* iMaskPixelData )
{
    ::ULIS::FVec2D* bezier = iSection->GetBezier();
    // Note, section are in paingroup coordinates (path's parent), not in path coordinates.
    BLMatrix2D& worldMatrix = iSection->GetOwner()->GetWorldMatrix();
    BLPoint pt[4] = { worldMatrix.mapPoint( bezier[0].x, bezier[0].y )
                    , worldMatrix.mapPoint( bezier[1].x, bezier[1].y )
                    , worldMatrix.mapPoint( bezier[2].x, bezier[2].y )
                    , worldMatrix.mapPoint( bezier[3].x, bezier[3].y ) };
    ::ULIS::FVec2D worldBezier[4] = { ::ULIS::FVec2D( pt[0].x, pt[0].y )
                                    , ::ULIS::FVec2D( pt[1].x, pt[1].y )
                                    , ::ULIS::FVec2D( pt[2].x, pt[2].y )
                                    , ::ULIS::FVec2D( pt[3].x, pt[3].y ) };

    return FOdysseyVector::PickBezier( worldBezier, iMaskRect, iMaskPixelData );
}

void
FOdysseyVectorGroupPaint::PickErasedSections( std::vector<FOdysseyVectorSection*>& oErasedSectionArray )
{
    BLImage* maskImage = GetEngine()->GetBLMask();
    BLImageData maskData;
    ::ULIS::FRectD maskRect;

    maskImage->getData( &maskData );

    maskRect = ::ULIS::FRectD( 0, 0, maskData.size.w, maskData.size.h );

    for( FOdysseyVectorSection& section : mSectionBuffer )
    {
        if( PickSection( &section, maskRect, (uint8*) maskData.pixelData ) )
        {
            oErasedSectionArray.push_back( &section );

            section.SetErased( true );
        }
    }
}

void
FOdysseyVectorGroupPaint::EraseSections( std::vector<FOdysseyVectorObject*>& oAddedPathArray
                                       , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                       , std::vector<FOdysseyVectorObject*>& oRemovedPathArray
                                       , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                       , bool iSplit )
{
    std::vector<FOdysseyVectorSection*> erasedSectionArray;
    BLImageData imageData;
    BLImage* blimg = GetEngine()->GetBLMask(); // the mask image must be selected by the vector engine at this point

    blimg->getData( &imageData );

    // first step : relink sections as they were all unlinked after the cycle detection process
    // Note: we don't stitch sections of size 0 here because it disturb the erasing process.
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        mSectionBuffer[i].Link();
    }

//    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
//    {
//        mGapSectionBuffer[i].Link();
//    }

    // first step
    PickErasedSections( erasedSectionArray );

    // second step. Extend erased section array with the neighbour sections until we reach
    // the end of the chain or an intersection
    for( FOdysseyVectorSection* section : erasedSectionArray )
    {
        section->SetErased( true );

        // static call
        FOdysseyVectorChain::ExtendErasedSection( section->GetVertex(0), section );
        // static call
        FOdysseyVectorChain::ExtendErasedSection( section->GetVertex(1), section );
    }

    // do not use mPathList because it may contains the canvas path
    for( FOdysseyVectorObject* child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( child );

            if( path->Erase( oAddedPathArray
                           , oAddedVertexArray
                           , oAddedSegmentArray
                           , oRemovedVertexArray
                           , oRemovedSegmentArray
                           , true
                           , iSplit ) )
            {
                oRemovedPathArray.push_back( path );
            }
        }
    }

    // unlink sections again
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        mSectionBuffer[i].Unlink( false );
        mSectionBuffer[i].SetErased( false ); // unmark
    }

//    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
//    {
//        mGapSectionBuffer[i].Unlink();
//    }

    // also check paths that were not intersected. If they hit, delete the whole thing.
    // do not use mPathList because it may contains the canvas path
    for( FOdysseyVectorObject* child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( child );

            if( path->GetPaintingCode() != mPaintingCode )
            {
                for( FOdysseyVectorChain& chain : path->GetChainArray() )
                {
                     // used just for being able to call the EraseSegments()
                    std::vector<FWayFragment> wayFragmentArray;
                     // used just for being able to call the EraseSegments()
                    std::vector<FWayPoint> wayPointArray;

                    wayPointArray.reserve( 10 );
                    wayFragmentArray.reserve( 10 );

                    if( chain.EraseSegments( &imageData
                                           , wayPointArray
                                           , wayFragmentArray ) )
                    {
                        for( FOdysseyVectorVertex* vertex : chain.GetVertexArray() )
                        {
                            path->RemoveVertex( vertex );

                           oRemovedVertexArray.push_back( vertex );
                        }

                        for( FOdysseyVectorSegment* segment : chain.GetSegmentArray() )
                        {
                            path->RemoveSegment( segment );

                            oRemovedSegmentArray.push_back( segment );
                        }
                    }
                }

                if( path->GetSegmentList().size() == 0 )
                {
                    oRemovedPathArray.push_back( path );
                }
            }
        }
    }
}

void
FOdysseyVectorGroupPaint::GetChildrenPaths( std::vector<FOdysseyVectorPath*>& oPathArray )
{
    // do not use mPathList because it may contains the canvas path
    for( FOdysseyVectorObject* child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( child );

            oPathArray.push_back( path );
        }
    }
}

void
FOdysseyVectorGroupPaint::PickSectionLessPaths( std::vector<FOdysseyVectorObject*>& oObjectArray )
{
    // do not use mPathList because it may contains the canvas path
    for( FOdysseyVectorObject* child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( child );

            if( path->GetPaintingCode() != mPaintingCode )
            {
                if( path->Pick( this, ::ULIS::FRectD( 0, 0, 0, 0 ), FOdysseyVectorObject::PICK_MASK_BASED ) )
                {
                    oObjectArray.push_back( path );
                }
            }
        }
    }
}
