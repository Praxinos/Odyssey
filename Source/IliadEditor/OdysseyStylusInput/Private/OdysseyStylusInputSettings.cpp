// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyStylusInputSettings.h"

#include "Editor.h"
#include "Editor/EditorEngine.h"

#include "IOdysseyStylusInputModule.h"

#define LOCTEXT_NAMESPACE "StylusInput"

FOnStylusInputDriverChanged UOdysseyStylusInputSettings::OnStylusInputDriverChanged;

UOdysseyStylusInputSettings::UOdysseyStylusInputSettings( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
    , StylusInputDriver("None")
{
}

void
UOdysseyStylusInputSettings::PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent )
{
    Super::PostEditChangeProperty( iPropertyChangedEvent );
}

FName
UOdysseyStylusInputSettings::GetStylusDriver() const
{
    return StylusInputDriver;
}

//static
FText
UOdysseyStylusInputSettings::GetFormatText( FName iStylusInputDriver )
{
    return FText::FromName( iStylusInputDriver );
}

#undef LOCTEXT_NAMESPACE
