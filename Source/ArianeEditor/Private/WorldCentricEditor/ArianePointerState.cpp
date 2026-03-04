// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane
#include "ArianePointerState.h"


FArianePointerState::~FArianePointerState()
{
}

FArianePointerState::FArianePointerState( double InViewportX
                                        , double InViewportY
                                        , const FStylusState& InStylusState )
    : ViewportX ( InViewportX )
    , ViewportY ( InViewportY )

{
    Z = InStylusState.GetZ();
    Timer = InStylusState.GetTimer();
    Tilt = InStylusState.GetTilt();
    TiltX = InStylusState.GetTiltX();
    TiltY = InStylusState.GetTiltY();
    Azimuth = InStylusState.GetAzimuth();
    Altitude = InStylusState.GetAltitude();
    Twist = InStylusState.GetTwist();
    Pressure = InStylusState.GetPressure();
}

FArianePointerState::FArianePointerState( double InViewportX
                                        , double InViewportY )
    : ViewportX ( InViewportX )
    , ViewportY ( InViewportY )
{
    Z = 0.0f;
    Timer = 0;
    Tilt = 0.0f;
    TiltX = 0.0f;
    TiltY = 0.0f;
    Azimuth = 0.0f;
    Altitude = 0.0f;
    Twist = 0.0f;
    Pressure = 1.0f;
}
