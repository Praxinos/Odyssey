// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OdysseyAntiAliasing.h"
#include "OdysseyBlendingMode.h"
#include "OdysseyBlendShader.h"
#include "OdysseyImageAnchor.h"

#include "OdysseyShadersBlueprintLibrary.generated.h"

UCLASS(BlueprintType)
class UOdysseyShadersBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, Category = "Odyssey | Shaders")
    static void Blend(
        const UObject* WorldContextObject,
        UTexture* SourceTexture = nullptr,
        UTextureRenderTarget2D* DestinationTexture = nullptr,
        FVector2D SourcePosition = FVector2D(0.f, 0.f),
        FVector2D SourceSize = FVector2D(1.f, 1.f),
        FOdysseyImageAnchor SourceAnchor = FOdysseyImageAnchor(),
        FVector2D Position = FVector2D(0.f, 0.f),
        FVector2D Scale = FVector2D(1.f, 1.f),
        float RotationInDegrees = 0.f,
        EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal,
        //TODO: AlphaMode
        float Opacity = 1.f,
        EOdysseyAntiAliasing AntiAliasing = EOdysseyAntiAliasing::NearestNeighbor
    );
};
