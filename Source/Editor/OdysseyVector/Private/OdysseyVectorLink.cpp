#include "OdysseyVectorLink.h"

FOdysseyVectorLink::~FOdysseyVectorLink()
{

}

FOdysseyVectorLink::FOdysseyVectorLink()
    : mPoint { nullptr, nullptr }
{

}

void
FOdysseyVectorLink::Init( FOdysseyVectorPoint* iPoint0
                        , FOdysseyVectorPoint* iPoint1 )
{
    mPoint[0] = iPoint0;
    mPoint[1] = iPoint1;
}

// static
FOdysseyVectorLink*
FOdysseyVectorLink::New(  FOdysseyVectorPoint* iPoint0
                        , FOdysseyVectorPoint* iPoint1 )
{
    FOdysseyVectorLink* link = NewObject<FOdysseyVectorLink>();

    link->Init ( iPoint0, iPoint1 );

    return link;
}

FOdysseyVectorPoint*
FOdysseyVectorLink::GetPoint( int iPointNum )
{
    return mPoint[iPointNum];
}

double
FOdysseyVectorLink::GetStraightDistance()
{
    ::ULIS::FVec2D vec = { mPoint[1]->GetX() - mPoint[0]->GetX(),
                           mPoint[1]->GetY() - mPoint[0]->GetY() };

    return vec.Distance();
}

::ULIS::FVec2D
FOdysseyVectorLink::GetVector( bool iNormalize )
{
    ::ULIS::FVec2D vec = mPoint[1]->GetCoords() - mPoint[0]->GetCoords();

    if ( iNormalize == true )
    {
        if ( vec.DistanceSquared() )
        {
            vec.Normalize();
        }
    }

    return vec;
}

double
FOdysseyVectorLink::GetDistanceSquared()
{
    ::ULIS::FVec2D dist = mPoint[1]->GetCoords() - mPoint[0]->GetCoords();

    return dist.DistanceSquared();
}

::ULIS::FVec2D
FOdysseyVectorLink::GetPointAt ( double t )
{
    ::ULIS::FVec2D& p0 =  mPoint[0]->GetCoords();
    ::ULIS::FVec2D& p1 =  mPoint[0]->GetCoords();
    ::ULIS::FVec2D vec = p1 - p0;

    return p0 + ( vec * t );
}
