// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyStrokePoint.h"


/////////////////////////////////////////////////////
// FOdysseyStrokePoint
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyStrokePoint::FOdysseyStrokePoint( float         iX
                                        , float         iY
                                        , float         iZ
                                        , float         iPressure
                                        , unsigned int  iTime
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
                                        , unsigned int  iDeltaTime
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
{}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Makers
//static
FOdysseyStrokePoint
FOdysseyStrokePoint::DefaultPoint()
{
    return FOdysseyStrokePoint( 0, 0, 0, 1 );
}

//static
FOdysseyStrokePoint
FOdysseyStrokePoint::ZeroPoint()
{
    return FOdysseyStrokePoint( 0, 0, 0, 0 );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Operators
bool
FOdysseyStrokePoint::operator==( const FOdysseyStrokePoint& iRhs ) const
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

FOdysseyStrokePoint&
FOdysseyStrokePoint::operator=( const FOdysseyStrokePoint& iRhs )
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

    return *this;
}

FOdysseyStrokePoint&
FOdysseyStrokePoint::operator+=( const FOdysseyStrokePoint& iRhs )
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

FOdysseyStrokePoint&
FOdysseyStrokePoint::operator-=( const FOdysseyStrokePoint& iRhs )
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

FOdysseyStrokePoint
FOdysseyStrokePoint::operator+( const FOdysseyStrokePoint& iRhs ) const
{
    FOdysseyStrokePoint out = *this;
    out += iRhs;

    return out;
}

FOdysseyStrokePoint
FOdysseyStrokePoint::operator-( const FOdysseyStrokePoint& iRhs ) const
{
    FOdysseyStrokePoint out = *this;
    out -= iRhs;

    return out;
}

FOdysseyStrokePoint&
FOdysseyStrokePoint::operator*=( float iRhs )
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

FOdysseyStrokePoint&
FOdysseyStrokePoint::operator/=( float iRhs )
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

FOdysseyStrokePoint
FOdysseyStrokePoint::operator*( float iRhs ) const
{
    FOdysseyStrokePoint out = *this;
    out.operator*=( iRhs );

    return out;
}

FOdysseyStrokePoint
FOdysseyStrokePoint::operator/( float iRhs ) const
{
    FOdysseyStrokePoint out = *this;
    out.operator/=( iRhs );

    return out;
}

FOdysseyStrokePoint 
operator*( float iLhs, const FOdysseyStrokePoint& iRhs )
{
    return iRhs.operator*( iLhs );
}

FOdysseyStrokePoint 
operator/( float iLhs, const FOdysseyStrokePoint& iRhs )
{
    return iRhs.operator/( iLhs );
}
