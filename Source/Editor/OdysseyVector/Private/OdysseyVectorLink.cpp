#include "OdysseyVectorLink.h"

UOdysseyVectorLink::~UOdysseyVectorLink()
{

}

UOdysseyVectorLink::UOdysseyVectorLink()
    : mPoint { nullptr, nullptr }
{

}

void
UOdysseyVectorLink::Init( UOdysseyVectorPoint* iPoint0
                        , UOdysseyVectorPoint* iPoint1 )
{
    mPoint[0] = iPoint0;
    mPoint[1] = iPoint1;
}

// static
UOdysseyVectorLink*
UOdysseyVectorLink::New(  UOdysseyVectorPoint* iPoint0
                        , UOdysseyVectorPoint* iPoint1 )
{
    UOdysseyVectorLink* link = NewObject<UOdysseyVectorLink>();

    link->Init ( iPoint0, iPoint1 );

    return link;
}

UOdysseyVectorPoint*
UOdysseyVectorLink::GetPoint( int iPointNum )
{
    return mPoint[iPointNum];
}

double
UOdysseyVectorLink::GetStraightDistance()
{
    ::ULIS::FVec2D vec = { mPoint[1]->GetX() - mPoint[0]->GetX(),
                           mPoint[1]->GetY() - mPoint[0]->GetY() };

    return vec.Distance();
}

::ULIS::FVec2D
UOdysseyVectorLink::GetVector( bool iNormalize )
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
UOdysseyVectorLink::GetDistanceSquared()
{
    ::ULIS::FVec2D dist = mPoint[1]->GetCoords() - mPoint[0]->GetCoords();

    return dist.DistanceSquared();
}

::ULIS::FVec2D
UOdysseyVectorLink::GetPointAt ( double t )
{
    ::ULIS::FVec2D& p0 =  mPoint[0]->GetCoords();
    ::ULIS::FVec2D& p1 =  mPoint[0]->GetCoords();
    ::ULIS::FVec2D vec = p1 - p0;

    return p0 + ( vec * t );
}
