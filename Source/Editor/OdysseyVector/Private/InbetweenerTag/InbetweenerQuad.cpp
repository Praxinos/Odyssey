#include "InbetweenerTag/InbetweenerQuad.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerQuad::FInbetweenerQuad( )
    : mFlags ( 0 )
{
}

FInbetweenerPoint** 
FInbetweenerQuad::GetPoints()
{
    return mPoint;
}

bool
FInbetweenerQuad::IsLinked()
{
    return ( mFlags & LINKED ) ? true : false; 
}

void
FInbetweenerQuad::Link()
{
    mFlags |= LINKED;

    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->AddQuad( this );
    }
}

void
FInbetweenerQuad::Unlink()
{
    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->RemoveQuad( this );
    }

    mFlags &= (~LINKED);
}
