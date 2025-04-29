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
        UTexture* BackgroundTexture = nullptr,
        UTexture* ForegroundTexture = nullptr,
        UTextureRenderTarget2D* DestinationTexture = nullptr,
        FIntRect SourceRect = FIntRect(),
        FIntRect DestinationRect = FIntRect(),
        FMatrix Transform = FMatrix(),
        EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal,
        float Opacity = 1.f,
        EOdysseyAntiAliasing AntiAliasing = EOdysseyAntiAliasing::NearestNeighbor
    );
};
