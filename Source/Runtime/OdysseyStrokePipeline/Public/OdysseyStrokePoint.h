// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


/////////////////////////////////////////////////////
// FOdysseyStrokePoint
struct ODYSSEYSTROKEPIPELINE_API FOdysseyStrokePoint
{
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
                       , float      iDistance_travelled             = 0
                       , FVector2D  iDirection_vector_tangent       = FVector2D()
                       , FVector2D  iDirection_vector_normal        = FVector2D()
                       , float      iDirection_angle_deg_tangent    = 0
                       , float      iDirection_angle_deg_normal     = 0
                       , FVector2D  iSpeed                          = FVector2D()
                       , FVector2D  iAcceleration                   = FVector2D()
                       , FVector2D  iJolt                           = FVector2D()
                       );

    // Makers
    static  FOdysseyStrokePoint  DefaultPoint();
    static  FOdysseyStrokePoint  ZeroPoint();

    // Operators
    bool operator==(                    const  FOdysseyStrokePoint&  iRhs )  const;
    FOdysseyStrokePoint& operator=(     const  FOdysseyStrokePoint&  iRhs );
    FOdysseyStrokePoint& operator+=(    const  FOdysseyStrokePoint&  iRhs );
    FOdysseyStrokePoint& operator-=(    const  FOdysseyStrokePoint&  iRhs );
    FOdysseyStrokePoint  operator+(     const  FOdysseyStrokePoint&  iRhs );
    FOdysseyStrokePoint  operator-(     const  FOdysseyStrokePoint&  iRhs );
    FOdysseyStrokePoint& operator*=(    float  iRhs );
    FOdysseyStrokePoint& operator/=(    float  iRhs );
    FOdysseyStrokePoint operator*(      float  iRhs )  const;
    FOdysseyStrokePoint operator/(      float  iRhs )  const;


    // Members
    float       x                               ;
    float       y                               ;
    float       z                               ;
    float       pressure                        ;
    float       altitude                        ;
    float       azimuth                         ;
    float       twist                           ;
    float       pitch                           ;
    float       roll                            ;
    float       yaw                             ;
    float       distance_travelled              ;
    FVector2D   direction_vector_tangent        ;
    FVector2D   direction_vector_normal         ;
    float       direction_angle_deg_tangent     ;
    float       direction_angle_deg_normal      ;
    FVector2D   speed                           ;
    FVector2D   acceleration                    ;
    FVector2D   jolt                            ;
};


FOdysseyStrokePoint operator*( float iLhs, const  FOdysseyStrokePoint&  iRhs );
FOdysseyStrokePoint operator/( float iLhs, const  FOdysseyStrokePoint&  iRhs );

