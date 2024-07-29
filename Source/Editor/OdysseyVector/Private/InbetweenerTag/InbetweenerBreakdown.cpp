#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

FInbetweenerBreakdown::~FInbetweenerBreakdown()
{
    delete mGrid;
}

FInbetweenerBreakdown::FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                            , FInbetweenerBreakdown* iMasterBreakdown
                                            , int32 iSourceInbetweenIndex
                                            , int32 iTargetInbetweenIndex )
    : mInbetweenerTag( iInbetweenerTag )
    , mSourceInbetweenIndex( iSourceInbetweenIndex )
    , mTargetInbetweenIndex( iTargetInbetweenIndex )
    , mIndex( 0 )
    , mGrid( nullptr )
    , mMasterBreakdown( iMasterBreakdown )
    , mPrevBreakdown( nullptr )
    , mNextBreakdown( nullptr )
{
    SetGrid( iInbetweenerTag->GetGridType() );
}

FInbetweenerBreakdown*
FInbetweenerBreakdown::GetMasterBreakdown()
{
    return mMasterBreakdown;
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
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.GetInterpolatedPointPositionBuffer()[pointCount * mTargetInbetweenIndex];

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

    if( mSourceInbetweenIndex == -1 )
    {
        return identityMatrix;
    }

    return mInbetweenerTag->GetChart().inbetweenBuffer[mSourceInbetweenIndex].matrix;
}

BLMatrix2D&
FInbetweenerBreakdown::GetTargetLocalMatrix()
{
    if( mTargetInbetweenIndex == mInbetweenerTag->GetInbetweenCount() )
    {
        return mInbetweenerTag->GetTargetLocalMatrix();
    }

    return mInbetweenerTag->GetChart().inbetweenBuffer[mTargetInbetweenIndex].matrix;
}

void
FInbetweenerBreakdown::SetSourceInbetweenIndex( int32 iSourceInbetweenIndex )
{
    mSourceInbetweenIndex = iSourceInbetweenIndex;

    if( mPrevBreakdown )
    {
        mPrevBreakdown->mTargetInbetweenIndex = iSourceInbetweenIndex;
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING 
                               // force deformation of interpolated paths at target
                               | FOdysseyVectorTagInbetweener::INVALIDATE_TARGET  );
}

void
FInbetweenerBreakdown::SetTargetInbetweenIndex( int32 iTargetInbetweenIndex )
{
    mTargetInbetweenIndex = iTargetInbetweenIndex;

    if( mNextBreakdown )
    {
        mNextBreakdown->mSourceInbetweenIndex = iTargetInbetweenIndex;
    }
    else
    {
        mInbetweenerTag->SetInbetweenCount( iTargetInbetweenIndex );
    }

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING 
                               // force deformation of interpolated paths at target
                               | FOdysseyVectorTagInbetweener::INVALIDATE_TARGET );
}

int32
FInbetweenerBreakdown::GetSourceInbetweenIndex()
{
    return mSourceInbetweenIndex;
}

int32
FInbetweenerBreakdown::GetTargetInbetweenIndex()
{
    return mTargetInbetweenIndex;
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
