// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyStylusInputSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"

#include "IOdysseyStylusInputModule.h"

#define LOCTEXT_NAMESPACE "StylusInput"

// The better default value for each OS
#if PLATFORM_WINDOWS
    const EOdysseyStylusInputDriver OdysseyStylusInputDriver_Default = OdysseyStylusInputDriver_NativeWindows;
#elif PLATFORM_MAC
    const EOdysseyStylusInputDriver OdysseyStylusInputDriver_Default = OdysseyStylusInputDriver_NSEvent;
#else
    const EOdysseyStylusInputDriver OdysseyStylusInputDriver_Default = OdysseyStylusInputDriver_None;
#endif

UOdysseyStylusInputSettings::UOdysseyStylusInputSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , StylusInputDriver( OdysseyStylusInputDriver_Default )
{
}

void
UOdysseyStylusInputSettings::PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent )
{
    Super::PostEditChangeProperty( iPropertyChangedEvent );

    //Get the name of the property that was changed
    FName PropertyName = ( iPropertyChangedEvent.Property != nullptr ) ? iPropertyChangedEvent.Property->GetFName() : NAME_None;

    // We test using GET_MEMBER_NAME_CHECKED so that if someone changes the property name
    // in the future this will fail to compile and we can update it.
    if( ( PropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyStylusInputSettings, StylusInputDriver ) ) )
    {
        //RefreshStylusInputDriver();
    }
}

EOdysseyStylusInputDriver
UOdysseyStylusInputSettings::GetStylusDriver() const
{
    return StylusInputDriver;
}

void
UOdysseyStylusInputSettings::RefreshStylusInputDriver()
{
    /* UOdysseyStylusInputSubsystem* input_subsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    input_subsystem->SetStylusInputDriver(StylusInputDriver); */
}

//static
FText
UOdysseyStylusInputSettings::GetFormatText( TSharedPtr<EOdysseyStylusInputDriver> iStylusInputDriver )
{
    switch( *iStylusInputDriver )
    {
        case OdysseyStylusInputDriver_None:     return LOCTEXT( "settings.driver.none", "None" );
#if PLATFORM_WINDOWS
        case OdysseyStylusInputDriver_Ink:      return LOCTEXT( "settings.driver.ink", "Ink" );
        case OdysseyStylusInputDriver_Wintab:   return LOCTEXT( "settings.driver.wintab", "Wintab" );
        case OdysseyStylusInputDriver_NativeWindows:      return LOCTEXT( "settings.driver.native", "Native" );
#elif PLATFORM_MAC
        case OdysseyStylusInputDriver_NSEvent:  return LOCTEXT( "settings.driver.nsevent", "NSEvent" );
#endif
    }

    return LOCTEXT( "settings.driver.invalid", "Invalid" );
}

#undef LOCTEXT_NAMESPACE
