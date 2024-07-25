#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

FInbetweenerBreakdown::~FInbetweenerBreakdown()
{
    delete mGrid;
}

FInbetweenerBreakdown::FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                            , int32 iSourceInbetweenIndex
                                            , int32 iTargetInbetweenIndex )
    : mInbetweenerTag( iInbetweenerTag )
    , mSourceInbetweenIndex( iSourceInbetweenIndex )
    , mTargetInbetweenIndex( iTargetInbetweenIndex )
    , mIndex( 0 )
    , mGrid( nullptr )
{
    SetGrid( iInbetweenerTag->GetGridType() );
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

BLMatrix2D&
FInbetweenerBreakdown::GetTargetLocalMatrix()
{
    uint32 inbetweenCount = mTargetInbetweenIndex - mSourceInbetweenIndex - 1;

    if( mInbetweenerTag->GetInbetweenCount() == inbetweenCount )
    {
        return mInbetweenerTag->GetTargetLocalMatrix();
    }

    return mInbetweenerTag->GetChart().inbetweenBuffer[mTargetInbetweenIndex].matrix;
}

void
FInbetweenerBreakdown::SetSourceInbetweenIndex( int32 iSourceInbetweenIndex )
{
    mSourceInbetweenIndex = iSourceInbetweenIndex;
}

void
FInbetweenerBreakdown::SetTargetInbetweenIndex( int32 iTargetInbetweenIndex )
{
    mTargetInbetweenIndex = iTargetInbetweenIndex;
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
