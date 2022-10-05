// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "Windows/COMPointer.h"
	#include <guiddef.h>
	#include <RTSCom.h>
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "IStylusState.h"

//---

/**
 * Specialized ink stylus state for a single frame.
 */
struct FInkStylusState
{
	FVector2D	Position;
	float		Z;
    float		Timer;
	FVector2D	Tilt;
	float		Azimuth;
	float		Altitude;
	float		Twist;
	float		NormalPressure;
	float		TangentPressure;
	FVector2D	Size;

	bool IsTouching : 1;
	bool IsInverted : 1;

	//---

	FInkStylusState()
		: Position(0, 0), Z(0), Timer(0), Tilt(0, 0), Azimuth(0), Altitude(0), Twist(0), NormalPressure(0), TangentPressure(0), Size(0, 0)
		, IsTouching(false), IsInverted(false)
	{
	}

	/** Convert this ink specialized state to the generic one */
	FStylusState ToPublicState() const
	{
		return FStylusState(Position, Z, Timer, Tilt, Azimuth, Altitude, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted);
	}
};

/**
 * Specialized ink packet types as derived from IRealTimeStylus::GetPacketDescriptionData or IInkTablet::IsPacketPropertySupported()
 */
enum class EInkPacketType
{
	None,
	X,				// == GUID_PACKETPROPERTY_GUID_X (or the string version STR_GUID_X )
	Y,				// == GUID_PACKETPROPERTY_GUID_Y
	Z,				// == GUID_PACKETPROPERTY_GUID_Z
	Status,			// == GUID_PACKETPROPERTY_GUID_STATUS
    Timer,			// == GUID_PACKETPROPERTY_GUID_TIMER_TICK
	NormalPressure, // == GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE
	TangentPressure,// == GUID_PACKETPROPERTY_GUID_TANGENT_PRESSURE
	ButtonPressure,	// == GUID_PACKETPROPERTY_GUID_BUTTON_PRESSURE
	Azimuth,		// == GUID_PACKETPROPERTY_GUID_AZIMUTH_ORIENTATION
	Altitude,		// == GUID_PACKETPROPERTY_GUID_ALTITUDE_ORIENTATION
	Twist,			// == GUID_PACKETPROPERTY_GUID_TWIST_ORIENTATION
	XTilt,			// == GUID_PACKETPROPERTY_GUID_X_TILT_ORIENTATION
	YTilt,			// == GUID_PACKETPROPERTY_GUID_Y_TILT_ORIENTATION
	Width,			// == GUID_PACKETPROPERTY_GUID_WIDTH
	Height,			// == GUID_PACKETPROPERTY_GUID_HEIGHT
};

/**
 * Description of a packet's information (property), as derived from PACKET_PROPERTY which is get from IRealTimeStylus::GetPacketDescriptionData.
 */
struct FInkPacketDescription
{
	EInkPacketType Type { EInkPacketType::None };
	int32 Minimum { 0 };
	int32 Maximum { 0 };
	float Resolution { 0 };
};

/**
 * Description of an input device (context info) specialized for Windows Ink driver.
 */
struct FInkTabletContextInfo
	: public IStylusInputDevice
{
public:
	/** The id of the input device */
	TABLET_CONTEXT_ID ID;
	/** The kind of the input device (mouse/pen/...) */
    TabletDeviceKind Kind;

public:
	/** List of all available properties for the given input device.
	 *  Get from IRealTimeStylus::GetPacketDescriptionData() of the tablet
	 *  Set when a new tablet is detected.
	 */
	TArray<FInkPacketDescription> PacketDescriptions;
	/** List of all available property types for the given input device.
	 *  Get from IInkTablet::IsPacketPropertySupported() of the tablet
	 *  (It should be more or less the same number as PacketDescriptions)
	 *  Set when a new tablet is detected.
	 */
	TArray<EInkPacketType> SupportedPackets;

	/** Fill all the available types (position/pressure/tilt/azimuth/...) in a generic way because it is used by the super class
	 *  Set when a new tablet is detected.
	 */
	void AddSupportedInput(EStylusInputType Type) { SupportedInputs.Add(Type); }

public:
	/** List of all new states received by the ink packets, converted to our ink state type.
	 *  This is an array because between 2 ticks of the subsystem, multiple ink packets can be received.
	 *  Filled each time an ink packet is get with FInkRealTimeStylusPlugin::HandlePacket.
	 */
	TArray<FInkStylusState> InkStates;
	/** To know when the stylus is on the tablet (between a down/up) */
    bool IsTouching;

public:
	/** To know when the InkStates must be processed by the subsystem tick */
	void SetDirty() { Dirty = true; }

	/** Called by the subsystem tick.
	 *  This will manage states in the super class:
	 *  - move the CurrentState inside the PreviousState
	 *  - convert InkStates to generic states
	 *  - fill the CurrentState with the new generic values
	 */
    virtual void Tick() override;
};

//---
//---
//---

/**
 * An implementation of an IStylusSyncPlugin for use with the RealTimeStylus API.
 */
class FInkRealTimeStylusPlugin : public IStylusSyncPlugin
{
public:
	FInkRealTimeStylusPlugin() = default;
	virtual ~FInkRealTimeStylusPlugin()
	{
		if (FreeThreadedMarshaller != nullptr)
		{
			FreeThreadedMarshaller->Release();
		}
	}

	virtual ULONG AddRef() override { return ++RefCount; }
	virtual ULONG Release() override
	{
		int NewRefCount = --RefCount;
		if (NewRefCount == 0)
			delete this;

		return NewRefCount;
	}

	virtual HRESULT QueryInterface(const IID& InterfaceID, void** Pointer) override;

public:
	/** Manage tablet detection.
	 *  Used to fill the tablet contexts.
	 */
	virtual HRESULT TabletAdded(IRealTimeStylus* RealTimeStylus, IInkTablet* InkTablet) override;
	virtual HRESULT TabletRemoved(IRealTimeStylus* RealTimeStylus, LONG iTabletIndex) override;

	/** Manage driver detection.
	 *  Used to fill the tablet contexts.
	 */
	virtual HRESULT RealTimeStylusEnabled(IRealTimeStylus* RealTimeStylus, ULONG Num, const TABLET_CONTEXT_ID* InTabletContexts) override;
	virtual HRESULT RealTimeStylusDisabled(IRealTimeStylus* RealTimeStylus, ULONG Num, const TABLET_CONTEXT_ID* InTabletContexts) override;

	/** Not used */
	virtual HRESULT StylusInRange(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletContext, STYLUS_ID StylusID) override { return S_OK; }
	/** Not used */
	virtual HRESULT StylusOutOfRange(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletContext, STYLUS_ID StylusID) override { return S_OK; }

	/** Manage stylus down/up.
	 *  Used to set/reset IsTouching variable of the tablet context.
	 */
	virtual HRESULT StylusDown(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketSize, LONG* Packet, LONG** InOutPackets) override;
	virtual HRESULT StylusUp(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketSize, LONG* Packet, LONG** InOutPackets) override;

	/** Not used */
	virtual HRESULT StylusButtonDown(IRealTimeStylus* RealTimeStylus, STYLUS_ID StylusID, const GUID* GUID, POINT* Position) override { return S_OK; }
	/** Not used */
	virtual HRESULT StylusButtonUp(IRealTimeStylus* RealTimeStylus, STYLUS_ID StylusID, const GUID* GUID, POINT* Position) override { return S_OK; }

	/** Manage all packets.
	 *  This is the "main loop".
	 */
	virtual HRESULT InAirPackets(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets, ULONG* NumOutPackets, LONG** PtrOutPackets) override
	{
		HandlePacket( RealTimeStylus, StylusInfo, PacketCount, PacketBufferLength, Packets );
		return S_OK;
	}
	virtual HRESULT Packets(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets, ULONG* NumOutPackets, LONG** PtrOutPackets) override
	{
		HandlePacket( RealTimeStylus, StylusInfo, PacketCount, PacketBufferLength, Packets );
		return S_OK;
	}

	/** Not used */
	virtual HRESULT CustomStylusDataAdded(IRealTimeStylus* RealTimeStylus, const GUID* GUID, ULONG Data, const BYTE* ByteData) override { return S_OK; }

	/** Not used */
	virtual HRESULT SystemEvent(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletContext, STYLUS_ID StylusID, SYSTEM_EVENT EventType, SYSTEM_EVENT_DATA EventData) override { return S_OK; }
	/** Not used */
	virtual HRESULT Error(IRealTimeStylus* RealTimeStylus, IStylusPlugin* Plugin, RealTimeStylusDataInterest DataInterest, HRESULT ErrorCode, LONG_PTR* Key) override { return S_OK; }

	/** Not used */
	virtual HRESULT UpdateMapping(IRealTimeStylus* RealTimeStylus) override { return S_OK; }

	/** To be able to override all above functions */
	virtual HRESULT DataInterest(RealTimeStylusDataInterest* OutDataInterest) override { *OutDataInterest = RTSDI_AllData; return S_OK; }

public:
	IUnknown* FreeThreadedMarshaller;

	/** The list of all tablets (input devices) connected */
	TArray<FInkTabletContextInfo> TabletContexts;

private:
	/** This is the main function which manages all the ink packets from the ink driver */
	void HandlePacket(IRealTimeStylus* RealTimeStylus, const StylusInfo* StylusInfo, ULONG PacketCount, ULONG PacketBufferLength, LONG* Packets);

	/** Find the corresponding context info of the given tablet id */
	FInkTabletContextInfo* FindTabletContext(TABLET_CONTEXT_ID TabletID);

	/** Manage new detected tablet */
	void AddTabletContext(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletID);
	/** Manage removed tablet */
	void RemoveTabletContext(IRealTimeStylus* RealTimeStylus, TABLET_CONTEXT_ID TabletID);

private:
	int RefCount { 1 };
};
