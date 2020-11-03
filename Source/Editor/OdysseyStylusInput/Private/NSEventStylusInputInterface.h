// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "IOdysseyStylusInputModule.h"

class FNSEventStylusInputInterfaceImpl;

class FNSEventStylusInputInterface : public IStylusInputInterfaceInternal
{
public:
	FNSEventStylusInputInterface(TUniquePtr<FNSEventStylusInputInterfaceImpl> InImpl);
	virtual ~FNSEventStylusInputInterface();

	virtual void Tick() override;
	virtual int32 NumInputDevices() const override;
	virtual IStylusInputDevice* GetInputDevice(int32 Index) const override;

    virtual TWeakPtr<SWindow> Window() const override;
    virtual TWeakPtr<SWidget> Widget() const override;

private:
	// pImpl to avoid including Wintab headers.
	TUniquePtr<FNSEventStylusInputInterfaceImpl> Impl;
};
