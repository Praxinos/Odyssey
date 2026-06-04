// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "IStylusState.h"

class FArianeEditor;
class FSceneView;
class FStylusState;

struct FArianePointerState
{
    public:
        ~FArianePointerState();
        FArianePointerState();
        FArianePointerState( double InViewportX
                           , double InViewportY
                           , const FStylusState& InStylusState );
        FArianePointerState( double InViewportX
                           , double InViewportY );

    public:
        double       ViewportX;
        double       ViewportY;
        float        Z;
        unsigned int Timer;
        double       Tilt;
        double       TiltX;
        double       TiltY;
        float        Azimuth;
        float        Altitude;
        float        Twist;
        float        Pressure;
};
