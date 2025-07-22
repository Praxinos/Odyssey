// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Math/MathFwd.h"

class FStoryboardLevelViewportClient;
class FStoryboardViewportZoomController
{
public:
    FStoryboardViewportZoomController(FStoryboardLevelViewportClient* iViewportClient);

public:
    float GetRotation() const; // in degrees
    void SetRotation( float iRotation ); // in degrees

    float GetZoom() const;
    void SetZoom( float iZoom, FVector2D iZoomPosition = FVector2D(0.0f, 0.0f) );

    FVector2D GetPan() const;
    void SetPan( FVector2D iPan );

    void Reset();

    FSlateRenderTransform GetTransform() const;

    FVector2D WidgetToOffset(const FVector2D& iPosition) const;
    FVector2D OffsetToWidget(const FVector2D& iPosition) const;

private:
    FStoryboardLevelViewportClient* mViewportClient;

    float mRotation = 0.f;
    FVector2D mPan = FVector2D( 0.f, 0.f );
    float mZoom = 1.f;
};
