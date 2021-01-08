// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Models/OdysseyPainterEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorCommands"

FOdysseyPainterEditorCommands::FOdysseyPainterEditorCommands()
    : TCommands<FOdysseyPainterEditorCommands>( "IliadPainterEditor", NSLOCTEXT( "Contexts", "IliadPainterEditor", "Iliad Painter Editor" ), NAME_None, FEditorStyle::GetStyleSetName() )
{
}

void
FOdysseyPainterEditorCommands::RegisterCommands()
{
    UI_COMMAND( AboutIliad, "About ILIAD", "About ILIAD",                                          EUserInterfaceActionType::Button, FInputChord( EKeys::F1 ) );
    UI_COMMAND( VisitPraxinosWebsite, "Praxinos Website...", "Praxinos Website...",                EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( VisitPraxinosForums, "Praxinos Forums...", "Praxinos Forums...",                   EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( ImportTexturesAsLayers, "Import Textures As Layers", "Import Textures As Layers",  EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::P ) );
    UI_COMMAND( ExportLayersAsTextures, "Export Layer As Textures", "Export Layer As Textures",    EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::E ) );    

    UI_COMMAND( ResetViewportRotation, "Reset Viewport Rotation", "Reset viewport rotation",       EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::R ) );
    UI_COMMAND( ResetViewportPosition, "Reset Viewport Position", "Reset viewport position",       EUserInterfaceActionType::Button, FInputChord( EKeys::Escape ) );
    UI_COMMAND( RotateViewportLeft, "Rotate Viewport Left", "Rotate viewport left",                EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::L ) );
    UI_COMMAND( RotateViewportRight, "Rotate Viewport Right", "Rotate viewport right",             EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::R ) );
    UI_COMMAND( SetZoom10Percent, "Set Zoom 10 Percent", "Set zoom at 10 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom20Percent, "Set Zoom 20 Percent", "Set zoom at 20 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom30Percent, "Set Zoom 30 Percent", "Set zoom at 30 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom40Percent, "Set Zoom 40 Percent", "Set zoom at 40 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom50Percent, "Set Zoom 50 Percent", "Set zoom at 50 percent",                 EUserInterfaceActionType::Button, FInputChord( EKeys::H ) );
    UI_COMMAND( SetZoom60Percent, "Set Zoom 60 Percent", "Set zoom at 60 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom70Percent, "Set Zoom 70 Percent", "Set zoom at 70 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom80Percent, "Set Zoom 80 Percent", "Set zoom at 80 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom90Percent, "Set Zoom 90 Percent", "Set zoom at 90 percent",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetZoom100Percent, "Set Zoom 100 Percent", "Set zoom at 100 percent",              EUserInterfaceActionType::Button, FInputChord( EKeys::Z ) );
    UI_COMMAND( SetZoomFitScreen, "Set Zoom Fit Screen", "Set zoom fit screen",                    EUserInterfaceActionType::Button, FInputChord( EKeys::S ) );
    UI_COMMAND( ZoomIn, "Zoom In", "Zoom in",                                                      EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_COMMAND( ZoomOut, "Zoom Out", "Zoom out",                                                   EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );
   
    UI_COMMAND( RefreshBrush, "Refresh Brush", "Refresh Brush",                                    EUserInterfaceActionType::Button, FInputChord( EKeys::F5 ) );
    
    UI_COMMAND( IncreaseBrushSize, "Increase Brush Size", "Increase Brush Size by 1",              EUserInterfaceActionType::Button, FInputChord( EKeys::Add ) );
    UI_COMMAND( DecreaseBrushSize, "Decrease Brush Size", "Decrease Brush Size by 1",              EUserInterfaceActionType::Button, FInputChord( EKeys::Subtract ) );
    UI_COMMAND( SetAlphaModeNormal, "Set Alpha Mode to Normal", "Set Alpha Mode to Normal",        EUserInterfaceActionType::Button, FInputChord( EKeys::B ) );
    UI_COMMAND( SetAlphaModeErase, "Set Alpha Mode to Erase", "Set Alpha Mode to Erase",           EUserInterfaceActionType::Button, FInputChord( EKeys::E ) );
    UI_COMMAND( SetAlphaModeTop, "Set Alpha Mode to Top", "Set Alpha Mode to Top",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeBack, "Set Alpha Mode to Back", "Set Alpha Mode to Back",              EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeSub, "Set Alpha Mode to Sub", "Set Alpha Mode to Sub",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeAdd, "Set Alpha Mode to Add", "Set Alpha Mode to Add",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeMul, "Set Alpha Mode to Mul", "Set Alpha Mode to Mul",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeMin, "Set Alpha Mode to Min", "Set Alpha Mode to Min",                 EUserInterfaceActionType::Button, FInputChord() );
    UI_COMMAND( SetAlphaModeMax, "Set Alpha Mode to Max", "Set Alpha Mode to Max",                 EUserInterfaceActionType::Button, FInputChord() );

    UI_COMMAND( SwitchTabletAPI, "Switch Tablet API", "Switch tablet API",                         EUserInterfaceActionType::Button, FInputChord( EKeys::F12 ) );
   
    UI_COMMAND( CreateNewLayer, "Create New Layer", "Create new image layer",                      EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::N ) );
    UI_COMMAND( DuplicateCurrentLayer, "Duplicate Current Layer", "Duplicate current layer",       EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::D ) );
    UI_COMMAND( FillCurrentLayer, "Fill Current Layer", "Fill current layer",                      EUserInterfaceActionType::Button, FInputChord( EKeys::F ) );
    UI_COMMAND( ClearCurrentLayer, "Clear Current Layer", "Clear current layer",                   EUserInterfaceActionType::Button, FInputChord( EKeys::Delete ) );
    UI_COMMAND( DeleteCurrentLayer, "Delete Current Layer", "Delete current layer",                EUserInterfaceActionType::Button, FInputChord( EModifierKey::Shift, EKeys::Delete ) );

#if PLATFORM_MAC
    UI_COMMAND( Undo, "Undo Iliad", "Undo stroke in Iliad",                                        EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo stroke in Iliad",                                        EUserInterfaceActionType::Button, FInputChord( EModifierKey::Command, EKeys::Y ) );
#else                                                                                              
    UI_COMMAND( Undo, "Undo Iliad", "Undo stroke in Iliad",                                        EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Z ) );
    UI_COMMAND( Redo, "Redo Iliad", "Redo stroke in Iliad",                                        EUserInterfaceActionType::Button, FInputChord( EModifierKey::Control, EKeys::Y ) );
#endif
    
}

#undef LOCTEXT_NAMESPACE
