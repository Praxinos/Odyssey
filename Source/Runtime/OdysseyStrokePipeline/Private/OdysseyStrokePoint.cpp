// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyStrokePoint.h"


/////////////////////////////////////////////////////
// FOdysseyStrokePoint
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyStrokePoint::FOdysseyStrokePoint( float         iX
                                        , float         iY
                                        , float         iZ
                                        , float         iPressure
                                        , float         iAltitude
                                        , float         iAzimuth
                                        , float         iTwist
                                        , float         iPitch
                                        , float         iRoll
                                        , float         iYaw
                                        , float         iDistance_travelled
                                        , FVector2D     iDirection_vector_tangent
                                        , FVector2D     iDirection_vector_normal
                                        , float         iDirection_angle_deg_tangent
                                        , float         iDirection_angle_deg_normal
                                        , FVector2D     iSpeed
                                        , FVector2D     iAcceleration
                                        , FVector2D     iJolt
                                        )
    : x                             (   iX                              )
    , y                             (   iY                              )
    , z                             (   iZ                              )
    , pressure                      (   iPressure                       )
    , altitude                      (   iAltitude                       )
    , azimuth                       (   iAzimuth                        )
    , twist                         (   iTwist                          )
    , pitch                         (   iPitch                          )
    , roll                          (   iRoll                           )
    , yaw                           (   iYaw                            )
    , distance_travelled            (   iDistance_travelled             )
    , direction_vector_tangent      (   iDirection_vector_tangent       )
    , direction_vector_normal       (   iDirection_vector_normal        )
    , direction_angle_deg_tangent   (   iDirection_angle_deg_tangent    )
    , direction_angle_deg_normal    (   iDirection_angle_deg_normal     )
    , speed                         (   iSpeed                          )
    , acceleration                  (   iAcceleration                   )
    , jolt                          (   iJolt                           )
{}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Makers
//static
FOdysseyStrokePoint
FOdysseyStrokePoint::DefaultPoint()
{
    return  FOdysseyStrokePoint( 0, 0, 0, 1 );
}


//static
FOdysseyStrokePoint
FOdysseyStrokePoint::ZeroPoint()
{
    return  FOdysseyStrokePoint( 0, 0, 0, 0 );
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Operators
bool
FOdysseyStrokePoint::operator==(  const  FOdysseyStrokePoint&  iRhs ) const
{
    return  x                           == iRhs.x                           &&
            y                           == iRhs.y                           &&
            z                           == iRhs.z                           &&
            pressure                    == iRhs.pressure                    &&
            altitude                    == iRhs.altitude                    &&
            azimuth                     == iRhs.azimuth                     &&
            twist                       == iRhs.twist                       &&
            pitch                       == iRhs.pitch                       &&
            roll                        == iRhs.roll                        &&
            yaw                         == iRhs.yaw                         &&
            distance_travelled          == iRhs.distance_travelled          &&
            direction_vector_tangent    == iRhs.direction_vector_tangent    &&
            direction_vector_normal     == iRhs.direction_vector_normal     &&
            direction_angle_deg_tangent == iRhs.direction_angle_deg_tangent &&
            direction_angle_deg_normal  == iRhs.direction_angle_deg_normal  &&
            speed                       == iRhs.speed                       &&
            acceleration                == iRhs.acceleration                &&
            jolt                        == iRhs.jolt                        ;
}


FOdysseyStrokePoint&
FOdysseyStrokePoint::operator=(  const  FOdysseyStrokePoint&  iRhs )
{
    x                           = iRhs.x                            ;
    y                           = iRhs.y                            ;
    z                           = iRhs.z                            ;
    pressure                    = iRhs.pressure                     ;
    altitude                    = iRhs.altitude                     ;
    azimuth                     = iRhs.azimuth                      ;
    twist                       = iRhs.twist                        ;
    pitch                       = iRhs.pitch                        ;
    roll                        = iRhs.roll                         ;
    yaw                         = iRhs.yaw                          ;
    distance_travelled          = iRhs.distance_travelled           ;
    direction_vector_tangent    = iRhs.direction_vector_tangent     ;
    direction_vector_normal     = iRhs.direction_vector_normal      ;
    direction_angle_deg_tangent = iRhs.direction_angle_deg_tangent  ;
    direction_angle_deg_normal  = iRhs.direction_angle_deg_normal   ;
    speed                       = iRhs.speed                        ;
    acceleration                = iRhs.acceleration                 ;
    jolt                        = iRhs.jolt                         ;
    return  *this;
}


FOdysseyStrokePoint&
FOdysseyStrokePoint::operator+=( const  FOdysseyStrokePoint&  iRhs )
{
    x                           += iRhs.x                            ;
    y                           += iRhs.y                            ;
    z                           += iRhs.z                            ;
    pressure                    += iRhs.pressure                     ;
    altitude                    += iRhs.altitude                     ;
    azimuth                     += iRhs.azimuth                      ;
    twist                       += iRhs.twist                        ;
    pitch                       += iRhs.pitch                        ;
    roll                        += iRhs.roll                         ;
    yaw                         += iRhs.yaw                          ;
    distance_travelled          += iRhs.distance_travelled           ;
    direction_vector_tangent    += iRhs.direction_vector_tangent     ;
    direction_vector_normal     += iRhs.direction_vector_normal      ;
    direction_angle_deg_tangent += iRhs.direction_angle_deg_tangent  ;
    direction_angle_deg_normal  += iRhs.direction_angle_deg_normal   ;
    speed                       += iRhs.speed                        ;
    acceleration                += iRhs.acceleration                 ;
    jolt                        += iRhs.jolt                         ;
    return  *this;
}


FOdysseyStrokePoint&
FOdysseyStrokePoint::operator-=( const  FOdysseyStrokePoint&  iRhs )
{
    x                           -= iRhs.x                            ;
    y                           -= iRhs.y                            ;
    z                           -= iRhs.z                            ;
    pressure                    -= iRhs.pressure                     ;
    altitude                    -= iRhs.altitude                     ;
    azimuth                     -= iRhs.azimuth                      ;
    twist                       -= iRhs.twist                        ;
    pitch                       -= iRhs.pitch                        ;
    roll                        -= iRhs.roll                         ;
    yaw                         -= iRhs.yaw                          ;
    distance_travelled          -= iRhs.distance_travelled           ;
    direction_vector_tangent    -= iRhs.direction_vector_tangent     ;
    direction_vector_normal     -= iRhs.direction_vector_normal      ;
    direction_angle_deg_tangent -= iRhs.direction_angle_deg_tangent  ;
    direction_angle_deg_normal  -= iRhs.direction_angle_deg_normal   ;
    speed                       -= iRhs.speed                        ;
    acceleration                -= iRhs.acceleration                 ;
    jolt                        -= iRhs.jolt                         ;
    return  *this;
}


FOdysseyStrokePoint
FOdysseyStrokePoint::operator+( const  FOdysseyStrokePoint&  iRhs )
{
    FOdysseyStrokePoint out = *this;
    out += iRhs;
    return  out;
}


FOdysseyStrokePoint
FOdysseyStrokePoint::operator-( const  FOdysseyStrokePoint&  iRhs )
{
    FOdysseyStrokePoint out = *this;
    out -= iRhs;
    return  out;
}

FOdysseyStrokePoint&
FOdysseyStrokePoint::operator*=( float  iRhs )
{
    x                           *= iRhs;
    y                           *= iRhs;
    z                           *= iRhs;
    pressure                    *= iRhs;
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
    return  *this;
}


FOdysseyStrokePoint&
FOdysseyStrokePoint::operator/=( float  iRhs )
{
    x                           /= iRhs;
    y                           /= iRhs;
    z                           /= iRhs;
    pressure                    /= iRhs;
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
    return  *this;
}


FOdysseyStrokePoint
FOdysseyStrokePoint::operator*( float  iRhs )  const
{
    FOdysseyStrokePoint out = *this;
    out.operator*=( iRhs );
    return  out;
}


FOdysseyStrokePoint
FOdysseyStrokePoint::operator/( float  iRhs )  const
{
    FOdysseyStrokePoint out = *this;
    out.operator/=( iRhs );
    return  out;
}


FOdysseyStrokePoint operator*( float iLhs, const  FOdysseyStrokePoint&  iRhs )
{
    return  iRhs.operator*( iLhs );
}


FOdysseyStrokePoint operator/( float iLhs, const  FOdysseyStrokePoint&  iRhs )
{
    return  iRhs.operator/( iLhs );
}

