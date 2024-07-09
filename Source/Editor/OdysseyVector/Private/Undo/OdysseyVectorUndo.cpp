#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorTagInbetweener.h"
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


FSnapshotTrajectory::~FSnapshotTrajectory()
{
}

FSnapshotTrajectory::FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory )
    : mTrajectory ( iTrajectory )
    , mHandleDirection { iTrajectory->GetHandle(0)->GetDirection()
                       , iTrajectory->GetHandle(1)->GetDirection() }
    , mHandleLengthRatio { iTrajectory->GetHandle(0)->GetLengthRatio()
                         , iTrajectory->GetHandle(1)->GetLengthRatio() }
{
}

FInbetweenerTrajectory*
FSnapshotTrajectory::GetTrajectory()
{
    return mTrajectory;
}

void
FSnapshotTrajectory::Restore()
{
    ::ULIS::FVec2D swapHandleDirection[2] = { mTrajectory->GetHandle(0)->GetDirection()
                                            , mTrajectory->GetHandle(1)->GetDirection() };
    double swapHandleLengthRatio[2] =  { mTrajectory->GetHandle(0)->GetLengthRatio()
                                       , mTrajectory->GetHandle(1)->GetLengthRatio() };

    mTrajectory->GetHandle(0)->Set( mHandleDirection[0], mHandleLengthRatio[0] );
    mTrajectory->GetHandle(1)->Set( mHandleDirection[1], mHandleLengthRatio[1] );

    mHandleDirection[0] = swapHandleDirection[0];
    mHandleDirection[1] = swapHandleDirection[1];

    mHandleLengthRatio[0] = swapHandleLengthRatio[0];
    mHandleLengthRatio[1] = swapHandleLengthRatio[1];
}

FSnapshotTagInbetweener::~FSnapshotTagInbetweener()
{
}

FSnapshotTagInbetweener::FSnapshotTagInbetweener( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                , uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mInbetweenerTag( iInbetweenerTag )
{
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
    {
        SaveTrajectories( mTrajectoryArray );
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::TRANSFORMATIONS )
    {
        mInbetweenerTag->GetTargetTransform( mTranslationX
                                           , mTranslationY
                                           , mRotation
                                           , mScalingX
                                           , mScalingY );
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INBETWEENCOUNT )
    {
        mInbetweenCount = mInbetweenerTag->GetInbetweenCount();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
    {
        bMapAsPolyline = mInbetweenerTag->GetMapAsPolyline();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CHART )
    {
        mChart = mInbetweenerTag->GetChart();
    }

    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
     || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
    {
        mGridSizeX = mInbetweenerTag->GetGridNumQuadX();
        mGridSizeY = mInbetweenerTag->GetGridNumQuadY();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
    {
        mGridType = mInbetweenerTag->GetGridType();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ARAPRIGIDITY )
    {
        FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(mInbetweenerTag->GetGrid());

        mARAPRigidity = arapGrid->GetRigidity();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
    {
        mInterpolationType = mInbetweenerTag->GetInterpolationType();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
    {
        mColor = mInbetweenerTag->GetColor();
    }

    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE     )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE     )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDGEOMETRY ) )
    {
        mInbetweenerTag->GetGrid()->GetGeometry( mGridGeometry
                                               , eInbetweenerPointPositionType::TargetPosition );
    }
}

bool
FSnapshotTagInbetweener::Restore()
{
    std::vector<FInbetweenerTrajectory*> swapTrajectoryArray;
    std::vector<::ULIS::FVec2D> swapGridGeometry;
    FInbetweenerChart swapChart;

    // pre-step. Backup grid geometry before being changed
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
    {
        SaveTrajectories( swapTrajectoryArray );
    }

    // pre-step. Backup grid geometry before being changed
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE     )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE     )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDGEOMETRY ) )
    {
        mInbetweenerTag->GetGrid()->GetGeometry( swapGridGeometry
                                               , eInbetweenerPointPositionType::TargetPosition );
    }

    // pre-step. Backup Chart before being changed by SetInbetweenCount
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INBETWEENCOUNT )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CHART          ) )
    {
        swapChart = mInbetweenerTag->GetChart();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::TRANSFORMATIONS )
    {
        double swapTranslationX, swapTranslationY, swapRotation, swapScalingX, swapScalingY;

        mInbetweenerTag->GetTargetTransform( swapTranslationX
                                           , swapTranslationY
                                           , swapRotation
                                           , swapScalingX
                                           , swapScalingY );

        mInbetweenerTag->SetTargetTransform( mTranslationX
                                           , mTranslationY
                                           , mRotation
                                           , mScalingX
                                           , mScalingY );

        mInbetweenerTag->UpdateMatrix();

        mTranslationX = swapTranslationX;
        mTranslationY = swapTranslationY;
        mRotation     = swapRotation;
        mScalingX     = swapScalingX;
        mScalingY     = swapScalingY;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INBETWEENCOUNT )
    {
        uint32 swapCount = mInbetweenerTag->GetInbetweenCount();

        mInbetweenerTag->SetInbetweenCount( mInbetweenCount );

        mInbetweenCount = swapCount;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
    {
        bool swapMapAsPolyline = mInbetweenerTag->GetMapAsPolyline();

        mInbetweenerTag->SetMapAsPolyline( bMapAsPolyline );

        bMapAsPolyline = swapMapAsPolyline;
    }

    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
     || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
    {
        uint32 swaGridSizeX = mInbetweenerTag->GetGridNumQuadX();
        uint32 swaGridSizeY = mInbetweenerTag->GetGridNumQuadY();

        mInbetweenerTag->SetGridNumQuad( mGridSizeX, mGridSizeY );

        mGridSizeX = swaGridSizeX;
        mGridSizeY = swaGridSizeY;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
    {
        eInbetweenerGridType swapGridType = mInbetweenerTag->GetGridType();

        mInbetweenerTag->SetGrid( mGridType, mGridSizeX, mGridSizeY );

        mGridType = swapGridType;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
    {
        eInbetweenerInterpolationType swapInterpolationType = mInbetweenerTag->GetInterpolationType();

        mInbetweenerTag->SetInterpolationType( mInterpolationType );

        mInterpolationType = swapInterpolationType;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ARAPRIGIDITY )
    {
        FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(mInbetweenerTag->GetGrid());
        uint32 swapARAPRigidity = arapGrid->GetRigidity();

        arapGrid->SetRigidity( mARAPRigidity );

        mARAPRigidity = swapARAPRigidity;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
    {
        const FColor swapColor = mInbetweenerTag->GetColor();

        mInbetweenerTag->SetColor( mColor );

        mColor = swapColor;
    }

    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
    {
        RestoreTrajectories();

        mTrajectoryArray = swapTrajectoryArray;
    }

    // restore grid geometry after params have been set
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE     )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE     )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDGEOMETRY ) )
    {
        mInbetweenerTag->GetGrid()->SetGeometry( mGridGeometry
                                               , eInbetweenerPointPositionType::TargetPosition );

        mGridGeometry = swapGridGeometry;
    }

    // restore chart after params have been set
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INBETWEENCOUNT )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CHART          ) )
    {
        mInbetweenerTag->GetChart() = mChart;

        mChart = swapChart;
    }

    return true; // restore succeeded
}

void
FSnapshotTagInbetweener::SaveTrajectories( std::vector<FInbetweenerTrajectory*>& oTrajectoryArray )
{
    std::list<FInbetweenerTrajectory*>& trajectoryList = mInbetweenerTag->GetTrajectoryList();

    oTrajectoryArray.clear();
    oTrajectoryArray.reserve( trajectoryList.size() );

    for( FInbetweenerTrajectory* trajectory : trajectoryList )
    {
        oTrajectoryArray.push_back( trajectory );
    }
}

void
FSnapshotTagInbetweener::RestoreTrajectories()
{
    mInbetweenerTag->RemoveAllTrajectories();

    for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
    {
        mInbetweenerTag->AddTrajectory( trajectory );
    }
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
    , mForegroundBucketSnapshot( &iObject->GetForegroundBucket(), 0 )
    , mBackgroundBucketSnapshot( &iObject->GetBackgroundBucket(), 0 )
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

        mForegroundBucketSnapshot = FSnapshotBucket( &foregroundBucket, FSnapshotFlags::Point::Bucket::PARAM );
        mBackgroundBucketSnapshot = FSnapshotBucket( &backgroundBucket, FSnapshotFlags::Point::Bucket::PARAM );
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

FSnapshotPoint::FSnapshotPoint( FOdysseyVectorPoint* iPoint, uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mPoint( iPoint )
{
    if( iSnapshotFlags & FSnapshotFlags::Point::POSITION )
    {
        mCoords = iPoint->GetCoords();
    }
}

void
FSnapshotPoint::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Point::POSITION )
    {
        ::ULIS::FVec2D swapCoords = mPoint->GetCoords();

        mPoint->Set( mCoords.x, mCoords.y );

        mCoords = swapCoords;
    }
}

FSnapshotVertex::~FSnapshotVertex()
{
}

FSnapshotVertex::FSnapshotVertex( FOdysseyVectorVertex* iVertex, uint64 iSnapshotFlags )
    : FSnapshotPoint( iVertex, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Point::Vertex::ALIGNMENT )
    {
        mAlignment = iVertex->IsHandleAligned();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
    {
        mLocked = iVertex->IsLocked();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
    {
        mRadius = iVertex->GetRadius();
    }
}

FOdysseyVectorVertex*
FSnapshotVertex::GetVertex()
{
    return static_cast<FOdysseyVectorVertex*>(mPoint);
}

void
FSnapshotVertex::Restore()
{
    FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(mPoint);

    FSnapshotPoint::Restore();

    if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::ALIGNMENT )
    {
        bool alignment = vertex->IsHandleAligned();

        vertex->SetHandleAligned( mAlignment );

        // swap for redo
        mAlignment = alignment;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
    {
        bool locked = vertex->IsLocked();

        vertex->SetLocked( mLocked );

        // swap for redo
        mLocked = locked;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
    {
        double swapRadius = vertex->GetRadius();

        vertex->SetRadius( mRadius );

        mRadius = swapRadius;
    }
}

FSnapshotBucket::~FSnapshotBucket()
{
}

FSnapshotBucket::FSnapshotBucket( FOdysseyVectorBucket* iBucket, uint64 iSnapshotFlags )
    : FSnapshotPoint( iBucket, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::COLORMODE )
    {
        mColorMode = iBucket->GetColorMode();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::SPREADINGPOLICY )
    {
        mSpreadingPolicy = iBucket->GetSpreadingPolicy();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
    {
        mSolidColor = iBucket->GetSolidColor();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::ROTATION )
    {
        mRotation = iBucket->GetRotation();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
    {
        mPropagated = iBucket->IsPropagated();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
    {
        mGradientColor0 = iBucket->GetGradientColor0();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
    {
        mGradientColor1 = iBucket->GetGradientColor1();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
    {
        mRadialRadius = iBucket->GetRadialRadius();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
    {
        mRadialOffset = iBucket->GetRadialOffset();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
    {
        mPaletteEntry = iBucket->GetPaletteEntry();
    }
}

FOdysseyVectorBucket*
FSnapshotBucket::GetBucket()
{
    return static_cast<FOdysseyVectorBucket*>(mPoint);
}

void
FSnapshotBucket::Restore()
{
    FOdysseyVectorBucket* bucket = static_cast<FOdysseyVectorBucket*>(mPoint);

    FSnapshotPoint::Restore();

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::COLORMODE )
    {
        eBucketColorMode currentColorMode = bucket->GetColorMode();

        bucket->SetColorMode( mColorMode );
        // swap
        mColorMode = currentColorMode;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SPREADINGPOLICY )
    {
        eBucketSpreadingPolicy currentSpreadingPolicy = bucket->GetSpreadingPolicy();

        bucket->SetSpreadingPolicy( mSpreadingPolicy );
        // swap
        mSpreadingPolicy = currentSpreadingPolicy;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
    {
        FColor currentSolidColor = bucket->GetSolidColor();

        bucket->SetSolidColor( mSolidColor );

        mSolidColor = currentSolidColor;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::ROTATION )
    {
        double currentRotation = bucket->GetRotation();

        bucket->SetRotation( mRotation );
        // swap
        mRotation = currentRotation;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
    {
        bool currentPropagated = bucket->IsPropagated();

        bucket->SetPropagated( mPropagated );
        // swap
        mPropagated = currentPropagated;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
    {
        FColor currrentGradientColor0 = bucket->GetGradientColor0();

        bucket->SetGradientColor0( mGradientColor0 );
        // swap
        mGradientColor0 = currrentGradientColor0;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
    {
        FColor currrentGradientColor1 = bucket->GetGradientColor1();

        bucket->SetGradientColor1( mGradientColor1 );
        // swap
        mGradientColor1 = currrentGradientColor1;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
    {
        double currentRadialRadius = bucket->GetRadialRadius();

        bucket->SetRadialRadius( mRadialRadius );
        // swap
        mRadialRadius = currentRadialRadius;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
    {
        ::ULIS::FVec2D currentRadialOffset = bucket->GetRadialOffset();

        bucket->SetRadialOffset( mRadialOffset );
        // swap
        mRadialOffset = currentRadialOffset;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
    {
        UOdysseyPaletteEntry* currentPaletteEntry = bucket->GetPaletteEntry();

        bucket->SetPaletteEntry( mPaletteEntry );
        // swap
        mPaletteEntry = currentPaletteEntry;
    }
}

FSnapshotSegmentCubic::~FSnapshotSegmentCubic()
{
}

FSnapshotSegmentCubic::FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic *iCubicSegment
                                            , uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mCubicSegment( iCubicSegment )
{
    if( iSnapshotFlags & FSnapshotFlags::Segment::Cubic::HANDLES )
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
    if( mSnapshotFlags & FSnapshotFlags::Segment::Cubic::HANDLES )
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
            mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotFlags::ALL ) );
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

                mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotFlags::ALL ) );
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
            mBucketSnapshotArray.push_back( FSnapshotBucket( bucket, FSnapshotFlags::ALL ) );
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
