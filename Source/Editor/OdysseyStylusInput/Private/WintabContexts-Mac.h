// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_MAC

#include "IStylusState.h"
#include "WintabContexts-Cocoa.h"
#include "Mac/CocoaWindow.h"

#if __LP64__
typedef unsigned int                    UInt32;
typedef signed int                      SInt32;
#else
typedef unsigned long                   UInt32;
typedef signed long                     SInt32;
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
struct FWintabStylusState
{
	FVector2D Position;
	float Z;
    float Azimuth;
    float Altitude;
	float Twist;
	float NormalPressure;
	float TangentPressure;
	FVector2D Size;
	bool IsTouching : 1;
	bool IsInverted : 1;
    
    FWintabStylusState() :
		Position(0, 0), Z(0), Azimuth(0), Altitude(0), Twist(0), NormalPressure(0), TangentPressure(0),
		Size(0, 0), IsTouching(false), IsInverted(false)
	{
	}

    FVector2D OrientationToTilt() const
    {
        return FVector2D( 0, 0 );
    }

	FStylusState ToPublicState() const
	{
		return FStylusState(Position, Z, OrientationToTilt(), Azimuth, Altitude, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted);
	}
};

/**
 * Description of a packet's information, as derived from IRealTimeStylus::GetPacketDescriptionData.
 */
struct FWTPacketDescription
{
	EWintabPacketType Type { EWintabPacketType::None };
	int32 Minimum { 0 };
	int32 Maximum { 0 };
	float Resolution { 0 };
};

struct FWTTabletContextInfo : public IStylusInputDevice
{
    UInt32 mContextID;
	UInt32 mTabletOfContext;
    
	TArray<FWintabStylusState> WindowsState;
    
    WintabContextCocoa* mContext;
        
    bool IsTouching;

    void SetDirty() { Dirty = true; }


    /*
	TArray<FWTPacketDescription> PacketDescriptions;
	TArray<EWintabPacketType> SupportedPackets;

    TArray< PACKET > mPacketsBuffer;

	TArray<FWintabStylusState> WindowsState;
    

	void AddSupportedInput(EStylusInputType Type) { SupportedInputs.Add(Type); }
	void CleanSupportedInput() { SupportedInputs.Empty(); }
     */
  
    
    
    virtual void Tick() override;
};

/**
 * An implementation of an IStylusSyncPlugin for use with the RealTimeStylus API.
 */
class FWintabContexts
{
public:
    FWintabContexts();
    ~FWintabContexts();
    
    bool OpenTabletContexts( FCocoaWindow* iHwnd );
    void CloseTabletContexts();
    
public:
	TArray<FWTTabletContextInfo> mTabletContexts;

private:
    id mEventMonitor;
    
    NSEvent* HandleNSEvent(NSEvent* Event);


};

#endif // PLATFORM_MAC
