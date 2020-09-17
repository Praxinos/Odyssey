// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Models/OdysseyPainterEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorCommands"

FOdysseyPainterEditorCommands::FOdysseyPainterEditorCommands()
    : TCommands<FOdysseyPainterEditorCommands>( "OdysseyPainterEditor", NSLOCTEXT( "Contexts", "OdysseyPainterEditor", "Odyssey Painter Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyPainterEditorCommands::RegisterCommands()
{
    UI_COMMAND( AboutIliad, "About ILIAD", "About ILIAD", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( VisitPraxinosWebsite, "Praxinos Website...", "Praxinos Website...", EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( VisitPraxinosForums, "Praxinos Forums...", "Praxinos Forums...", EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( ResetViewportPosition, "ResetViewportPosition", "ResetViewportPosition",EUserInterfaceActionType::Button, FInputChord( EKeys::NumPadFive ) );
    UI_COMMAND( ResetViewportRotation, "ResetViewportRotation", "ResetViewportRotation",EUserInterfaceActionType::Button, FInputChord( EKeys::NumPadTwo ) );
    UI_COMMAND( RotateViewportLeft, "RotateViewportLeft", "RotateViewportLeft",         EUserInterfaceActionType::Button, FInputChord( EKeys::NumPadOne ) );
    UI_COMMAND( RotateViewportRight, "RotateViewportRight", "RotateViewportRight",      EUserInterfaceActionType::Button, FInputChord( EKeys::NumPadThree ) );

    UI_COMMAND( SetZoom10Percent, "SetZoom10Percent", "SetZoom10Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::Ampersand ) );
    UI_COMMAND( SetZoom20Percent, "SetZoom20Percent", "SetZoom20Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::E_AccentAigu ) );
    UI_COMMAND( SetZoom30Percent, "SetZoom30Percent", "SetZoom30Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::Quote ) );
    UI_COMMAND( SetZoom40Percent, "SetZoom40Percent", "SetZoom40Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::Apostrophe ) );
    UI_COMMAND( SetZoom50Percent, "SetZoom50Percent", "SetZoom50Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::LeftParantheses ) );
    UI_COMMAND( SetZoom60Percent, "SetZoom60Percent", "SetZoom60Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::Hyphen ) );
    UI_COMMAND( SetZoom70Percent, "SetZoom70Percent", "SetZoom70Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::E_AccentGrave ) );
    UI_COMMAND( SetZoom80Percent, "SetZoom80Percent", "SetZoom80Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::Underscore ) );
    UI_COMMAND( SetZoom90Percent, "SetZoom90Percent", "SetZoom90Percent",               EUserInterfaceActionType::Button, FInputChord( EKeys::C_Cedille ) );
    UI_COMMAND( SetZoom100Percent, "SetZoom100Percent", "SetZoom100Percent",            EUserInterfaceActionType::Button, FInputChord( EKeys::A_AccentGrave ) );
    UI_COMMAND( SetZoomFitScreen, "SetZoomFitScreen", "SetZoomFitScreen",               EUserInterfaceActionType::Button, FInputChord( EKeys::F ) );
    UI_COMMAND( ZoomIn, "ZoomIn", "ZoomIn",                                             EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_COMMAND( ZoomOut, "ZoomOut", "ZoomOut",                                          EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );

    
#if PLATFORM_MAC
    UI_COMMAND( Undo, "Undo Iliad", "Undo Iliad",                                       EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo Iliad",                                       EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Y ) );
    UI_COMMAND( FillCurrentLayer, "FillCurrentLayer", "FillCurrentLayer",               EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::F ) );
    UI_COMMAND( ClearCurrentLayer, "ClearCurrentLayer", "ClearCurrentLayer",            EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::C ) );
    UI_COMMAND( CreateNewLayer, "CreateNewLayer", "CreateNewLayer",                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::N ) );
    UI_COMMAND( DuplicateCurrentLayer, "DuplicateCurrentLayer", "DuplicateCurrentLayer",EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::D ) );
    UI_COMMAND( DeleteCurrentLayer, "DeleteCurrentLayer", "DeleteCurrentLayer",         EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::X ) );
    UI_COMMAND( SwitchTabletAPI, "SwitchTabletAPI", "SwitchTabletAPI",                  EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::A ) );
#else
    UI_COMMAND( Undo, "Undo Iliad", "Undo Iliad",                                       EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo Iliad",                                       EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Y ) );
    UI_COMMAND( FillCurrentLayer, "FillCurrentLayer", "FillCurrentLayer",               EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::F ) );
    UI_COMMAND( ClearCurrentLayer, "ClearCurrentLayer", "ClearCurrentLayer",            EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::C ) );
    UI_COMMAND( CreateNewLayer, "CreateNewLayer", "CreateNewLayer",                     EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::N ) );
    UI_COMMAND( DuplicateCurrentLayer, "DuplicateCurrentLayer", "DuplicateCurrentLayer",EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::D ) );
    UI_COMMAND( DeleteCurrentLayer, "DeleteCurrentLayer", "DeleteCurrentLayer",         EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::X ) );
    UI_COMMAND( SwitchTabletAPI, "SwitchTabletAPI", "SwitchTabletAPI",                  EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::A ) );
#endif
    
}

#undef LOCTEXT_NAMESPACE
