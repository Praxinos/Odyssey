// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Input/OdysseyRay.h"


/////////////////////////////////////////////////////
// FOdysseyRay
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyRay::FOdysseyRay(                FVector        iRayOrigin
                                       , FVector        iRayDirection
                                       , FOdysseyPoint  iPoint )
    : mRayOrigin    ( iRayOrigin )
    , mRayDirection ( iRayDirection )
    , mPoint        ( iPoint )
{}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Makers
//static
FOdysseyRay
FOdysseyRay::DefaultRay()
{
    return FOdysseyRay( FVector(0,0,0), FVector(0,0,0), FOdysseyPoint::DefaultPoint() );
}

//static
FOdysseyRay
FOdysseyRay::ZeroRay()
{
    return FOdysseyRay( FVector(0,0,0), FVector(0,0,0), FOdysseyPoint::ZeroPoint() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Operators

bool
FOdysseyRay::operator==( const FOdysseyRay& iRhs ) const
{
    return    mRayOrigin                       == iRhs.mRayOrigin
           && mRayDirection                    == iRhs.mRayDirection
           && mPoint                           == iRhs.mPoint;
}

FOdysseyRay&
FOdysseyRay::operator=( const FOdysseyRay& iRhs )
{
    mRayOrigin = iRhs.mRayOrigin;
    mRayDirection = iRhs.mRayDirection;
    mPoint = iRhs.mPoint;

    return *this;
}

FOdysseyRay&
FOdysseyRay::operator+=( const FOdysseyRay& iRhs )
{
    mRayOrigin += iRhs.mRayOrigin;
    mRayDirection += iRhs.mRayDirection;
    mPoint += iRhs.mPoint;

    return *this;
}

FOdysseyRay&
FOdysseyRay::operator-=( const FOdysseyRay& iRhs )
{
    mRayOrigin -= iRhs.mRayOrigin;
    mRayDirection -= iRhs.mRayDirection;
    mPoint -= iRhs.mPoint;

    return *this;
}

FOdysseyRay
FOdysseyRay::operator+( const FOdysseyRay& iRhs ) const
{
    FOdysseyRay out = *this;
    out += iRhs;

    return out;
}

FOdysseyRay
FOdysseyRay::operator-( const FOdysseyRay& iRhs ) const
{
    FOdysseyRay out = *this;
    out -= iRhs;

    return out;
}

FOdysseyRay&
FOdysseyRay::operator*=( float iRhs )
{
    mRayOrigin *= iRhs;
    mRayDirection *= iRhs;
    mPoint *= iRhs;

    return *this;
}

FOdysseyRay&
FOdysseyRay::operator/=( float iRhs )
{
    mRayOrigin /= iRhs;
    mRayDirection /= iRhs;
    mPoint /= iRhs;

    return *this;
}

FOdysseyRay
FOdysseyRay::operator*( float iRhs ) const
{
    FOdysseyRay out = *this;
    out.operator*=( iRhs );

    return out;
}

FOdysseyRay
FOdysseyRay::operator/( float iRhs ) const
{
    FOdysseyRay out = *this;
    out.operator/=( iRhs );

    return out;
}

FOdysseyRay 
operator*( float iLhs, const FOdysseyRay& iRhs )
{
    return iRhs.operator*( iLhs );
}

FOdysseyRay 
operator/( float iLhs, const FOdysseyRay& iRhs )
{
    return iRhs.operator/( iLhs );
}

FOdysseyRay 
FOdysseyRay::Average( const FOdysseyRay& iRay1, const FOdysseyRay& iRay2 )
{
    FOdysseyRay outRay = (iRay1 + iRay2) / 2;

    if( FMath::Abs( iRay1.mPoint.azimuth - iRay2.mPoint.azimuth ) > 180 )
    {
        outRay.mPoint.azimuth = FMath::Fmod( (iRay1.mPoint.azimuth + iRay2.mPoint.azimuth + 360) / 2, 360.0 );
    }

    return outRay;
}

FOdysseyRay 
FOdysseyRay::Average( const TArray< FOdysseyRay>& iRays )
{
    //Empty array case
    if( iRays.Num() == 0 )
        return FOdysseyRay::ZeroRay();

    //Init base outRay
    FOdysseyRay outRay = iRays[0];

    for( int i = 1; i < iRays.Num(); i++ )
        outRay += iRays[i];

    outRay /= iRays.Num();

    //Init correctAzimuth
    TArray<float> azimuths;
    for( int i = 0; i < iRays.Num(); i++)
    {
        azimuths.Add(iRays[i].mPoint.azimuth);
    }

    int currentIndex = 0;
    while( azimuths.Num() != 1 )
    {       
        if( FMath::Abs( azimuths[currentIndex] - azimuths[currentIndex + 1] ) > 180 )
        {
            azimuths[currentIndex] = FMath::Fmod( (azimuths[currentIndex] + azimuths[currentIndex + 1] + 360) / 2, 360.0 );
        }
        else
        {
            azimuths[currentIndex] = ( azimuths[currentIndex] + azimuths[currentIndex + 1 ] ) / 2;
        }

        currentIndex++;

        if( currentIndex >= azimuths.Num() - 1 )
        {
            currentIndex = 0;
            azimuths.Pop();
        }
    }
    outRay.mPoint.azimuth = azimuths[0];
    //---

    return outRay;

}
/*
void
FOdysseyRay::ComputeRelativeParameters(const FOdysseyRay& iPreviousRay, bool iComputeTravelledDistance)
{
    deltaPosition = FVector2D( x - iPreviousRay.x, y - iPreviousRay.y );
    deltaTime = FMath::Max(time, iPreviousRay.time) - FMath::Min(time, iPreviousRay.time);
    if( deltaTime == 0) deltaTime++;

    speed = deltaTime != 0 ? deltaPosition / deltaTime : FVector2D(0,0);
    acceleration = speed - iPreviousRay.speed;
    jolt = acceleration - iPreviousRay.acceleration;

    direction_angle_deg_tangent = atan2( y - iPreviousRay.y, x - iPreviousRay.x ) * 180.f / 3.14159265359f;
    direction_angle_deg_normal = direction_angle_deg_tangent + 90;
    direction_vector_tangent = deltaPosition.GetSafeNormal();
    direction_vector_normal = FVector2D( -direction_vector_tangent.Y, direction_vector_tangent.X );
    
    if (iComputeTravelledDistance)
        distance_travelled = iPreviousRay.distance_travelled + deltaPosition.Size();
}
*/

FOdysseyRay
FOdysseyRay::Lerp( const FOdysseyRay& iRay1, const FOdysseyRay& iRay2, float iT )
{
    FOdysseyRay outRay = iRay1 + (iRay2 - iRay1) * iT;

    if( FMath::Abs( iRay1.mPoint.azimuth - iRay2.mPoint.azimuth ) > 180 )
    {
        int sign = iRay1.mPoint.azimuth >= iRay2.mPoint.azimuth ? 1 : -1;
        outRay.mPoint.azimuth = FMath::Fmod( iRay1.mPoint.azimuth + (iRay2.mPoint.azimuth - iRay1.mPoint.azimuth + 360 * sign ) * iT + 360, 360.0 ) ;
    }

    return outRay;
}