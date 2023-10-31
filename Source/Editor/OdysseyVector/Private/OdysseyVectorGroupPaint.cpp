#include "OdysseyVectorGroupPaint.h"

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
//               \            
//                \           
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

// MUST be even number
#define EDGESUBSAMPLES 8


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

    mGroupPaintParam.Painted = true;
    mGroupPaintParam.Monochrome = false;
    mGroupPaintParam.MonochromeColor = FColor( 160, 160, 160, 255 );
    mGroupPaintParam.GapTolerance = 12.0f;
    mGroupPaintParam.Realtime = false;
    mGroupPaintParam.Wireframe = false;
    mGroupPaintParam.WireframeColor = FColor( 255, 255, 255, 255 );
 
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
    mGroupPaintParam.Realtime = iRealtime;
}

bool
FOdysseyVectorGroupPaint::IsRealtime()
{
    return mGroupPaintParam.Realtime;
}

// CubicSegment-CubicSegment intersection test. The test is performed using straight sub-segments
// that are precomputed by the PaintGroup object when updated and stored in the path's FractionCache,
// as it would be too complicated to do maths using the parametric bezier and I'm not that smart.
// Actual intersections vertices are created in this method. We create 2 vertices per intersection.
// This is required because a segment can intersect itself, in that case we need to be able to create
// a section that has 2 different vertices as endpoints and not the same one. 
// A Tolerance value is accepted to test for near-intersections, that will be created later in the
// process.
uint32
FOdysseyVectorGroupPaint::IntersectSegment( FOdysseyVectorSegmentCubic* iSegment0
                                          , FOdysseyVectorSegmentCubic* iSegment1
                                          , double iTolerance
                                          , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    FOdysseyVectorVertex* segment0Vertex0 = iSegment0->GetVertex(0);
    FOdysseyVectorVertex* segment0Vertex1 = iSegment0->GetVertex(1);
    std::vector<FOdysseyVectorFraction>& segment0FractionCache = iSegment0->GetFractionCache();
    ::ULIS::FVec2D segment0Point0 = iSegment0->GetFractionCacheStartPointInParent();
    ::ULIS::FVec2D segment0Point1 = iSegment0->GetFractionCacheEndPointInParent();

    FOdysseyVectorVertex* segment1Vertex0 = iSegment1->GetVertex(0);
    FOdysseyVectorVertex* segment1Vertex1 = iSegment1->GetVertex(1);
    std::vector<FOdysseyVectorFraction>& segment1FractionCache = iSegment1->GetFractionCache();
    ::ULIS::FVec2D segment1Point0 = iSegment1->GetFractionCacheStartPointInParent();
    ::ULIS::FVec2D segment1Point1 = iSegment1->GetFractionCacheEndPointInParent();

    uint32 intersectionCount = 0;

    for ( int i = 0; i < segment0FractionCache.size(); i++ )
    {
        FOdysseyVectorFraction* segment0Poly = &segment0FractionCache[i];
        int p = i - 1;
        int n = i + 1;

        for( int j = 0; j < segment1FractionCache.size(); j++ )
        {
            FOdysseyVectorFraction* segment1Poly = &segment1FractionCache[j];
            double segment0PolySubT, segment1PolySubT;

            // to speed things up a bit (actually I've found out that it speeds things up x2 or x3)
            if( ( ( segment0Poly->xMaxInParent + iTolerance ) > ( segment1Poly->xMinInParent - iTolerance ) ) && ( ( segment0Poly->xMinInParent - iTolerance ) < ( segment1Poly->xMaxInParent + iTolerance ) )
             && ( ( segment0Poly->yMaxInParent + iTolerance ) > ( segment1Poly->yMinInParent - iTolerance ) ) && ( ( segment0Poly->yMinInParent - iTolerance ) < ( segment1Poly->yMaxInParent + iTolerance ) ) )
            {
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
                        ::ULIS::FVec2D segment0PolyVector = ( segment0Poly->lineVertex[1] - segment0Poly->lineVertex[0] );
                        ::ULIS::FVec2D segment1PolyVector = ( segment1Poly->lineVertex[1] - segment1Poly->lineVertex[0] );
                        // Find intersections coordinates in respective coordinates systems (struct member lineVertex).
                        ::ULIS::FVec2D segment0ISXCoords = { segment0Poly->lineVertex[0].x + ( segment0PolyVector.x * segment0PolySubT )
                                                           , segment0Poly->lineVertex[0].y + ( segment0PolyVector.y * segment0PolySubT ) };
                        ::ULIS::FVec2D segment1ISXCoords = { segment1Poly->lineVertex[0].x + ( segment1PolyVector.x * segment1PolySubT )
                                                           , segment1Poly->lineVertex[0].y + ( segment1PolyVector.y * segment1PolySubT ) };
                        // find value T at intersection. This is coordinates system-independent.
                        double segment0T = segment0Poly->fromT + ( segment0PolySubT * ( segment0Poly->toT - segment0Poly->fromT ) );
                        double segment1T = segment1Poly->fromT + ( segment1PolySubT * ( segment1Poly->toT - segment1Poly->fromT ) );

                        if( ( segment0T != 0.0f && segment1T != 1.0f )
                         && ( segment0T != 1.0f && segment1T != 0.0f ) )
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
                            iIntersectionArray.emplace_back( intersection );

                            iSegment0->AddIntersection( intersection->GetVertex(0) );
                            iSegment1->AddIntersection( intersection->GetVertex(1) );

                            intersectionCount++;
                        }
                    }
// this part is for detecting near-intersections. We only consider path tips (segmentCount = 1)
/////////////////////////////// UGLY. NEEDS REFACTORING !!!! //////////////////
                    /*else
                    {*/
                      if( iTolerance && ( iSegment0 != iSegment1 ) ) // limitation: tolerance can only work with different segments, otherwise it's too complicated to have something coherent
                      {
                            if( ( j == 0 ) && ( segment1Vertex0->GetSegmentCount() == 1 ) )
                            {
                                double distance;
                                double t = DistanceToSegmentConstrained( segment1Point0
                                                                       , segment0Poly->lineVertexInParent[0]
                                                                       , segment0Poly->lineVertexInParent[1]
                                                                       , distance );

                                if( distance < segment1Vertex0->GetDistanceToNearestSegment() )
                                {
                                    double segment0T = segment0Poly->fromT + ( ( segment0Poly->toT - segment0Poly->fromT ) * t );

                                    segment1Vertex0->SetNearestSegment( iSegment0, distance, segment0T );
                                }
                            }

                            if( ( j == ( segment1FractionCache.size() - 1 ) ) && ( segment1Vertex1->GetSegmentCount() == 1 ) )
                            {
                                double distance;
                                double t = DistanceToSegmentConstrained( segment1Point1
                                                                       , segment0Poly->lineVertexInParent[0]
                                                                       , segment0Poly->lineVertexInParent[1]
                                                                       , distance );

                                if( distance < segment1Vertex1->GetDistanceToNearestSegment() )
                                {
                                    double segment0T = segment0Poly->fromT + ( ( segment0Poly->toT - segment0Poly->fromT ) * t );

                                    segment1Vertex1->SetNearestSegment( iSegment0, distance, segment0T );
                                }
                            }

                            if( ( i == 0 ) && ( segment0Vertex0->GetSegmentCount() == 1 ) )
                            {
                                double distance;
                                double t = DistanceToSegmentConstrained( segment0Point0
                                                                       , segment1Poly->lineVertexInParent[0]
                                                                       , segment1Poly->lineVertexInParent[1]
                                                                       , distance );

                                if( distance < segment0Vertex0->GetDistanceToNearestSegment() )
                                {
                                    double otherSegmentT = segment1Poly->fromT + ( ( segment1Poly->toT - segment1Poly->fromT ) * t );

                                    segment0Vertex0->SetNearestSegment( iSegment1, distance, otherSegmentT );
                                }
                            }

                            if( ( i == ( segment0FractionCache.size() - 1 ) ) && ( segment0Vertex1->GetSegmentCount() == 1 ) )
                            {
                                double distance;
                                double t = DistanceToSegmentConstrained( segment0Point1
                                                                       , segment1Poly->lineVertexInParent[0]
                                                                       , segment1Poly->lineVertexInParent[1]
                                                                       , distance );

                                if( distance < segment0Vertex1->GetDistanceToNearestSegment() )
                                {
                                    double otherSegmentT = segment1Poly->fromT + ( ( segment1Poly->toT - segment1Poly->fromT ) * t );

                                    segment0Vertex1->SetNearestSegment( iSegment1, distance, otherSegmentT );
                                }
                            }
                      }
                    /*}*/
///////////////////////////////////
                }
            }
        }
    }

    return intersectionCount;
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

uint32
FOdysseyVectorGroupPaint::GetType()
{
    return FOdysseyVectorObject::VECTORGROUPPAINTTYPE;
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
        std::list<FOdysseyVectorCycle*>::iterator it;

        doPropagate = false;

        for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
        {
            FOdysseyVectorCycle *cycle = (*it);

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
    std::list<FOdysseyVectorCycle*>::iterator it;

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
    std::list<FOdysseyVectorCycle*>::iterator it;

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
FOdysseyVectorGroupPaint::DrawChildren( BLContext* iBLContext, double iCombinedOpacity, uint64 iFlags )
{
    for( FOdysseyVectorObject *child : mChildrenList )
    {
        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath *childPath = static_cast<FOdysseyVectorPath*>(child);

            if( mGroupPaintParam.Wireframe == false )
            {
                childPath->Draw( iBLContext, iCombinedOpacity, iFlags );
            }
            else
            {
                childPath->DrawStructure( iBLContext, mGroupPaintParam.WireframeColor, 1.0f, true );
            }
        }
        else
        {
            child->Draw( iBLContext, iCombinedOpacity, iFlags );
        }
    }
}

void
FOdysseyVectorGroupPaint::Draw( BLContext* iBLContext, double iAncestorsOpacity, uint64 iFlags )
{
    double combinedOpacity = iAncestorsOpacity * mObjectParam.Opacity;

    iBLContext->save();
    iBLContext->transform( mLocalMatrix );

    iBLContext->setCompOp( BL_COMP_OP_SRC_OVER );

    DrawShape( iBLContext, combinedOpacity, iFlags );
    // get sure cycles are drawn before paths
    iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

    DrawChildren( iBLContext, combinedOpacity, iFlags );

    iBLContext->restore();
}

void
FOdysseyVectorGroupPaint::TransferChild( FOdysseyVectorObject* iFosterChild, FOdysseyVectorObject* iInsertAfter )
{
    FOdysseyVectorGroup::TransferChild( iFosterChild, iInsertAfter );
}

void
FOdysseyVectorGroupPaint::UpdateShape( uint32 iUpdateFlags )
{
    std::list<FOdysseyVectorPath*>::iterator it;
   BLMatrix2D identityMatrix = BLMatrix2D( BLMatrix2D::makeIdentity() );

    if( mGroupPaintParam.Painted )
    {
        if( ( mGroupPaintParam.Realtime == true  )
       || ( ( mGroupPaintParam.Realtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATEPAINTGROUPS ) ) )
        {
            for( FOdysseyVectorObject* childPath : mPathList )
            {
                if( childPath->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(childPath);
                    std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
                    BLMatrix2D conversionMatrix;

                    FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, path->GetWorldMatrix(), conversionMatrix );

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

                                segment->SetPaintingReady( true );
                            }
                        }
                    }
                }
            }
        }
    }

    FOdysseyVectorGroup::UpdateShape( iUpdateFlags ); // updates BBox

    if( mGroupPaintParam.Painted )
    {
        if( ( mGroupPaintParam.Realtime == true  )
       || ( ( mGroupPaintParam.Realtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATEPAINTGROUPS ) ) )
        {
            if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE )
            {
                Clear();
                FindCycles();

                if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
                {
                    mInvalidationFlags &= (~INVALIDATE_SHAPE);
                }
            }

            if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD )
            {
                Clear();
                FindCycles();

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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( FOdysseyVectorCycle *cycle : mCycleList )
    {
        cycle->Draw( iBLContext, iCombinedOpacity, iFlags, mGroupPaintParam.Monochrome, mGroupPaintParam.MonochromeColor );
    }

    if( mGroupPaintParam.Wireframe )
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
        std::list<FOdysseyVectorCycle*>::iterator it;

        for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
        {
            FOdysseyVectorCycle *cycle = (*it);

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
        uint32 sectionCount = iSectionBuffer.size();
        uint32 gapCount = iGapSegmentBuffer.size();

        // if both vertices are regular vertices, only the one with the highest ptr has the right to create the segment
        if( ( ( nearestVertex->GetNearestVertex() == iVertex )  && ( iVertex > nearestVertex ) )
        ||    ( nearestVertex->GetNearestVertex() != iVertex )
        // or if the nearest vertex is an intersection vertex.
        ||    ( nearestVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() ) )
        {
            // Warning: setting a parent path here leads to bugs, due to path update of a segment not really belonging to it.
            iGapSegmentBuffer.emplace_back( this, nearestVertex, iVertex );
            //iGapSegmentBuffer[gapCount].Link(); // not necessary. saves us some cpu cycles
            iGapSegmentBuffer[gapCount].Update();

            iSectionBuffer.emplace_back( &iGapSegmentBuffer[gapCount], nullptr, nearestVertex, iVertex );
            iSectionBuffer[sectionCount].Link();

/*
            ::ULIS::FVec2D delta = iVertex->GetCoords() - nearestVertex->GetCoords();
            double length = delta.Distance() * 0.33f;

            if( ( iVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
             && ( iVertex->GetSectionCount() == 2 ) )
            {
                ::ULIS::FVec2D vertexVector = iVertex->GetFirstSegment()->GetVectorFromVertex( iVertex, true );

                iGapSegmentBuffer[gapCount].GetHandle(1)->Set( iVertex->GetCoords() - ( vertexVector * length ) );
            }

            if( ( nearestVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
             && ( nearestVertex->GetSectionCount() == 2 ) )
            {
                ::ULIS::FVec2D vertexVector = nearestVertex->GetFirstSegment()->GetVectorFromVertex( nearestVertex, true );

                iGapSegmentBuffer[gapCount].GetHandle(0)->Set( nearestVertex->GetCoords() - ( vertexVector * length ) );
            }
*/
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
        uint32 sectionCount = iSectionBuffer.size();

        iSectionBuffer.emplace_back();
        // creates topology
        iSectionBuffer[sectionCount].Init( iSegment, iConversionMatrix, sectionVertex0, sectionVertex1 );
        iSectionBuffer[sectionCount].Link();

        sectionVertex0 = sectionVertex1;
    }
}

void
FOdysseyVectorGroupPaint::CreatePathSections( FOdysseyVectorPath* iPath
                                            , BLMatrix2D* iConversionMatrix
                                            , std::vector<FOdysseyVectorSection>& iSectionBuffer
                                            , std::vector<FOdysseyVectorSegmentCubicGap>& iGapSegmentBuffer )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*it);
        FOdysseyVectorVertex* vertex0 = segment->GetVertex( 0 );
        FOdysseyVectorVertex* vertex1 = segment->GetVertex( 1 );

        CreateSegmentSections( segment, iConversionMatrix, iSectionBuffer );

        //TODO::Possible optimization: call only if nearestVertex exists
        CreateVertexGapSegment( vertex0, iSectionBuffer, iGapSegmentBuffer );
        CreateVertexGapSegment( vertex1, iSectionBuffer, iGapSegmentBuffer );
    }
}

uint32
FOdysseyVectorGroupPaint::IntersectSegmentWithList( FOdysseyVectorSegment* iSegment
                                                  , std::list<FOdysseyVectorSegment*>& iSegmenList
                                                  , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    uint32 intersectionCount = 0;

    for( std::list<FOdysseyVectorSegment*>::iterator sit = iSegmenList.begin(); sit != iSegmenList.end(); ++sit )
    {
        FOdysseyVectorSegmentCubic *intersectSegment = static_cast<FOdysseyVectorSegmentCubic*>(*sit);

        if( intersectSegment->GetPaintingCode() != mPaintingCode )
        {
            ::ULIS::FRectD intersectRect = intersectSegment->GetBoundingBox( false ) & iSegment->GetBoundingBox( false );

            if( intersectRect.Area() )
            {
                intersectionCount += IntersectSegment( static_cast<FOdysseyVectorSegmentCubic*>(iSegment)
                                                     , intersectSegment
                                                     , mGroupPaintParam.GapTolerance
                                                     , iIntersectionArray );
            }
        }
    }

    return intersectionCount;
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
        if( GetCycleNormalVector( iVertexArray, iSectionArray ) > 0.0f )
        {
            mCycleList.push_back( new FOdysseyVectorCycle( this, iVertexArray, iSectionArray ) );
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

void
FOdysseyVectorGroupPaint::FindCycles()
{
    std::vector<FExplorationPair> explorationPairsBuffer;

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
            ::ULIS::FVec2D nearestVertexAt = nearestSegment->GetPointAt( nearestSegmentT );
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
    return mGroupPaintParam.GapTolerance;
}

void
FOdysseyVectorGroupPaint::SetGapTolerance( double iGapTolerance )
{
    mGroupPaintParam.GapTolerance = iGapTolerance;

    Invalidate();
}

bool
FOdysseyVectorGroupPaint::IsMonochrome()
{
    return mGroupPaintParam.Monochrome;
}

void
FOdysseyVectorGroupPaint::SetMonochrome( bool iIsMonochrome )
{
    mGroupPaintParam.Monochrome = iIsMonochrome;
}

FColor&
FOdysseyVectorGroupPaint::GetMonochromeColor()
{
    return mGroupPaintParam.MonochromeColor;
}

void
FOdysseyVectorGroupPaint::SetMonochromeColor( const FColor& iMonochromeColor )
{
    mGroupPaintParam.MonochromeColor = iMonochromeColor;
}

void
FOdysseyVectorGroupPaint::SetMonochromeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mGroupPaintParam.MonochromeColor.R = iR;
    mGroupPaintParam.MonochromeColor.G = iG;
    mGroupPaintParam.MonochromeColor.B = iB;
    mGroupPaintParam.MonochromeColor.A = iA;
}

void
FOdysseyVectorGroupPaint::GetMonochromeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA )
{
    oR = mGroupPaintParam.MonochromeColor.R;
    oG = mGroupPaintParam.MonochromeColor.G;
    oB = mGroupPaintParam.MonochromeColor.B;
    oA = mGroupPaintParam.MonochromeColor.A;
}

bool
FOdysseyVectorGroupPaint::IsWireframe()
{
    return mGroupPaintParam.Wireframe;
}

void
FOdysseyVectorGroupPaint::SetWireframe( bool iIsWireframe )
{
    mGroupPaintParam.Wireframe = iIsWireframe;
}

FColor&
FOdysseyVectorGroupPaint::GetWireframeColor()
{
    return mGroupPaintParam.WireframeColor;
}

void
FOdysseyVectorGroupPaint::SetWireframeColor( const FColor& iWireframeColor )
{
    mGroupPaintParam.WireframeColor = iWireframeColor;
}

void
FOdysseyVectorGroupPaint::SetWireframeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mGroupPaintParam.WireframeColor.R = iR;
    mGroupPaintParam.WireframeColor.G = iG;
    mGroupPaintParam.WireframeColor.B = iB;
    mGroupPaintParam.WireframeColor.A = iA;
}

void
FOdysseyVectorGroupPaint::GetWireframeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA )
{
    oR = mGroupPaintParam.WireframeColor.R;
    oG = mGroupPaintParam.WireframeColor.G;
    oB = mGroupPaintParam.WireframeColor.B;
    oA = mGroupPaintParam.WireframeColor.A;
}

bool
FOdysseyVectorGroupPaint::IsPainted()
{
    return mGroupPaintParam.Painted;
}

void
FOdysseyVectorGroupPaint::SetPainted( bool iPainted )
{
    mGroupPaintParam.Painted = iPainted;

    Invalidate();
}

void
FOdysseyVectorGroupPaint::AddChild( FOdysseyVectorObject* iChild, FOdysseyVectorObject* iInsertAfter )
{
    FOdysseyVectorObject::AddChild( iChild, iInsertAfter );

    if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

        mPathList.push_back( path );
    }
}

void
FOdysseyVectorGroupPaint::RemoveChild( FOdysseyVectorObject* iChild )
{
    FOdysseyVectorObject::RemoveChild( iChild );

    if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

        mPathList.remove( path );
    }
}

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    std::list<FOdysseyVectorPath*> intersectedPathList = mPathList; // copy
    uint32 totalGapSegmentCount = 0;
    uint32 totalSectionCount = 0;
    // act as boolean without the need to reinitialize its value
    static uint32 paintingCode;

    mPaintingCode = ++paintingCode;

    for( std::list<FOdysseyVectorPath*>::iterator pit = mPathList.begin(); pit != mPathList.end(); ++pit )
    {
        FOdysseyVectorPath *path = (*pit);
        std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
        std::list<FOdysseyVectorVertex*>& vertexList = path->GetVertexList();
        uint32 intersectionCount = 0;

        for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
        {
            FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*sit);

            for( std::list<FOdysseyVectorPath*>::iterator iit = intersectedPathList.begin(); iit != intersectedPathList.end(); ++iit )
            {
                FOdysseyVectorPath *intersectedPath = (*iit);
                std::list<FOdysseyVectorSegment*>& intersectPathSegmentList = intersectedPath->GetSegmentList();

                intersectionCount += IntersectSegmentWithList ( segment, intersectPathSegmentList, mIntersectionArray );
            }

            segment->SetPaintingCode( mPaintingCode ); // set as treated. It will be excluded from later intersection tests.

            if( segment->GetIntersectionVertexCount() )
            {
                segment->GetPath()->SetPaintingCode( mPaintingCode );
            }
        }

        // create near-intersections
        for( std::list<FOdysseyVectorVertex*>::iterator vit = vertexList.begin(); vit != vertexList.end(); ++vit )
        {
            FOdysseyVectorVertex *vertex = static_cast<FOdysseyVectorVertex*>(*vit);

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

        intersectedPathList.pop_front(); // we don't need the path anymore. By and by the list will empty by itself.
    }

    // we now need another loop to count and to reserve the memory in one block to create the sections.
    // we do that so that we can alloc the buffers at once instead of allocating a lot of new sections/segments.
    // This is needed especially with gaps because segments get all their intersections at the end of the whole intersecting process.
    for( FOdysseyVectorPath *path : mPathList )
    {
        if( path->GetPaintingCode() == mPaintingCode )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = (*sit);
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
    // reserving whole block is required to avoid memory shifting.
    mSectionBuffer.reserve( totalSectionCount + totalGapSegmentCount );

    // create sections for exact intersections on each segment

    for( FOdysseyVectorPath *path : mPathList )
    {
        // only for path that have intersected segments.
        if( path->GetPaintingCode() == mPaintingCode )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
            BLMatrix2D& pathWorldMatrix = path->GetWorldMatrix();
            BLMatrix2D conversionMatrix;

            FOdysseyVector::MatrixMultiply( mInverseWorldMatrix, pathWorldMatrix, conversionMatrix );

            CreatePathSections( path, &conversionMatrix, mSectionBuffer, mGapSegmentBuffer );

            // create a cycle right now for untouched looped-paths
            if( ( path->IsLoop() == true ) && ( path->HasIntersections() == false ) )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                path->ToVertexAndSectionArray( vertexArray, sectionArray );

                if( vertexArray.size() )
                {
                    mCycleList.push_back( new FOdysseyVectorCycle( this, vertexArray, sectionArray ) );
                }
            }

            // Do some clearing here, taking advantage of that loop to save some CPU cycles.
            for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = (*sit);

                segment->ClearIntersections();
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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);
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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);
        std::list<FOdysseyVectorCycle*>::iterator cit;

        for( cit = mCycleList.begin(); cit != mCycleList.end(); ++cit )
        {
           FOdysseyVectorCycle* innerCycle = (*cit);

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

void
FOdysseyVectorGroupPaint::SimplifyGraph()
{
    bool keepSimplifying;

    do
    {
        keepSimplifying = false;

        for( int i = 0; i < mSectionBuffer.size(); i++ )
        {
            FOdysseyVectorSection *section = &mSectionBuffer[i];

            if( section->IsLinked() == true )
            {
                if( section->GetSegment() )
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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);

        delete cycle;
    }

    mCycleList.clear();

    // Clear gap segments. Not necessary to unlink them, as these are not linked. Only section are linked.
    mGapSegmentBuffer.clear();


    // clean section topology (unlinking has been moved after the cycle detection).

    mSectionBuffer.clear();



    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorObject *child = (*oit);

        if( child->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            std::list<FOdysseyVectorSegment*>& pathSegmentList = path->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator sit = pathSegmentList.begin(); sit != pathSegmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*sit);

                // reset nearest segment
                segment->GetVertex(0)->SetNearestSegment( nullptr, mGroupPaintParam.GapTolerance/*DBL_MAX*/, 0.0f );
                segment->GetVertex(1)->SetNearestSegment( nullptr, mGroupPaintParam.GapTolerance/*DBL_MAX*/, 0.0f );
            }
        }
    }

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

     for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
     {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);
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

    groupPaintCopy->mGroupPaintParam = mGroupPaintParam;

    CopyBuckets( groupPaintCopy, false );

    return groupPaintCopy;
}

void
FOdysseyVectorGroupPaint::GetSelectedPoints( std::vector<FOdysseyVectorPoint*>& oPointArray
                                           , ePointSelectionFlags iPointSelectionFlags )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        if( child->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            path->GetSelectedPoints( oPointArray, iPointSelectionFlags );
        }
    }

    if( iPointSelectionFlags & ePointSelectionFlags::Bucket )
    {
        for( std::list<FOdysseyVectorBucket*>::iterator it = mSelectedBucketList.begin(); it != mSelectedBucketList.end(); ++it )
        {
            FOdysseyVectorBucket *bucket = (*it);

            oPointArray.push_back( bucket );
        }
    }
}

bool
FOdysseyVectorGroupPaint::GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox, bool iWorld )
{
    std::list<FOdysseyVectorObject*>::iterator it;
    bool inited = false;

    for( it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = *it;

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
    BLImage* maskImage = GetScene()->GetEngine()->GetBLMask();
    std::list<FOdysseyVectorBucket*>::iterator it;
    BLImageData imageData;

    maskImage->getData( &imageData );

    for( it = mBucketList.begin(); it != mBucketList.end(); ++it )
    {
        FOdysseyVectorBucket* bucket = (*it);
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
    std::list<FOdysseyVectorCycle*>::iterator it;

    if( mBBox.HitTest( ::ULIS::FVec2D( localCoord.x, localCoord.y ) ) )
    {
        for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
        {
            FOdysseyVectorCycle *cycle = (*it);

            // TODO: Bounding volume for cycles for faster search
            if( cycle->HitTest( localCoord.x, localCoord.y ) == true )
            {
                return cycle;
            }
        }
    }

    return nullptr;
}

void
FOdysseyVectorGroupPaint::GetSectionsFromSegment( FOdysseyVectorSegment* iSegment
                                                , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        if( mSectionBuffer[i].GetSegment() == iSegment )
        {
            oSectionArray.push_back( &mSectionBuffer[i] );
        }
    }
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
FOdysseyVectorGroupPaint::EraseSegment( FOdysseyVectorSegment* iSegment
                                      , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                      , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray )
{
    std::vector<FOdysseyVectorSection*> sectionArray;
    FOdysseyVectorPath* path = iSegment->GetPath();
    std::vector<FVertexPair> vertexPairArray;

    GetSectionsFromSegment( iSegment, sectionArray );

    // first step : find to which extent the segment is erased. We check on both sides of the segment
    // until we reach a section that is marked as erased or til the end. this is the role of
    // ReachVertexFromSection(). We also get sure the vertexPair we found is unique.
    for( FOdysseyVectorSection* section : sectionArray )
    {
        // only for non-erased sections
        if( section->IsErased() == false )
        {
            FOdysseyVectorVertex* endVertex[2] = { ReachVertexFromSection( section->GetVertex(0)
                                                                         , section
                                                                         , iSegment ),
                                                   ReachVertexFromSection( section->GetVertex(1)
                                                                         , section
                                                                         , iSegment ) };
            FVertexPair vertexPair = FVertexPair( endVertex[0], endVertex[1] );

            if( std::find( vertexPairArray.begin(), vertexPairArray.end(), vertexPair ) == vertexPairArray.end() )
            {
                vertexPairArray.push_back( vertexPair );
            }
        }
    }

    // create new vertices and segment in between
    for( int i = 0; i < vertexPairArray.size(); i++ )
    {
        FVertexPair* vertexPair = &vertexPairArray[i];
        FOdysseyVectorVertex* endVertex[2] = { vertexPair->vertex[0], vertexPair->vertex[1] };

        if( ( endVertex[0] != iSegment->GetVertex(0) ) || ( endVertex[1] != iSegment->GetVertex(1) ) )
        {
            double radiusDelta = iSegment->GetVertex(1)->GetRadius() - iSegment->GetVertex(0)->GetRadius();
            double t0 = endVertex[0]->GetT( iSegment );
            double t1 = endVertex[1]->GetT( iSegment );

            if( endVertex[0]->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                double radius = iSegment->GetVertex(0)->GetRadius() + ( radiusDelta * endVertex[0]->GetT( iSegment ) );

                endVertex[0] = new FOdysseyVectorVertex( endVertex[0]->GetPath()
                                                       , endVertex[0]->GetX()
                                                       , endVertex[0]->GetY()
                                                       , radius );

                oAddedVertexArray.push_back( endVertex[0] );
            }

            if( endVertex[1]->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                double radius = iSegment->GetVertex(0)->GetRadius() + ( radiusDelta * endVertex[1]->GetT( iSegment ) );

                endVertex[1] = new FOdysseyVectorVertex( endVertex[1]->GetPath()
                                                       , endVertex[1]->GetX()
                                                       , endVertex[1]->GetY()
                                                       , radius );

                oAddedVertexArray.push_back( endVertex[1] );
            }

            if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);
                //::ULIS::FVec2D* bezier = cubicSegment->GetBezier();
                ::ULIS::FVec2D extractedBezier[4];
                FOdysseyVectorSegmentCubic* newSegment;

                // Note: the bezier we extract must not have its vertices exactly on the former
                // intersected segment because due to floating point imprecision, it might intersect
                // again even after the extraction. So we shorten it a little bit, hence the 
                // t0 + ( ( t1 - t0 ) * 0.001f ) and t1 + ( ( t0 - t1 ) * 0.001f )
                FOdysseyVector::BezierExtract( cubicSegment->GetVertex(0)->GetCoords()
                                             , cubicSegment->GetHandle(0)->GetCoords()
                                             , cubicSegment->GetHandle(1)->GetCoords()
                                             , cubicSegment->GetVertex(1)->GetCoords()
                                             , t0 + ( ( t1 - t0 ) * 0.001f )
                                             , t1 + ( ( t0 - t1 ) * 0.001f )
                                             , extractedBezier[0]
                                             , extractedBezier[1]
                                             , extractedBezier[2]
                                             , extractedBezier[3] );
                // adjust coordinates for the reason described right above
                endVertex[0]->Set( extractedBezier[0].x, extractedBezier[0].y );
                endVertex[1]->Set( extractedBezier[3].x, extractedBezier[3].y );

                newSegment = new FOdysseyVectorSegmentCubic( iSegment->GetPath()
                                                           , endVertex[0]
                                                           , extractedBezier[1].x
                                                           , extractedBezier[1].y
                                                           , extractedBezier[2].x
                                                           , extractedBezier[2].y
                                                           , endVertex[1]
                                                           , true );

                oAddedSegmentArray.push_back( newSegment );
            }
        }
    }
}

void
FOdysseyVectorGroupPaint::ExtendErasedSection( FOdysseyVectorVertex* iVertex
                                             , FOdysseyVectorSection* iInitiatorSection
                                             , FOdysseyVectorSection* iFromSection
                                             , std::vector<FOdysseyVectorSection*>& oErasedSectionArray )
{
    FOdysseyVectorSection* fromSection = iFromSection;
    FOdysseyVectorVertex* currentVertex = iVertex;

    while(   fromSection 
        && ( fromSection->GetSegment()->GetClass() != FOdysseyVectorSegmentCubicGap::StaticClass() )
        && ( currentVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() ) )
    {
        FOdysseyVectorSection* nextSection = currentVertex->GetOtherSection( fromSection, false );

        if( nextSection == iInitiatorSection )
        {
            break;
        }

        if( nextSection )
        {
            if( nextSection->IsErased() == false )
            {
                oErasedSectionArray.push_back( nextSection );

                nextSection->SetErased( true );

                currentVertex = nextSection->GetOtherVertex( currentVertex );
            }
        }

        fromSection = nextSection;
    }
}

void
FOdysseyVectorGroupPaint::EraseSections( std::vector<FOdysseyVectorSection*>& iErasedSectionArray
                                       , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                       , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                       , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    std::vector<FOdysseyVectorSection*> extendedErasedSectionArray;
                                        // Arbitrary value
    extendedErasedSectionArray.reserve( iErasedSectionArray.size() * 2 );

    // first step : relink sections as they were all unlinked after the cycle detection process
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        mSectionBuffer[i].Link();
    }

    // second step. Extend erased section array with the neighbour sections
    for( FOdysseyVectorSection* erasedSection : iErasedSectionArray )
    {
        extendedErasedSectionArray.push_back( erasedSection );

        erasedSection->SetErased( true );

        ExtendErasedSection( erasedSection->GetVertex(0), erasedSection, erasedSection, extendedErasedSectionArray );
        ExtendErasedSection( erasedSection->GetVertex(1), erasedSection, erasedSection, extendedErasedSectionArray );
    }

    // retrieve and prepare all concerned segments for removal
    for( FOdysseyVectorSection* extendedErasedSection : extendedErasedSectionArray )
    {
        FOdysseyVectorSegment* segment = extendedErasedSection->GetSegment();

        // filter. A segment could be a of type SegmentGap as well
        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            // check if not already removed.
            if( std::find( oRemovedSegmentArray.begin(), oRemovedSegmentArray.end(), cubicSegment ) == oRemovedSegmentArray.end() )
            {
                oRemovedSegmentArray.push_back( cubicSegment );
            }
        }
    }

    // get sections for segment (there are no gap segments in this array)
    for( FOdysseyVectorSegment* segment : oRemovedSegmentArray )
    {
        EraseSegment( segment, oAddedVertexArray, oAddedSegmentArray );
    }

    // unlink sections again
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        mSectionBuffer[i].Unlink();
    }

    // remove erased segments
    for( FOdysseyVectorSegment* segment : oRemovedSegmentArray )
    {
        segment->GetPath()->RemoveSegment( segment );
    }

    // add new vertices
    for( FOdysseyVectorVertex* vertex : oAddedVertexArray )
    {
        vertex->GetPath()->AddVertex( vertex );
    }

    // add new segments
    for( FOdysseyVectorSegment* segment : oAddedSegmentArray )
    {
        segment->GetPath()->AddSegment( segment );
    }

    // clean if needed
    for( FOdysseyVectorSegment* segment : oRemovedSegmentArray )
    {
        if( segment->GetVertex(0)->GetSegmentCount() == 0 )
        {
            oRemovedVertexArray.push_back( segment->GetVertex(0) );
        }

        if( segment->GetVertex(1)->GetSegmentCount() == 0 )
        {
            oRemovedVertexArray.push_back( segment->GetVertex(1) );
        }
    }

    // remove erased vertices
    for( FOdysseyVectorVertex* vertex : oRemovedVertexArray )
    {
        vertex->GetPath()->RemoveVertex( vertex );
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

void
FOdysseyVectorGroupPaint::GetSectionsFromPath( FOdysseyVectorPath* iPath
                                             , std::vector<FOdysseyVectorSection*>& oPickedSectionArray )
{
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        if( mSectionBuffer[i].GetSegment()->GetPath() == iPath )
        {
            oPickedSectionArray.push_back( &mSectionBuffer[i] );
        }
    }
}

bool
FOdysseyVectorGroupPaint::PickSection( FOdysseyVectorSection* iSection
                                     , const ::ULIS::FRectD& iMaskRect
                                     , const uint8* iMaskPixelData )
{
    ::ULIS::FVec2D* bezier = iSection->GetBezier();
    BLPoint pt[4] = { mWorldMatrix.mapPoint( bezier[0].x, bezier[0].y )
                    , mWorldMatrix.mapPoint( bezier[1].x, bezier[1].y )
                    , mWorldMatrix.mapPoint( bezier[2].x, bezier[2].y )
                    , mWorldMatrix.mapPoint( bezier[3].x, bezier[3].y ) };
    ::ULIS::FVec2D worldBezier[4] = { ::ULIS::FVec2D( pt[0].x, pt[0].y )
                                    , ::ULIS::FVec2D( pt[1].x, pt[1].y )
                                    , ::ULIS::FVec2D( pt[2].x, pt[2].y )
                                    , ::ULIS::FVec2D( pt[3].x, pt[3].y ) };

    return FOdysseyVector::PickBezier( worldBezier, iMaskRect, iMaskPixelData );
}

bool
FOdysseyVectorGroupPaint::PickSections( std::vector<FOdysseyVectorSection*>& iSectionArray
                                     ,  std::vector<FOdysseyVectorSection*>& oPickedSectionArray )
{
    BLImage* maskImage = GetScene()->GetEngine()->GetBLMask();
    BLImageData maskData;
    ::ULIS::FRectD maskRect;
    bool picked = false;

    maskImage->getData( &maskData );

    maskRect = ::ULIS::FRectD( 0, 0, maskData.size.w, maskData.size.h );

    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        if( PickSection( iSectionArray[i], maskRect, (uint8*) maskData.pixelData ) )
        {
            oPickedSectionArray.push_back( iSectionArray[i] );

            picked = true;
        }
    }

    return picked;
}

bool
FOdysseyVectorGroupPaint::PickSections( std::vector<FOdysseyVectorSection*>& oPickedSectionArray )
{
    BLImage* maskImage = GetScene()->GetEngine()->GetBLMask();
    BLImageData maskData;
    ::ULIS::FRectD maskRect;
    bool picked = false;

    maskImage->getData( &maskData );

    maskRect = ::ULIS::FRectD( 0, 0, maskData.size.w, maskData.size.h );

    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        if( PickSection( &mSectionBuffer[i], maskRect, (uint8*) maskData.pixelData ) )
        {
            oPickedSectionArray.push_back( &mSectionBuffer[i] );

            picked = true;
        }
    }

    return picked;
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
