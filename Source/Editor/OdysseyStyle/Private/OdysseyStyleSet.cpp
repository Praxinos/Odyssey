// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyStyleSet.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/CoreStyle.h"

TSharedPtr< ISlateStyle > FOdysseyStyle::Instance = NULL;

void FOdysseyStyle::ResetToDefault()
{
    SetStyle( FCoreStyle::Create("OdysseyStyle") );
}

void FOdysseyStyle::SetStyle( const TSharedRef< ISlateStyle >& NewStyle )
{
    if( Instance != NewStyle )
    {
        if ( Instance.IsValid() )
        {
            FSlateStyleRegistry::UnRegisterSlateStyle( *Instance.Get() );
        }

        Instance = NewStyle;

        if ( Instance.IsValid() )
        {
            FSlateStyleRegistry::RegisterSlateStyle( *Instance.Get() );
        }
        else
        {
            ResetToDefault();
        }
    }
}
