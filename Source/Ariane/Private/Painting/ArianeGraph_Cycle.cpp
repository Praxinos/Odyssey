// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeGraph.h"
// for measurements
#include <chrono>

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FArianeGraph::FCycle::~FCycle()
{
}

FArianeGraph::FCycle::FCycle( const TArray<uint32>& NodeIndices
                            , const TArray<FSection*>& Sections )
    : //mBucket( nullptr )
    //, mPropagatedBucket( nullptr )
      ContourNodeIndices ( NodeIndices )
    , ContourSections ( Sections )
    , ParentCycle( nullptr )
    //, Propagated( false )
{
    Build( );
}

void
FArianeGraph::FCycle::Merge( FArianeGraph::FCycle* MergeCycle )
{
    for( int i = 0; i < MergeCycle->ContourSections.Num(); i++ )
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
        if( MergeCycle->ContourSections[i]->CycleCount < 2 )
        {
            MergeCycle->ContourSections[i]->AddCycle( this );

            InnerSections.Add( MergeCycle->ContourSections[i] );
        }
    }

    CombinedPath.add_path( MergeCycle->ContourPath );
}

//static
/*
void
FArianeGraph::FCycle::ToBucketArray( TArray<FArianeGraph::FCycle*>& iCyleArray
                                  , TArray<FOdysseyVectorBucket*>& oBucketArray )
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
*/

FArianeGraph::FCycle*
FArianeGraph::FCycle::GetParentCycle()
{
    return ParentCycle;
}

void
FArianeGraph::FCycle::SetParentCycle( FArianeGraph::FCycle *InParentCycle )
{
    this->ParentCycle = InParentCycle;
}

bool
FArianeGraph::FCycle::HasNode( FNode* Node )
{
    // First test : get sure they don't share a common section
    // which would in that case mean that we do not fit in the parent cycle
    for( int i = 0; i < ContourSections.Num(); i++ )
    {
        FSection* Section = ContourSections[i];
        uint32 ContourVertexIndex = ContourNodeIndices[i];
        FNode* ContourNode = Section->Nodes[ContourVertexIndex];

        if( ContourNode == Node )
        {
            return true;
        }
    }

    return false;
}

bool
FArianeGraph::FCycle::FitsIn( FArianeGraph::FCycle* ParentCandidate )
{
    // First test : get sure they don't share a common section
    // which would in that case mean that we do not fit in the parent cycle
    for( int i = 0; i < ContourSections.Num(); i++ )
    {
        FSection* Section = ContourSections[i];
        uint32 ContourNodeIndex = ContourNodeIndices[i];
        FNode* ContourNode = Section->Nodes[ContourNodeIndex];
        FVector2D MidPoint = Section->GetPointAt( 0.5f );
        BLPoint Pt = BLPoint( MidPoint.X, MidPoint.Y );

        if( Section->GetOtherCycle( this ) == ParentCandidate )
        {
            return false;
        }

        // also check they don't have a vertex in common.
        if( ParentCandidate->ContourPath.hit_test( Pt, BL_FILL_RULE_EVEN_ODD ) != BL_HIT_TEST_IN )
        {
            return false;
        }
    }

    return true;
}

void
FArianeGraph::FCycle::Build()
{
    int32 ArraySize = ContourSections.Num();
    int seg = 0;
    BLBox bbox;

    if ( ContourSections.Num() )
    {
        FSection* FirstSection = ContourSections[0];
        uint32 FirstNodeIndex = ContourNodeIndices[0];
        FNode* FirstNode = FirstSection->Nodes[FirstNodeIndex];

        ContourPath.move_to( FirstNode->Position.X, FirstNode->Position.Y );

        for( int i = 0; i < ArraySize; i++ )
        {
            int n = ( i + 1 ) % ArraySize;
            FSection* Section = ContourSections[i];
            uint32 SectionNodeIndex = ContourNodeIndices[i];
            uint32 SectionNextNodeIndex = ( SectionNodeIndex == 0 ) ? 1 : 0;

            if( Section->GetClass() == FSectionLinear::StaticClass() )
            {
                FSectionLinear* LinearSection = static_cast<FSectionLinear*>(Section);

                ContourPath.line_to( Section->Nodes[SectionNextNodeIndex]->Position.X
                                   , Section->Nodes[SectionNextNodeIndex]->Position.Y );
            }

            if( Section->GetClass() == FSectionCubic::StaticClass() )
            {
                FSectionCubic* CubicSection = static_cast<FSectionCubic*>(Section);
                FVector2D* SectionBezier = CubicSection->GetBezier();

                Section->AddCycle( this );

                // check if we need to revert the bezier. Indeed, a cycle is a combination of sections
                // that may not go the same way. We have to run through them the same way.
                if ( SectionNodeIndex < SectionNextNodeIndex )
                {
                    ContourPath.cubic_to( SectionBezier[1].X, SectionBezier[1].Y
                                        , SectionBezier[2].X, SectionBezier[2].Y
                                        , SectionBezier[3].X, SectionBezier[3].Y );
                }
                else
                {
                    ContourPath.cubic_to( SectionBezier[2].X, SectionBezier[2].Y
                                        , SectionBezier[1].X, SectionBezier[1].Y
                                        , SectionBezier[0].X, SectionBezier[0].Y );
                }
            }
        }

        ContourPath.close();
    }

    CombinedPath = ContourPath;

    ContourPath.get_bounding_box( &bbox );

    BBox = FBox2D( FVector2D( bbox.x0, bbox.y0 )
                 , FVector2D( bbox.x1, bbox.y1 ) );
}

/*
void
FArianeGraph::FCycle::SetBucket( FOdysseyVectorBucket* iBucket )
{
    mBucket = mPropagatedBucket = iBucket;
}

FOdysseyVectorBucket*
FArianeGraph::FCycle::GetBucket()
{
    return mBucket;
}

void
FArianeGraph::FCycle::SetPropagatedBucket( FOdysseyVectorBucket* iPropagatedBucket )
{
    mPropagatedBucket = iPropagatedBucket;
}

FOdysseyVectorBucket*
FArianeGraph::FCycle::GetPropagatedBucket()
{
    return mPropagatedBucket;
}

void
FArianeGraph::FCycle::PropagateBucket( TArray<FSection*> iSectionArray
                                    , TArray<FArianeGraph::FCycle*>& oNextCycleArray )
{
    // test outer sections
    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        FArianeGraph::FCycle* neighbourCycle = iSectionArray[i]->GetOtherCycle( this );

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
FArianeGraph::FCycle::PropagateBucket( TArray<FArianeGraph::FCycle*>& oNextCycleArray )
{
    FOdysseyVectorBucket* bucket = mBucket ? mBucket : mPropagatedBucket;

    if( bucket && bucket->IsPropagated() )
    {
        PropagateBucket( mContourSectionArray, oNextCycleArray );
        PropagateBucket( mInnerSectionArray  , oNextCycleArray );
    }
}
*/

bool
FArianeGraph::FCycle::HitTest( double iX, double iY )
{
    BLPoint Point = { iX, iY };

    uint32 Ret = CombinedPath.hit_test( Point, BL_FILL_RULE_EVEN_ODD );

    return ( Ret == BL_HIT_TEST_IN ) ? true : false;
}

/*
// for debugging purposes
static void
ShowCycle( TArray<FNode*>& Nodes
         , TArray<FSection*>& Sections)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), Nodes.Num() );

    for( int i = 0; i < Nodes.size(); i++ )
    {
        FEdge* segment = Sections[i]->GetSegment();
        FNode* vertex0 = segment->GetVertex(0);
        FNode* vertex1 = segment->GetVertex(1);
        BLPoint pt0 = owner->GetWorldMatrix().map_point( vertex0->GetCoords().x, vertex0->GetCoords().y );
        BLPoint pt1 = owner->GetWorldMatrix().map_point( vertex1->GetCoords().x, vertex1->GetCoords().y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], Sections[i], Sections[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

::ULIS::FRectD
FArianeGraph::FCycle::GetBBox( bool iWorld )
{
    if( iWorld )
    {
        BLMatrix2D& worldMatrix = mOwner->GetWorldMatrix();
        BLPoint pt[4] = { worldMatrix.map_point( mBBox.x          , mBBox.y           )
                        , worldMatrix.map_point( mBBox.x + mBBox.w, mBBox.y           )
                        , worldMatrix.map_point( mBBox.x + mBBox.w, mBBox.y + mBBox.h )
                        , worldMatrix.map_point( mBBox.x          , mBBox.y + mBBox.h ) };
        double xmin = ::ULIS::FMath::Min4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymin = ::ULIS::FMath::Min4( pt[0].y, pt[1].y, pt[2].y, pt[3].y )
             , xmax = ::ULIS::FMath::Max4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymax = ::ULIS::FMath::Max4( pt[0].y, pt[1].y, pt[2].y, pt[3].y );

        return ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    }

    return mBBox;
}
*/
