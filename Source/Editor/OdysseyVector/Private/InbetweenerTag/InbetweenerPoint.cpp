#include "InbetweenerTag/InbetweenerPoint.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerPoint::FInbetweenerPoint()
{
}

void
FInbetweenerPoint::Init( FInbetweenerGrid* iGrid )
{
    mGrid = iGrid;
}

void
FInbetweenerPoint::SetSourcePosition( double iX, double iY )
{
    mSourcePosition.x = iX;
    mSourcePosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_BBOX );
}

void
FInbetweenerPoint::SetTargetPosition( double iX, double iY )
{
    mTargetPosition.x = iX;
    mTargetPosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_BBOX );
}

void
FInbetweenerPoint::SetMotionPosition( double iX, double iY )
{
    mMotionPosition.x = iX;
    mMotionPosition.y = iY;
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

::ULIS::FVec2D
FInbetweenerPoint::GetPosition( eInbetweenerPointPositionType iPositionType )
{
    switch( iPositionType )
    {
        case eInbetweenerPointPositionType::SourcePosition : 
        return mSourcePosition;

        case eInbetweenerPointPositionType::MotionPosition : 
        return mMotionPosition;

        case eInbetweenerPointPositionType::TargetPosition : 
        return mTargetPosition;

        default:
        break;
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}
