// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Settings/EposSequenceEditorSettings.h"

FBoardSettings::FBoardSettings()
    : DefaultTickFrameRate( 24000, 1 )
    , DefaultDisplayFrameRate( 24, 1 )
{
}

FShotSettings::FShotSettings()
    : DefaultTickFrameRate( 24000, 1 )
    , DefaultDisplayFrameRate( 24, 1 )
{
}

//---

void
UEposSequenceEditorSettings::PostInitProperties()
{
    Super::PostInitProperties();

    UpdateValues();
}

#if WITH_EDITOR

void
UEposSequenceEditorSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    // If the user changed one of these 2 properties, leave the one that they changed alone, and
    // re-adjust the other one.
    if( PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED( FCameraLensSettings, MinFocalLength ) )
    {
        CameraSettings.LensSettings.MaxFocalLength = FMath::Max( CameraSettings.LensSettings.MinFocalLength, CameraSettings.LensSettings.MaxFocalLength );
    }
    else if( PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED( FCameraLensSettings, MaxFocalLength ) )
    {
        CameraSettings.LensSettings.MinFocalLength = FMath::Min( CameraSettings.LensSettings.MinFocalLength, CameraSettings.LensSettings.MaxFocalLength );
    }

    // Recalculate everything based on any new values.
    UpdateValues();
}

#endif

void
UEposSequenceEditorSettings::UpdateValues()
{
    // validate incorrect values
    CameraSettings.LensSettings.MaxFocalLength = FMath::Max( CameraSettings.LensSettings.MinFocalLength, CameraSettings.LensSettings.MaxFocalLength );

    // respect physical limits of the (simulated) hardware
    CameraSettings.CurrentFocalLength = FMath::Clamp( CameraSettings.CurrentFocalLength, CameraSettings.LensSettings.MinFocalLength, CameraSettings.LensSettings.MaxFocalLength );
    CameraSettings.CurrentAperture = FMath::Clamp( CameraSettings.CurrentAperture, CameraSettings.LensSettings.MinFStop, CameraSettings.LensSettings.MaxFStop );

    //FieldOfView = GetHorizontalFieldOfView();
    CameraSettings.Filmback.SensorAspectRatio = ( CameraSettings.Filmback.SensorHeight > 0.f ) ? ( CameraSettings.Filmback.SensorWidth / CameraSettings.Filmback.SensorHeight ) : 0.f;
    //AspectRatio = Filmback.SensorAspectRatio;

#if WITH_EDITORONLY_DATA
    //CurrentHorizontalFOV = FieldOfView;          // informational variable only, for editor users
    CameraSettings.CurrentHorizontalFOV = 0.f;
    if( CameraSettings.CurrentFocalLength > 0.f )
        CameraSettings.CurrentHorizontalFOV = FMath::RadiansToDegrees( 2.f * FMath::Atan( CameraSettings.Filmback.SensorWidth / ( 2.f * CameraSettings.CurrentFocalLength ) ) );
#endif
}
