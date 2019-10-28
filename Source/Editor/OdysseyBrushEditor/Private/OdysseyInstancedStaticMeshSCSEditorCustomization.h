// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "EditorViewportClient.h"
#include "OdysseyBrushEditorModule.h"
#include "ISOdysseyCSEditorCustomization.h"

class FInstancedStaticMeshSCSEditorCustomization : public ISCSEditorCustomization
{
public:
    static TSharedRef<ISCSEditorCustomization> MakeInstance(TSharedRef< class IOdysseyBrushEditor > InOdysseyBrushEditor);

public:
    /** ISCSEditorCustomization interface */
    virtual bool HandleViewportClick(const TSharedRef<FEditorViewportClient>& InViewportClient, class FSceneView& InView, class HHitProxy* InHitProxy, FKey InKey, EInputEvent InEvent, uint32 InHitX, uint32 InHitY) override;
    virtual bool HandleViewportDrag(class USceneComponent* InComponentScene, class USceneComponent* InComponentTemplate, const FVector& InDeltaTranslation, const FRotator& InDeltaRotation, const FVector& InDeltaScale, const FVector& InPivot) override;
    virtual bool HandleGetWidgetLocation(class USceneComponent* InSceneComponent, FVector& OutWidgetLocation) override;
    virtual bool HandleGetWidgetTransform(class USceneComponent* InSceneComponent, FMatrix& OutWidgetTransform) override;

protected:
    /** Ensure that selection bits are in sync w/ the number of instances */
    void ValidateSelectedInstances(class UInstancedStaticMeshComponent* InComponent);

private:
    /** The blueprint editor we are bound to */
    TWeakPtr<class IOdysseyBrushEditor> OdysseyBrushEditorPtr;
};
