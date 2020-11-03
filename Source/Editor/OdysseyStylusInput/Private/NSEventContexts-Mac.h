// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_MAC

#include "IStylusState.h"
#include "Mac/CocoaWindow.h"

#if __LP64__
typedef unsigned int                    UInt32;
#else
typedef unsigned long                   UInt32;
#endif

/**
 * Packet types as derived from IRealTimeStylus::GetPacketDescriptionData.
 */
enum class EWintabPacketType
{
	None,
	X,
	Y,
	Z,
	Status,
    Timer,
	NormalPressure,
	TangentPressure,
	ButtonPressure,
	Azimuth,
	Altitude,
	Twist,
	Width,
	Height,
};

/**
 * Stylus state for a single frame.
 */
struct FNSEventStylusState
{
	FVector2D Position;
	float Z;
    unsigned int Timer;
    float Azimuth;
    float Altitude;
	float Twist;
    FVector2D Tilt;
	float NormalPressure;
	float TangentPressure;
	FVector2D Size;
	bool IsTouching : 1;
	bool IsInverted : 1;
    
    FNSEventStylusState() :
		Position(0, 0), Z(0), Timer(0), Azimuth(0), Altitude(0), Twist(0), Tilt(0, 0), NormalPressure(0), TangentPressure(0),
		Size(0, 0), IsTouching(false), IsInverted(false)
	{
	}

    //Set the Altitude and Azimuth fields based on what is currently inside Tilt
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
		return FStylusState(Position, Z, Timer, Tilt, Azimuth, Altitude, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted);
	}
};

struct FWTTabletContextInfo : public IStylusInputDevice
{
    bool mIsInverted;
    void SetDirty() { Dirty = true; }
    bool IsDirty() { return Dirty; }
    void Clear() { CurrentState.Empty(); PreviousState.Empty(); }

    TArray< FNSEventStylusState > mPacketsBuffer;
    
    virtual void Tick() override;
};

/**
 * An implementation of an IStylusSyncPlugin for use with the RealTimeStylus API.
 */
class FNSEventContexts
{
public:
    FNSEventContexts();
    ~FNSEventContexts();
    
    bool OpenContext( FCocoaWindow* iHwnd );
    void CloseContext();
    
public:
	FWTTabletContextInfo mTabletContext;

private:
    id mEventMonitor;
    
    NSEvent* HandleNSEvent(NSEvent* Event);
};

#endif // PLATFORM_MAC
