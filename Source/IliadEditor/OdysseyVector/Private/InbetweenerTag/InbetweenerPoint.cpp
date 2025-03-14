// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerPoint::FInbetweenerPoint( FInbetweenerGrid* iGrid
                                    , double iSourcePositionX
                                    , double iSourcePositionY
                                    , double iU
                                    , double iV )
    : mFlags ( 0 )
    , mU ( iU )
    , mV ( iV )
{
    mGrid = iGrid;

    // Note: this will be ignored for breakdowns that are not the first breakdown
    mSourcePosition.x = iSourcePositionX;
    mSourcePosition.y = iSourcePositionY;

    mGrid->Invalidate( FInbetweenerGrid::INVALIDATE_SOURCE );
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

// this is only called for the leading breakdown
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

// this is only relevant for the leading breakdown
bool
FInbetweenerPoint::IsNeeded()
{
    return ( mFlags & NEEDED ) ? true : false;
}

void
FInbetweenerPoint::SetTargetPosition( double iX, double iY, bool iInvalidate )
{
    FInbetweenerBreakdown* nextBreakdown = mGrid->GetBreakdown()->GetNextBreakdown();
    uint64 invalidationFlags = FOdysseyVectorTagInbetweener::INVALIDATE_CELLS
                             | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING;
    uint32 pointIndex = GetIndex();

    mTargetPosition.x = iX;
    mTargetPosition.y = iY;

    // this invalidation is not hierarchical.
    mGrid->Invalidate( FInbetweenerGrid::INVALIDATE_TARGET );

    if( nextBreakdown )
    {
    // this invalidation is not hierarchical.
        nextBreakdown->GetGrid()->Invalidate( FInbetweenerGrid::INVALIDATE_SOURCE );
    }

    // this might be called in the Update function, that's why there are cases when we don't want to invalidate.
    // Indeed, it is not wise to call invalidate() during the update.
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

uint32
FInbetweenerPoint::GetIndex()
{
    return this - &mGrid->GetPointBuffer()[0];
}

void
FInbetweenerPoint::SetID( uint32 iID )
{
    FInbetweenerBreakdown* firstBreakdown = mGrid->GetBreakdown()->GetInbetweenerTag()->GetBreakdownList().front();

    if( firstBreakdown != mGrid->GetBreakdown() )
    {
        uint32 pointIndex = GetIndex();

        firstBreakdown->GetGrid()->GetPointBuffer()[pointIndex].SetID( iID );

        return;
    }

    mID = iID;
}

uint32
FInbetweenerPoint::GetID()
{
    FInbetweenerBreakdown* firstBreakdown = mGrid->GetBreakdown()->GetInbetweenerTag()->GetBreakdownList().front();

    if( firstBreakdown != mGrid->GetBreakdown() )
    {
        uint32 pointIndex = GetIndex();

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
                uint32 pointIndex = GetIndex();

                return prevBreakdown->GetGrid()->GetPointBuffer()[pointIndex].GetPosition( eInbetweenerPointPositionType::TargetPosition );
            }
        }
        return mSourcePosition;

        case eInbetweenerPointPositionType::InterpPosition :
        return mInterpPosition;

        case eInbetweenerPointPositionType::DeformPosition :
        return mDeformPosition;

        case eInbetweenerPointPositionType::TargetPosition :
        {
            if( GetQuadCount() == 0 )
            {
                FInbetweenerBreakdown* firstBreakdown = mGrid->GetBreakdown()->GetInbetweenerTag()->GetBreakdownList().front();
                uint32 pointIndex = GetIndex();

                return firstBreakdown->GetGrid()->GetPointBuffer()[pointIndex].mSourcePosition;
            }
        }
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
/* commented-out : set in constructor
        case eInbetweenerPointPositionType::SourcePosition :
            SetSourcePosition( iX, iY, iInvalidate );
        break;
*/
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
