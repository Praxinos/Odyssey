// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyViewportDrawingEditorUtils
{
public:
	/** IToolkit interface */
    static void RetrieveTexturesForComponent(const UMeshComponent* Component, TArray<FPaintableTexture>& OutTextures);

private:
    static void InternalQueryPaintableTextures(int32 MaterialIndex, const UMeshComponent* MeshComponent, int32& OutDefaultIndex, TArray<struct FPaintableTexture>& InOutTextureList);
};
