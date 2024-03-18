#include "OdysseyVectorPoint.h"

FOdysseyVectorPoint::~FOdysseyVectorPoint()
{
}

FOdysseyVectorPoint::FOdysseyVectorPoint()
{
    Set( 0.0f, 0.0f );
}

FOdysseyVectorPoint::FOdysseyVectorPoint( double iX, double iY )
{
    Set( iX, iY );
}

void
FOdysseyVectorPoint::Set( const ::ULIS::FVec2D& iCoords )
{
    SetCoords( iCoords.x, iCoords.y );
}

void
FOdysseyVectorPoint::Set( double iX, double iY )
{
    SetCoords( iX, iY );
}

void 
FOdysseyVectorPoint::SetX( double iX )
{
    SetCoords( iX, mCoords.y );
}

void 
FOdysseyVectorPoint::SetY( double iY )
{
    SetCoords( mCoords.x, iY );
}

void 
FOdysseyVectorPoint::SetCoords( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;
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
