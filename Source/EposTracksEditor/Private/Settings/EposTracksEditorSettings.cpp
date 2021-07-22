// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Settings/EposTracksEditorSettings.h"

void
UEposTracksEditorSettings::PostInitProperties()
{
    Super::PostInitProperties();

    UpdateValues();
}

FName
UEposTracksEditorSettings::GetContainerName() const
{
    return TEXT( "Editor" );
}

FName
UEposTracksEditorSettings::GetCategoryName() const
{
    return TEXT( "Plugins" );
}

void
UEposTracksEditorSettings::SetDefaultDuration( float iDefaultDuration )
{
    if( DefaultDuration == iDefaultDuration )
        return;

    DefaultDuration = iDefaultDuration;
    SaveConfig();
}

#if WITH_EDITOR

void
UEposTracksEditorSettings::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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
UEposTracksEditorSettings::UpdateValues()
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
