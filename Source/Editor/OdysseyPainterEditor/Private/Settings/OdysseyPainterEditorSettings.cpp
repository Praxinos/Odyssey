// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyPainterEditorSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"

#include "IOdysseyStylusInputModule.h"

#if PLATFORM_WINDOWS
    #define OdysseyStylusInputDriver_Default OdysseyStylusInputDriver_Ink
#elif PLATFORM_MAC
    #define OdysseyStylusInputDriver_Default OdysseyStylusInputDriver_NSEvent
#else
    #define OdysseyStylusInputDriver_Default OdysseyStylusInputDriver_None
#endif

UOdysseyPainterEditorSettings::UOdysseyPainterEditorSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , Background( kOdysseyPainterEditorBackground_Checkered )
    , BackgroundColor( FColor( 127, 127, 127 ) )
    , CheckerColorOne( FColor( 166, 166, 166 ) )
    , CheckerColorTwo( FColor( 134, 134, 134 ) )
    , CheckerSize( 10 )
    , Rotation( EKeys::R )
    , Pan( EKeys::P )
    , PickColor( EKeys::LeftAlt )
    , FitToViewport( true )
    , TextureBorderColor( FColor::White )
    , StylusInputDriver( OdysseyStylusInputDriver_Default )
    , TextureBorderEnabled( false )
{
}

void
UOdysseyPainterEditorSettings::PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent )
{
    Super::PostEditChangeProperty( iPropertyChangedEvent );

    //UE_LOG( LogIliad, Log, TEXT( "posteditchangeproperty: %s" ), *iPropertyChangedEvent.GetPropertyName().ToString() );

    //Get the name of the property that was changed  
    FName PropertyName = ( iPropertyChangedEvent.Property != nullptr ) ? iPropertyChangedEvent.Property->GetFName() : NAME_None;

    // We test using GET_MEMBER_NAME_CHECKED so that if someone changes the property name  
    // in the future this will fail to compile and we can update it.  
    if( ( PropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorSettings, StylusInputDriver ) ) )
    {
        RefreshStylusInputDriver();
    }
}

void
UOdysseyPainterEditorSettings::RefreshStylusInputDriver()
{
    TSharedPtr<IStylusInputInterfaceInternal> stylus_input;
    
    switch( StylusInputDriver )
    {
        case OdysseyStylusInputDriver_None:
            stylus_input = nullptr;
            break;
    #if PLATFORM_WINDOWS
        case OdysseyStylusInputDriver_Ink:
            stylus_input = CreateStylusInputInterface();
            break;
        case OdysseyStylusInputDriver_Wintab:
            stylus_input = CreateStylusInputInterfaceWintab();
            break;
    #elif PLATFORM_MAC
        case OdysseyStylusInputDriver_NSEvent:
            stylus_input = CreateStylusInputInterfaceNSEvent();
            break;
    #endif
        default:
            stylus_input = nullptr;
    }
    

    if( !stylus_input.IsValid() )
        StylusInputDriver = OdysseyStylusInputDriver_None;

    UOdysseyStylusInputSubsystem* input_subsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    input_subsystem->SetStylusInputInterface( stylus_input );
}
