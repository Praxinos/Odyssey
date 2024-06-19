#include "InbetweenerTag/InbetweenerPoint.h"
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
FInbetweenerPoint::SetSourcePosition( double iX, double iY )
{
    mSourcePosition.x = iX;
    mSourcePosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEBBOX );
}

void
FInbetweenerPoint::SetTargetPosition( double iX, double iY )
{
    mTargetPosition.x = iX;
    mTargetPosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_TARGETBBOX
                                          | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
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

const ::ULIS::FVec2D&
FInbetweenerPoint::GetSourcePosition()
{
    return mSourcePosition;
}

const ::ULIS::FVec2D&
FInbetweenerPoint::GetTargetPosition()
{
    return mTargetPosition;
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
                              , double iY )
{
    switch( iPositionType )
    {
        case eInbetweenerPointPositionType::SourcePosition : 
            SetSourcePosition( iX, iY );
        break;

        case eInbetweenerPointPositionType::InterpPosition : 
            mInterpPosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::DeformPosition : 
            mDeformPosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::TargetPosition : 
            SetTargetPosition( iX, iY );
        break;

        default:
        break;
    }
}