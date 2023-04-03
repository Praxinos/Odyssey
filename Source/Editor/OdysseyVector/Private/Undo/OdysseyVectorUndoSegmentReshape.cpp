#include "Undo/OdysseyVectorUndoSegmentReshape.h"

FOdysseyVectorUndoSegmentReshape::~FOdysseyVectorUndoSegmentReshape()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }

    mSegmentShapeBeforeArray.clear();
    mSegmentShapeAfterArray.clear();
}

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
}

void
FOdysseyVectorUndoSegmentReshape::LoadArray( std::vector<FSegmentShape>& mSegmentShapeArray )
{
    for( int i = 0; i < mSegmentShapeArray.size(); i++ )
    {
        if( mSegmentShapeArray[i].segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( mSegmentShapeArray[i].segment );
            ::ULIS::FVec2D& point0 = cubicSegment->GetVertex(0)->GetCoords( nullptr );
            ::ULIS::FVec2D& point1 = cubicSegment->GetVertex(1)->GetCoords( nullptr );
            ::ULIS::FVec2D& point2 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& point3 = cubicSegment->GetHandle(1)->GetCoords();

            point0 = mSegmentShapeArray[i].point[0];
            point1 = mSegmentShapeArray[i].point[1];
            point2 = mSegmentShapeArray[i].point[2];
            point3 = mSegmentShapeArray[i].point[3];
        }

        mSegmentShapeArray[i].segment->Invalidate();
    }

    mScene->Update( 0 );
}

void
FOdysseyVectorUndoSegmentReshape::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    LoadArray( mSegmentShapeAfterArray );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoSegmentReshape::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    LoadArray( mSegmentShapeBeforeArray );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSegmentReshape::ToString() const
{
    return FString("FOdysseyVectorUndoSegmentReshape");
}
