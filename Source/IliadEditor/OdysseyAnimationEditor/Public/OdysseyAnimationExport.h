// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"

class UPaperFlipbook;
class UTexture2D;
enum class EOdysseyExportImageFormat : uint8;

namespace Odyssey
{
    /**
     * Exports frames of an animation, or of any object rendering animation frames, as assets or files.
     * Each frame is rendered with FOdysseyAnimationRenderContext::MakeForFrame().
     * @param iRange The frames to export, in animation frames, bounds included.
     * @param Path The content path (assets) or the directory (images) to export into.
     * @return The created assets or files; empty or null on failure.
     */
    ODYSSEYANIMATIONEDITOR_API UPaperFlipbook* ExportAsFlipbook(UObject* iObject, const FInt32Range& iRange, float iFramesPerSecond, FString AssetName, FString Path);
    ODYSSEYANIMATIONEDITOR_API TArray<UTexture2D*> ExportAsTextureSequence(UObject* iObject, const FInt32Range& iRange, FString AssetName, FString Path);
    ODYSSEYANIMATIONEDITOR_API TArray<FString> ExportAsImageSequence(UObject* iObject, const FInt32Range& iRange, FString Filename, FString Path, EOdysseyExportImageFormat Format);
}
