#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorScene.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorUndo::~FOdysseyVectorUndo()
{
}

FOdysseyVectorUndo::FOdysseyVectorUndo( FOdysseyVectorScene *iScene )
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
    : mObject( iObject )
    , mObjectSnapshotFlags( iObjectSnapshotFlags )
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

FSnapshotVertex::~FSnapshotVertex()
{
}

FSnapshotVertex::FSnapshotVertex( FOdysseyVectorVertex* iVertex
                                , uint32 iVertexSnapshotFlags )
    : mVertexSnapshotFlags( iVertexSnapshotFlags )
    , mVertex( iVertex )
    , mCoords( iVertex->GetCoords() )
    , mRadius( iVertex->GetRadius() )
{
}

void
FSnapshotVertex::Restore()
{
    if( mVertexSnapshotFlags & SNAPSHOT_POSITION )
    {
        ::ULIS::FVec2D swapCoords = mVertex->GetCoords();

        mVertex->Set( mCoords.x, mCoords.y );

        mCoords = swapCoords;
    }

    if( mVertexSnapshotFlags & SNAPSHOT_RADIUS )
    {
        double swapRadius = mVertex->GetRadius();

        mVertex->SetRadius( mRadius );

        mRadius = swapRadius;
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
    mHandleCoords[0] = iCubicSegment->GetHandle(0)->GetCoords();
    mHandleCoords[1] = iCubicSegment->GetHandle(1)->GetCoords();
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
            mVertexSnapshotArray.emplace_back( vertex, FSnapshotVertex::SNAPSHOT_ALL );
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
}

void
FSnapshotPath::Restore()
{
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
}

void
FSnapshotGroupPaint::Restore()
{
    FSnapshotObject::Restore();

    if( mPaintGroupSnapshotFlags & SNAPSHOT_BUCKETS )
    {
        for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
        {
            mBucketSnapshotArray[i].Restore();
        }
    }
}
