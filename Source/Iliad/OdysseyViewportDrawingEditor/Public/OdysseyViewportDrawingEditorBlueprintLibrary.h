// Copyright Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyViewportDrawingEditorBlueprintLibrary.generated.h"

UCLASS()
class UOdysseyViewportDrawingEditorBlueprintLibrary :
    public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    /** Starts an analytics session without any custom attributes specified */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static void ActivateViewportEditorMode(const FName& EditorMode);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor", meta = (DeprecatedFunction, DeprecationMessage = "Use GetOdysseyViewportDrawingEditorMode"))
    static FName GetIliadViewportDrawingEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetOdysseyViewportDrawingEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetModelingEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetNoneEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetDefaultEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetPlacementEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetMeshPaintEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetLandscapeEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetFoliageEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetLevelEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetStreamingLevelEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetPhysicsEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetActorPickerEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetSceneDepthPickerEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetAnimationEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetBrushEditingEditorMode();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Editor")
    static FName GetFractureEditorMode();
};
