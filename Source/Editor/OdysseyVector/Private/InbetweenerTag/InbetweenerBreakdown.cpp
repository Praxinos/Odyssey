#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

FInbetweenerBreakdown::~FInbetweenerBreakdown()
{
    delete mGrid;
}

FInbetweenerBreakdown::FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag( iInbetweenerTag )
    , mSourceDrawingIndex( 0 )
    , mTargetDrawingIndex( 1 )
    , mIndex( 0 )
    , mGrid( nullptr )
    , mPrevBreakdown( nullptr )
    , mNextBreakdown( nullptr )
{
    SetGrid( iInbetweenerTag->GetGridType() );
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetMasterBreakdown()
{
    return mInbetweenerTag->GetMasterBreakdown();
}

void
FInbetweenerBreakdown::SetPrevBreakdown( FInbetweenerBreakdown* iPrevBreakdown )
{
    mPrevBreakdown = iPrevBreakdown;
}

void
FInbetweenerBreakdown::SetNextBreakdown( FInbetweenerBreakdown* iNextBreakdown )
{
    mNextBreakdown = iNextBreakdown;
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetPrevBreakdown()
{
    return mPrevBreakdown;
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetNextBreakdown()
{
    return mNextBreakdown;
}

FOdysseyVectorTagInbetweener*
FInbetweenerBreakdown::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

void
FInbetweenerBreakdown::SetIndex( uint32 iIndex )
{
    mIndex = iIndex;
}

uint32
FInbetweenerBreakdown::GetIndex()
{
    return mIndex;
}

void
FInbetweenerBreakdown::DrawPathsAtTarget( BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mInbetweenerTag->GetOwner()->GetWorldMatrix();

    iBLContext->save();
    iBLContext->resetMatrix();

    worldMatrix.transform( GetTargetLocalMatrix() );

    for( FInterpolatedPath& interpolatedPath : mInbetweenerTag->GetInterpolatedPathBuffer() )
    {
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.GetInterpolatedPointPositionBuffer()[pointCount * mTargetDrawingIndex];

        mInbetweenerTag->DrawPathAt( &interpolatedPath
                                   , pointPositionBuffer
                                   , worldMatrix
                                   , iBLContext );
    }

    iBLContext->restore();
}

BLMatrix2D&
FInbetweenerBreakdown::GetSourceLocalMatrix()
{
    static BLMatrix2D identityMatrix = BLMatrix2D::makeIdentity();

    if( mSourceDrawingIndex == 0 )
    {
        return identityMatrix;
    }

    return mInbetweenerTag->GetChart().drawingBuffer[mSourceDrawingIndex].matrix;
}

BLMatrix2D&
FInbetweenerBreakdown::GetTargetLocalMatrix()
{
    if( mTargetDrawingIndex == mInbetweenerTag->GetDrawingCount() )
    {
        return mInbetweenerTag->GetTargetLocalMatrix();
    }

    return mInbetweenerTag->GetChart().drawingBuffer[mTargetDrawingIndex].matrix;
}

void
FInbetweenerBreakdown::SetSourceDrawingIndex( uint32 iSourceDrawingIndex )
{
    mSourceDrawingIndex = iSourceDrawingIndex;

    if( mPrevBreakdown )
    {
        mPrevBreakdown->mTargetDrawingIndex = iSourceDrawingIndex;
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING 
                               | FOdysseyVectorTagInbetweener::INVALIDATE_BREAKDOWN_LIST
                               // force deformation of interpolated paths at target
                               | FOdysseyVectorTagInbetweener::INVALIDATE_TARGET  );
}

void
FInbetweenerBreakdown::SetTargetDrawingIndex( uint32 iTargetDrawingIndex )
{
    mTargetDrawingIndex = iTargetDrawingIndex;

    if( mNextBreakdown )
    {
        mNextBreakdown->mSourceDrawingIndex = iTargetDrawingIndex;
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING 
                               | FOdysseyVectorTagInbetweener::INVALIDATE_BREAKDOWN_LIST
                               // force deformation of interpolated paths at target
                               | FOdysseyVectorTagInbetweener::INVALIDATE_TARGET );
}

uint32
FInbetweenerBreakdown::GetSourceDrawingIndex()
{
    return mSourceDrawingIndex;
}

uint32
FInbetweenerBreakdown::GetTargetDrawingIndex()
{
    return mTargetDrawingIndex;
}

void
FInbetweenerBreakdown::SetGrid( eInbetweenerGridType iGridType )
{
    if( mGrid )
    {
        delete mGrid;

        mGrid = nullptr;
    }

    switch( iGridType )
    {
        case eInbetweenerGridType::ARAP :
            mGrid = new FInbetweenerGridARAP( this );
        break;

        default:
            mGrid = new FInbetweenerGridFFD( this );
        break;
    }

    mGrid->Make();
}

FInbetweenerGrid*
FInbetweenerBreakdown::GetGrid()
{
    return mGrid;
}
