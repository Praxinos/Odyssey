// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Math/Vector2D.h"

/**
 * The types of stylus inputs that can be potentially supported by a stylus.
 */
enum class ODYSSEYSTYLUSINPUT_API EStylusInputType
{
	Position,
	Z,
	Timer,
	Pressure,
	Tilt,
	TangentPressure,
	ButtonPressure,
	Twist,
	Size
};

//---

/**
 * The current state of a single stylus, as sent by IStylusMessageHandler. 
 */
class ODYSSEYSTYLUSINPUT_API FStylusState
{
public:
	FStylusState()
		: Position(0, 0), Z(0), Timer(0), Tilt(0), TiltX(0), TiltY(0), Azimuth(0), Altitude(0), Twist(0)
		, Pressure(0), TangentPressure(0), Size(0, 0)
		, IsDown(false), IsInverted(false)
	{
	}

	FStylusState(FVector2D InPosition, float InZ, unsigned int InTimer, const FVector2D& InTilt, float InTwist,
		float InPressure, float InTanPressure, FVector2D InSize, 
		bool InDown, bool InInverted)
		: Position(InPosition), Z(InZ), Timer(InTimer), TiltX(InTilt.X), TiltY(InTilt.Y), Twist(InTwist)
		, Pressure(InPressure), TangentPressure(InTanPressure), Size(InSize)
		, IsDown(InDown), IsInverted(InInverted)
	{
		double tiltx_rad = FMath::DegreesToRadians(TiltX);
		double tilty_rad = FMath::DegreesToRadians(TiltY);
        
		double sin_tiltx = FMath::Sin(tiltx_rad);
        double sin_tilty = FMath::Sin(tilty_rad);
		
		double azimuth_rad = FMath::Atan2(sin_tilty, sin_tiltx);
		if (azimuth_rad < -PI / 2)
		{
			Azimuth = FMath::RadiansToDegrees(azimuth_rad + 5 * PI / 2);
		}
		else
		{
			Azimuth = FMath::RadiansToDegrees(azimuth_rad + PI / 2);
		}

		double dist = FVector2D::Distance({0, 0}, {sin_tiltx, sin_tilty});
		double ratiox = FMath::Abs(FMath::Cos(azimuth_rad));
		double ratioy = FMath::Abs(FMath::Sin(azimuth_rad));
		double ratio = ratiox > ratioy ? ratiox : ratioy;
		dist *= ratio;
		double tilt_rad = FMath::Asin(dist);
        Tilt = FMath::RadiansToDegrees(tilt_rad);
        
		Altitude = FMath::Cos(tilt_rad);
	}

	FStylusState(FVector2D InPosition, float InZ, unsigned int InTimer, double InAzimuth, double InAltitude, float InTwist,
		float InPressure, float InTanPressure, FVector2D InSize, 
		bool InDown, bool InInverted)
		: Position(InPosition), Z(InZ), Timer(InTimer), Azimuth(InAzimuth), Altitude(InAltitude), Twist(InTwist)
		, Pressure(InPressure), TangentPressure(InTanPressure), Size(InSize)
		, IsDown(InDown), IsInverted(InInverted)
	{
		double tilt_rad = FMath::Acos(Altitude);
		Tilt = FMath::RadiansToDegrees(tilt_rad);

		double dist = FMath::Sin(tilt_rad);
		double azimuth_rad = FMath::DegreesToRadians(Azimuth);

		double cosx = FMath::Cos(azimuth_rad) * dist;
		double siny = FMath::Sin(azimuth_rad) * dist;
		TiltX = FMath::RadiansToDegrees(FMath::Asin(cosx));
		TiltY = FMath::RadiansToDegrees(FMath::Asin(siny));
	}

	FStylusState(const FStylusState& Other)
		: Position(Other.Position), Z(Other.Z), Timer(Other.Timer), Tilt(Other.Tilt), TiltX(Other.TiltX), TiltY(Other.TiltY), Azimuth(Other.Azimuth), Altitude(Other.Altitude), Twist(Other.Twist)
		, Pressure(Other.Pressure), TangentPressure(Other.TangentPressure), Size(Other.Size)
		, IsDown(Other.IsDown), IsInverted(Other.IsInverted)
	{
	}

	/**
	 * The current position of the stylus on screen in pixels (floating point in screenspace coordinates).
	 */
	FVector2D GetPosition() const { return Position; }

	/**
	 * The current height of the stylus above the tablet.
	 * Defaults to 0 if EStylusInputType::Z is not supported.
	 */
	float GetZ() const { return Z; }

	/**
	 * Elapsed time in ms from the start of the tablet context
	 * Defaults to 0 if not supported
	 */
	unsigned int GetTimer() const { return Timer; }

	/** 
	 * The current tilt along the Z axis in degrees, normalized to the range of [0, 90]
	 * Defaults to 0 if EStylusInputType::Tilt is not supported.
	 * A value of 0 means that the stylus is perfectly vertical.
	 * A value of 90 means that the stylus is perfectly horizontal.
	 */
	double GetTilt() const { return Tilt; }

	/** 
	 * The current tilt along the X axis in degrees, normalized to the range of [-90, 90]. 
	 * Defaults to 0 if EStylusInputType::Tilt is not supported.
	 * A value of 0 means that the stylus is perfectly vertical.
	 * A value of -90 means that the stylus is perfectly horizontal and tilted to the left.
	 * A value of 90 means that the stylus is perfectly horizontal and tilted to the right.
	 */
	double GetTiltX() const { return TiltX; }

	/** 
	 * The current tilt along the Y axis in degrees, normalized to the range of [-90, 90]. 
	 * Defaults to 0 if EStylusInputType::Tilt is not supported.
	 * A value of 0 means that the stylus is perfectly vertical.
	 * A value of -90 means that the stylus is perfectly horizontal and tilted away from the user.
	 * A value of 90 means that the stylus is perfectly horizontal and tilted towards the user.
	 */
	double GetTiltY() const { return TiltY; }

	/**
	 * The current azimuth, normalized to the range of [0, 360].
	 * Defaults to 0 if EStylusInputType::Tilt is not supported.
	 * A value of 0 means that the stylus is tilted forwards, away from the user.
	 * A value of 90 means that the stylus is tilted to the right direction, and so on in clockwise.
	 */
	float GetAzimuth() const { return Azimuth; }

	/**
	 * The current altitude of the stylus eraser (if drawing with the tip) or the stylus tip (if drawing with the eraser), normalized to the range of [0, 1].
	 * Defaults to 0 if EStylusInputType::Tilt is not supported.
	 * A value of 1 means the stylus is perfectly vertical
	 * A value of 0 means the stylus is perfectly horizontal
	 */
	float GetAltitude() const { return Altitude; }

	/**
	 * The current twist amount around the stylus' own axis in degrees, normalized to the range of [0, 360).
	 * Defaults to 0 if EStylusInputType::Twist is not supported.
	 * A value of 360 represents a full rotation clockwise.
	 * TODO: define where is the 0 point (same as azimuth ?)
	 */
	float GetTwist() const { return Twist; }

	/**
	 * Get the current pressure along the tablet's normal, usually straight down.
	 * Normalized to the range [0, 1]. 
	 * Defaults to 0 if EStylusInputType::Pressure is not supported.
	 */
	float GetPressure() const { return Pressure; }

	/**
	 * Get the current pressure along the tablet's surface.
	 * Normalized to the range [0, 1].
	 * Defaults to 0 if EStylusInputType::TangentPressure is not supported.
	 */
	float GetTangentPressure() const { return TangentPressure; }

	/**
	 * Get the size of the touch in logical coordinates.
	 * Defaults to (0,0) if EStylusInputType::Size is not supported.
	 */
	FVector2D GetSize() const { return Size; }
	
	/**
	 * Is the stylus inverted? Ie. the eraser part is pointing down. 
	 * Defaults to false if EStylusInputType::Tilt is not supported.
	 */
	bool IsStylusInverted() const { return IsInverted; }

	/** Is the stylus currently touching the tablet? */
	bool IsStylusDown() const { return IsDown; }

private:
	FVector2D	Position;
	float		Z;
    unsigned int Timer;
	double	Tilt;
	double	TiltX;
	double	TiltY;
	float		Azimuth;
	float		Altitude;
	float		Twist;
	float		Pressure;
	float		TangentPressure;
	FVector2D	Size;

	bool IsDown : 1;
	bool IsInverted : 1;
};

//---

/** An input device representing a stylus and its current state. */
class ODYSSEYSTYLUSINPUT_API IStylusInputDevice
{
public:
	virtual ~IStylusInputDevice() {}

	/**
	 * Get the current stylus states.
	 * This is an array because between 2 subsystem ticks, multiple tablet packets can be received.
	 */
	const TArray<FStylusState>& GetCurrentState() const { return CurrentState; }

	/**
	 * Get the previous stylus states.
	 * This is an array because between 2 subsystem ticks, multiple tablet packets can be received.
	 */
	const TArray<FStylusState>& GetPreviousState() const { return PreviousState; }

	/**
	 * Get the supported inputs of this tablet to know which data in FStylusState are valid or not.
	 */
	const TArray<EStylusInputType>& GetSupportedInputs() const { return SupportedInputs; }

public:
	/** Update the input device. Not intended to be called externally. */ 
	virtual void Tick() = 0;

	/** Does the input device need to be ticked? */
	bool IsDirty() const { return Dirty; }

protected:
	TArray<FStylusState>		CurrentState;
	TArray<FStylusState>		PreviousState;
	TArray<EStylusInputType>	SupportedInputs;

	bool Dirty : 1;
};

//---

/**
 * Interface to implement for classes that want to receive messages when a stylus state change occurs.
 * Will trigger once per frame.
 */
class IStylusMessageHandler
{
public:
	virtual void OnStylusStateChanged(const TWeakPtr<SWidget> iWidget, const FStylusState& NewState, int32 StylusIndex) = 0;
};
