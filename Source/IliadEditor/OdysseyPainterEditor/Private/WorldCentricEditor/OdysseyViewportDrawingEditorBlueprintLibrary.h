// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyViewportDrawingEditorExtension.h"

#include "OdysseyViewportDrawingEditorBlueprintLibrary.generated.h"

class AActor;
class UMaterialInterface;
class UMeshComponent;
class UTexture;

UCLASS()
class UOdysseyViewportDrawingEditorBlueprintLibrary :
    public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    /** Starts an analytics session without any custom attributes specified */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static void ActivateViewportEditorMode(const FName& EditorMode);

    /**
     * Return the name of the Odyssey Editor Mode.
     *
     * Deprecated: use Get Odyssey Viewport Drawing Editor Mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor", meta = (DeprecatedFunction, DeprecationMessage = "Use GetOdysseyViewportDrawingEditorMode"))
    static FName GetIliadViewportDrawingEditorMode();

    /**
     * Return the name of the Odyssey Editor Mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetOdysseyViewportDrawingEditorMode();

    /**
     * Return the name of the Modeling editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetModelingEditorMode();

    /** Return the name of the None editor mode (no mode). */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetNoneEditorMode();

    /**
     * Return the name of the Default editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetDefaultEditorMode();

    /**
     * Return the name of the Placement editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetPlacementEditorMode();

    /**
     * Return the name of the Mesh Paint editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetMeshPaintEditorMode();

    /**
     * Return the name of the Landscape editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetLandscapeEditorMode();

    /**
     * Return the name of the Foliage editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetFoliageEditorMode();

    /**
     * Return the name of the Level editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetLevelEditorMode();

    /**
     * Return the name of the Streaming Level editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetStreamingLevelEditorMode();

    /**
     * Return the name of the Physics editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetPhysicsEditorMode();

    /**
     * Return the name of the Actor Picker editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetActorPickerEditorMode();

    /**
     * Return the name of the Scene Depth Picker editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetSceneDepthPickerEditorMode();

    /**
     * Return the name of the Animation (Control Rig) editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetAnimationEditorMode();

    /**
     * Return the name of the Brush Editing (Geometry) editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetBrushEditingEditorMode();

    /**
     * Return the name of the Fracture editor mode.
     *
     * Use it with Activate Viewport Editor Mode to switch to this mode.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetFractureEditorMode();

public:
    /** Check if the Odyssey Mode is the current active one */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor")
    static bool IsOdysseyViewportDrawingEditorModeActive();

    //--- Painting Method

    /** Get the current painting adapter method */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor|ViewportDrawing")
    static EOdysseyViewportDrawingPaintingAdapterMethod GetOdysseyViewportDrawingPaintingAdapterMethod();

    /** Set the painting adapter method */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor|ViewportDrawing")
    static void SetOdysseyViewportDrawingPaintingAdapterMethod( EOdysseyViewportDrawingPaintingAdapterMethod Method );

    //--- Actor

    /** Get the selected actor */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor|ViewportDrawing")
    static AActor* GetOdysseyViewportDrawingActor();

    /** Set the selected actor */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor|ViewportDrawing")
    static void SetOdysseyViewportDrawingActor( AActor* Actor );

    //--- Component

    /** Get the selected component in the actor */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor|ViewportDrawing")
    static UMeshComponent* GetOdysseyViewportDrawingComponent();

    /** Set the selected component in the actor */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor|ViewportDrawing")
    static void SetOdysseyViewportDrawingComponent( UMeshComponent* Component );

    /** Get the selectable components in the actor */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor|ViewportDrawing")
    static TArray<UMeshComponent*> GetOdysseyViewportDrawingSelectableComponents();

    //--- Material

    /** Get the selected material in the component */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor|ViewportDrawing")
    static UMaterialInterface* GetOdysseyViewportDrawingMaterial();

    /** Set the selected material in the component */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor|ViewportDrawing")
    static void SetOdysseyViewportDrawingMaterial( UMaterialInterface* Material );

    /** Get the selectable material in the component */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Editor|ViewportDrawing" )
    static TArray<UMaterialInterface*> GetOdysseyViewportDrawingSelectableMaterials();

    //--- Texture

    /** Get the selected material in the component */
    UFUNCTION(BlueprintPure, Category="Odyssey|Editor|ViewportDrawing")
    static UTexture* GetOdysseyViewportDrawingTexture();

    /** Set the selected material in the component */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor|ViewportDrawing")
    static void SetOdysseyViewportDrawingTexture( UTexture* Texture );

    /** Get the selectable textures in the material */
    UFUNCTION( BlueprintPure, Category = "Odyssey|Editor|ViewportDrawing" )
    static TArray<UTexture*> GetOdysseyViewportDrawingSelectableTextures();
};
