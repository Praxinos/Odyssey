// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IStylusState.h"
#include "Mac/CocoaWindow.h"

/**
 * NSEvent Stylus state for a single frame.
 */
struct FNSEventStylusState
{
    FVector2D          Position;
    float              Z;
    unsigned int       Timer;
    float              Azimuth;
    float              Altitude;
    float              Twist;
    FVector2D          Tilt;
    float              NormalPressure;
    float              TangentPressure;
    FVector2D          Size;
    bool               IsTouching : 1;
    bool               IsInverted : 1;
    
    FNSEventStylusState() :
        Position( 0, 0 ), Z( 0 ), Timer( 0 ), Azimuth( 0 ), Altitude( 0 ), Twist( 0 ), Tilt( 0, 0 ), NormalPressure( 0 ), TangentPressure( 0 ), Size( 0, 0 ),
        IsTouching( false ), IsInverted( false )
    {
    }

    //Set the Altitude and Azimuth fields based on the Tilt field
    void TiltToOrientation()
    {
        Azimuth = 0;
        if( Tilt.X != 0 )
        {
            Azimuth = PI/2 - FMath::Atan2( FMath::Cos( Tilt.X ) * FMath::Sin( Tilt.Y ), FMath::Cos( Tilt.Y ) * FMath::Sin( Tilt.X ) );
            if( Azimuth < 0 )
                Azimuth+= 2 * PI;
        }
        
        Altitude = PI / 2 - FMath::Acos(FMath::Cos( Tilt.X ) * FMath::Cos( Tilt.Y ) );
        
        Altitude = FMath::RadiansToDegrees( Altitude );
        Azimuth = FMath::RadiansToDegrees( Azimuth );
    }

    FStylusState ToPublicState() const
    {
        return FStylusState( Position, Z, Timer, Tilt, Azimuth, Altitude, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted );
    }
};

/**
* Description of an input device (context info) specialized for NSEvent.
*/
struct FNSEventTabletContextInfo 
    : public IStylusInputDevice
{
    //Is the stylus upside down ?
    bool mIsInverted;

    /** To know when the NSEvent States must be processed by the subsystem tick.
    We dirty it when we handle a serie of NSEvent packets*/
    void SetDirty() { Dirty = true; }

    /** Clear the context info when we switch context */
    void Clear() { CurrentState.Empty(); PreviousState.Empty(); }

    TArray< FNSEventStylusState > mPacketsBuffer;
    
    virtual void Tick() override;
};

/**
* A container to store the currently used (tablet) context.
*/
class FNSEventContext
{
public:
    FNSEventContext();
    ~FNSEventContext();
    
    /** Create a (tablet) context and link it to this windows */
    bool OpenContext( FCocoaWindow* iHwnd );
    /** Destroy the context when not needed anymore */
    void CloseContext();
    
private:
    /** The id of the monitor which receive the events */
    id mEventMonitor;

    /** The method which handles NSEvents and store them as packets in mTabletContext */
    NSEvent* HandleNSEvent(NSEvent* Event);

public:
    FNSEventTabletContextInfo mTabletContext;
};
