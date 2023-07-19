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

    mSegmentShapeArray.clear();
}

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo( iScene )
{
}

void
FOdysseyVectorUndoSegmentReshape::RecordSegment( std::vector<FOdysseyVectorSegment*>& iSegmentArray )
{
    mSegmentShapeArray.reserve( iSegmentArray.size() );

    for( int i = 0; i < iSegmentArray.size(); i++ )
    {
        mSegmentShapeArray.push_back( FSegmentShape( iSegmentArray[i] ) );
    }
}

void
FOdysseyVectorUndoSegmentReshape::RecordSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentShapeArray.push_back( FSegmentShape( iSegment ) );
}

bool
FOdysseyVectorUndoSegmentReshape::HasSegment( FOdysseyVectorSegment* iSegment )
{
    for( int i = 0; i < mSegmentShapeArray.size(); i++ )
    {
        if( mSegmentShapeArray[i].segment == iSegment )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorUndoSegmentReshape::SwapArray()
{
    for( int i = 0; i < mSegmentShapeArray.size(); i++ )
    {
        if( mSegmentShapeArray[i].segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( mSegmentShapeArray[i].segment );
            ::ULIS::FVec2D& point0 = cubicSegment->GetVertex(0)->GetCoords();
            ::ULIS::FVec2D& point1 = cubicSegment->GetVertex(1)->GetCoords();
            ::ULIS::FVec2D& point2 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& point3 = cubicSegment->GetHandle(1)->GetCoords();
            ::ULIS::FVec2D swapPoint0 = point0;
            ::ULIS::FVec2D swapPoint1 = point1;
            ::ULIS::FVec2D swapPoint2 = point2;
            ::ULIS::FVec2D swapPoint3 = point3;

            point0 = mSegmentShapeArray[i].point[0];
            point1 = mSegmentShapeArray[i].point[1];
            point2 = mSegmentShapeArray[i].point[2];
            point3 = mSegmentShapeArray[i].point[3];

            mSegmentShapeArray[i].point[0] = swapPoint0;
            mSegmentShapeArray[i].point[1] = swapPoint1;
            mSegmentShapeArray[i].point[2] = swapPoint2;
            mSegmentShapeArray[i].point[3] = swapPoint3;
        }

        mSegmentShapeArray[i].segment->GetVertex(0)->InvalidateSegments();
        mSegmentShapeArray[i].segment->GetVertex(1)->InvalidateSegments();
    }
}

void
FOdysseyVectorUndoSegmentReshape::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    SwapArray();

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoSegmentReshape::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    SwapArray();

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSegmentReshape::ToString() const
{
    return FString("FOdysseyVectorUndoSegmentReshape");
}
