// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IStylusState.h"
#include <windows.h>

// Implementation of the Native driver
class FNativeStylusInputDevice
	: public IStylusInputDevice
{
public:
	virtual ~FNativeStylusInputDevice() {};
	virtual void Tick() override;
	
public:
	void SetPenMask(const PEN_MASK& iMask);
	void OnPointerUpdate(POINTER_PEN_INFO& iPenInfo);

private:
	TArray<FStylusState> mNativeState;
};
