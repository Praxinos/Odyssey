// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS

#include "Windows/WindowsHWrapper.h"
#include "WintabLibrary-Windows.h"

#include "IStylusState.h"

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
	XTilt,
	YTilt,
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
	FVector2D Tilt;
	float Twist;
	float NormalPressure;
	float TangentPressure;
	FVector2D Size;
	bool IsTouching : 1;
	bool IsInverted : 1;

    FWintabStylusState() :
		Position(0, 0), Z(0), Tilt(0, 0), Twist(0), NormalPressure(0), TangentPressure(0),
		Size(0, 0), IsTouching(false), IsInverted(false)
	{
	}

	FStylusState ToPublicState() const
	{
		return FStylusState(Position, Z, Tilt, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted);
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
    HCTX mTabletContext;

	TArray<FWTPacketDescription> PacketDescriptions;
	TArray<EWintabPacketType> SupportedPackets;

	TArray<FWintabStylusState> WindowsState;
    bool IsTouching;

	void AddSupportedInput(EStylusInputType Type) { SupportedInputs.Add(Type); }
	void CleanSupportedInput() { SupportedInputs.Empty(); }

	void SetDirty() { Dirty = true; }

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

    bool OpenTabletContexts( HWND iHwnd );
    void CloseTabletContexts();

public:
	TArray<FWTTabletContextInfo> mTabletContexts;
};

#endif // PLATFORM_WINDOWS