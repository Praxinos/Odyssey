// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"

class UPaperFlipbook;
class UTexture2D;
enum class EOdysseyExportImageFormat : uint8;

#if WITH_EDITOR
namespace Odyssey
{
    /** Exports animated frames using an animation render context for each request. */
    ODYSSEYANIMATION_API UPaperFlipbook* ExportAsFlipbook(UObject* iObject, const FInt32Range& iRange, float iFramesPerSecond, FString AssetName, FString Path);
    ODYSSEYANIMATION_API TArray<UTexture2D*> ExportAsTextureSequence(UObject* iObject, const FInt32Range& iRange, FString AssetName, FString Path);
    ODYSSEYANIMATION_API TArray<FString> ExportAsImageSequence(UObject* iObject, const FInt32Range& iRange, FString Filename, FString Path, EOdysseyExportImageFormat Format);
}
#endif
