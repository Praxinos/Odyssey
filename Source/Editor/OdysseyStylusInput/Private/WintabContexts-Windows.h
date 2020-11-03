// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS

#include "Windows/WindowsHWrapper.h"
#include "WintabLibrary-Windows.h"

#define PACKETDATA	(PK_X | PK_Y | PK_Z | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_TANGENT_PRESSURE | PK_CURSOR | PK_SERIAL_NUMBER | PK_TIME | PK_CHANGED | PK_STATUS | PK_ORIENTATION | PK_ROTATION )
#define PACKETMODE	PK_BUTTONS
#include "Windows/PKTDEF.H"

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
struct FWintabStylusState
{
	FVector2D Position;
	float Z;
    unsigned long Timer;
    float Azimuth;
    float Altitude;
	float Twist;
	float NormalPressure;
	float TangentPressure;
	FVector2D Size;
	bool IsTouching : 1;
	bool IsInverted : 1;

    FWintabStylusState() :
		Position(0, 0), Z(0), Timer(0), Azimuth(0), Altitude(0), Twist(0), NormalPressure(0), TangentPressure(0),
		Size(0, 0), IsTouching(false), IsInverted(false)
	{
	}

    FVector2D OrientationToTilt() const
    {
        return FVector2D( 0, 0 );

    //    // https://gist.github.com/telegraphic/841212e8ab3252f5cffe
    //    // https://www.mathworks.com/help/phased/ref/azel2phitheta.html

    //    float cos_theta = FMath::Cos( Altitude ) * FMath::Cos( Azimuth );
    //    float theta /*tilt.X*/ = FMath::Acos( cos_theta );

    //    //float tan_phi = FMath::Tan( Altitude ) / FMath::Sin( Azimuth );
    //    float phi /*tilt.Y*/ = FMath::Atan2( FMath::Tan( Altitude ), FMath::Sin( Azimuth ) );
    //    phi = FMath::Fmod( phi + 2 * PI, 2 * PI );

    //    return FVector2D( theta, phi );

        // https://code.woboq.org/qt5/qtbase/src/plugins/platforms/windows/qwindowstabletsupport.cpp.html#590
        //// Convert from azimuth and altitude to x tilt and y tilt. What
        //// follows is the optimized version. Here are the equations used:
        //// X = sin(azimuth) * cos(altitude)
        //// Y = cos(azimuth) * cos(altitude)
        //// Z = sin(altitude)
        //// X Tilt = arctan(X / Z)
        //// Y Tilt = arctan(Y / Z)
        ////TOTEST
        //const float radAzim = FMath::DegreesToRadians( Azimuth / 10.0 );
        //const float tanAlt = FMath::Tan( FMath::DegreesToRadians( std::abs( Altitude / 10.0 ) ) );
        //const float radX = FMath::Tan( FMath::Sin( radAzim ) / tanAlt );
        //const float radY = FMath::Tan( FMath::Cos( radAzim ) / tanAlt );
        //float tiltX = FMath::RadiansToDegrees( radX );
        //float tiltY = FMath::RadiansToDegrees( -radY );

        //return FVector2D( tiltX, tiltY );
    }

	FStylusState ToPublicState() const
	{
		return FStylusState(Position, Z, Timer, OrientationToTilt(), Azimuth, Altitude, Twist, NormalPressure, TangentPressure, Size, IsTouching, IsInverted);
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

    TArray< PACKET > mPacketsBuffer;

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
