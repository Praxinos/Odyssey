// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h" 
#include "Input/OdysseyPoint.h"

/////////////////////////////////////////////////////
// FOdysseyRay
struct ODYSSEYCORE_API FOdysseyRay
{
public:
    // Construction / Destruction
    FOdysseyRay(         FVector        iRayOrigin                              = FVector(0,0,0)
                       , FVector        iRayDirection                           = FVector(0,0,0)
                       , FOdysseyPoint  iPoint                                  = FOdysseyPoint::ZeroPoint()
                       );

public:
    // Makers
    static FOdysseyRay DefaultRay();
    static FOdysseyRay ZeroRay();
    
public:
    // Operators
    bool                 operator==( const FOdysseyRay& iRhs ) const;
    FOdysseyRay& operator= ( const FOdysseyRay& iRhs );
    FOdysseyRay& operator+=( const FOdysseyRay& iRhs );
    FOdysseyRay& operator-=( const FOdysseyRay& iRhs );
    FOdysseyRay  operator+ ( const FOdysseyRay& iRhs ) const;
    FOdysseyRay  operator- ( const FOdysseyRay& iRhs ) const;
    FOdysseyRay& operator*=( float iRhs );
    FOdysseyRay& operator/=( float iRhs );
    FOdysseyRay  operator* ( float iRhs ) const;
    FOdysseyRay  operator/ ( float iRhs ) const;
    
    //void ComputeRelativeParameters(const FOdysseyPoint& iPreviousPoint, bool iComputeTravelledDistance = false);

public: 
    //Returns the average of two rays (It is NOT (iRay1 + iRay2) / 2)
    static FOdysseyRay Average( const FOdysseyRay& iRay1, const FOdysseyRay& iRay2 );
    
    //Same with multiple rays
    static FOdysseyRay Average( const TArray< FOdysseyRay>& iRays );
    
    //Returns the linear interpolation between two rays (It is NOT iRay1 + (iRay2 - iRay1 ) * iT )
    static FOdysseyRay Lerp( const FOdysseyRay& iRay1, const FOdysseyRay& iRay2, float iT );

public:
    // Members
    FVector mRayOrigin;
    FVector mRayDirection;
    FOdysseyPoint mPoint;
};

//---

//FOdysseyPoint operator*( float iLhs, const FOdysseyPoint& iRhs );
//FOdysseyPoint operator/( float iLhs, const FOdysseyPoint& iRhs );
