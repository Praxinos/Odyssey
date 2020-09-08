// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

/////////////////////////////////////////////////////
// FOdysseyStrokePoint
struct ODYSSEYSTROKEPIPELINE_API FOdysseyStrokePoint
{
public:
    // Construction / Destruction
    FOdysseyStrokePoint( float      iX                              = 0
                       , float      iY                              = 0
                       , float      iZ                              = 0
                       , float      iPressure                       = 1.f
                       , float      iAltitude                       = 0
                       , float      iAzimuth                        = 0
                       , float      iTwist                          = 0
                       , float      iPitch                          = 0
                       , float      iRoll                           = 0
                       , float      iYaw                            = 0
                       , float      iDistanceTravelled              = 0
                       , FVector2D  iDirectionVectorTangent         = FVector2D()
                       , FVector2D  iDirectionVectorNormal          = FVector2D()
                       , float      iDirectionAngleDegreesTangent   = 0
                       , float      iDirectionAngleDegreesNormal    = 0
                       , FVector2D  iSpeed                          = FVector2D()
                       , FVector2D  iAcceleration                   = FVector2D()
                       , FVector2D  iJolt                           = FVector2D()
                       );

public:
    // Makers
    static FOdysseyStrokePoint DefaultPoint();
    static FOdysseyStrokePoint ZeroPoint();

public:
    // Operators
    bool                 operator==( const FOdysseyStrokePoint& iRhs ) const;
    FOdysseyStrokePoint& operator= ( const FOdysseyStrokePoint& iRhs );
    FOdysseyStrokePoint& operator+=( const FOdysseyStrokePoint& iRhs );
    FOdysseyStrokePoint& operator-=( const FOdysseyStrokePoint& iRhs );
    FOdysseyStrokePoint  operator+ ( const FOdysseyStrokePoint& iRhs ) const;
    FOdysseyStrokePoint  operator- ( const FOdysseyStrokePoint& iRhs ) const;
    FOdysseyStrokePoint& operator*=( float iRhs );
    FOdysseyStrokePoint& operator/=( float iRhs );
    FOdysseyStrokePoint  operator* ( float iRhs ) const;
    FOdysseyStrokePoint  operator/ ( float iRhs ) const;

public: //TODO: rename to m* or even better Get/Set*() ...
    // Members
    float       x;
    float       y;
    float       z;
    float       pressure;
    float       altitude;
    float       azimuth;
    float       twist;
    float       pitch;
    float       roll;
    float       yaw;
    float       distance_travelled;
    FVector2D   direction_vector_tangent;
    FVector2D   direction_vector_normal;
    float       direction_angle_deg_tangent;
    float       direction_angle_deg_normal;
    FVector2D   speed;
    FVector2D   acceleration;
    FVector2D   jolt;
};

//---

FOdysseyStrokePoint operator*( float iLhs, const FOdysseyStrokePoint& iRhs );
FOdysseyStrokePoint operator/( float iLhs, const FOdysseyStrokePoint& iRhs );
