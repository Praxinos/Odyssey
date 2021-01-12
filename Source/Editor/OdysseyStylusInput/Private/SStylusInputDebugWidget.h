// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Widgets/SCompoundWidget.h"

#include "IOdysseyStylusInputModule.h"
#include "IStylusState.h"

class SStylusInputDebugWidget : public SCompoundWidget, 
	public IStylusMessageHandler
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
	FText GetTiltText() const { return GetVector2Text(State.GetTilt()); }
	FText GetSizeText() const { return GetVector2Text(State.GetSize()); }

	FText GetIndexText() const { return FText::FromString(FString::FromInt(LastIndex)); }

	FText GetPressureText() const { return GetFloatText(State.GetPressure()); }
	FText GetTangentPressureText() const { return GetFloatText(State.GetTangentPressure()); }
	FText GetZText() const { return GetFloatText(State.GetZ()); }
	FText GetTwistText() const { return GetFloatText(State.GetTwist()); }

	static FText GetVector2Text(FVector2D Value);
	static FText GetFloatText(float Value);
};
