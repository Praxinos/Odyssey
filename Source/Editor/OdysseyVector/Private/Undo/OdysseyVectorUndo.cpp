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

FSnapshotObject::FSnapshotObject( FOdysseyVectorObject* iObject, uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mObject( iObject )
    , mPreviousChild( nullptr )
{
    if( mSnapshotFlags & FSnapshotFlags::Object::TRANSFORMATIONS )
    {
        iObject->GetTransform( mTranslationX
                             , mTranslationY
                             , mRotation
                             , mScalingX
                             , mScalingY );
    }


    if( mSnapshotFlags & FSnapshotFlags::Object::HIERARCHY )
    {
        mParent = iObject->GetParent();
        mPreviousChild = iObject->GetParent()->GetPreviousChild( iObject );
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::CHILDREN_TRANSFORMATIONS )
    {
        std::list<FOdysseyVectorObject*>& childrenObjectList = iObject->GetChildrenList();

        mChildrenSnapshotArray.reserve( childrenObjectList.size() );

        for( FOdysseyVectorObject* child : childrenObjectList )
        {
            mChildrenSnapshotArray.push_back( new FSnapshotObject( child, FSnapshotFlags::Object::TRANSFORMATIONS ) );
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::COLORING )
    {
        FOdysseyVectorBucket& foregroundBucket = iObject->GetForegroundBucket();
        FOdysseyVectorBucket& backgroundBucket = iObject->GetBackgroundBucket();

        mForegroundBucketSnapshot = FSnapshotBucket( &foregroundBucket, FSnapshotBucket::SNAPSHOT_PARAM );
        mBackgroundBucketSnapshot = FSnapshotBucket( &backgroundBucket, FSnapshotBucket::SNAPSHOT_PARAM );
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
    {
        mOpacity = iObject->GetOpacity();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::NAME )
    {
        mName = iObject->GetName();
    }
}

bool
FSnapshotObject::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Object::TRANSFORMATIONS )
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

    if( mSnapshotFlags & FSnapshotFlags::Object::HIERARCHY )
    {
        FOdysseyVectorObject* currentParent = mObject->GetParent();
        FOdysseyVectorObject* currentPreviousChild = mObject->GetParent()->GetPreviousChild(mObject);
        uint32 transferRetval = mParent->TransferChild( mObject, mPreviousChild );

        if( transferRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_SUCCESS )
        {
            // swap
            mParent = currentParent;
            mPreviousChild = currentPreviousChild;
        }

        // the object could not be transferred, normally because the previous child
        // has not been transferred yet.
        if( transferRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_ERROR )
        {
            return false;
        }

        if( transferRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_FORBIDDEN )
        {
        // nothing to do, this is nominal for the root object
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::CHILDREN_TRANSFORMATIONS )
    {
        for( int i = 0; i < mChildrenSnapshotArray.size(); i++ )
        {
            mChildrenSnapshotArray[i]->Restore();
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::COLORING )
    {
        mForegroundBucketSnapshot.Restore();
        mBackgroundBucketSnapshot.Restore();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
    {
        double currentOpacity = mObject->GetOpacity();

        mObject->SetOpacity( mOpacity );
        // swap
        mOpacity = currentOpacity;
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::NAME )
    {
        FString currentName = mObject->GetName();

        mObject->SetName( mName );
        // swap
        mName = currentName;
    }

    return true; // restore succeeded
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
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(mObject);

    // free removed vertices and segments
    if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
    {
        for( FOdysseyVectorVertex* vertex : mTopologyVertexList )
        {
            if( path->HasVertex( vertex ) == false )
            {
                delete vertex;
            }
        }

        for( FOdysseyVectorSegment* segment : mTopologySegmentList )
        {
            if( path->HasSegment( segment ) == false )
            {
                delete segment;
            }
        }
    }
}

FSnapshotPath::FSnapshotPath( FOdysseyVectorPath* iPath, uint64 iSnapshotFlags )
    : FSnapshotObject( iPath, iSnapshotFlags )
{
    // snapshot vertex and segment list
    if( iSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
    {
        mTopologyVertexList = iPath->GetVertexList();
        mTopologySegmentList = iPath->GetSegmentList();
    }

     // Snapshot vertices position & radius
    if( iSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
    {
        std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

        mVertexSnapshotArray.reserve( vertexList.size() );

        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotPoint::SNAPSHOT_ALL, FSnapshotVertex::SNAPSHOT_ALL) );
        }
    }

     // Snapshot segments' handles position
    if( iSnapshotFlags & FSnapshotFlags::Object::Path::SEGMENTS )
    {
        std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();

        mCubicSegmentSnapshotArray.reserve( segmentList.size() );

        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic(cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_ALL));
            }
        }
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::SELECTED_VERTICES )
    {
        mSelectedVertexList = iPath->GetSelectedVertexList();
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
    {
        mJointType = iPath->GetJointType();
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
    {
        mMiterLimit = iPath->GetMiterLimit();
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
    {
        mBrush = iPath->GetBrush();
    }
}

bool
FSnapshotPath::Restore()
{
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(mObject);

    if( FSnapshotObject::Restore() )
    {
        if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
        {
            std::list<FOdysseyVectorVertex*> swapTopologyVertexList = path->GetVertexList();
            std::list<FOdysseyVectorSegment*> swapTopologySegmentList = path->GetSegmentList();

            path->RemoveAllSegments();
            path->RemoveAllVertices();

            for( FOdysseyVectorVertex* vertex : mTopologyVertexList )
            {
                path->AddVertex( vertex );
            }

            for( FOdysseyVectorSegment* segment : mTopologySegmentList )
            {
                path->AddSegment( segment );
            }

            mTopologyVertexList = swapTopologyVertexList;
            mTopologySegmentList = swapTopologySegmentList;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
        {
            for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
            {
                mVertexSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::SEGMENTS )
        {
            for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
            {
                mCubicSegmentSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::SELECTED_VERTICES )
        {
            std::list<FOdysseyVectorVertex*> currentVertexList = path->GetSelectedVertexList();

            path->UnselectAllVertices();

            for( FOdysseyVectorVertex* vertex : mSelectedVertexList )
            {
                path->SelectVertex( vertex );
            }

            mSelectedVertexList = currentVertexList;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
        {
            eJointType currentJointType = path->GetJointType();

            path->SetJointType( mJointType, true );
            // swap
            mJointType = currentJointType;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
        {
            double currentMiterLimit = path->GetMiterLimit();

            path->SetMiterLimit( mMiterLimit, true );

            mMiterLimit = currentMiterLimit;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
        {
            FOdysseyVectorBrush currentBrush = path->GetBrush();

            path->SetBrush( mBrush );
            // swap
            mBrush = currentBrush;
        }

        return true;
    }

    return false;
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
                                        , uint64 iSnapshotFlags )
    : FSnapshotObject( iPaintGroup, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Object::GroupPaint::BUCKETS )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();

        mBucketSnapshotArray.reserve( bucketList.size() );

        for( FOdysseyVectorBucket* bucket : bucketList )
        {
            mBucketSnapshotArray.push_back( FSnapshotBucket( bucket, FSnapshotBucket::SNAPSHOT_ALL ));
        }
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::GroupPaint::SELECTED_BUCKETS )
    {
        mSelectedBucketList = iPaintGroup->GetSelectedBucketList();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::PAINTED )
    {
        bPainted = iPaintGroup->IsPainted();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROME )
    {
        bMonochrome = iPaintGroup->IsMonochrome();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROMECOLOR )
    {
        mMonochromeColor = iPaintGroup->GetMonochromeColor();
    }
    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::REALTIME )
    {
        bRealtime = iPaintGroup->IsRealtime();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::GAPTOLERANCE )
    {
        mGapTolerance = iPaintGroup->GetGapTolerance();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAME )
    {
        bWireframe = iPaintGroup->IsWireframe();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAMECOLOR )
    {
        mWireframeColor = iPaintGroup->GetWireframeColor();
    }
}

bool
FSnapshotGroupPaint::Restore()
{
    if( FSnapshotObject::Restore() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mObject);

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::BUCKETS )
        {
            for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
            {
                mBucketSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::SELECTED_BUCKETS )
        {
            std::list<FOdysseyVectorBucket*> currentBucketList = paintGroup->GetSelectedBucketList();

            paintGroup->UnselectAllBuckets();

            for( FOdysseyVectorBucket* bucket : mSelectedBucketList )
            {
                paintGroup->SelectBucket( bucket );
            }
            // swap
            mSelectedBucketList = currentBucketList;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::PAINTED )
        {
            bool currentPainted = paintGroup->IsPainted();

            paintGroup->SetPainted( bPainted );
            // swap
            bPainted = currentPainted;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROME )
        {
            bool currentMonochrome = paintGroup->IsMonochrome();

            paintGroup->SetMonochrome( bMonochrome );
            // swap
            bMonochrome = currentMonochrome;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROMECOLOR )
        {
            FColor currentMonochromeColor = paintGroup->GetMonochromeColor();

            paintGroup->SetMonochromeColor( mMonochromeColor );
            // swap
            mMonochromeColor = currentMonochromeColor;
        }
        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::REALTIME )
        {
            bool currentRealtime = paintGroup->IsRealtime();

            paintGroup->SetRealtime( bRealtime );
            // swap
            bRealtime = currentRealtime;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::GAPTOLERANCE )
        {
            double currentGapTolerance = paintGroup->GetGapTolerance();

            paintGroup->SetGapTolerance( mGapTolerance );
            // swap
            mGapTolerance = currentGapTolerance;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAME )
        {
            bool currentWireframe = paintGroup->IsWireframe();

            paintGroup->SetWireframe( bWireframe );
            // swap
            bWireframe = currentWireframe;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAMECOLOR )
        {
            FColor currentWireframeColor = paintGroup->GetWireframeColor();

            paintGroup->SetWireframeColor( mWireframeColor );
            // swap
            mWireframeColor = currentWireframeColor;
        }

        return true;
    }

    return false;
}
