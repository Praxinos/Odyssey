// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardViewport/StoryboardViewportZoomController.h"

#include "Math/UnitConversion.h"

#include "StoryboardViewport/SStoryboardLevelViewport.h"

#define MinZoom 0.01
#define MaxZoom 200.0

FStoryboardViewportZoomController::FStoryboardViewportZoomController(FStoryboardLevelViewportClient* iViewportClient)
    : mViewportClient(iViewportClient)
{
}

float FStoryboardViewportZoomController::GetRotation() const
{
    return mRotation;
}

void
FStoryboardViewportZoomController::SetRotation( float iRotation )
{
    //Compute new pan value
    float oldRotationRadians = FUnitConversion::Convert( mRotation, EUnit::Degrees, EUnit::Radians );

    float newRotationDegrees = iRotation;
    while(newRotationDegrees <= -180)
    {
        newRotationDegrees += 360.f;
    }
    while(newRotationDegrees > 180)
    {
        newRotationDegrees -= 360.f;
    }

    float newRotationRadians = FUnitConversion::Convert( newRotationDegrees, EUnit::Degrees, EUnit::Radians );

    FSlateRenderTransform transform = GetTransform();
    transform = transform.Concatenate(FSlateRenderTransform(FQuat2D(newRotationRadians - oldRotationRadians)));

    mRotation = newRotationDegrees;
    mPan = WidgetToOffset(transform.GetTranslation());
    mViewportClient->UpdateCameraBounds();
}

float
FStoryboardViewportZoomController::GetZoom() const
{
    return mZoom;
}

void
FStoryboardViewportZoomController::SetZoom( float iZoom, FVector2D iZoomPosition )
{
    float oldZoom = mZoom;
    float newZoom = FMath::Clamp( iZoom, MinZoom, MaxZoom );
    FTransform2D zoomTransform = FTransform2D(newZoom / oldZoom, FVector2D(0,0));

    FSlateRenderTransform transform = GetTransform();
    transform = transform.Concatenate(FTransform2D(OffsetToWidget(-iZoomPosition)));
    transform = FTransform2D( Concatenate(transform, zoomTransform.GetMatrix()));
    transform = transform.Concatenate(FTransform2D(OffsetToWidget(iZoomPosition)));

    mZoom = newZoom;
    mPan = WidgetToOffset(transform.GetTranslation());
    mViewportClient->UpdateCameraBounds();
}

FVector2D
FStoryboardViewportZoomController::GetPan() const
{
    return mPan;
}

void
FStoryboardViewportZoomController::SetPan( FVector2D iPan )
{
    mPan = iPan;
    mViewportClient->UpdateCameraBounds();
}

void
FStoryboardViewportZoomController::Reset()
{
    SetPan(FVector2D(0.f, 0.f));
    SetZoom(1.f);
    SetRotation(0.f);
}

FVector2D
FStoryboardViewportZoomController::WidgetToOffset(const FVector2D& iPosition) const
{
    FVector2D position = iPosition;
    position *= FVector2D(-1.f, 1.0f);
    position *= 2.f;
    position /= mViewportClient->GetViewportGeometry().WidgetSize;

    return position;
}

FVector2D
FStoryboardViewportZoomController::OffsetToWidget(const FVector2D& iPosition) const
{
    FVector2D position = iPosition;
    position *= FVector2D(-1.f, 1.0f);
    position /= 2.f;
    position *= mViewportClient->GetViewportGeometry().WidgetSize;

    return position;
}

FSlateRenderTransform
FStoryboardViewportZoomController::GetTransform() const
{
    FVector2D viewportPan = OffsetToWidget(mPan);

    float radian = FUnitConversion::Convert( mRotation, EUnit::Degrees, EUnit::Radians );
    FSlateRenderTransform transform = FSlateRenderTransform();

    transform = transform.Concatenate(FSlateRenderTransform( FQuat2D( radian ) ));
    transform = transform.Concatenate(FSlateRenderTransform( mZoom ));
    transform = transform.Concatenate(FSlateRenderTransform( viewportPan ));

    return transform;
}
