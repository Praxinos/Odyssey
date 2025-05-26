// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorSegmentExtended.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVector.h"

FOdysseyVectorSegmentExtended::~FOdysseyVectorSegmentExtended()
{

}

FOdysseyVectorSegmentExtended::FOdysseyVectorSegmentExtended( FOdysseyVectorObject* iOwner
                                                            , FOdysseyVectorVertex* iPoint0
                                                            , FOdysseyVectorVertex* iPoint1
                                                            , bool iNeedsWidth )
    : FOdysseyVectorSegment( iOwner, iPoint0, iPoint1, iNeedsWidth )
{
    mIsInvalidated = false;

    mLength = ::ULIS::FVec2D( mPoint[1]->GetCoords() - mPoint[0]->GetCoords() ).Distance();

    mFractionCache.clear();
    mFractionCache.emplace_back( GetVertex(0), 0.0f, GetVertex(1), 1.0f, mLength );
}

bool
FOdysseyVectorSegmentExtended::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorSegment::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorSegmentExtended::DrawStructure( BLContext* iBLContext
                                            , FOdysseyVectorObject* iParentObject
                                            , bool iWorld )
{
    BLMatrix2D& worldMatrix = iParentObject->GetWorldMatrix();
    ::ULIS::FVec2D vertex0Coords = GetVertex(0)->GetCoords();
    ::ULIS::FVec2D vertex1Coords = GetVertex(1)->GetCoords();
    BLPoint point0 = iWorld ? worldMatrix.mapPoint( vertex0Coords.x, vertex0Coords.y ) : BLPoint( vertex0Coords.x, vertex0Coords.y );
    BLPoint point1 = iWorld ? worldMatrix.mapPoint( vertex1Coords.x, vertex1Coords.y ) : BLPoint( vertex1Coords.x, vertex1Coords.y );
    BLPath path;

    path.moveTo( point0 );
    path.lineTo( point1 );

    iBLContext->strokePath( path );
}
