#include "InbetweenerTag/InbetweenerPoint.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerPoint::FInbetweenerPoint()
    : bIsDeformable ( true ) // what is this ?
{
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
    bIsDeformable = iIsDeformable;
}

bool
FInbetweenerPoint::IsDeformable()
{
    return bIsDeformable;
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

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_TARGETBBOX );
}

void
FInbetweenerPoint::SetMotionPosition( double iX, double iY )
{
    mMotionPosition.x = iX;
    mMotionPosition.y = iY;
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

::ULIS::FVec2D
FInbetweenerPoint::GetPosition( eInbetweenerPointPositionType iPositionType )
{
    switch( iPositionType )
    {
        case eInbetweenerPointPositionType::SourcePosition : 
        return mSourcePosition;

        case eInbetweenerPointPositionType::MotionPosition : 
        return mMotionPosition;

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
            mSourcePosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::MotionPosition : 
            mMotionPosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::DeformPosition : 
            mDeformPosition = ::ULIS::FVec2D( iX, iY );
        break;

        case eInbetweenerPointPositionType::TargetPosition : 
            mTargetPosition = ::ULIS::FVec2D( iX, iY );
        break;

        default:
        break;
    }
}