// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "MeshPaintTypes.h"

class FOdysseyViewportDrawingEditorUtils
{
public:
    static void RetrieveTexturesForComponent(const UMeshComponent* Component, TArray<FPaintableTexture>& OutTextures);
    static bool GenerateSeamMask(UMeshComponent* MeshComponent, int32 UVSet, UTextureRenderTarget2D* SeamRenderTexture, UTexture* Texture, UTextureRenderTarget2D* RenderTargetTexture);
    static void CopyTextureToRenderTargetTexture(UTexture* SourceTexture, UTextureRenderTarget2D* RenderTargetTexture, ERHIFeatureLevel::Type FeatureLevel);

private:
    static void InternalQueryPaintableTextures(int32 MaterialIndex, const UMeshComponent* MeshComponent, int32& OutDefaultIndex, TArray<struct FPaintableTexture>& InOutTextureList);
};
