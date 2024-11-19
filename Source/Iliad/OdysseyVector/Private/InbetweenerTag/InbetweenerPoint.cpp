// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerPoint::FInbetweenerPoint()
    : mFlags ( 0 )
{
}

void
FInbetweenerPoint::SetU( double iU )
{
    mU = iU;
}

void
FInbetweenerPoint::SetV( double iV )
{
    mV = iV;
}

void
FInbetweenerPoint::Init( FInbetweenerGrid* iGrid )
{
    mGrid = iGrid;
}

uint32
FInbetweenerPoint::GetQuadCount()
{
    FInbetweenerBreakdown* firstBreakdown = mGrid->GetBreakdown()->GetInbetweenerTag()->GetBreakdownList().front();

    if( firstBreakdown != mGrid->GetBreakdown() )
    {
        uint32 pointIndex = this - &mGrid->GetPointBuffer()[0];

        return firstBreakdown->GetGrid()->GetPointBuffer()[pointIndex].GetQuadCount();
    }

    return mQuadList.size();
}

void
FInbetweenerPoint::SetDeformable( bool iIsDeformable )
{
    if( iIsDeformable == true )
    {
        mFlags |= DEFORMABLE;
    }
    else
    {
        mFlags &= (~DEFORMABLE);
    }
}

bool
FInbetweenerPoint::IsDeformable()
{
    return ( mFlags & DEFORMABLE ) ? true : false;
}

void
FInbetweenerPoint::SetNeeded( bool iIsNeeded )
{
    if( iIsNeeded == true )
    {
        mFlags |= NEEDED;
    }
    else
    {
        mFlags &= (~NEEDED);
    }
}

bool
FInbetweenerPoint::IsNeeded()
{
    return ( mFlags & NEEDED ) ? true : false;
}

void
FInbetweenerPoint::SetSourcePosition( double iX, double iY, bool iInvalidate )
{
    FInbetweenerBreakdown* prevBreakdown = mGrid->GetBreakdown()->GetPrevBreakdown();
    uint64 invalidationFlags = FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEGRID
                             | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS
                             | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING;
    uint32 pointIndex = this - &mGrid->GetPointBuffer()[0];

    mSourcePosition.x = iX;
    mSourcePosition.y = iY;

// commented-out: target impacts source but source does not impact target. this makes file loading much easier.
// We don't manipulate the source grid anyways.
/*
    if( prevBreakdown )
    {
        prevBreakdown->GetGrid()->GetPointBuffer()[pointIndex].mTargetPosition.x = mSourcePosition.x;
        prevBreakdown->GetGrid()->GetPointBuffer()[pointIndex].mTargetPosition.y = mSourcePosition.y;

        invalidationFlags |= FOdysseyVectorTagInbetweener::INVALIDATE_TARGETGRID;
    }
*/
    if( iInvalidate )
        mGrid->GetBreakdown()->GetInbetweenerTag()->Invalidate( invalidationFlags );
}

void
FInbetweenerPoint::SetTargetPosition( double iX, double iY, bool iInvalidate )
{
    //FInbetweenerBreakdown* nextBreakdown = mGrid->GetBreakdown()->GetNextBreakdown();
    uint64 invalidationFlags = FOdysseyVectorTagInbetweener::INVALIDATE_TARGETGRID
                             | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS
                             | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING;
    uint32 pointIndex = this - &mGrid->GetPointBuffer()[0];

    mTargetPosition.x = iX;
    mTargetPosition.y = iY;
/*
    if( nextBreakdown )
    {
        nextBreakdown->GetGrid()->GetPointBuffer()[pointIndex].mSourcePosition.x = mTargetPosition.x;
        nextBreakdown->GetGrid()->GetPointBuffer()[pointIndex].mSourcePosition.y = mTargetPosition.y;

        invalidationFlags |= FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEGRID;
    }
*/
    if( iInvalidate )
        mGrid->GetBreakdown()->GetInbetweenerTag()->Invalidate( invalidationFlags );
}

void
FInbetweenerPoint::SetInterpPosition( double iX, double iY )
{
    mInterpPosition.x = iX;
    mInterpPosition.y = iY;
}

void
FInbetweenerPoint::SetDeformPosition( double iX, double iY )
{
    mDeformPosition.x = iX;
    mDeformPosition.y = iY;
}

::ULIS::FVec2D
FInbetweenerPoint::GetSourcePosition()
{
    return GetPosition( eInbetweenerPointPositionType::SourcePosition );
}

::ULIS::FVec2D
FInbetweenerPoint::GetTargetPosition()
{
    return GetPosition( eInbetweenerPointPositionType::TargetPosition );
}

FInbetweenerGrid*
FInbetweenerPoint::GetGrid()
{
    return mGrid;
}

void
FInbetweenerPoint::AddQuad( FInbetweenerQuad* iQuad )
{
    mQuadList.push_back( iQuad );
}

void
FInbetweenerPoint::RemoveQuad( FInbetweenerQuad* iQuad )
{
    mQuadList.remove( iQuad );
}

std::list<FInbetweenerQuad*>&
FInbetweenerPoint::GetQuadList()
{
    return mQuadList;
}

void
FInbetweenerPoint::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FInbetweenerPoint::GetID()
{
    FInbetweenerBreakdown* firstBreakdown = mGrid->GetBreakdown()->GetInbetweenerTag()->GetBreakdownList().front();

    if( firstBreakdown != mGrid->GetBreakdown() )
    {
        uint32 pointIndex = this - &mGrid->GetPointBuffer()[0];

        return firstBreakdown->GetGrid()->GetPointBuffer()[pointIndex].GetID();
    }

    return mID;
}

double
FInbetweenerPoint::GetU()
{
    return mU;
}

double
FInbetweenerPoint::GetV()
{
    return mV;
}

::ULIS::FVec2D
FInbetweenerPoint::GetPosition( eInbetweenerPointPositionType iPositionType )
{
    switch( iPositionType )
    {
        case eInbetweenerPointPositionType::SourcePosition :
        {
            FInbetweenerBreakdown* prevBreakdown = mGrid->GetBreakdown()->GetPrevBreakdown();

            if( prevBreakdown )
            {
                uint32 pointIndex = this - &mGrid->GetPointBuffer()[0];

                return prevBreakdown->GetGrid()->GetPointBuffer()[pointIndex].GetPosition( eInbetweenerPointPositionType::TargetPosition );
            }
        }
        return mSourcePosition;

        case eInbetweenerPointPositionType::InterpPosition :
        return mInterpPosition;

        case eInbetweenerPointPositionType::DeformPosition :
        return mDeformPosition;

        case eInbetweenerPointPositionType::TargetPosition :
        return mTargetPosition;

        default:
        break;
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FInbetweenerPoint::SetPosition( eInbetweenerPointPositionType iPositionType
                              , double iX
                              , double iY
                              , bool iInvalidate )
{
    switch( iPositionType )
    {
        case eInbetweenerPointPositionType::SourcePosition :
            SetSourcePosition( iX, iY, iInvalidate );
        break;

        case eInbetweenerPointPositionType::InterpPosition :
            mInterpPosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::DeformPosition :
            mDeformPosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::TargetPosition :
            SetTargetPosition( iX, iY, iInvalidate );
        break;

        default:
        break;
    }
}
