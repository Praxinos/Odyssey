// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h" 

/////////////////////////////////////////////////////
// FOdysseyStrokePoint
struct ODYSSEYSTROKEPIPELINE_API FOdysseyStrokePoint
{
public:
    // Construction / Destruction
    FOdysseyStrokePoint( float          iX                              = 0
                       , float          iY                              = 0
                       , float          iZ                              = 0
                       , float          iPressure                       = 1.f
                       , unsigned long  iTime                           = 0
                       , float          iAltitude                       = 0
                       , float          iAzimuth                        = 0
                       , float          iTwist                          = 0
                       , float          iPitch                          = 0
                       , float          iRoll                           = 0
                       , float          iYaw                            = 0
                       , float          iDistanceTravelled              = 0
                       , FVector2D      iDirectionVectorTangent         = FVector2D(0,0)
                       , FVector2D      iDirectionVectorNormal          = FVector2D(0,0)
                       , float          iDirectionAngleDegreesTangent   = 0
                       , float          iDirectionAngleDegreesNormal    = 0
                       , FVector2D      iSpeed                          = FVector2D(0,0)
                       , FVector2D      iAcceleration                   = FVector2D(0,0)
                       , FVector2D      iJolt                           = FVector2D(0,0)
                       , FVector2D      iDeltaPosition                  = FVector2D(0,0)
                       , int            iDeltaTime                      = 0
                       , TArray<FKey>   iKeysDown                       = TArray<FKey>()
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

public: 
    //Returns the average of two points (It is NOT (iPt1 + iPt2) / 2)
    static FOdysseyStrokePoint Average( const FOdysseyStrokePoint& iPt1, const FOdysseyStrokePoint& iPt2 );
    
    //Same with multiple points
    static FOdysseyStrokePoint Average( const TArray< FOdysseyStrokePoint>& iPoints );

    //Returns the linear interpolation between two points (It is NOT iPt1 + (iPt2 - iPt1 ) * iT )
    static FOdysseyStrokePoint Lerp( const FOdysseyStrokePoint& iPt1, const FOdysseyStrokePoint& iPt2, float iT );


public: //TODO: rename to m* or even better Get/Set*() ...
    // Members
    float           x;
    float           y;
    float           z;
    float           pressure;
    unsigned long   time;
    float           altitude;
    float           azimuth;
    float           twist;
    float           pitch;
    float           roll;
    float           yaw;
    float           distance_travelled;
    FVector2D       direction_vector_tangent;
    FVector2D       direction_vector_normal;
    float           direction_angle_deg_tangent;
    float           direction_angle_deg_normal;
    FVector2D       speed;
    FVector2D       acceleration;
    FVector2D       jolt;
    FVector2D       deltaPosition;
    int             deltaTime;
    TArray<FKey>    keysDown;
};

//---

FOdysseyStrokePoint operator*( float iLhs, const FOdysseyStrokePoint& iRhs );
FOdysseyStrokePoint operator/( float iLhs, const FOdysseyStrokePoint& iRhs );
