#include "OdysseyVectorPoint.h"

FOdysseyVectorPoint::~FOdysseyVectorPoint()
{
}

FOdysseyVectorPoint::FOdysseyVectorPoint()
{
    Set( 0.0f, 0.0f, 0.0f );
}

FOdysseyVectorPoint::FOdysseyVectorPoint( double iX, double iY, double iRadius )
{
    Set( iX, iY, iRadius );
}

void
FOdysseyVectorPoint::Set( const ::ULIS::FVec2D& iCoords )
{
    SetCoords( iCoords.x, iCoords.y, mRadius );
}

void
FOdysseyVectorPoint::Set( double iX, double iY, double iRadius )
{
    SetCoords( iX, iY, iRadius );
}

void
FOdysseyVectorPoint::Set( double iX, double iY )
{
    SetCoords( iX, iY, mRadius );
}

void 
FOdysseyVectorPoint::SetX( double iX )
{
    SetCoords( iX, mCoords.y, mRadius );
}

void 
FOdysseyVectorPoint::SetY( double iY )
{
    SetCoords( mCoords.x, iY, mRadius );
}

void 
FOdysseyVectorPoint::SetCoords( double iX, double iY, double iRadius )
{
    mCoords.x = iX;
    mCoords.y = iY;
    mRadius = iRadius;
}

::ULIS::FVec2D&
FOdysseyVectorPoint::GetCoords()
{
    return mCoords;
}

double 
FOdysseyVectorPoint::GetX()
{
    return mCoords.x;
}

double 
FOdysseyVectorPoint::GetY()
{
    return mCoords.y;
}

double
FOdysseyVectorPoint::GetRadius()
{
    return mRadius;
}

void
FOdysseyVectorPoint::SetRadius( double iRadius )
{
    mRadius = iRadius;
}

void
FOdysseyVectorPoint::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FOdysseyVectorPoint::GetID()
{
    return mID;
}

//static
void
FOdysseyVectorPoint::ArrayToVertexArray( std::vector<FOdysseyVectorPoint*>& iPointArray
                                       , std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    if( iPointArray.size() )
    {
        oVertexArray.reserve( iPointArray.size() );

        for( int i = 0; i < iPointArray.size(); i++ )
        {
            oVertexArray.push_back( static_cast<FOdysseyVectorVertex*>(iPointArray[i]) );
        }
    }
}
