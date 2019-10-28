// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"
#include "OdysseyStyleSet.h"
//#include "Classes/EditorStyleSettings.h"
#include "ISettingsModule.h"

struct FPropertyChangedEvent;

/**
 * Declares the Editor's visual style.
 */
class FSlateOdysseyStyle
    : public FOdysseyStyle
{
public:

    static void Initialize()
    {
        StyleInstance = Create();
        SetStyle( StyleInstance.ToSharedRef() );
    }

    static void Shutdown()
    {
        ResetToDefault();
        ensure( StyleInstance.IsUnique() );
        StyleInstance.Reset();
    }

    class FStyle : public FSlateStyleSet
    {
    public:
        FStyle();

        void Initialize();
        void SetupGeneralStyles();
        void SetupClassIconsAndThumbnails();

        const FVector2D Icon7x16;
        const FVector2D Icon8x4;
        const FVector2D Icon16x4;
        const FVector2D Icon8x8;
        const FVector2D Icon10x10;
        const FVector2D Icon12x12;
        const FVector2D Icon12x16;
        const FVector2D Icon14x14;
        const FVector2D Icon16x16;
        const FVector2D Icon16x20;
        const FVector2D Icon20x20;
        const FVector2D Icon22x22;
        const FVector2D Icon24x24;
        const FVector2D Icon25x25;
        const FVector2D Icon32x32;
        const FVector2D Icon40x40;
        const FVector2D Icon48x48;
        const FVector2D Icon64x64;
        const FVector2D Icon36x24;
        const FVector2D Icon128x128;

        const TSharedRef< FLinearColor > DefaultForeground_LinearRef;
        const FSlateColor DefaultForeground;

        FTableRowStyle NormalTableRowStyle;
    };

    static TSharedRef< class FSlateOdysseyStyle::FStyle > Create()
    {
        TSharedRef< class FSlateOdysseyStyle::FStyle > NewStyle = MakeShareable( new FSlateOdysseyStyle::FStyle() );
        NewStyle->Initialize();
        return NewStyle;
    }

    static TSharedPtr< FSlateOdysseyStyle::FStyle > StyleInstance;


};
