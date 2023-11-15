#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorUndo::~FOdysseyVectorUndo()
{
}

FOdysseyVectorUndo::FOdysseyVectorUndo( FOdysseyVectorGroupPaint *iScene )
    : mApplied( true )
    , mScene( iScene )
{
}

void
FOdysseyVectorUndo::Apply( UObject* iIgnored )
{
    mApplied = true;
}

void
FOdysseyVectorUndo::Revert( UObject* iIgnored )
{
    mApplied = false;
}

FSnapshotObject::~FSnapshotObject()
{
    for( int i = 0; i < mChildrenSnapshotArray.size(); i++ )
    {
        delete mChildrenSnapshotArray[i];
    }
}

FSnapshotObject::FSnapshotObject( FOdysseyVectorObject* iObject, uint32 iObjectSnapshotFlags )
    : mObjectSnapshotFlags( iObjectSnapshotFlags )
    , mObject( iObject )
{
    if( iObjectSnapshotFlags & SNAPSHOT_TRANSFORMATIONS )
    {
        iObject->GetTransform( mTranslationX
                             , mTranslationY
                             , mRotation
                             , mScalingX
                             , mScalingY );
    }

    if( iObjectSnapshotFlags & SNAPSHOT_CHILDREN_TRANSFORMATIONS )
    {
        std::list<FOdysseyVectorObject*>& childrenObjectList = iObject->GetChildrenList();

        mChildrenSnapshotArray.reserve( childrenObjectList.size() );

        for( FOdysseyVectorObject* child : childrenObjectList )
        {
            mChildrenSnapshotArray.push_back( new FSnapshotObject( child, SNAPSHOT_TRANSFORMATIONS ) );
        }
    }
}

void
FSnapshotObject::Restore()
{
    if( mObjectSnapshotFlags & SNAPSHOT_TRANSFORMATIONS )
    {
        double swapTranslationX, swapTranslationY, swapRotation, swapScalingX, swapScalingY;

        mObject->GetTransform( swapTranslationX
                             , swapTranslationY
                             , swapRotation
                             , swapScalingX
                             , swapScalingY );

        mObject->SetTransform( mTranslationX
                             , mTranslationY
                             , mRotation
                             , mScalingX
                             , mScalingY );

        mObject->UpdateMatrix();

        mTranslationX = swapTranslationX;
        mTranslationY = swapTranslationY;
        mRotation     = swapRotation;
        mScalingX     = swapScalingX;
        mScalingY     = swapScalingY;
    }

    if( mObjectSnapshotFlags & SNAPSHOT_CHILDREN_TRANSFORMATIONS )
    {
        for( int i = 0; i < mChildrenSnapshotArray.size(); i++ )
        {
            mChildrenSnapshotArray[i]->Restore();
        }
    }
}

FSnapshotPoint::~FSnapshotPoint()
{
}

FSnapshotPoint::FSnapshotPoint( FOdysseyVectorPoint* iPoint
                              , uint32 iPointSnapshotFlags )
    : mPointSnapshotFlags( iPointSnapshotFlags )
    , mPoint( iPoint )
    , mCoords( iPoint->GetCoords() )
    , mRadius( iPoint->GetRadius() )
{
}

void
FSnapshotPoint::Restore()
{
    if( mPointSnapshotFlags & SNAPSHOT_POSITION )
    {
        ::ULIS::FVec2D swapCoords = mPoint->GetCoords();

        mPoint->Set( mCoords.x, mCoords.y );

        mCoords = swapCoords;
    }

    if( mPointSnapshotFlags & SNAPSHOT_RADIUS )
    {
        double swapRadius = mPoint->GetRadius();

        mPoint->SetRadius( mRadius );

        mRadius = swapRadius;
    }
}

FSnapshotVertex::~FSnapshotVertex()
{
}

FSnapshotVertex::FSnapshotVertex( FOdysseyVectorVertex* iVertex
                                , uint32 iPointSnapshotFlags
                                , uint32 iVertexSnapshotFlags )
    : FSnapshotPoint( iVertex, iPointSnapshotFlags)
    , mVertexSnapshotFlags( iVertexSnapshotFlags )
{
    if( iVertexSnapshotFlags & SNAPSHOT_ALIGNMENT )
    {
        // note: we only save the flags that are "manually" set by the user,
        // i am unsure about the consistency of other flags
        mAlignment = iVertex->IsHandleAligned();
    }
}

void
FSnapshotVertex::Restore()
{
    FSnapshotPoint::Restore();

    if( mVertexSnapshotFlags & SNAPSHOT_ALIGNMENT )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(mPoint);
        bool alignment = vertex->IsHandleAligned();

        vertex->SetHandleAligned( mAlignment );

        // swap for redo
        mAlignment = alignment;
    }
}

FSnapshotSegmentCubic::~FSnapshotSegmentCubic()
{
}

FSnapshotSegmentCubic::FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic *iCubicSegment
                                            , uint32 iCubicSegmentSnapshotFlags )
    : mCubicSegmentSnapshotFlags( iCubicSegmentSnapshotFlags )
    , mCubicSegment( iCubicSegment )
{
    if( mCubicSegmentSnapshotFlags & SNAPSHOT_HANDLES )
    {
        mHandleCoords[0] = iCubicSegment->GetHandle(0)->GetCoords();
        mHandleCoords[1] = iCubicSegment->GetHandle(1)->GetCoords();
    }
}

FOdysseyVectorSegmentCubic*
FSnapshotSegmentCubic::GetCubicSegment()
{
    return mCubicSegment;
}

void
FSnapshotSegmentCubic::Restore()
{
    if( mCubicSegmentSnapshotFlags & SNAPSHOT_HANDLES )
    {
        ::ULIS::FVec2D swapCoords[2] = { mCubicSegment->GetHandle(0)->GetCoords()
                                       , mCubicSegment->GetHandle(1)->GetCoords() };

        mCubicSegment->GetHandle(0)->Set( mHandleCoords[0].x, mHandleCoords[0].y );
        mCubicSegment->GetHandle(1)->Set( mHandleCoords[1].x, mHandleCoords[1].y );

        mHandleCoords[0] = swapCoords[0];
        mHandleCoords[1] = swapCoords[1];
    }
}

FSnapshotPath::~FSnapshotPath()
{
}

FSnapshotPath::FSnapshotPath( FOdysseyVectorPath* iPath
                            , uint32 iObjectSnapshotFlags
                            , uint32 iPathSnapshotFlags )
    : FSnapshotObject( iPath, iObjectSnapshotFlags )
    , mPathSnapshotFlags( iPathSnapshotFlags )
{
    if( iPathSnapshotFlags & SNAPSHOT_VERTICES )
    {
        std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

        mVertexSnapshotArray.reserve( vertexList.size() );

        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            mVertexSnapshotArray.emplace_back( vertex
                                             , FSnapshotPoint::SNAPSHOT_ALL
                                             , FSnapshotVertex::SNAPSHOT_ALL );
        }
    }

    if( iPathSnapshotFlags & SNAPSHOT_SEGMENTS )
    {
        std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();

        mCubicSegmentSnapshotArray.reserve( segmentList.size() );

        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                mCubicSegmentSnapshotArray.emplace_back( cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_ALL );
            }
        }
    }

    if( iPathSnapshotFlags & SNAPSHOT_SELECTED_VERTICES )
    {
        mSelectedVertexList = iPath->GetSelectedVertexList();
    }
}

void
FSnapshotPath::Restore()
{
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(mObject);

    FSnapshotObject::Restore();

    if( mPathSnapshotFlags & SNAPSHOT_VERTICES )
    {
        for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
        {
            mVertexSnapshotArray[i].Restore();
        }
    }

    if( mPathSnapshotFlags & SNAPSHOT_SEGMENTS )
    {
        for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
        {
            mCubicSegmentSnapshotArray[i].Restore();
        }
    }

    if( mPathSnapshotFlags & SNAPSHOT_SELECTED_VERTICES )
    {
        std::list<FOdysseyVectorVertex*> currentVertexList = path->GetSelectedVertexList();

        path->UnselectAllVertices();

        for( FOdysseyVectorVertex* vertex : mSelectedVertexList )
        {
            path->SelectVertex( vertex );
        }

        mSelectedVertexList = currentVertexList;
    }
}

FSnapshotBucket::~FSnapshotBucket()
{
}

FSnapshotBucket::FSnapshotBucket( FOdysseyVectorBucket* iBucket, uint32 iBucketSnapshotFlags )
    : mBucketSnapshotFlags( iBucketSnapshotFlags )
    , mBucket ( iBucket )
{
    if( mBucketSnapshotFlags & SNAPSHOT_POSITION )
    {
        mCoords = iBucket->GetCoords();
    }

    if( mBucketSnapshotFlags & SNAPSHOT_PARAM )
    {
        mBucketParam = iBucket->mBucketParam;
    }
}

void
FSnapshotBucket::Restore()
{
    if( mBucketSnapshotFlags & SNAPSHOT_POSITION )
    {
        ::ULIS::FVec2D swapCoords = mBucket->GetCoords();

        mBucket->Set( mCoords.x, mCoords.y );
        // swap
        mCoords = swapCoords;
    }

    if( mBucketSnapshotFlags & SNAPSHOT_PARAM )
    {
        FBucketParam swapBucketParam = mBucket->mBucketParam;

        mBucket->mBucketParam = mBucketParam;
        mBucket->Invalidate();
        // swap
        mBucketParam = swapBucketParam;
    }
}

FSnapshotGroupPaint::~FSnapshotGroupPaint()
{
}

FSnapshotGroupPaint::FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup
                                        , uint32 iObjectSnapshotFlags
                                        , uint32 iPaintGroupSnapshotFlags )
    : FSnapshotObject( iPaintGroup, iObjectSnapshotFlags )
    , mPaintGroupSnapshotFlags( iPaintGroupSnapshotFlags )
{
    if( iPaintGroupSnapshotFlags & SNAPSHOT_BUCKETS )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();

        mBucketSnapshotArray.reserve( bucketList.size() );

        for( FOdysseyVectorBucket* bucket : bucketList )
        {
            mBucketSnapshotArray.emplace_back( bucket, FSnapshotBucket::SNAPSHOT_ALL );
        }
    }

    if( iPaintGroupSnapshotFlags & SNAPSHOT_SELECTED_BUCKETS )
    {
        mSelectedBucketList = iPaintGroup->GetSelectedBucketList();
    }
}

void
FSnapshotGroupPaint::Restore()
{
    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mObject);

    FSnapshotObject::Restore();

    if( mPaintGroupSnapshotFlags & SNAPSHOT_BUCKETS )
    {
        for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
        {
            mBucketSnapshotArray[i].Restore();
        }
    }

    if( mPaintGroupSnapshotFlags & SNAPSHOT_SELECTED_BUCKETS )
    {
        std::list<FOdysseyVectorBucket*> currentBucketList = paintGroup->GetSelectedBucketList();

        paintGroup->UnselectAllBuckets();

        for( FOdysseyVectorBucket* bucket : mSelectedBucketList )
        {
            paintGroup->SelectBucket( bucket );
        }

        mSelectedBucketList = currentBucketList;
    }
}
