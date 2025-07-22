// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#include "OdysseyViewportDrawingEditorBlueprintLibrary.h"
#include "Editor.h"
#include "EditorModes.h"
#include "EditorModeManager.h"
#include "OdysseyViewportDrawingEditorEdMode.h"

void
UOdysseyViewportDrawingEditorBlueprintLibrary::ActivateViewportEditorMode(const FName& EditorMode)
{
    GLevelEditorModeTools().ActivateMode( EditorMode, true );
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetIliadViewportDrawingEditorMode()
{
    return GetOdysseyViewportDrawingEditorMode();
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingEditorMode()
{
    return FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetModelingEditorMode()
{
    return TEXT("EM_ModelingToolsEditorMode");
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetNoneEditorMode()
{
    return FBuiltinEditorModes::EM_None;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetDefaultEditorMode()
{
    return FBuiltinEditorModes::EM_Default;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetPlacementEditorMode()
{
    return FBuiltinEditorModes::EM_Placement;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetMeshPaintEditorMode()
{
    return FBuiltinEditorModes::EM_MeshPaint;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetLandscapeEditorMode()
{
    return FBuiltinEditorModes::EM_Landscape;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetFoliageEditorMode()
{
    return FBuiltinEditorModes::EM_Foliage;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetLevelEditorMode()
{
    return FBuiltinEditorModes::EM_Level;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetStreamingLevelEditorMode()
{
    return FBuiltinEditorModes::EM_StreamingLevel;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetPhysicsEditorMode()
{
    return FBuiltinEditorModes::EM_Physics;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetActorPickerEditorMode()
{
    return FBuiltinEditorModes::EM_ActorPicker;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetSceneDepthPickerEditorMode()
{
    return FBuiltinEditorModes::EM_SceneDepthPicker;
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetAnimationEditorMode()
{
    return TEXT("EditMode.ControlRig");
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetBrushEditingEditorMode()
{
    return TEXT("EM_Geometry");
}

FName
UOdysseyViewportDrawingEditorBlueprintLibrary::GetFractureEditorMode()
{
    return TEXT("EM_FractureEditorMode");
}
