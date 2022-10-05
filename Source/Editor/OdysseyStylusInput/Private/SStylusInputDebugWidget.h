// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Widgets/SCompoundWidget.h"

#include "IOdysseyStylusInputModule.h"
#include "IStylusState.h"

// A debug widget which implements the IStylusMessageHandler interface to get stylus messages
class SStylusInputDebugWidget
	: public SCompoundWidget
	, public IStylusMessageHandler
{
public:
	SStylusInputDebugWidget();
	virtual ~SStylusInputDebugWidget();

	SLATE_BEGIN_ARGS(SStylusInputDebugWidget)
	{}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, UOdysseyStylusInputSubsystem& InSubsystem);
	void OnStylusStateChanged(const TWeakPtr<SWidget> iWidget, const FStylusState& InState, int32 InIndex)
	{
		State = InState;
		LastIndex = InIndex;
	}

private:
    UOdysseyStylusInputSubsystem* InputSubsystem;
	FStylusState State;
	int32 LastIndex;

	ECheckBoxState IsTouching() const;
	ECheckBoxState IsInverted() const;

	FText GetPositionText() const { return GetVector2Text(State.GetPosition()); }
	bool IsPositionAvailable() const { return IsAvailable( EStylusInputType::Position ); }
	FText GetZText() const { return GetFloatText(State.GetZ()); }
	bool IsZAvailable() const { return IsAvailable( EStylusInputType::Z ); }

	FText GetTiltText() const { return GetVector2Text(State.GetTilt()); }
	bool IsTiltAvailable() const { return IsAvailable( EStylusInputType::Tilt ); }
	FText GetAzimuthText() const { return GetFloatText(State.GetAzimuth()); }
	bool IsAzimuthAvailable() const { return IsAvailable( EStylusInputType::Azimuth ); }
	FText GetAltitudeText() const { return GetFloatText(State.GetAltitude()); }
	bool IsAltitudeAvailable() const { return IsAvailable( EStylusInputType::Altitude ); }
	FText GetTwistText() const { return GetFloatText(State.GetTwist()); }
	bool IsTwistAvailable() const { return IsAvailable( EStylusInputType::Twist ); }

	FText GetPressureText() const { return GetFloatText(State.GetPressure()); }
	bool IsPressureAvailable() const { return IsAvailable( EStylusInputType::Pressure ); }
	FText GetTangentPressureText() const { return GetFloatText(State.GetTangentPressure()); }
	bool IsPressureTangentAvailable() const { return IsAvailable( EStylusInputType::TangentPressure ); }

	FText GetSizeText() const { return GetVector2Text(State.GetSize()); }
	bool IsSizeAvailable() const { return IsAvailable( EStylusInputType::Size ); }

	FText GetIndexText() const { return FText::FromString(FString::FromInt(LastIndex)); }

	//...

	bool IsAvailable( EStylusInputType iType ) const
	{
		const IStylusInputDevice* device = InputSubsystem->GetInputDevice( LastIndex );
		if( !device )
			return false;

		return device->GetSupportedInputs().Contains( iType );
	}

	static FText GetVector2Text(FVector2D Value);
	static FText GetFloatText(float Value);
};
