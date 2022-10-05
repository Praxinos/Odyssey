// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "IOdysseyStylusInputModule.h"

class FWintabStylusInputInterfaceImpl;

// Implementation of the Wintab driver
class FWintabStylusInputInterface
	: public IStylusInputInterfaceInternal
{
public:
	FWintabStylusInputInterface(TUniquePtr<FWintabStylusInputInterfaceImpl> InImpl);
	virtual ~FWintabStylusInputInterface();

	virtual void Tick() override;
	virtual int32 NumInputDevices() const override;
	virtual IStylusInputDevice* GetInputDevice(int32 Index) const override;

    virtual TWeakPtr<SWindow> Window() const override;
    virtual TWeakPtr<SWidget> Widget() const override;

private:
	// Impl to avoid including Wintab headers.
	TUniquePtr<FWintabStylusInputInterfaceImpl> Impl;
};

TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceWintab();
