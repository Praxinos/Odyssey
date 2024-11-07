// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Input/OdysseyPoint.h"


/////////////////////////////////////////////////////
// FOdysseyPoint
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPoint::FOdysseyPoint( float         iX
                                        , float         iY
                                        , float         iZ
                                        , float         iPressure
                                        , unsigned long iTime
                                        , float         iAltitude
                                        , float         iAzimuth
                                        , float         iTwist
                                        , float         iPitch
                                        , float         iRoll
                                        , float         iYaw
                                        , float         iDistanceTravelled
                                        , FVector2D     iDirectionVectorTangent
                                        , FVector2D     iDirectionVectorNormal
                                        , float         iDirectionAngleDegreesTangent
                                        , float         iDirectionAngleDegreesNormal
                                        , FVector2D     iSpeed
                                        , FVector2D     iAcceleration
                                        , FVector2D     iJolt
                                        , FVector2D     iDeltaPosition
                                        , int           iDeltaTime
                                        , TArray<FKey>  iKeysDown
                                        )
    : x                             ( iX )
    , y                             ( iY )
    , z                             ( iZ )
    , pressure                      ( iPressure )
    , time                          ( iTime )
    , altitude                      ( iAltitude )
    , azimuth                       ( iAzimuth )
    , twist                         ( iTwist )
    , pitch                         ( iPitch )
    , roll                          ( iRoll )
    , yaw                           ( iYaw )
    , distance_travelled            ( iDistanceTravelled )
    , direction_vector_tangent      ( iDirectionVectorTangent )
    , direction_vector_normal       ( iDirectionVectorNormal )
    , direction_angle_deg_tangent   ( iDirectionAngleDegreesTangent )
    , direction_angle_deg_normal    ( iDirectionAngleDegreesNormal )
    , speed                         ( iSpeed )
    , acceleration                  ( iAcceleration )
    , jolt                          ( iJolt )
    , deltaPosition                 ( iDeltaPosition )
    , deltaTime                     ( iDeltaTime )
    , keysDown                      ( iKeysDown )
{}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Makers
//static
FOdysseyPoint
FOdysseyPoint::DefaultPoint()
{
    return FOdysseyPoint( 0, 0, 0, 1 );
}

//static
FOdysseyPoint
FOdysseyPoint::ZeroPoint()
{
    return FOdysseyPoint( 0, 0, 0, 0 );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Operators
bool
FOdysseyPoint::operator==( const FOdysseyPoint& iRhs ) const
{
    return    x                           == iRhs.x
           && y                           == iRhs.y
           && z                           == iRhs.z
           && pressure                    == iRhs.pressure
           && time                        == iRhs.time
           && altitude                    == iRhs.altitude
           && azimuth                     == iRhs.azimuth
           && twist                       == iRhs.twist
           && pitch                       == iRhs.pitch
           && roll                        == iRhs.roll
           && yaw                         == iRhs.yaw
           && distance_travelled          == iRhs.distance_travelled
           && direction_vector_tangent    == iRhs.direction_vector_tangent
           && direction_vector_normal     == iRhs.direction_vector_normal
           && direction_angle_deg_tangent == iRhs.direction_angle_deg_tangent
           && direction_angle_deg_normal  == iRhs.direction_angle_deg_normal
           && speed                       == iRhs.speed
           && acceleration                == iRhs.acceleration
           && jolt                        == iRhs.jolt
           && deltaPosition               == iRhs.deltaPosition
           && deltaTime                   == iRhs.deltaTime;
}

FOdysseyPoint&
FOdysseyPoint::operator=( const FOdysseyPoint& iRhs )
{
    x                           = iRhs.x;
    y                           = iRhs.y;
    z                           = iRhs.z;
    pressure                    = iRhs.pressure;
    time                        = iRhs.time;
    altitude                    = iRhs.altitude;
    azimuth                     = iRhs.azimuth;
    twist                       = iRhs.twist;
    pitch                       = iRhs.pitch;
    roll                        = iRhs.roll;
    yaw                         = iRhs.yaw;
    distance_travelled          = iRhs.distance_travelled;
    direction_vector_tangent    = iRhs.direction_vector_tangent;
    direction_vector_normal     = iRhs.direction_vector_normal;
    direction_angle_deg_tangent = iRhs.direction_angle_deg_tangent;
    direction_angle_deg_normal  = iRhs.direction_angle_deg_normal;
    speed                       = iRhs.speed;
    acceleration                = iRhs.acceleration;
    jolt                        = iRhs.jolt;
    deltaPosition               = iRhs.deltaPosition;
    deltaTime                   = iRhs.deltaTime;
    keysDown                    = iRhs.keysDown;

    return *this;
}

FOdysseyPoint&
FOdysseyPoint::operator+=( const FOdysseyPoint& iRhs )
{
    x                           += iRhs.x;
    y                           += iRhs.y;
    z                           += iRhs.z;
    pressure                    += iRhs.pressure;
    time                        += iRhs.time;
    altitude                    += iRhs.altitude;
    azimuth                     += iRhs.azimuth;
    twist                       += iRhs.twist;
    pitch                       += iRhs.pitch;
    roll                        += iRhs.roll;
    yaw                         += iRhs.yaw;
    distance_travelled          += iRhs.distance_travelled;
    direction_vector_tangent    += iRhs.direction_vector_tangent;
    direction_vector_normal     += iRhs.direction_vector_normal;
    direction_angle_deg_tangent += iRhs.direction_angle_deg_tangent;
    direction_angle_deg_normal  += iRhs.direction_angle_deg_normal;
    speed                       += iRhs.speed;
    acceleration                += iRhs.acceleration;
    jolt                        += iRhs.jolt;
    deltaPosition               += iRhs.deltaPosition;
    deltaTime                   += iRhs.deltaTime;

    return *this;
}

FOdysseyPoint&
FOdysseyPoint::operator-=( const FOdysseyPoint& iRhs )
{
    x                           -= iRhs.x;
    y                           -= iRhs.y;
    z                           -= iRhs.z;
    pressure                    -= iRhs.pressure;
    time                        -= iRhs.time;
    altitude                    -= iRhs.altitude;
    azimuth                     -= iRhs.azimuth;
    twist                       -= iRhs.twist;
    pitch                       -= iRhs.pitch;
    roll                        -= iRhs.roll;
    yaw                         -= iRhs.yaw;
    distance_travelled          -= iRhs.distance_travelled;
    direction_vector_tangent    -= iRhs.direction_vector_tangent;
    direction_vector_normal     -= iRhs.direction_vector_normal;
    direction_angle_deg_tangent -= iRhs.direction_angle_deg_tangent;
    direction_angle_deg_normal  -= iRhs.direction_angle_deg_normal;
    speed                       -= iRhs.speed;
    acceleration                -= iRhs.acceleration;
    jolt                        -= iRhs.jolt;
    deltaPosition               -= iRhs.deltaPosition;
    deltaTime                   -= iRhs.deltaTime;

    return *this;
}

FOdysseyPoint
FOdysseyPoint::operator+( const FOdysseyPoint& iRhs ) const
{
    FOdysseyPoint out = *this;
    out += iRhs;

    return out;
}

FOdysseyPoint
FOdysseyPoint::operator-( const FOdysseyPoint& iRhs ) const
{
    FOdysseyPoint out = *this;
    out -= iRhs;

    return out;
}

FOdysseyPoint&
FOdysseyPoint::operator*=( float iRhs )
{
    x                           *= iRhs;
    y                           *= iRhs;
    z                           *= iRhs;
    pressure                    *= iRhs;
    time                        *= iRhs;
    altitude                    *= iRhs;
    azimuth                     *= iRhs;
    twist                       *= iRhs;
    pitch                       *= iRhs;
    roll                        *= iRhs;
    yaw                         *= iRhs;
    distance_travelled          *= iRhs;
    direction_vector_tangent    *= iRhs;
    direction_vector_normal     *= iRhs;
    direction_angle_deg_tangent *= iRhs;
    direction_angle_deg_normal  *= iRhs;
    speed                       *= iRhs;
    acceleration                *= iRhs;
    jolt                        *= iRhs;
    deltaPosition               *= iRhs;
    deltaTime                   *= iRhs;

    return *this;
}

FOdysseyPoint&
FOdysseyPoint::operator/=( float iRhs )
{
    x                           /= iRhs;
    y                           /= iRhs;
    z                           /= iRhs;
    pressure                    /= iRhs;
    time                        /= iRhs;
    altitude                    /= iRhs;
    azimuth                     /= iRhs;
    twist                       /= iRhs;
    pitch                       /= iRhs;
    roll                        /= iRhs;
    yaw                         /= iRhs;
    distance_travelled          /= iRhs;
    direction_vector_tangent    /= iRhs;
    direction_vector_normal     /= iRhs;
    direction_angle_deg_tangent /= iRhs;
    direction_angle_deg_normal  /= iRhs;
    speed                       /= iRhs;
    acceleration                /= iRhs;
    jolt                        /= iRhs;
    deltaPosition               /= iRhs;
    deltaTime                   /= iRhs;

    return *this;
}

FOdysseyPoint
FOdysseyPoint::operator*( float iRhs ) const
{
    FOdysseyPoint out = *this;
    out.operator*=( iRhs );

    return out;
}

FOdysseyPoint
FOdysseyPoint::operator/( float iRhs ) const
{
    FOdysseyPoint out = *this;
    out.operator/=( iRhs );

    return out;
}

FOdysseyPoint 
operator*( float iLhs, const FOdysseyPoint& iRhs )
{
    return iRhs.operator*( iLhs );
}

FOdysseyPoint 
operator/( float iLhs, const FOdysseyPoint& iRhs )
{
    return iRhs.operator/( iLhs );
}

FOdysseyPoint 
FOdysseyPoint::Average( const FOdysseyPoint& iPt1, const FOdysseyPoint& iPt2 )
{
    FOdysseyPoint outPoint = (iPt1 + iPt2) / 2;

    if( FMath::Abs( iPt1.azimuth - iPt2.azimuth ) > 180 )
    {
        outPoint.azimuth = FMath::Fmod( (iPt1.azimuth + iPt2.azimuth + 360) / 2, 360.0 );
    }

    return outPoint;
}

FOdysseyPoint 
FOdysseyPoint::Average( const TArray< FOdysseyPoint>& iPoints )
{
    //Empty array case
    if( iPoints.Num() == 0 )
        return FOdysseyPoint::ZeroPoint();

    //Init base outPoint
    FOdysseyPoint outPoint = iPoints[0];

    for( int i = 1; i < iPoints.Num(); i++ )
        outPoint += iPoints[i];

    outPoint /= iPoints.Num();

    //Init correctAzimuth
    TArray<float> azimuths;
    for( int i = 0; i < iPoints.Num(); i++)
    {
        azimuths.Add(iPoints[i].azimuth);
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
    outPoint.azimuth = azimuths[0];
    //---

    return outPoint;

}

void
FOdysseyPoint::ComputeRelativeParameters(const FOdysseyPoint& iPreviousPoint, bool iComputeTravelledDistance)
{
    deltaPosition = FVector2D( x - iPreviousPoint.x, y - iPreviousPoint.y );
    deltaTime = FMath::Max(time, iPreviousPoint.time) - FMath::Min(time, iPreviousPoint.time);
    if( deltaTime == 0) deltaTime++;

    speed = deltaTime != 0 ? deltaPosition / deltaTime : FVector2D(0,0);
    acceleration = speed - iPreviousPoint.speed;
    jolt = acceleration - iPreviousPoint.acceleration;

    direction_angle_deg_tangent = atan2( y - iPreviousPoint.y, x - iPreviousPoint.x ) * 180.f / 3.14159265359f;
    direction_angle_deg_normal = direction_angle_deg_tangent + 90;
    direction_vector_tangent = deltaPosition.GetSafeNormal();
    direction_vector_normal = FVector2D( -direction_vector_tangent.Y, direction_vector_tangent.X );
    
    if (iComputeTravelledDistance)
        distance_travelled = iPreviousPoint.distance_travelled + deltaPosition.Size();
}


FOdysseyPoint 
FOdysseyPoint::Lerp( const FOdysseyPoint& iPt1, const FOdysseyPoint& iPt2, float iT )
{
    FOdysseyPoint outPoint = iPt1 + (iPt2 - iPt1) * iT;

    if( FMath::Abs( iPt1.azimuth - iPt2.azimuth ) > 180 )
    {
        int sign = iPt1.azimuth >= iPt2.azimuth ? 1 : -1;
        outPoint.azimuth = FMath::Fmod( iPt1.azimuth + (iPt2.azimuth - iPt1.azimuth + 360 * sign ) * iT + 360, 360.0 ) ;
    }

    return outPoint;
}