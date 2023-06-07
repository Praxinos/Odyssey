#include "OdysseyVectorPoint.h"

FOdysseyVectorPoint::~FOdysseyVectorPoint()
{

}

FOdysseyVectorPoint::FOdysseyVectorPoint()
{
    Set( 0.0f,0.0f );
    SetRadius( 1.0f );
}

FOdysseyVectorPoint::FOdysseyVectorPoint( double iX, double iY, double iRadius )
{
    Init( iX, iY, iRadius );
}

void
FOdysseyVectorPoint::Init( double iX, double iY, double iRadius )
{
    Set( iX, iY );
    SetRadius( iRadius );
}

void
FOdysseyVectorPoint::SetCoords( const ::ULIS::FVec2D& iCoords )
{
    mCoords.x  = iCoords.x;
    mCoords.y  = iCoords.y;
}

void
FOdysseyVectorPoint::SetCoords( double iX, double iY )
{
    mCoords.x  = iX;
    mCoords.y  = iY;
}

::ULIS::FVec2D&
FOdysseyVectorPoint::GetCoords()
{
    return mCoords;
}

//static
FOdysseyVectorPoint*
FOdysseyVectorPoint::New( double iX, double iY, double iRadius )
{
    FOdysseyVectorPoint* point = new FOdysseyVectorPoint();

    point->Init ( iX, iY, iRadius );

    return point;
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

void 
FOdysseyVectorPoint::SetX( double iX )
{
    mCoords.x  = iX;
}

void 
FOdysseyVectorPoint::SetY( double iY )
{
    mCoords.y = iY;
}

void 
FOdysseyVectorPoint::Set( double iX, double iY )
{
    SetX( iX );
    SetY( iY );
}

void 
FOdysseyVectorPoint::Set( double iX, double iY, double iRadius )
{
    SetX( iX );
    SetY( iY );
    SetRadius( iRadius );
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
