// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "IOdysseyStylusInputModule.h"

class FNativeStylusInputInterfaceImpl;

// Implementation of the Native driver
class FNativeStylusInputInterface
    : public IStylusInputInterfaceInternal
{
public:
    FNativeStylusInputInterface(TUniquePtr<FNativeStylusInputInterfaceImpl> InImpl);
    virtual ~FNativeStylusInputInterface();

    virtual void Tick() override;
    virtual int32 NumInputDevices() const override;
    virtual IStylusInputDevice* GetInputDevice(int32 Index) const override;

    virtual TWeakPtr<SWindow> Window() const override;
    virtual TWeakPtr<SWidget> Widget() const override;

private:
    // Impl to avoid including Native headers.
    TUniquePtr<FNativeStylusInputInterfaceImpl> Impl;
};

TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterfaceNative();
