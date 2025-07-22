// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "IOdysseyStylusInputModule.h"

class FNSEventStylusInputInterfaceImpl;

// Implementation of the NSEvent (MacOSX) driver
class FNSEventStylusInputInterface
    : public IStylusInputInterfaceInternal
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
    // Impl to avoid including MacOSX headers.
    TUniquePtr<FNSEventStylusInputInterfaceImpl> Impl;
};

TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceNSEvent();
