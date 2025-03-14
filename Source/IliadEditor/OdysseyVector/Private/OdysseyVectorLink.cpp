// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorLink.h"

FOdysseyVectorLink::~FOdysseyVectorLink()
{
}

FOdysseyVectorLink::FOdysseyVectorLink( FOdysseyVectorPoint* iPoint0, FOdysseyVectorPoint* iPoint1 )
{
    mPoint[0] = iPoint0;
    mPoint[1] = iPoint1;
}

void
FOdysseyVectorLink::SetPoint0( FOdysseyVectorPoint* iPoint )
{
    mPoint[0] = iPoint;
}

void
FOdysseyVectorLink::SetPoint1( FOdysseyVectorPoint* iPoint )
{
    mPoint[1] = iPoint;
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
FOdysseyVectorLink::GetVector( FOdysseyVectorPoint* iPoint, bool iNormalize )
{
    ::ULIS::FVec2D vec = GetVector( iNormalize );

    return ( iPoint == mPoint[0] ) ? vec : -vec;
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
