// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportDrawingEditorBlueprintLibrary.h"

#include "Editor.h"
#include "EditorModes.h"
#include "EditorModeManager.h"
#include "MeshPaintTypes.h"

#include "OdysseyViewportDrawingEditorEdMode.h"
#include "OdysseyViewportDrawingEditorExtension.h"
#include "OdysseyViewportDrawingEditorToolkit.h"

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

//---

//static
bool
UOdysseyViewportDrawingEditorBlueprintLibrary::IsOdysseyViewportDrawingEditorModeActive()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return false;

    return true;
}

//static
EOdysseyViewportDrawingPaintingAdapterMethod
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingPaintingAdapterMethod()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;

    return extension->PaintingAdapterMethod();
}

//static
void
UOdysseyViewportDrawingEditorBlueprintLibrary::SetOdysseyViewportDrawingPaintingAdapterMethod( EOdysseyViewportDrawingPaintingAdapterMethod iNewMethod )
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return;

    extension->SetPaintingAdapterMethod( iNewMethod );
}

//static
AActor*
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingActor()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return nullptr;

    return extension->Actor();
}

//static
void
UOdysseyViewportDrawingEditorBlueprintLibrary::SetOdysseyViewportDrawingActor( AActor* iNewActor )
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return;

    extension->SetActor( iNewActor );
}

//static
UMeshComponent*
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingComponent()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return nullptr;

    return extension->Component();
}

//static
void
UOdysseyViewportDrawingEditorBlueprintLibrary::SetOdysseyViewportDrawingComponent( UMeshComponent* iNewComponent )
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return;

    extension->SetComponent( iNewComponent );
}

//static
TArray<UMeshComponent*>
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingSelectableComponents()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return TArray<UMeshComponent*>();

    return extension->SelectableComponents();
}

//static
UMaterialInterface*
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingMaterial()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return nullptr;

    return extension->Material();
}

//static
void
UOdysseyViewportDrawingEditorBlueprintLibrary::SetOdysseyViewportDrawingMaterial( UMaterialInterface* iNewMaterial )
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return;

    extension->SetMaterial( iNewMaterial );
}

//static
TArray<UMaterialInterface*>
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingSelectableMaterials()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return TArray<UMaterialInterface*>();

    TArray<UMaterialInterface*> materials;
    extension->SelectableMaterials( materials );

    return materials;
}

//static
UTexture*
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingTexture()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return nullptr;

    return extension->Texture();
}

//static
void
UOdysseyViewportDrawingEditorBlueprintLibrary::SetOdysseyViewportDrawingTexture( UTexture* iNewTexture )
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return;

    extension->SetTexture( iNewTexture, false );
}

//static
TArray<UTexture*>
UOdysseyViewportDrawingEditorBlueprintLibrary::GetOdysseyViewportDrawingSelectableTextures()
{
    FOdysseyViewportDrawingEditorEdMode* EdMode = GLevelEditorModeTools().GetActiveModeTyped<FOdysseyViewportDrawingEditorEdMode>( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = EdMode ? EdMode->GetViewportDrawingEditorToolkit() : nullptr;
    TSharedPtr<FOdysseyViewportDrawingEditorExtension> extension = toolkit ? toolkit->GetViewportDrawingExtension() : nullptr;
    if( !extension )
        return TArray<UTexture*>();

    TArray<FPaintableTexture> paintableTextures = extension->SelectableTextures();

    TArray<UTexture*> textures;
    for( const FPaintableTexture& paintableTexture : paintableTextures )
        textures.Add( paintableTexture.Texture );

    return textures;
}
