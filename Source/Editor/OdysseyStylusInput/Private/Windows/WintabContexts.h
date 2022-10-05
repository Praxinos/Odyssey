// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Windows/WindowsHWrapper.h"

#include "WintabLibrary.h"

// Definition of which data/properties we want inside wintab packets
// Processed in the following wintab #include
// (The #include contains examples)
#define PACKETDATA	(PK_X | PK_Y | PK_Z | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE | PK_CURSOR | PK_SERIAL_NUMBER | PK_TIME | PK_CHANGED | PK_STATUS | PK_ORIENTATION | PK_ROTATION )
#define PACKETMODE	PK_BUTTONS
#include "Windows/PKTDEF.H"

#include "IStylusState.h"

//---

/**
 * Specialized wintab stylus state for a single frame.
 */
struct FWintabStylusState
{
    FVector2D   Position;
    float       Z;
    unsigned long Timer;
    float       Azimuth;
    float       Altitude;
    float       Twist;
    float       NormalPressure;
    float       TangentPressure;
    FVector2D   Size;

    bool IsTouching : 1;
    bool IsInverted : 1;

    FWintabStylusState()
        : Position( 0, 0 ), Z( 0 ), Timer( 0 ), Azimuth( 0 ), Altitude( 0 ), Twist( 0 ), NormalPressure( 0 ), TangentPressure( 0 ), Size( 0, 0 )
        , IsTouching( false ), IsInverted( false )
    {
    }

    /** Convert altitude/azimuth values to tilt value
     *  Wintab doesn't have native tilt value, it must be computed from altitude/azimuth
     *  This will be more convenient to the end developer to not have to manage this computation in its window/widget/...
     * 
     *  TODO: To make it clean:
     *  - Tilt enum must be added in EWintabPacketType
     *  - Tilt enum must be added in FWintabTabletContextInfo::SupportedPackets AND FWintabTabletContextInfo::AddSupportedInput() inside SetupTabletSupportedPackets() in .cpp
     *  - Then, once the wintab state is converted to the generic state in the FWintabTabletContextInfo::Tick() through FWintabStylusState::ToPublicState(), the conversion from altitude/azimuth to tilt is done here
     * 
     *  For the moment, it's not managed, but here is some links to make the computation:
     *  - https://gist.github.com/telegraphic/841212e8ab3252f5cffe
     *  - https://www.mathworks.com/help/phased/ref/azel2phitheta.html
     *  - https://code.woboq.org/qt5/qtbase/src/plugins/platforms/windows/qwindowstabletsupport.cpp.html#590
     *  Don't forget that the result must feet the requirements inside FStylusState::GetTilt() comment.
     * 
     */
    FVector2D OrientationToTilt() const
    {
        return FVector2D( 0, 0 );
    }

    /** Convert this wintab specialized state to the generic one */
    FStylusState ToPublicState() const
    {
        return FStylusState( Position, Z, Timer, OrientationToTilt(), Azimuth, Altitude, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted );
    }
};

/**
 * Specialized wintab packet types as derived from FWintabLibrary::WTInfoW().
 */
enum class EWintabPacketType
{
	None,
	X,                  // == DVC_X
	Y,                  // == DVC_Y
	Z,                  // == DVC_Z
    Timer,              // simulated inside Tick(), it seems there are problems with the real packet value
	NormalPressure,     // == DVC_NPRESSURE
	TangentPressure,    // == DVC_TPRESSURE
	Azimuth,            // == DVC_ORIENTATION
	Altitude,           // == DVC_ORIENTATION
	Twist,              // == DVC_ORIENTATION
};

/**
 * Description of a packet's information, as derived from FWintabLibrary::WTInfoW().
 */
struct FWintabPacketDescription
{
	EWintabPacketType Type { EWintabPacketType::None };
	int32 Minimum { 0 };
	int32 Maximum { 0 };
	float Resolution { 0 };
};

/**
 * Description of an input device (context info) specialized for Wintab driver.
 */
struct FWintabTabletContextInfo
    : public IStylusInputDevice
{
public:
    /** The context handle of the input device */
    HCTX mTabletContext;

public:
    /** List of all available properties for the given input device.
     *  Get from FWintabLibrary::WTInfoW() of the tablet
     *  Set when a new tablet is detected.
     */
	TArray<FWintabPacketDescription> PacketDescriptions;
    /** List of all available property types for the given input device.
     *  Deduce from PacketDescriptions
     *  Set when a new tablet is detected.
     */
	TArray<EWintabPacketType> SupportedPackets;

    /** Fill all the available types (position/pressure/tilt/azimuth/...) in a generic way because it is used by the super class
     *  Set when a new tablet is detected.
     */
	void AddSupportedInput(EStylusInputType Type) { SupportedInputs.Add(Type); }
    /** Empty the generic supported input */
	void CleanSupportedInput() { SupportedInputs.Empty(); }

public:
    /** List of all new states received by the wintab packets, converted to our wintab state type.
     *  This is an array because between 2 ticks of the subsystem, multiple wintab packets can be returned.
     *  Filled by FWintabLibrary::WTPacketsGet() inside the tick.
     */
	TArray<FWintabStylusState> WintabStates;
    /** To know when the stylus is on the tablet (between a down/up) */
    bool IsTouching;

public:
    /** To know when the WintabStates must be processed by the subsystem tick
     *  For wintab, this must always be the case, because the wintab packets are directly processed inside the subsystem tick
     */
	void SetDirty() { Dirty = true; }

    /** Called by the subsystem tick.
     *  This will manage states in the super class:
     *  - get new packets from wintab driver
     *  - fill WintabStates from wintab packets
     *  - move the CurrentState inside the PreviousState
     *  - convert WintabStates to generic states
     *  - fill the CurrentState with the new generic states
     */
    virtual void Tick() override;

    /** List of all new available wintab packets directly from wintab driver in Tick() */
    TArray< PACKET > mPacketsBuffer;
};

/**
 * A container to store all wintab contexts (tablets) detected.
 */
class FWintabContexts
{
public:
    FWintabContexts();
    ~FWintabContexts();

    /** Create all contexts (tablets) and link them to this windows */
    bool OpenTabletContexts( HWND iHwnd );
    /** Destroy all contexts */
    void CloseTabletContexts();

public:
	TArray<FWintabTabletContextInfo> mTabletContexts;
};
