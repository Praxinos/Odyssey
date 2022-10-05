// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h" 

/////////////////////////////////////////////////////
// FOdysseyPoint
struct ODYSSEYCORE_API FOdysseyPoint
{
public:
    // Construction / Destruction
    FOdysseyPoint( float          iX                              = 0
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
    static FOdysseyPoint DefaultPoint();
    static FOdysseyPoint ZeroPoint();

public:
    // Operators
    bool                 operator==( const FOdysseyPoint& iRhs ) const;
    FOdysseyPoint& operator= ( const FOdysseyPoint& iRhs );
    FOdysseyPoint& operator+=( const FOdysseyPoint& iRhs );
    FOdysseyPoint& operator-=( const FOdysseyPoint& iRhs );
    FOdysseyPoint  operator+ ( const FOdysseyPoint& iRhs ) const;
    FOdysseyPoint  operator- ( const FOdysseyPoint& iRhs ) const;
    FOdysseyPoint& operator*=( float iRhs );
    FOdysseyPoint& operator/=( float iRhs );
    FOdysseyPoint  operator* ( float iRhs ) const;
    FOdysseyPoint  operator/ ( float iRhs ) const;

    void ComputeRelativeParameters(const FOdysseyPoint& iPreviousPoint, bool iComputeTravelledDistance = false);

public: 
    //Returns the average of two points (It is NOT (iPt1 + iPt2) / 2)
    static FOdysseyPoint Average( const FOdysseyPoint& iPt1, const FOdysseyPoint& iPt2 );
    
    //Same with multiple points
    static FOdysseyPoint Average( const TArray< FOdysseyPoint>& iPoints );

    //Returns the linear interpolation between two points (It is NOT iPt1 + (iPt2 - iPt1 ) * iT )
    static FOdysseyPoint Lerp( const FOdysseyPoint& iPt1, const FOdysseyPoint& iPt2, float iT );


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

FOdysseyPoint operator*( float iLhs, const FOdysseyPoint& iRhs );
FOdysseyPoint operator/( float iLhs, const FOdysseyPoint& iRhs );
