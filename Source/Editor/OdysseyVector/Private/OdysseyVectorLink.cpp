#include "OdysseyVectorLink.h"

FOdysseyVectorLink::~FOdysseyVectorLink()
{

}

FOdysseyVectorLink::FOdysseyVectorLink()
    : mPoint { nullptr, nullptr }
{

}

FOdysseyVectorLink::FOdysseyVectorLink( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 )
{
    Init( iPoint0, iPoint1 );
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
    FOdysseyVectorLink* link = new FOdysseyVectorLink();

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
    ::ULIS::FVec2D vec = ::ULIS::FVec2D( mPoint[1]->GetX() - mPoint[0]->GetX()
                                       , mPoint[1]->GetY() - mPoint[0]->GetY() );

    if ( iNormalize == true )
    {
        if ( vec.DistanceSquared() )
        {
            vec.Normalize();
        }
    }

    return vec;
}

::ULIS::FVec2D
FOdysseyVectorLink::GetPointAt ( double t )
{
    ::ULIS::FVec2D vec = ::ULIS::FVec2D( mPoint[1]->GetX() - mPoint[0]->GetX()
                                       , mPoint[1]->GetY() - mPoint[0]->GetY() );
    ::ULIS::FVec2D pointAt = ::ULIS::FVec2D( mPoint[0]->GetX() + ( vec.x * t )
                                           , mPoint[0]->GetY() + ( vec.y * t ) );

    return pointAt;
}
