#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVector.h"
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
GetCycleNormalVector( std::vector<FOdysseyVectorVertex*>& iVertexArray
                    , std::vector<FOdysseyVectorSection*>& iSectionArray );
static void
BlockPath( std::vector<FOdysseyVectorVertex*>& iVertexArray
         , std::vector<FOdysseyVectorSection*>& iSectionArray );

FOdysseyVectorGroupPaint::~FOdysseyVectorGroupPaint()
{
    Clear();

    for( FOdysseyVectorBucket* bucket : mBucketList )
    {
        delete bucket;
    }

    mBucketList.clear();
}

FOdysseyVectorGroupPaint::FOdysseyVectorGroupPaint( const FString& iName )
    : FOdysseyVectorGroup( iName )
{
    SetName( iName );

    mIntersectionArray.reserve( 60 );

    bPainted = true;
    bMonochrome = false;
    mMonochromeColor = FColor( 160, 160, 160, 255 );
    mGapTolerance = 12.0f;
    bRealtime = false;
    bWireframe = false;
    mWireframeColor = FColor( 255, 255, 255, 255 );
    bMultithreaded = true; 

    mBackgroundBucket.SetSolidColor( 160, 160, 160, 0 );

    //mGapSegmentBuffer.reserve( 200 );
    //mSectionBuffer.reserve( 200 );
}

// This is a constrained version of a segment-to-point proximity test.
// It means that if the projection of the point on the segment is beyond limits,
// it will remain within limits (0.0f) or (1.0f). On the figure below, x would be at t=0.0
//
//                    ° (point)
//     segment        |
// 1______________0   x (proj. point will also be at t = 0.0,
//                       even though it is outside the segment)
//
static double
DistanceToSegmentConstrained( const ::ULIS::FVec2D& iPt
                            , const ::ULIS::FVec2D& iSegmentP0
                            , const ::ULIS::FVec2D& iSegmentP1
                            , double&         oDistance)
{
    double t = FOdysseyVector::DistanceToSegment( iPt, iSegmentP0, iSegmentP1, oDistance );

    if( t < 0.0f )
    {
        t = 0.0f;

        oDistance = ( iSegmentP0 - iPt ).Distance();
    }


    if( t > 1.0f )
    {
        t = 1.0f;

        oDistance = ( iSegmentP1 - iPt ).Distance();
    }

    return t;
}

void
FOdysseyVectorGroupPaint::SetRealtime( bool iRealtime )
{
    bRealtime = iRealtime;
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

// CubicSegment-CubicSegment intersection test. The test is performed using straight sub-segments
// that are precomputed by the PaintGroup object when updated and stored in the path's FractionCache,
// as it would be too complicated to do maths using the parametric bezier and I'm not that smart.
// Actual intersections vertices are created in this method. We create 2 vertices per intersection.
// This is required because a segment can intersect itself, in that case we need to be able to create
// a section that has 2 different vertices as endpoints and not the same one. 
// A Tolerance value is accepted to test for near-intersections, that will be created later in the
// process.
void
FOdysseyVectorGroupPaint::IntersectSegment( FOdysseyVectorSegmentCubic* iSegment0
                                          , FOdysseyVectorSegmentCubic* iSegment1
                                          , const ::ULIS::FVec2D& iSegment1MinInParentWithTolerance
                                          , const ::ULIS::FVec2D& iSegment1MaxInParentWithTolerance
                                          , double iTolerance
                                          , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    FOdysseyVectorVertex* segment0Vertex0 = iSegment0->GetVertex(0);
    FOdysseyVectorVertex* segment0Vertex1 = iSegment0->GetVertex(1);
    std::vector<FOdysseyVectorFraction>& segment0FractionCache = iSegment0->GetFractionCache();
    ::ULIS::FVec2D segment0Point0InParent = iSegment0->GetFractionCacheStartPointInParent();
    ::ULIS::FVec2D segment0Point1InParent = iSegment0->GetFractionCacheEndPointInParent();

    FOdysseyVectorVertex* segment1Vertex0 = iSegment1->GetVertex(0);
    FOdysseyVectorVertex* segment1Vertex1 = iSegment1->GetVertex(1);
    std::vector<FOdysseyVectorFraction>& segment1FractionCache = iSegment1->GetFractionCache();
    ::ULIS::FVec2D segment1Point0InParent = iSegment1->GetFractionCacheStartPointInParent();
    ::ULIS::FVec2D segment1Point1InParent = iSegment1->GetFractionCacheEndPointInParent();
    ::ULIS::FVec2D* segment1Bezier = iSegment1->GetBezier();

    //uint32 intersectionCount = 0;

    for ( int i = 0; i < segment0FractionCache.size(); i++ )
    {
        FOdysseyVectorFraction* segment0Poly = &segment0FractionCache[i];
        ::ULIS::FVec2D segment0PolyVector = ( segment0Poly->lineVertex[1] - segment0Poly->lineVertex[0] );
        int p = i - 1;
        int n = i + 1;

        if( iSegment0 >= iSegment1 )
        {
            if( ( segment0Poly->xMinInParent < iSegment1MaxInParentWithTolerance.x )
             && ( segment0Poly->xMaxInParent > iSegment1MinInParentWithTolerance.x )
             && ( segment0Poly->yMinInParent < iSegment1MaxInParentWithTolerance.y )
             && ( segment0Poly->yMaxInParent > iSegment1MinInParentWithTolerance.y ) )
            {
                for( int j = 0; j < segment1FractionCache.size(); j++ )
                {
                    FOdysseyVectorFraction* segment1Poly = &segment1FractionCache[j];
                    double segment0PolySubT, segment1PolySubT;

                    // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                    if( ( segment0Poly->xMaxInParent > segment1Poly->xMinInParent ) && ( segment0Poly->xMinInParent < segment1Poly->xMaxInParent )
                     && ( segment0Poly->yMaxInParent > segment1Poly->yMinInParent ) && ( segment0Poly->yMinInParent < segment1Poly->yMaxInParent ) )
                    {
                        ::ULIS::FVec2D segment1PolyVector = ( segment1Poly->lineVertex[1] - segment1Poly->lineVertex[0] );

                        if(   ( iSegment0 != iSegment1 )
                        // check this is not the same sub-segment or adjacent sub-segment, or else they would always intersect
                         || ( ( iSegment0 == iSegment1 ) && ( ( i - j ) > 1 ) ) )
                        {

                            // Test intersections in PaintGroup's coordinates system (struct member lineVertexInParent).
                            if ( FOdysseyVector::IntersectSegment ( segment0Poly->lineVertexInParent[0]
                                                                  , segment0Poly->lineVertexInParent[1]
                                                                  , segment1Poly->lineVertexInParent[0]
                                                                  , segment1Poly->lineVertexInParent[1]
                                                                  , &segment0PolySubT
                                                                  , &segment1PolySubT ) )
                            {
                                // Find intersections coordinates in respective coordinates systems (struct member lineVertex).
                                // Note: we cannot use segment->GetPointAt() to determine the position
                                // of the intersection because it will not match the intersection that
                                // we detect via linear means. Cubic segments are not linear.
                                ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->lineVertex[0].x + ( segment0PolyVector.x * segment0PolySubT )
                                                                   , segment0Poly->lineVertex[0].y + ( segment0PolyVector.y * segment0PolySubT ) };
                                ::ULIS::FVec2D segment1ISXCoords = { segment1Poly->lineVertex[0].x + ( segment1PolyVector.x * segment1PolySubT )
                                                                   , segment1Poly->lineVertex[0].y + ( segment1PolyVector.y * segment1PolySubT ) };
                                // find value T at intersection. This is coordinates system-independent.
                                double segment0T = segment0Poly->fromT + ( segment0PolySubT * ( segment0Poly->toT - segment0Poly->fromT ) );
                                double segment1T = segment1Poly->fromT + ( segment1PolySubT * ( segment1Poly->toT - segment1Poly->fromT ) );

                                if( ( segment0T > 0.0f && segment0T < 1.0f )
                                 && ( segment1T > 0.0f && segment1T < 1.0f ) )
                                {
                                    bool selfIntersects = ( iSegment0 == iSegment1 );
                                    FOdysseyVectorIntersection* intersection = new FOdysseyVectorIntersection( selfIntersects
                                                                                                           ,   iSegment0->GetPath()
                                                                                                           ,   segment0ISXCoords.x
                                                                                                           ,   segment0ISXCoords.y
                                                                                                           ,   segment0T
                                                                                                           ,   iSegment1->GetPath()
                                                                                                           ,   segment1ISXCoords.x
                                                                                                           ,   segment1ISXCoords.y
                                                                                                           ,   segment1T );

                                    mMutex.lock();
                                    iIntersectionArray.emplace_back( intersection );

                                    iSegment0->AddIntersection( intersection->GetVertex(0) );
                                    iSegment1->AddIntersection( intersection->GetVertex(1) );
                                    mMutex.unlock();

                                    //intersectionCount++;
                                }
                            }
                        }
                    }
                }
            }
        }

// this part is for detecting near-intersections. We only consider path tips (segmentCount = 1)
////////////////////////////// Gap detection /////////////////////////////////
        if( ( iTolerance  ) && ( iSegment0 != iSegment1 ) )
        {
            if( segment1Vertex0->GetSegmentCount() == 1 )
            {
                // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                if ( ( segment1Point0InParent.x > ( segment0Poly->xMinInParent - iTolerance ) ) && ( segment1Point0InParent.x < ( segment0Poly->xMaxInParent + iTolerance ) )
                  && ( segment1Point0InParent.y > ( segment0Poly->yMinInParent - iTolerance ) ) && ( segment1Point0InParent.y < ( segment0Poly->yMaxInParent + iTolerance ) ) )
                {
                    // check distance at endpoints
                    double distance;
                    double t = DistanceToSegmentConstrained( segment1Point0InParent
                                                           , segment0Poly->lineVertexInParent[0]
                                                           , segment0Poly->lineVertexInParent[1]
                                                           , distance );

                    //if ( ( t > 0.0f ) && ( t < 1.0f ) )
                    {
                        mMutex.lock();
                        if( ( distance < iTolerance )
                         && ( distance < segment1Vertex0->GetDistanceToNearestSegment() ) )
                        {
                            double segmentT = segment0Poly->fromT + ( ( segment0Poly->toT - segment0Poly->fromT ) * t );
                            // Note: we cannot use segment->GetPointAt() to determine the position
                            // of the intersection because it will not match the intersection that
                            // we detect via linear means. Cubic segments are not linear.
                            ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->lineVertex[0].x + ( segment0PolyVector.x * t )
                                                               , segment0Poly->lineVertex[0].y + ( segment0PolyVector.y * t ) };

                            segment1Vertex0->SetNearestSegment( iSegment0, distance, segmentT, segment0ISXCoords );

                        }
                        mMutex.unlock();
                    }
                }
            }

            if( segment1Vertex1->GetSegmentCount() == 1 )
            {
                // to speed things up a bit (actually I've found out that it speeds things up by 2 or by 3)
                if ( ( segment1Point1InParent.x > ( segment0Poly->xMinInParent - iTolerance ) ) && ( segment1Point1InParent.x < ( segment0Poly->xMaxInParent + iTolerance ) )
                  && ( segment1Point1InParent.y > ( segment0Poly->yMinInParent - iTolerance ) ) && ( segment1Point1InParent.y < ( segment0Poly->yMaxInParent + iTolerance ) ) )
                {
                    double distance;
                    double t = DistanceToSegmentConstrained( segment1Point1InParent
                                                           , segment0Poly->lineVertexInParent[0]
                                                           , segment0Poly->lineVertexInParent[1]
                                                           , distance );

                    //if ( ( t > 0.0f ) && ( t < 1.0f ) )
                    {
                        mMutex.lock();
                        if( ( distance < iTolerance )
                         && ( distance < segment1Vertex1->GetDistanceToNearestSegment() ) )
                        {
                            double segmentT = segment0Poly->fromT + ( ( segment0Poly->toT - segment0Poly->fromT ) * t );
                            // Note: we cannot use segment->GetPointAt() to determine the position
                            // of the intersection because it will not match the intersection that
                            // we detect via linear means. Cubic segments are not linear.
                            ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->lineVertex[0].x + ( segment0PolyVector.x * t )
                                                               , segment0Poly->lineVertex[0].y + ( segment0PolyVector.y * t ) };

                            segment1Vertex1->SetNearestSegment( iSegment0, distance, segmentT, segment0ISXCoords );
                        }
                        mMutex.unlock();
                    }
                }
            }
        }
///////////////////////////////////
    }
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
    bool doPropagate = true;

    while( doPropagate )
    {
        doPropagate = false;

        for( FOdysseyVectorCycle *cycle : mCycleList )
        {
            if( ( cycle->GetBucket() == nullptr ) && ( cycle->GetPropagatedBucket() == nullptr ) )
            {
                if( cycle->PropagateBucket() == true )
                {
                    doPropagate = true;
                }
            }
        }
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

    if( bPainted )
    {
        if( ( bRealtime == true  )
       || ( ( bRealtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATEPAINTGROUPS ) ) )
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
                                std::vector<FOdysseyVectorFraction>& polygonCache = cubicSegment->GetFractionCache();

                                // convert polygon cache coordinates to paintgroup's coordinates
                                // for faster intersection test
                                for( int i = 0; i < polygonCache.size(); i++ )
                                {
                                    FOdysseyVectorFraction* polygon = &polygonCache[i];

                                    BLPoint lineVertex0 = conversionMatrix.mapPoint( polygon->lineVertex[0].x
                                                                                   , polygon->lineVertex[0].y );
                                    polygon->lineVertexInParent[0].x = lineVertex0.x;
                                    polygon->lineVertexInParent[0].y = lineVertex0.y;

                                    BLPoint lineVertex1 = conversionMatrix.mapPoint( polygon->lineVertex[1].x
                                                                                   , polygon->lineVertex[1].y );
                                    polygon->lineVertexInParent[1].x = lineVertex1.x;
                                    polygon->lineVertexInParent[1].y = lineVertex1.y;

                                    // this may be a bit too memory-consuming. Don't know. Keep it for now.
                                    polygon->xMaxInParent = ::ULIS::FMath::Max( polygon->lineVertexInParent[0].x, polygon->lineVertexInParent[1].x );
                                    polygon->yMaxInParent = ::ULIS::FMath::Max( polygon->lineVertexInParent[0].y, polygon->lineVertexInParent[1].y );
                                    polygon->xMinInParent = ::ULIS::FMath::Min( polygon->lineVertexInParent[0].x, polygon->lineVertexInParent[1].x );
                                    polygon->yMinInParent = ::ULIS::FMath::Min( polygon->lineVertexInParent[0].y, polygon->lineVertexInParent[1].y );
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
       || ( ( bRealtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATEPAINTGROUPS ) ) )
        {
            if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE )
            {
                FindCycles(); // also calls Clear()

                if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
                {
                    mInvalidationFlags &= (~INVALIDATE_SHAPE);
                }
            }

            if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD )
            {
                FindCycles(); // also calls Clear()

                if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
                {
                    mInvalidationFlags &= (~INVALIDATE_CHILD);
                }
            }

            Colorize();

            if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
            {
                mInvalidationFlags &= (~INVALIDATE_COLOR);
            }
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
FOdysseyVectorGroupPaint::DrawShape( BLContext* iBLContext, double iCombinedOpacity, uint64 iFlags )
{
    FOdysseyVectorEngine* vectorEngine = GetEngine();
    BLImageData& imageData = vectorEngine->GetRenderData();

    ::ULIS::FRectD screen;

    screen.x = 0;
    screen.y = 0;
    screen.w = imageData.size.w;
    screen.h = imageData.size.h;

    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        ::ULIS::FRectD cycleWorldBBox = cycle->GetBBox( true );

        if( cycleWorldBBox.Area() > 1.0f )
        {
            if( ( ( cycleWorldBBox.x                    ) < screen.w )
             && ( ( cycleWorldBBox.x + cycleWorldBBox.w ) > 0        )
             && ( ( cycleWorldBBox.y                    ) < screen.h )
             && ( ( cycleWorldBBox.y + cycleWorldBBox.h ) > 0        ) )
            {
                cycle->Draw( iBLContext
                           , iCombinedOpacity
                           , iFlags
                           , bMonochrome
                           , mMonochromeColor );
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

bool
FOdysseyVectorGroupPaint::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
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
}

void
FOdysseyVectorGroupPaint::CreateVertexGapSegment( FOdysseyVectorVertex* iVertex
                                                , std::vector<FOdysseyVectorSection>& iSectionBuffer
                                                , std::vector<FOdysseyVectorSegmentCubicGap>& iGapSegmentBuffer )
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
            // Warning: setting a parent path here leads to bugs, due to path update of a segment not really belonging to it.
            FOdysseyVectorSegmentCubicGap& newGapSegment = iGapSegmentBuffer.emplace_back( this, nearestVertex, iVertex );
            //iGapSegmentBuffer[gapCount].Link(); // not necessary. saves us some cpu cycles
            newGapSegment.Update();

            FOdysseyVectorSection& newGapSection = iSectionBuffer.emplace_back( &newGapSegment, nullptr, nearestVertex, iVertex );
            newGapSection.Link();
        }
    }
}

void
FOdysseyVectorGroupPaint::CreateSegmentSections( FOdysseyVectorSegment* iSegment
                                               , BLMatrix2D* iConversionMatrix
                                               , std::vector<FOdysseyVectorSection>& iSectionBuffer )
{
    std::list<FOdysseyVectorVertexIntersection*>& intersectionVertexList = iSegment->GetIntersectionVertexList();
    std::vector<FOdysseyVectorVertex*> vertertexArray;
    FOdysseyVectorVertex* sectionVertex0;

    vertertexArray.reserve( 2 + intersectionVertexList.size() );

    // retrieve intersection vertices + segment end points from lower t value to higher t value
    iSegment->GetAllVertices( vertertexArray );

    // Create sections
    sectionVertex0 = vertertexArray[0];

    for( int i = 1; i < vertertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* sectionVertex1 = vertertexArray[i];
        FOdysseyVectorSection& newSection = iSectionBuffer.emplace_back();
        // creates topology
        newSection.Init( iSegment, iConversionMatrix, sectionVertex0, sectionVertex1 );
        newSection.Link();

        sectionVertex0 = sectionVertex1;
    }
}

void
FOdysseyVectorGroupPaint::CreatePathSections( FOdysseyVectorPath* iPath
                                            , BLMatrix2D* iConversionMatrix )
{
    for( FOdysseyVectorSegment *segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex( 0 );
        FOdysseyVectorVertex* vertex1 = segment->GetVertex( 1 );

        CreateSegmentSections( segment, iConversionMatrix, mSectionBuffer );

        //TODO::Possible optimization: call only if nearestVertex exists
        CreateVertexGapSegment( vertex0, mGapSectionBuffer, mGapSegmentBuffer );
        CreateVertexGapSegment( vertex1, mGapSectionBuffer, mGapSegmentBuffer );
    }
}

void
FOdysseyVectorGroupPaint::IntersectSegmentWithList( FOdysseyVectorSegment* iSegment
                                                  , const std::list<FOdysseyVectorSegment*>& iSegmenList
                                                  , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
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
                            , mGapTolerance
                            , iIntersectionArray );
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
    BLPoint pt = iVertex->GetPath()->GetWorldMatrix().mapPoint( iVertex->GetCoords().x, iVertex->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Vertex: [x:%f y:%f]"), pt.x, pt.y);
}

static void
PrintSection( FOdysseyVectorSection* iSection)
{
    FOdysseyVectorPath* path = iSection->GetVertex(0)->GetPath();
    FOdysseyVectorSegment* segment = iSection->GetSegment();
    BLPoint pt0 = path->GetWorldMatrix().mapPoint( iSection->GetVertex(0)->GetCoords().x, iSection->GetVertex(0)->GetCoords().y );
    BLPoint pt1 = path->GetWorldMatrix().mapPoint( iSection->GetVertex(1)->GetCoords().x, iSection->GetVertex(1)->GetCoords().y );
    BLPoint segpt0 = path->GetWorldMatrix().mapPoint( segment->GetVertex(0)->GetCoords().x, segment->GetVertex(0)->GetCoords().y );
    BLPoint segpt1 = path->GetWorldMatrix().mapPoint( segment->GetVertex(1)->GetCoords().x, segment->GetVertex(1)->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Section: [x:%f y:%f] -- [x:%f y:%f]/segment[x:%f y:%f] -- [x:%f y:%f] - flags : %d"), pt0.x, pt0.y, pt1.x, pt1.y, segpt0.x, segpt0.y, segpt1.x, segpt1.y, iSection->GetFlags() );
}

static void
PrintCycle( std::vector<FOdysseyVectorVertex*>& vertexArray
          , std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = sectionArray[i]->GetSegment();
        BLPoint pt0 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(0)->GetCoords().x, sectionArray[i]->GetVertex(0)->GetCoords().y );
        BLPoint pt1 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(1)->GetCoords().x, sectionArray[i]->GetVertex(1)->GetCoords().y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

uint32
FOdysseyVectorGroupPaint::FindPath( FOdysseyVectorSection* iReturnSection
                                  , FOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation
                                  , uint32 iDepth ) // we could also use iVertexArray.size()
{
    FOdysseyVectorVertex* nextVertex = ( iSection->GetVertex(0) == iVertex ) ? iSection->GetVertex(1)
                                                                             : iSection->GetVertex(0);
    //::ULIS::FVec2D sectionVector = -iSection->GetVectorFromVertex( nextVertex, false, false );
    bool hasPrimary = false;
    bool hasSecondary = false;
    bool hasTertiary = false;
    uint32 ret = FOdysseyVectorGroupPaint::NOCYCLE;
    static int i;
    bool isLoop = false;

    iSectionArray.push_back( iSection );
    iVertexArray.push_back( iVertex );
    iSection->Block( iVertex );

//PrintSection( iSection );

    // loop checking if initiator is of type 
    if( iVertexArray[0]->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
    {
        FOdysseyVectorVertexIntersection* initiatorVertex = static_cast<FOdysseyVectorVertexIntersection*>(iVertexArray[0]);

        isLoop = initiatorVertex->GetIntersection()->HasVertex( static_cast<FOdysseyVectorVertexIntersection*>(nextVertex) );
    }

    if( iVertexArray[0]->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        isLoop = ( iVertexArray[0] == nextVertex );
    }

    if( ( isLoop == true )// cycle detected
    && ( ( ( iReturnSection->IsLinked() == true ) && ( iReturnSection == iSection ) ) // 1 return path accepted
        || ( iReturnSection->IsLinked() == false ) ) ) // any return path accepted
    {
        double normalVector = GetCycleNormalVector( iVertexArray, iSectionArray );

//UE_LOG(LogTemp, Warning, TEXT("Cycle detected %f"), GetCycleNormalVector( iVertexArray, iSectionArray ) ); 
        if( normalVector > 0.0f )
        {
//UE_LOG(LogTemp, Warning, TEXT("Cycle committed") ); 
            mCycleList.push_back( new FOdysseyVectorCycle( this, iVertexArray, iSectionArray, normalVector ) );
        }

        ret = FOdysseyVectorGroupPaint::HASCYCLE;
    }
    else
    {
        if( nextVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorSection* primaryNextSection = /*nextVertex->GetOtherSection( iSection, false )*/nextVertex->GetCycleNextSection( iSection, 1.0f );

            if( primaryNextSection )
            {
                if( primaryNextSection->IsBlocked( nextVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }
        }

        if( nextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
        {
            FOdysseyVectorVertexIntersection* nextIntersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>( nextVertex );
            FOdysseyVectorVertexIntersection* nextPartnerVertex = nextIntersectionVertex->GetPartner();
            FOdysseyVectorSection* primaryNextSection = nextIntersectionVertex->GetCycleNextSection( iSection, 1.0f );
            FOdysseyVectorSection* secondaryNextSection = nextIntersectionVertex->GetOtherSection( iSection, true );
            FOdysseyVectorSection* tertiaryNextSection = nextIntersectionVertex->GetCycleNextSection( iSection, -1.0f );

            if( primaryNextSection )
            {
                if( primaryNextSection->IsBlocked( nextPartnerVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextPartnerVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }

            if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && secondaryNextSection )
            {
                if( secondaryNextSection->IsBlocked( nextIntersectionVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextIntersectionVertex, secondaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }

            if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && tertiaryNextSection )
            {
                if( tertiaryNextSection->IsBlocked( nextPartnerVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextPartnerVertex, tertiaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }
        }
    }

    iVertexArray.pop_back();
    iSectionArray.pop_back();

    return ret;
}

// find the overall orientation of the cycle
static double
GetCycleNormalVector( std::vector<FOdysseyVectorVertex*>& iVertexArray
                    , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    double z = 0;
    int32 arraySize = iSectionArray.size();

    for( int i = 0; i < arraySize; i++ )
    {
        int n = ( i + 1 ) % arraySize;
        FOdysseyVectorSection* sectioni = iSectionArray[i];
        FOdysseyVectorSegment* segment = iSectionArray[i]->GetSegment();
        FOdysseyVectorVertex* vertexi = iVertexArray[i];
        FOdysseyVectorVertex* vertexn = iVertexArray[n];

        if( vertexn->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
        {
            FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(vertexn);
            FOdysseyVectorSection* sectionn = iSectionArray[n];

            if( ( sectioni->GetSegment() != sectionn->GetSegment() )
             || ( intersectionVertex->GetIntersection()->SelfIntersects() == true ) )
            {
                vertexn = intersectionVertex->GetPartner();
            }
        }

        // Note: we use FOdysseyVectorSection::GetVertexCoords() because coords will be in Paintgroup's space.
        ::ULIS::FVec2D& viCoords = sectioni->GetVertexCoords( vertexi );
        ::ULIS::FVec2D& vnCoords = sectioni->GetVertexCoords( vertexn );

        if( segment->GetClass() == FOdysseyVectorSegmentCubicGap::StaticClass() )
        {
            z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
        }

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            double ti = vertexi->GetT( sectioni );
            double tn = vertexn->GetT( sectioni );
            double deltaT = tn - ti;
            int subdiv = 8;
            double stepT = deltaT / subdiv;
            double t0 = ti;

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
    if( iExplorationPair->departSection )
    {
        if( iExplorationPair->departSection->IsLinked() == true )
        {
            if( iExplorationPair->departSection->IsBlocked( iExplorationPair->departVertex ) == false )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                vertexArray.reserve( 10 );
                sectionArray.reserve( 10 );

                uint32 ret = FindPath( iExplorationPair->returnSection
                                     , iExplorationPair->departVertex // lies on departSection
                                     , iExplorationPair->departSection
                                     , vertexArray
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
    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        mIntersectionArray[i]->BuildExplorationPairs( explorationPairsBuffer );
    }

    // Build exploration pair before simplification
    for( int i = 0; i < mGapSegmentBuffer.size(); i++ )
    {
        FOdysseyVectorVertex* vertex0 = mGapSegmentBuffer[i].GetVertex(0);
        FOdysseyVectorVertex* vertex1 = mGapSegmentBuffer[i].GetVertex(1);

        if( vertex0->GetClass() == FOdysseyVectorVertex::StaticClass() )
            vertex0->BuildExplorationPairs( explorationPairsBuffer );

        if( vertex1->GetClass() == FOdysseyVectorVertex::StaticClass() )
            vertex1->BuildExplorationPairs( explorationPairsBuffer );
    }

    SimplifyGraph();

    // explore the graph from intersections
    for( int i = 0; i < explorationPairsBuffer.size(); i++ )
    {
        Explore( &explorationPairsBuffer[i] );
    }

    // sort cycles in order to always have a propagation that starts from the same cycles.
    // This is only required when using multithreading.
    mCycleList.sort( []( FOdysseyVectorCycle* iCycleA, FOdysseyVectorCycle* iCycleB )
                     {
                         return iCycleA->mNormal > iCycleB->mNormal;
                     } );

    OrderCycles();

    MergeCycles();


    // section topology must be unlinked now or else if we transfer path to another group, it may not be cleaned
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        FOdysseyVectorSection *section = &mSectionBuffer[i];

        if( section->IsLinked() == true )
        {
            section->Unlink();
        }
    }

    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
    {
        FOdysseyVectorSection *section = &mGapSectionBuffer[i];

        if( section->IsLinked() == true )
        {
            section->Unlink();
        }
    }

    //auto stopTotal = std::chrono::high_resolution_clock::now();
    //auto durationTotal = std::chrono::duration_cast<std::chrono::microseconds>(stopTotal - startTotal);
    //UE_LOG(LogTemp, Warning, TEXT("FindCycles Exec time %llu"), durationTotal.count() );
}

static FOdysseyVectorVertex*
CreateNearIntersection( FOdysseyVectorVertex *iVertex
                      , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    FOdysseyVectorSegment *nearestSegment = iVertex->GetNearestSegment();
    FOdysseyVectorVertex* nearestVertex = nullptr;

    if( nearestSegment )
    {
        double nearestSegmentT = iVertex->GetNearestSegmentT();

        if( ( nearestSegmentT == 0.0f ) || ( nearestSegmentT == 1.0f ) )
        {
            nearestVertex = iVertex->GetNearestSegment()->GetVertex( (int) nearestSegmentT );

            iVertex->SetNearestVertex( nearestVertex );
        }
        else
        {
            //::ULIS::FVec2D nearestVertexAt = nearestSegment->GetPointAt( nearestSegmentT );
            ::ULIS::FVec2D nearestVertexAt = iVertex->GetNearestSegmentIntersectionCoords();
            FOdysseyVectorIntersection* intersection = new FOdysseyVectorIntersection( false
                                                                                     , nearestSegment->GetPath()
                                                                                     , nearestVertexAt.x
                                                                                     , nearestVertexAt.y
                                                                                     , nearestSegmentT
                                                                                     , nearestSegment->GetPath()
                                                                                     , nearestVertexAt.x
                                                                                     , nearestVertexAt.y
                                                                                     , 0.0f );

            iIntersectionArray.emplace_back( intersection );

            nearestSegment->AddIntersection( intersection->GetVertex(0) );

            nearestVertex = intersection->GetVertex(1);

            iVertex->SetNearestVertex( nearestVertex );
        }
    }

    return nearestVertex;
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

    Invalidate();
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

    Invalidate();
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

uint32
FOdysseyVectorGroupPaint::AddChild( FOdysseyVectorObject* iChild, FOdysseyVectorObject* iInsertAfter )
{
    uint32 additionRetval = FOdysseyVectorObject::AddChild( iChild, iInsertAfter );

    if( additionRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_SUCCESS )
    {
        if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

            mPathList.push_back( path );
        }
    }

    return additionRetval;
}

uint32
FOdysseyVectorGroupPaint::RemoveChild( FOdysseyVectorObject* iChild )
{
    uint32 removalRetval = FOdysseyVectorObject::RemoveChild( iChild );

    if( removalRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_SUCCESS )
    {
        if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

            mPathList.remove( path );
        }
    }

    return removalRetval;
}

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    //std::list<FOdysseyVectorPath*> intersectedPathList = mPathList; // copy
    uint32 totalGapSegmentCount = 0;
    uint32 totalSectionCount = 0;
    // act as boolean without the need to reinitialize its value
    static uint32 paintingCode;

    mPaintingCode = ++paintingCode;

    if( bMultithreaded )
    {
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
                    /*intersectionCount += */IntersectSegmentWithList ( segment
                                                                      , intersectedPath->GetSegmentList()
                                                                      , mIntersectionArray );
                }
            }
        } );
#endif
#ifndef perpathpersegment
        std::for_each( std::execution::par_unseq
                      , mPathList.begin()
                      , mPathList.end()
                      , [this]( FOdysseyVectorPath *path )
        {
        //for( FOdysseyVectorPath *path : mPathList )
        //{
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            std::for_each( std::execution::par_unseq
                         , segmentList.begin()
                         , segmentList.end()
                         , [this]( FOdysseyVectorSegment *segment )
            {
                for( FOdysseyVectorPath *intersectedPath : mPathList )
                {
                    /*intersectionCount += */IntersectSegmentWithList ( segment
                                                                      , intersectedPath->GetSegmentList()
                                                                      , mIntersectionArray );
                }
            } );
        } );
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
                    /*intersectionCount += */IntersectSegmentWithList ( segment
                                                                      , intersectedPath->GetSegmentList()
                                                                      , mIntersectionArray );
                }
            }
        }
    }

    for( FOdysseyVectorPath *path : mPathList )
    {
        for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
        {
            //segment->SetPaintingCode( mPaintingCode ); // set as treated. It will be excluded from later intersection tests.

            if( segment->GetIntersectionVertexCount() )
            {
                segment->GetPath()->SetPaintingCode( mPaintingCode );
            }
        }

        // create near-intersections
        for( FOdysseyVectorVertex *vertex : path->GetVertexList() )
        {
            if( vertex->GetNearestSegment() )
            {
                FOdysseyVectorVertex* nearestVertex = CreateNearIntersection( vertex, mIntersectionArray );

                vertex->GetNearestSegment()->GetPath()->SetPaintingCode( mPaintingCode );
                vertex->GetPath()->SetPaintingCode( mPaintingCode );
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
                FOdysseyVectorVertex* nearestVertex0 = vertex0->GetNearestVertex();
                FOdysseyVectorVertex* nearestVertex1 = vertex1->GetNearestVertex();

                totalSectionCount += ( 1 + segment->GetIntersectionVertexCount() );

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
    mGapSegmentBuffer.reserve( totalGapSegmentCount );
    // we most store gap sections separately to be able to check afterwards if they don't intersect
    // without going through the whole section buffer.
    mGapSectionBuffer.reserve( totalGapSegmentCount );
    // reserving whole block is required to avoid memory shifting.
    mSectionBuffer.reserve( totalSectionCount );

    // create sections for exact intersections on each segment

    for( FOdysseyVectorPath *path : mPathList )
    {
        // only for path that have intersected segments.
        if( path->GetPaintingCode() == mPaintingCode )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
            BLMatrix2D& pathWorldMatrix = path->GetWorldMatrix();
            BLMatrix2D conversionMatrix;

            // TODO: possible optimization: the conversion matrix is also computed in Clear(). It could be stored
            FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, pathWorldMatrix, conversionMatrix );

            CreatePathSections( path, &conversionMatrix );

            // create a cycle right now for untouched looped-paths
            if( ( path->IsLoop() == true ) && ( path->HasIntersections() == false ) )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                path->ToVertexAndSectionArray( vertexArray, sectionArray );

                if( vertexArray.size() )
                {
                    mCycleList.push_back( new FOdysseyVectorCycle( this, vertexArray, sectionArray, 0.0f ) );
                }
            }

            if( bMultithreaded )
            {
                // Do some clearing here, taking advantage of that loop to save some CPU cycles.
                std::for_each( std::execution::par_unseq
                             , segmentList.begin()
                             , segmentList.end()
                             , [this]( FOdysseyVectorSegment *segment )
                {
                    segment->ClearIntersections();
                } );
            }
            else
            {
                for( FOdysseyVectorSegment *segment : path->GetSegmentList() )
                {
                    segment->ClearIntersections();
                }
            }
        }
        else
        {
            mSectionLessPathList.push_back( path );
        }
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

    for( FOdysseyVectorSection& otherSection : iGapSectionBuffer )
    {
        if( ( &otherSection != &iSection ) && otherSection.IsLinked() )
        {
            ::ULIS::FVec2D* otherSectionBezier = otherSection.GetBezier();
            double otherSectionT;
            double sectionT;

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
            section.Unlink();
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
                //if( section->GetSegment() )
                {
                    if( ( section->GetVertex(0)->GetSectionCount() == 1 )
                    ||  ( section->GetVertex(1)->GetSectionCount() == 1 ) )
                    {
                        keepSimplifying = true;

                        section->Unlink();
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

                        section->Unlink();
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

    // Clear gap segments. Not necessary to unlink them, as these are not linked. Only section are linked.
    mGapSegmentBuffer.clear();
    mGapSectionBuffer.clear();

    // clean section topology (unlinking has been moved after the cycle detection).

    mSectionBuffer.clear();

    // <TODO:MULTITHREADABLE>
    if( bMultithreaded )
    {
        std::for_each( std::execution::par_unseq
                     , mPathList.begin()
                     , mPathList.end()
                     , [ this ]( FOdysseyVectorPath *path )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            std::for_each( std::execution::par_unseq
                         , segmentList.begin()
                         , segmentList.end()
                         , []( FOdysseyVectorSegment *segment )
            {
                // reset nearest segment
                segment->GetVertex(0)->ResetNearestSegment();
                segment->GetVertex(1)->ResetNearestSegment();
            } );
        } );
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
                // reset nearest segment
                segment->GetVertex(0)->ResetNearestSegment();
                segment->GetVertex(1)->ResetNearestSegment();

                // take advantage of this loop to compute the box
                SetSegmentBBox( segment, conversionMatrix );
            }
        }
    }
    // </TODO:MULTITHREADABLE>

    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        delete mIntersectionArray[i];
    }

    mIntersectionArray.clear();

    mSectionLessPathList.clear();
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
FOdysseyVectorGroupPaint::CopyShape()
{
    FOdysseyVectorGroupPaint* groupPaintCopy = new FOdysseyVectorGroupPaint( "Paint Group Copy" );

    groupPaintCopy->SetPainted( bPainted );
    groupPaintCopy->SetMonochrome( bMonochrome );
    groupPaintCopy->SetMonochromeColor( mMonochromeColor );
    groupPaintCopy->SetRealtime( bRealtime );
    groupPaintCopy->SetGapTolerance( mGapTolerance );
    groupPaintCopy->SetWireframe( bWireframe );
    groupPaintCopy->SetWireframeColor( mWireframeColor );

    CopyBuckets( groupPaintCopy, false );

    return groupPaintCopy;
}

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

void
FOdysseyVectorGroupPaint::EraseSections( const ::ULIS::FRectD &iRoi
                                       , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                                       , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                       , std::vector<FOdysseyVectorObject*>& oRemovedPathArray
                                       , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                       , bool iSplit )
{
    BLImageData imageData;
    BLImage* blimg = GetEngine()->GetBLMask(); // the mask image must be selected by the vector engine at this point

    blimg->getData( &imageData );

    // first step : relink sections as they were all unlinked after the cycle detection process
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        mSectionBuffer[i].Link();
    }

//    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
//    {
//        mGapSectionBuffer[i].Link();
//    }

    for( FOdysseyVectorPath* path : mPathList )
    {
        if( path->Erase( iRoi
                       , oAddedPathArray
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

    // unlink sections again
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        mSectionBuffer[i].Unlink();
    }

//    for( int i = 0; i < mGapSectionBuffer.size(); i++ )
//    {
//        mGapSectionBuffer[i].Unlink();
//    }

    // also check paths that were not intersected. If they hit, delete the whole thing.
    for( FOdysseyVectorPath* path : mSectionLessPathList )
    {
        for( FOdysseyVectorChain& chain : path->GetChainArray() )
        {
            std::vector<FWayFragment> wayFragmentArray;
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

void
FOdysseyVectorGroupPaint::GetChildrenPaths( std::vector<FOdysseyVectorPath*>& oPathArray )
{
    for( FOdysseyVectorPath* path : mPathList )
    {
        oPathArray.push_back( path );
    }
}

// Section-less paths are listed at PaintGroup updates. They allow us to quickly determine
// whther or no a path has section and thus we can delete it completely when using the eraser
// on it for example, instead of trying to find which sections were erased or so. 
void
FOdysseyVectorGroupPaint::PickSectionLessPaths( std::vector<FOdysseyVectorObject*>& oObjectArray )
{
    for( FOdysseyVectorPath* path : mSectionLessPathList )
    {
        if( path->Pick( this, ::ULIS::FRectD( 0, 0, 0, 0 ), FOdysseyVectorObject::PICK_MASK_BASED ) )
        {
            oObjectArray.push_back( path );
        }
    }
}
